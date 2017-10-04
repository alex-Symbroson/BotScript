
//$ g++ -std=c++11 -O3 -c Variables.cpp

#include "variables.hpp"
#include "error.hpp"

namespace Variables {

		//list of instances of all created Variables
	vector<var> all;

		//returns the name of the type of a Variable
	const char* getType(uint8_t t) {
		debug("Variables::getType(%i);", t);
		switch(t) {
			case T_NIL: return "undefined";
			case T_VAR: return "name";
			case T_INT: return "integer";
			case T_FLT: return "float";
			case T_STR: return "string";
			case T_LST: return "list";
			case T_OBJ: return "object";
			case T_PIN: return "pin";
			case T_TRM: return "term";
			case T_FNC: return "function";
			default   : return "unknown";
		}
	}
/*
		//convert Variable to string equivalent
	void _stringify(var value, string* str) {
		if(value->builtin) {
			*str += "[builtin]";
			return;
		}

		switch(value->type) {
			case T_PIN: case T_INT: case T_FLT:
				switch(value->type) {
					case T_INT: *str += "Int "; break;
					case T_PIN: *str += "Pin "; break;
					case T_FLT: *str += "Flt "; break;
				}
				*str += stringify(getInt(value));
			break;
			case T_STR: case T_VAR: *str += getStr(value); break;
			case T_LST: case T_TRM: case T_FNC:
				switch(value->type) {
					case T_LST: *str += stringify(getLst(value), "[]"); return;
					case T_TRM: *str += stringify(getLst(value), "()"); return;
					case T_FNC: *str += stringify(getLst(value), "{}"); return;
				}
			case T_OBJ: *str += stringify(getObj(value)); break;
		}
	}
*/
		//convert variable to string equivalent
	void _stringify(var value, string* str, bool mark) {
		//debug("Variables::_stringify(var, string* \"%s\", bool %s)", str->c_str(), mark?"true":"false");
		if(value->builtin) {
			//printf("tried to stringify builtin");
			*str += " [builtin] ";
			return;
		}

		switch(value->type) {
			case T_PIN: case T_INT: case T_FLT:
				//debug("Variables::_stringify case INT");
				if(value->type == T_PIN) *str += "Pin ";
				*str += to_string(getInt(value));
			break;
			case T_STR: case T_VAR:
				//debug("Variables::_stringify case STR");
				if(value->type == T_VAR) *str += "Var ";

				if(mark) *str += "\"" + getStr(value) + "\"";
				else *str += getStr(value);
			break;
			case T_LST: case T_TRM: {
				//debug("Variables::_stringify case LST");
				if(mark) {
					if(value->type == T_LST) *str += "[";
					else *str += "(";
				}

				var_lst::iterator first(getLst(value).begin());
				for(var val : getLst(value)) {
					if(val != *first) *str += ",";
					_stringify(val, str, mark);
				}

				if(mark) {
					if(value->type == T_LST) *str += "]";
					else *str += ")";
				}
			} break;
			case T_FNC: {
				//debug("Variables::_stringify case FNC");
				var_obj fnc = getObj(value);
				*str += getStr(fnc["name"]) + "(" + stringify(fnc["args"], false)
					+ ")" + "{" + stringify(fnc["content"]) + "}";
			} break;
			case T_OBJ: {
				//debug("Variables::_stringify case OBJ");
				*str += "[";
				var_obj::iterator first = getObj(value).begin();
				for(pair<string,var> kvp : getObj(value)) {
					if(kvp.first != first->first) *str += ",";
					*str += "\"" + kvp.first + "\":";
					_stringify(kvp.second, str, mark);
				}
				*str += "]";
			} break;
			default: //T_NIL
				//debug("Variables::_stringify case NIL");
				*str += "undefined";
		}
	}

		//direct stringify function
	string stringify(var value, bool mark) {
		debug("Variables::stringify(var)");

		string s = "";
		_stringify(value, &s, mark);
		return s;
	}

	string stringify(char** list, uint len) {
		debug("Variables::stringify(char**, uint)");
		string s = "";
		while(len--)
			if(len) s = string(", ") + list[len] + s;
			else s = list[len] + s;
		return s;
	}

	string stringify(int value) {
		debug("Variables::stringify(int)");
		return to_string(value);
	}

