
//$ g++ -std=c++11 -O3 -c variables.cpp

#include "variables.hpp"
#include "error.hpp"

namespace Variables {

		//list of instances of all created variables
	vector<var> variables;

		//returns the name of the type of a variable
	const char* getType(uint8_t t) {
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

		//convert variable to string equivalent
	void _stringify(var value, string* str) {
		switch(value->type) {
			case T_PIN: case T_INT: case T_FLT:
				if(value->type == T_PIN) *str += "Pin(";
					/**/else if(value->type == T_INT) *str += "T_INT ";
					/**/else *str += "T_FLT ";
				*str += to_string(getInt(value));
				if(value->type == T_PIN) *str += ")";
			break;
			case T_STR: case T_VAR:
				if(value->type == T_STR) *str += "\"";
					/**/else *str += "T_VAR ";
				*str += getStr(value);
				if(value->type == T_STR) *str += "\"";
			break;
			case T_LST: case T_TRM: case T_FNC:
				switch(value->type) {
					case T_LST: *str += "["; break;
					case T_TRM: *str += "("; break;
					case T_FNC: *str += "{"; break;
				}
				for(var val : getLst(value)) {
					string::reverse_iterator end(str->end());
					if(*end != '[' && *end != '{' && *end != '(') *str += ",";
					_stringify(val, str);
				}
				switch(value->type) {
					case T_LST: *str += "]"; break;
					case T_TRM: *str += ")"; break;
					case T_FNC: *str += "}"; break;
				}
			break;
			case T_OBJ:
				*str += "[";
				for(pair<string,var> kvp : getObj(value)) {
					if(str->operator[](str->length()-1)!='[') *str += ",";
					*str += "\"";
					*str += kvp.first + "\":";
					_stringify(kvp.second, str);
				}
				*str += "]";
			break;
		}
	}

		//direct stringify function
	string stringify(var value) {
		string str = "";
		_stringify(value, &str);
		return str;
	}
	/*
	var parse(string str) {
		return _parse(&str);
	}*/

	//note: when using create(*ptr, int) instead of
	//create(val) the .out file will be smaller
		//undefined / null
	var create(void* value, uint8_t type, bool builtin) {
		variables.push_back(new variable( value, type));
		return variables.back();
	}

		//integers
	forward_list<var_int> integers;
	var create(var_int value, uint8_t type, bool builtin) {
		variables.push_back(new variable(&value, type));
		return variables.back();
	}
	var_int* addInt(var_int* v) {
		integers.push_front(*v);
		return &integers.front();
	}

		//floating points
	forward_list<var_flt> floats;
	var create(var_flt value, uint8_t type, bool builtin) {
		variables.push_back(new variable(&value, type));
		return variables.back();
	}
	var_flt* addFlt(var_flt* v) {
		floats.push_front(*v);
		return &floats.front();
	}

		//strings
	forward_list<var_str> strings;
	var create(var_str value, uint8_t type, bool builtin) {
		variables.push_back(new variable(&value, type));
		return variables.back();
	}
	var_str* addStr(var_str* v) {
		strings.push_front(*v);
		return &strings.front();
	}

		//lists
	forward_list<var_lst> lists;
	var create(var_lst value, uint8_t type, bool builtin) {
		variables.push_back(new variable(&value, type));
		return variables.back();
	}
	var_lst* addLst(var_lst* v) {
		lists.push_front(*v);
		return &lists.front();
	}

		//objects
	forward_list<var_obj> objects;
	var create(var_obj value, uint8_t type, bool builtin) {
		variables.push_back(new variable(&value, type));
		return variables.back();
	}
	var_obj* addObj(var_obj* v) {
		objects.push_front(*v);
		return &objects.front();
	}

		//free space -> deletes ALL variables
	void free() {
		while(variables.size()) delete variables.front();
	}
}

/* * * * * * * * * *
 * class variable  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * */

variable::variable(void* v, uint8_t type, bool builtin) {
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
		case T_TRM:
		case T_FNC: value = Variables::addLst((var_lst*)v); return;
		case T_OBJ: value = Variables::addObj((var_obj*)v); return;
	}
}

	//destructor: search variable value in variables and erase it
variable::~variable() {
	vector<var>::iterator it = Variables::variables.begin();
	while(*it != this) it++;
	Variables::variables.erase(it);
	//cout << "deleted " << this << " " << stringify(this) << endl;
}

	//assign value v to the variable
void variable::set(void* v, uint8_t type) {
	if(!type) type = this->type;

		//variable and v has different types -> incompatible types error
	if(this->type && this->type != type)
		err_ict(Variables::getType(this->type), Variables::getType(type));

		//assign v to variable
	switch(type) {
		case T_NIL: variable(v, type); return;
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
var variable::keys() {
	if(type == T_OBJ) {
		var_lst keys;
		for(pair<string, var> kvp : *getObj()) keys.push_back(new variable(kvp.first));
		return new variable(keys);
	}
	err_imu(getType(type), "keys");
}

var variable::values() {
	if(type == T_OBJ) {
		var_lst vals;
		for(pair<string,var> kvp : *getObj()) vals.push_back(kvp.second);
		return new variable(vals);
	}
	err_imu(getType(type), "values");
}*/

	//random access for strings and lists
var variable::at(int i) {
	if(type == T_LST) return (*(var_lst*)value)[i];
	else if(type == T_FLT) {
		char c = ((var_str*)value)->at(i);
		return Variables::create(string(&c));
	}
	error("%ss don't support random access", Variables::getType(type));
}

	//random access for objects
var variable::at(string key) {
	if(type == T_OBJ) return (*(var_obj*)value)[key];
	error("%s don't support random access", Variables::getType(type));
}

	//creates a new variable instance
var variable::copy() {
	return Variables::create(value, type);
}

	//assign variable instance to variable
void variable::operator = (var v) {
	if(type == v->type) value = v->value;
	else err_ict(Variables::getType(type), Variables::getType(v->type));
}

	//add variable instance to variable
void variable::operator += (var v) {
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

	//substract variable instance from integer variable
void variable::operator -= (var v) {
	if(type == T_INT) {
		if(v->type == T_INT) *(var_int*)value += *(var_int*)v->value;
		else err_ict("integer", Variables::getType(v->type));
	} else err_iop(Variables::getType(type), "-=");
}

//void _parse(...) {}
