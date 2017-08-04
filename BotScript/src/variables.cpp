
//c++ -std=c++11 -O3 -o variables.out variables.cpp;./variables.out

#ifndef _VARIABLES_CPP_
#define _VARIABLES_CPP_ 21933

#include <forward_list>  //alternative linked list container
#include <unordered_map> //k:v object container
#include "extern.h"

#ifndef MAIN
	#define MAIN _VARIABLES_CPP_
	int main();
#endif

	//type id's
#define NIL 0
#define VAR 1
#define INT 2
#define STR 3
#define LST 4
#define OBJ 5
#define PIN 6
#define TRM 7
#define FNC 8

class variable;

typedef variable* var;
typedef int var_int;
typedef string var_str;
typedef vector<var> var_lst;
typedef unordered_map<string, var> var_obj;

namespace Variables {

	const char* getType(uint8_t t) {
		switch(t) {
			case NIL: return "undefined";
			case VAR: return "name";
			case INT: return "integer";
			case STR: return "string";
			case LST: return "list";
			case OBJ: return "object";
			case PIN: return "pin";
			case TRM: return "term";
			case FNC: return "function";
			default : return "unknown";
		}
	}

	void _stringify(var value, string* str);

	string stringify(var value) {
		string str = "";
		_stringify(value, &str);
		return str;
	}
/*
	var parse(string str) {
		return _parse(&str);
	}*/

	var create(void* value, uint8_t type = NIL);

		//integers
	forward_list<var_int> integers;
	//return new variable((var_str*)value, STR);
 	var create(var_int value, uint8_t type = INT);
	var_int* addInt(var_int* v) {
		integers.push_front(*v);
		return &integers.front();
	}

		//strings
	forward_list<var_str> strings;
	var create(var_str value, uint8_t type = STR);
	var_str* addStr(var_str* v) {
		strings.push_front(*v);
		return &strings.front();
	}

		//lists
	forward_list<var_lst> lists;
	var create(var_lst value, uint8_t type = LST);
	var_lst* addLst(var_lst* v) {
		lists.push_front(*v);
		return &lists.front();
	}

		//objects
	forward_list<var_obj> objects;
	var create(var_obj value, uint8_t type = OBJ);
	var_obj* addObj(var_obj* v) {
		objects.push_front(*v);
		return &objects.front();
	}

	vector<var> variables;
	void free();
};

class variable {
public:
	uint8_t type;
	void* value;

		//constuctor
	variable(void* v, uint8_t type) {
		if(!type) return;
		this->type = type;
		switch(type) {
			case INT:
			case PIN: value = Variables::addInt((var_int*)v); return;
			case STR:
			case VAR: value = Variables::addStr((var_str*)v); return;
			case LST:
			case TRM:
			case FNC: value = Variables::addLst((var_lst*)v); return;
			case OBJ: value = Variables::addObj((var_obj*)v); return;
		}
	}

		//destructor
	~variable() {
		vector<var>::iterator it = Variables::variables.begin();
		while(*it != this) it++;
		Variables::variables.erase(it);
		//cout << "deleted " << this << " " << Variables::stringify(this) << endl;
	}

	var_int* getInt() {return (var_int*)value;}
	var_str* getStr() {return (var_str*)value;}
	var_lst* getLst() {return (var_lst*)value;}
	var_obj* getObj() {return (var_obj*)value;}

	void set(void* v, uint8_t type = 0) {
		if(!type) type = this->type;
		if(this->type && this->type != type)
			Error::ict(Variables::getType(this->type), Variables::getType(type));

		switch(type) {
			case NIL: variable(v, type); return;
			case INT:
			case PIN: *(var_int*)value = *(var_int*)v; return;
			case STR: *(var_str*)value = *(var_str*)v; return;
			case LST:
			case TRM:
			case FNC: *(var_lst*)value = *(var_lst*)v; return;
			case OBJ: *(var_obj*)value = *(var_obj*)v; return;
		}
	}
/*
	var keys() {
		if(type == OBJ) {
			var_lst keys;
			for(pair<string, var> kvp : *getObj()) keys.push_back(new variable(kvp.first));
			return new variable(keys);
		}
		Error::imu(Variables::getType(type), "keys");
	}

	var values() {
		if(type == OBJ) {
			var_lst vals;
			for(pair<string,var> kvp : *getObj()) vals.push_back(kvp.second);
			return new variable(vals);
		}
		Error::imu(Variables::getType(type), "values");
	}*/

		//random access for strings and lists
	var at(int i) {
		if(type == LST) return (*(var_lst*)value)[i];
		else if(type == STR) {
			char c = ((var_str*)value)->at(i);
			return Variables::create(string(&c));
		}
		Error::error("%ss don't support random access", Variables::getType(type));
	}

