
//c++ -std=c++11 -O3 -o variables.out variables.cpp;./variables.out

#ifndef MAIN
	#define MAIN 5
	int main();
#endif


#include <forward_list>   //alternative linked list container
#include <unordered_map>  //k:v object container
#include "extern.h"

class variable;

typedef variable* var;
typedef vector<var> var_lst;
typedef unordered_map<string,var> var_obj;

namespace Variables {

	const char* getType(int t) {
		return t==0?"undefined":t==1?"integer":t==2?"string":t==3?"list":t==4?"object":"unknown";
	}

		//integers
	forward_list<int> integers;
	int* addInt(int* v) {
		integers.push_front(*v);
		return &integers.front();
	}

		//strings
	forward_list<string> strings;
	string* addStr(string* v) {
		strings.push_front(*v);
		return &strings.front();
	}

		//lists
	forward_list<var_lst> lists;
	var_lst* addLst(var_lst* v) {
		lists.push_front(*v);
		return &lists.front();
	}

		//objects
	forward_list<var_obj> objects;
	var_obj* addObj(var_obj* v) {
		objects.push_front(*v);
		return &objects.front();
	}
};

class variable {
public:
	int type;    //0:null, 1:int, 2:str, 3:lst, 4:obj
	void* value;

		//undefined constuctor
	variable(int* null) {type = 0;}

	int* getInt() {return (int*)value;}
	void get(int** v) {*v = (int*)value;}
	void set(int v) {*(int*)value = v;}
		//int constuctor
	variable(int v) {
		this->type = 1;
		this->value = Variables::addInt(&v);
	}


	string* getStr() {return (string*)value;}
	void get(string** v) {*v = (string*)value;}
	void set(string v) {*(string*)value = v;}
		//string constuctor
	variable(string v) {
		this->type = 2;
		this->value = Variables::addStr(&v);
	}


	var_lst* getLst() {return (var_lst*)value;}
	void get(var_lst** v) {*v = (var_lst*)value;}
	void set(var_lst v) {*(var_lst*)value = v;}
		//list constuctor
	variable(var_lst v) {
		this->type = 3;
		this->value = Variables::addLst(&v);
	}


	var_obj* getObj() {return (var_obj*)value;}
	void get(var_obj** v) {*v = (var_obj*)value;}
	void set(var_obj v) {*(var_obj*)value = v;}
		//object constuctor
	variable(var_obj v) {
		this->type = 4;
		this->value = Variables::addObj(&v);
	}

	var getKeys() {
		switch(type) {
			case 4:
				var keys;
				for(pair<string,var> kvp : *getObj()) ((var_lst*)value)->push_back(new variable(kvp.first));
			return keys;
		}
		Error::error("%ss have no 'Keys()' member",Variables::getType(type));
	}

		//random access for strings and lists
	var at(int i) {
		if(type==3) return (*(var_lst*)value)[i];
		else if(type==2) return new variable((*(string*)(value))[i]);
		Error::error("%ss don't support random access",Variables::getType(type));
	}

		//random access for objects
	var at(string key) {
		if(type==4) return (*(var_obj*)value)[key];
		Error::error("%s don't support random access",Variables::getType(type));
	}

	var copy() {
		switch(type) {
			case 0: return new variable(nullptr);
			case 1: return new variable(*(int*)value);
			case 2: return new variable(*(string*)value);
			case 3: return new variable(*(var_lst*)value);
			case 4: return new variable(*(var_obj*)value);
		}
	}

	void operator=(var v) {
		switch(type) {
			case 1: if(v->type==1) value = v->value; return;
			case 2: if(v->type==2) value = v->value; return;
			case 3: if(v->type==3) value = v->value; return;
			case 4: if(v->type==4) value = v->value; return;
		}
		Error::ict(Variables::getType(type), Variables::getType(v->type));
	}

	void operator+=(var v) {
		switch(type) {
			case 0: Error::iop("undefined","+=");
			case 1: if(v->type==1) *(int*)value += *(int*)v->value; return;
			case 2: if(v->type==2) *(string*)value += *(string*)v->value; return;
			case 3: if(v->type==3) ((var_lst*)value)->insert(((var_lst*)value)->end(), ((var_lst*)v)->begin(), ((var_lst*)v)->end()); return;
			case 4: if(v->type==4) ((var_obj*)value)->insert(((var_obj*)v)->begin(), ((var_obj*)v)->end()); return;
		}
		Error::ict(Variables::getType(type), Variables::getType(v->type));
	}

	void operator-=(var v) {
		if(type == 1)
			if(v->type == 1) *(int*)value += *(int*)v->value;
			else Error::ict("integer", Variables::getType(v->type));
		else Error::iop(Variables::getType(type), "-=");
	}
};

#if MAIN == 5

int main() {

	int* temp1;
	var a = new variable(5);
	a->get(&temp1);
	cout << *temp1 << endl;

	*a = 10;
	cout << *a->getInt() << endl;


	var list = new variable(var_lst({new variable("Hello "), new variable("World! "), new variable(123)}));
	cout << *list->at(0)->getStr() << *list->at(1)->getStr() << *list->at(2)->getInt() << endl;

	*list->at(2) = a; //reference
	a->set(15);
	cout << *list->at(2)->getInt() << endl;

	*list->at(2) = a->copy(); //real copy
	a->set(20);
	cout << *list->at(2)->getInt() << endl;

	var_obj obj = {{"a", new variable(1)}, {"b", new variable(4)}};
	obj["f"] = new variable("reference");
	var vobj = new variable(obj);
	cout << *vobj->at("f")->getStr() << endl;

	for(pair<string,var> kvp : obj) cout << kvp.first << endl;
	//for(string v : keys) cout << v << endl;
	return 0;
}

#endif
