
//c ++- std = c ++ 11 - O3 - o variables.out variables.cpp;./variables.out

#ifndef MAIN
	#define MAIN 5
	int main();
#endif


#include <forward_list>  //alternative linked list container
#include <unordered_map> //k:v object container
#include "extern.h"

#define NIL 0
#define INT 1
#define STR 2
#define LST 3
#define OBJ 4
#define PIN 5
#define FNC 6

class variable;

typedef variable* var;
typedef int var_int;
typedef string var_str;
typedef vector<var> var_lst;
typedef unordered_map<string, var> var_obj;

namespace Variables {

	const char* getType(int t) {
		switch(t) {
			case NIL: return "undefined";
			case INT: return "integer";
			case STR: return "string";
			case LST: return "list";
			case OBJ: return "object";
			case PIN: return "pin";
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

	var create(void* value, int type = 0);

		//integers
	forward_list<var_int> integers;
	//return new variable((var_str*)value, STR);
 	var create(var_int value, int type = 1);
	var_int* addInt(var_int* v) {
		integers.push_front(*v);
		return &integers.front();
	}

		//strings
	forward_list<var_str> strings;
	var create(var_str value, int type = 2);
	var_str* addStr(var_str* v) {
		strings.push_front(*v);
		return &strings.front();
	}

		//lists
	forward_list<var_lst> lists;
	var create(var_lst value, int type = 3);
	var_lst* addLst(var_lst* v) {
		lists.push_front(*v);
		return &lists.front();
	}

		//objects
	forward_list<var_obj> objects;
	var create(var_obj value, int type = 4);
	var_obj* addObj(var_obj* v) {
		objects.push_front(*v);
		return &objects.front();
	}
};

class variable {
public:
	int type; //0:null, 1:int, 2:str, 3:lst, 4:obj
	void* value;

	void set(void* v) {
		switch(type) {
			case INT: case PIN: *(var_int*)value = *(var_int*)v; return;
			case STR:           *(var_str*)value = *(var_str*)v; return;
			case LST: case FNC: *(var_lst*)value = *(var_lst*)v; return;
			case OBJ:           *(var_obj*)value = *(var_obj*)v; return;
		}
	}

		//constuctor
	variable(void* v, int type) {
		if(this->type = type, !type) return; //?
		switch(type) {
			case INT: case PIN: value = Variables::addInt((var_int*)v); return;
			case STR:           value = Variables::addStr((var_str*)v); return;
			case LST: case FNC: value = Variables::addLst((var_lst*)v); return;
			case OBJ:           value = Variables::addObj((var_obj*)v); return;
		}
	}

	var_int* getInt() {return (var_int*)value;}
	var_str* getStr() {return (var_str*)value;}
	var_lst* getLst() {return (var_lst*)value;}
	var_obj* getObj() {return (var_obj*)value;}
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
			case INT: if(v->type == INT) *(var_int*)value += *(var_int*)v->value; return;
			case STR: if(v->type == STR) *(var_str*)value += *(var_str*)v->value; return;
			case LST: if(v->type == LST) ((var_lst*)value)->insert(((var_lst*)value)->end(), ((var_lst*)v)->begin(), ((var_lst*)v)->end()); return;
			case OBJ: if(v->type == OBJ) ((var_obj*)value)->insert(((var_obj*)v)->begin(), ((var_obj*)v)->end()); return;
			default: Error::ict(Variables::getType(type), Variables::getType(v->type));
		}
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
var Variables::create(void*   value, int type) {return new variable( value, type);}
var Variables::create(var_int value, int type) {return new variable(&value, type);}
var Variables::create(var_str value, int type) {return new variable(&value, type);}
var Variables::create(var_lst value, int type) {return new variable(&value, type);}
var Variables::create(var_obj value, int type) {return new variable(&value, type);}

void Variables::_stringify(var value, string* str) {
	switch(value->type) {
		case INT: case PIN: *str += to_string(*value->getInt()); break;
		case STR:
			*str += "\"";
			*str += *value->getStr() + "\"";
		break;
		case LST: case FNC:
			*str += "[";
			for(var val : *value->getLst()) {
				if(str->operator[](str->length()-1)!='[') *str += ",";
				_stringify(val, str);
			}
			*str += "]";
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

#if MAIN == 5

int main() {
	var a = Variables::create(5);
	cout << *a->getInt() << endl;


	var list = Variables::create(var_lst( {
		Variables::create("Hello "),
		Variables::create("World! "),
		Variables::create(123)
	} ));
	cout << *list->at(0)->getStr() << *list->at(1)->getStr() << *list->at(2)->getInt() << endl;

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
	string temp2 = "finished";
	vobj->at("f")->set(&temp2);
	(*vobj->getObj())["list"] = list;
	cout << *vobj->at("f")->getStr() << endl;

	cout << Variables::stringify(vobj) << endl;
	return 0;
}

#endif