	string stringify(int* value, uint len) {
		debug("Variables::stringify(int**, int)");
		string s = "[";
		for(uint i = 0; i < len; i++)
			if(len) s += value[i];
			else s += value[i] + ", ";
		return (s + ']');
	}

	string stringify(var_lst value, const char brackets[]) {
		debug("Variables::stringify(var_lst)");
		string s = "";
		for(var v : value)
			s += stringify(v) + ", ";
		return brackets[0] + s + brackets[1];
	}

	string stringify(var_obj value) {
		debug("Variables::stringify(var_obj)");
		string s = "[";
		for(pair<string, var> v : value)
			s += "\"" + v.first + "\":" + stringify(v.second) + ", ";
		s += ']';
		return s;
	}
	/*
	var parse(string str) {
		return _parse(&str);
	}*/

	//note: when using create(*ptr, int) instead of
	//create(val) the .out file will be smaller
		//undefined / null
	var create(void* value, uint8_t type, bool builtin) {
		debug("Variables::create(void*, %i (%s), %i)", type, Variables::getType(type), builtin);

		all.push_back(new Variable(value, type));
		return all.back();
	}

		//integers
	forward_list<var_int> integers;
	var create(var_int value, uint8_t type, bool builtin) {
		debug("Variables::create(var_int %li, %i (%s), %i)", value, type, Variables::getType(type), builtin);

		all.push_back(new Variable(&value, type));
		return all.back();
	}
	var_int* addInt(var_int* v) {
		debug("Variables::addInt(var_int* %li)", *v);

		integers.push_front(*v);
		return &integers.front();
	}

		//floating points
	forward_list<var_flt> floats;
	var create(var_flt value, uint8_t type, bool builtin) {
		debug("Variables::create(flt, %.15f %i (%s), %i)", value, type, Variables::getType(type), builtin);

		all.push_back(new Variable(&value, type));
		return all.back();
	}
	var_flt* addFlt(var_flt* v) {
		debug("Variables::addInt(var_flt* %.15f)", *v);

		floats.push_front(*v);
		return &floats.front();
	}

		//strings
	forward_list<var_str> strings;
	var create(var_str value, uint8_t type, bool builtin) {
		debug("Variables::create(var_str %s, %i (%s), %i)", value.c_str(), type, Variables::getType(type), builtin);

		all.push_back(new Variable(&value, type));
		return all.back();
	}
	var_str* addStr(var_str* v) {
		debug("Variables::addStr(var_str* %s)", v->c_str());

		strings.push_front(*v);
		return &strings.front();
	}

		//lists
	forward_list<var_lst> lists;
	var create(var_lst value, uint8_t type, bool builtin) {
		debug("Variables::create(var_lst, %i (%s), %i)", type, Variables::getType(type), builtin);

		all.push_back(new Variable(&value, type));
		return all.back();
	}
	var_lst* addLst(var_lst* v) {
		debug("Variables::addLst(var_lst*)");

		lists.push_front(*v);
		return &lists.front();
	}

		//objects
	forward_list<var_obj> objects;
	var create(var_obj value, uint8_t type, bool builtin) {
		debug("Variables::create(var_obj, %i (%s), %i)", type, Variables::getType(type), builtin);

		all.push_back(new Variable(&value, type));
		return all.back();
	}
	var_obj* addObj(var_obj* v) {
		debug("Variables::addObj(var_obj*)");

		objects.push_front(*v);
		return &objects.front();
	}

		//free space -> deletes ALL Variables
	void free() {
		debug("Variables::free()");
		while(all.size()) delete all.front();
	}
}

/* * * * * * * * * *
 * class Variable  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * */

Variable::Variable(void* v, uint8_t type, bool builtin) {
	debug("Variable::constructor %i %s", type, Variables::getType(type));
	if(!type) return;
	this->type = type;
	this->builtin = builtin;
	switch(type) {
		case T_INT:
		case T_PIN: value = Variables::addInt((var_int*)v); return;
		case T_FLT: value = Variables::addFlt((var_flt*)v); return;
		case T_STR:
		case T_VAR: value = Variables::addStr((var_str*)v); return;
		case T_LST:
		case T_TRM: value = Variables::addLst((var_lst*)v); return;
		case T_OBJ:
		case T_FNC: value = Variables::addObj((var_obj*)v); return;
	}
}

	//destructor: search Variable value in Variables and erase it
