
//c++ -std=c++11 -O3 -o variables.out variables.cpp;./variables.out

#ifndef MAIN
	#define MAIN 5
	int main();
#endif


#include <vector>         //alternative list container
#include <forward_list>   //alternative linked list container
#include <unordered_map>  //k:v object container
#include "extern.h"

class variable;

typedef variable* var;
typedef vector<var> bs_list;
typedef unordered_map<string,var> bs_object;

const char* getType(int t) {
	return t==0?"undefined":t==1?"integer":t==2?"string":t==3?"list":t==4?"object":"unknown";
}

namespace Variables {

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
	forward_list<bs_list> lists;
	bs_list* addLst(bs_list* v) {
		lists.push_front(*v);
		return &lists.front();
	}

		//objects
	forward_list<bs_object> objects;
	bs_object* addObj(bs_object* v) {
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


	bs_list* getLst() {return (bs_list*)value;}
	void get(bs_list** v) {*v = (bs_list*)value;}
	void set(bs_list v) {*(bs_list*)value = v;}
		//list constuctor
	variable(bs_list v) {
		this->type = 3;
		this->value = Variables::addLst(&v);
	}


	bs_object* getObj() {return (bs_object*)value;}
	void get(bs_object** v) {*v = (bs_object*)value;}
	void set(bs_object v) {*(bs_object*)value = v;}
		//object constuctor
	variable(bs_object v) {
		this->type = 4;
		this->value = Variables::addObj(&v);
	}

		//random access for strings and lists
	var at(int i) {
		if(type==3) return (*(bs_list*)value)[i];
		else if(type==2) return new variable((*(string*)(value))[i]);
		error("%ss don't support random access",getType(type));
	}

		//random access for objects
	var at(string key) {
		if(type==4) return (*(bs_object*)value)[key];
		error("%s don't support random access",getType(type));
	}

	var copy() {
		switch(type) {
			case 0: return new variable(nullptr);
			case 1: return new variable(*(int*)value);
			case 2: return new variable(*(string*)value);
			case 3: return new variable(*(bs_list*)value);
			case 4: return new variable(*(bs_object*)value);
		}
	}

	void operator=(var v) {
		const char* vtype = getType(v->type);

		switch(type) {
			case 0: Error::ict("undefined", vtype);

			case 1: if(v->type==1) value = v->value; break;
				Error::ict("integer", vtype);

			case 2: if(v->type==2) value = v->value; break;
				Error::ict("string", vtype);

			case 3: if(v->type==3) value = v->value; break;
				Error::ict("list", vtype);

			case 4: if(v->type==4) value = v->value; break;
				Error::ict("object", vtype);
		}
	}

	void operator+=(var v) {
		const char* vtype = getType(v->type);

		switch(type) {
			case 0: Error::iop("undefined","+=");

			case 1: if(v->type==1) *(int*)value += *(int*)v->value; break;
				Error::ict("integer", vtype);

			case 2: if(v->type==2) value = v->value; break;
				Error::ict("string", vtype);

			case 3: if(v->type==3) value = v->value; break;
				Error::ict("list", vtype);

			case 4: if(v->type==4) value = v->value; break;
				Error::ict("object", vtype);
		}
	}
};

#if MAIN == 5

int main() {

	int* temp1;
	var a = new variable(15);
	a->get(&temp1);
	cout << *temp1 << endl;

	*a = new variable(20);
	cout << *a->getInt() << endl;


	var list = new variable(bs_list({new variable("Hello "), new variable("World!")}));
	cout << *list->at(0)->getStr() << endl;


	bs_object obj;
	obj["f"] = new variable(555);
	var vobj = new variable(obj);
	cout << *vobj->at("f")->getInt() << endl;

	return 0;
}

#endif