		//random access for objects
	var at(string key) {
		if(type == OBJ) return (*(var_obj*)value)[key];
		Error::error("%s don't support random access", Variables::getType(type));
	}

	var copy() {
		return Variables::create(value, type);
	}

	void operator = (var v) {
		if(type == v->type) value = v->value;
		else Error::ict(Variables::getType(type), Variables::getType(v->type));
	}

	void operator += (var v) {
		switch(type) {
			case NIL: Error::iop("undefined", "+=");
			case INT: if(v->type == INT) *(var_int*)value += *(var_int*)v->value; else break; return;
			case STR: if(v->type == STR) *(var_str*)value += *(var_str*)v->value; else break; return;
			case LST: if(v->type == LST) ((var_lst*)value)->insert(((var_lst*)value)->end(), v->getLst()->begin(), v->getLst()->end()); else break; return;
			case OBJ: if(v->type == OBJ) ((var_obj*)value)->insert(v->getObj()->begin(), v->getObj()->end()); else break; return;
			default: Error::iop(Variables::getType(type), "+=");
		}
		Error::ict(Variables::getType(type), Variables::getType(v->type));
	}

	void operator -= (var v) {
		if(type == INT)
			if(v->type == INT) *(var_int*)value += *(var_int*)v->value;
			else Error::ict("integer", Variables::getType(v->type));
		else Error::iop(Variables::getType(type), "-=");
	}
};

	//define in Variables declared create functions
	//using Variables::create(*ptr, int) is better than Variables::create(val) (*.out file smaller)
var  Variables::create(void*   value, uint8_t type) {variables.push_back(new variable( value, type)); return variables.back();}
var  Variables::create(var_int value, uint8_t type) {variables.push_back(new variable(&value, type)); return variables.back();}
var  Variables::create(var_str value, uint8_t type) {variables.push_back(new variable(&value, type)); return variables.back();}
var  Variables::create(var_lst value, uint8_t type) {variables.push_back(new variable(&value, type)); return variables.back();}
var  Variables::create(var_obj value, uint8_t type) {variables.push_back(new variable(&value, type)); return variables.back();}
void Variables::free() {while(variables.size()) delete variables.front();}
void Variables::_stringify(var value, string* str) {
	switch(value->type) {
		case PIN: case INT:
			if(value->type == PIN) *str+="Pin(";
			*str += to_string(*value->getInt());
			if(value->type == PIN) *str += ")";
		break;
		case STR: case VAR:
			if(value->type == STR) *str += "\"";
			*str += *value->getStr();
			if(value->type == STR) *str += "\"";
		break;
		case LST: case TRM: case FNC:
			switch(value->type) {
				case LST: *str += "["; break;
				case TRM: *str += "("; break;
				case FNC: *str += "{"; break;
			}
			for(var val : *value->getLst()) {
				string::reverse_iterator end(str->end());
				if(*end != '[' && *end != '{' && *end != '(') *str += ",";
				_stringify(val, str);
			}
			switch(value->type) {
				case LST: *str += "]"; break;
				case TRM: *str += ")"; break;
				case FNC: *str += "}"; break;
			}
		break;
		case OBJ:
			*str += "[";
			for(pair<string,var> kvp : *value->getObj()) {
				if(str->operator[](str->length()-1)!='[') *str += ",";
				*str += "\"";
				*str += kvp.first + "\":";
				_stringify(kvp.second, str);
			}
			*str += "]";
		break;
	}
}
//void Variables::_parse(...) {}

#if MAIN == _VARIABLES_CPP_

int main() {

	var a = Variables::create(5);
	cout << *a->getInt() << endl;

	var list = Variables::create(var_lst( {
		Variables::create("Hello "),
		Variables::create("World! "),
		Variables::create(123)
	} ));
	cout << *list->at(0)->getStr() << *list->at(1)->getStr() << *list->at(2)->getInt() << endl;
	*list += list; //list->getLst()->insert(list->getLst()->end(), list->getLst()->begin(), list->getLst()->end());

	*list->at(2) = a; //reference
	int temp = 15;
	a->set(&temp);
	cout << *list->at(2)->getInt() << endl;

	temp = 20;
	*list->at(2) = a->copy(); //real copy
	a->set(&temp);
	cout << *list->at(2)->getInt() << endl;

	var_obj obj = {{"a", Variables::create(254)}, {"b", Variables::create("eof") }};
	obj["f"] = Variables::create("reference");
	var vobj = Variables::create(&obj, OBJ);
	(*vobj->getObj())["list"] = list->copy();

	cout << Variables::stringify(vobj) << endl;

	Variables::free();
	return 0;
}

#endif //MAIN == _VARIABLES_CPP_

#endif //_VARIABLES_CPP_