Variable::~Variable() {
	debug("Variable::destructor() var: %i (%s) = %s", this->type,
		  Variables::getType(this->type), Variables::sstringify(this));
	vector<var>::iterator it = Variables::all.begin();
	while(*it != this) it++;
	Variables::all.erase(it);
	//cout << "deleted " << this << " " << stringify(this) << endl;
}

	//assign value v to the Variable
void Variable::set(void* v, uint8_t type) {
	debug("Variables::set(void, %i (%s))", type, Variables::getType(type));

	if(!type) type = this->type;

		//Variable and v has different types -> incompatible types error
	if(this->type && this->type != type)
		err_ict(Variables::getType(this->type), Variables::getType(type));

		//assign v to Variable
	switch(type) {
		case T_NIL: Variable(v, type); return;
		case T_INT:
		case T_PIN: *(var_int*)value = *(var_int*)v; return;
		case T_FLT: *(var_flt*)value = *(var_flt*)v; return;
		case T_STR: *(var_str*)value = *(var_str*)v; return;
		case T_LST:
		case T_TRM: *(var_lst*)value = *(var_lst*)v; return;
		case T_OBJ:
		case T_FNC: *(var_obj*)value = *(var_obj*)v; return;
	}
}
/*
var Variable::keys() {
	if(type == T_OBJ) {
		var_lst keys;
		for(pair<string, var> kvp : *getObj()) keys.push_back(new Variable(kvp.first));
		return new Variable(keys);
	}
	err_imu(getType(type), "keys");
}

var Variable::values() {
	if(type == T_OBJ) {
		var_lst vals;
		for(pair<string,var> kvp : *getObj()) vals.push_back(kvp.second);
		return new Variable(vals);
	}
	err_imu(getType(type), "values");
}*/

	//random access for strings and lists
var Variable::at(int i) {
	debug("Variable::at(int %i)", i);

	if(type == T_LST) return (*(var_lst*)value)[i];
	else if(type == T_FLT) {
		char c = ((var_str*)value)->at(i);
		return Variables::create(string(&c));
	}
	error("%ss don't support random access", Variables::getType(type));
	return V_NULL;
}

	//random access for objects
var Variable::at(string key) {
	debug("Variable::at(\"%s\")", key.c_str());

	if(type == T_OBJ) return (*(var_obj*)value)[key];
	error("%s don't support random access", Variables::getType(type));
	return V_NULL;
}

	//creates a new Variable instance
var Variable::copy() {
	debug("Variable::copy()");
	return Variables::create(value, type);
}

	//assign Variable instance to Variable
void Variable::operator = (var v) {
	debug("Variable::op=()");
	if(type == v->type) value = v->value;
	else err_ict(Variables::getType(type), Variables::getType(v->type));
}

	//add Variable instance to Variable
void Variable::operator += (var v) {
	debug("Variable::op+=()");
	switch(type) {
		case T_NIL: err_iop("undefined", "+="); //can't add smth to null -> invalid operator
		case T_INT: if(v->type == T_INT) *(var_int*)value += *(var_int*)v->value; else break; return;
		case T_FLT: if(v->type == T_FLT) *(var_flt*)value += *(var_flt*)v->value; else break; return;
		case T_STR: if(v->type == T_STR) *(var_str*)value += *(var_str*)v->value; else break; return;
		case T_LST: if(v->type == T_LST) ((var_lst*)value)->insert(((var_lst*)value)->end(), getLst(v).begin(), getLst(v).end()); else break; return;
		case T_OBJ: if(v->type == T_OBJ) ((var_obj*)value)->insert(getObj(v).begin(), getObj(v).end()); else break; return;
		default: err_iop(Variables::getType(type), "+=");
	}
	err_ict(Variables::getType(type), Variables::getType(v->type));
}

	//substract Variable instance from integer Variable
void Variable::operator -= (var v) {
	debug("Variable::op-=()");
	if(type == T_INT) {
		if(v->type == T_INT) *(var_int*)value += *(var_int*)v->value;
		else err_ict("integer", Variables::getType(v->type));
	} else err_iop(Variables::getType(type), "-=");
}

//void _parse(...) {}
