
//c++ -std=c++11 -O3 variables.cpp;./a.out

#include <iostream>
#include <vector>
#include <forward_list>
#include <unordered_map>

using namespace std;

//extern "C" int sprintf(const char* f, ...);

class variable;

typedef variable* var;
typedef vector<var> bs_list;
typedef unordered_map<string,var> bs_object;


class variables {
public:

	static forward_list<int> integers;
	static int* addInt(int* v) {
		integers.push_front(*v);
		return &integers.front();
	}

	static forward_list<string> strings;
	static string* addStr(string* v) {
		strings.push_front(*v);
		return &strings.front();
	}

	static forward_list<bs_list> lists;
	static bs_list* addLst(bs_list* v) {
		lists.push_front(*v);
		return &lists.front();
	}

	static forward_list<bs_object> objects;
	static bs_object* addObj(bs_object* v) {
		objects.push_front(*v);
		return &objects.front();
	}
};

forward_list<int> variables::integers;
forward_list<string> variables::strings;
forward_list<bs_list> variables::lists;
forward_list<bs_object> variables::objects;

class variable {
public:
	int type;
	void* value;

	variable(int* null) {
		type = 0;
	}


	int* getInt() {return (int*)value;}
	void get(int** v) {*v = (int*)value;}
	void set(int v) {*(int*)value = v;}
	variable(int v) {
		this->type = 1;
		this->value = variables::addInt(&v);
	}


	string* getStr() {return (string*)value;}
	void get(string** v) {*v = (string*)value;}
	void set(string v) {*(string*)value = v;}
	variable(string v) {
		this->type = 2;
		this->value = variables::addStr(&v);
	}


	bs_list* getLst() {return (bs_list*)value;}
	void get(bs_list** v) {*v = (bs_list*)value;}
	void set(bs_list v) {*(bs_list*)value = v;}
	variable(bs_list v) {
		this->type = 3;
		this->value = variables::addLst(&v);
	}


	bs_object* getObj() {return (bs_object*)value;}
	void get(bs_object** v) {*v = (bs_object*)value;}
	void set(bs_object v) {*(bs_object*)value = v;}
	variable(bs_object v) {
		this->type = 4;
		this->value = variables::addObj(&v);
	}


	var get(int i) {
		switch(type) {
			case 2:
				return new variable((*(string*)(value))[i]);
			break;

			case 3:
				return (*(bs_list*)value)[i];
			break;

			default:
				//err(0,"%s don't support random access",{"undefined","integers"}[type]);
			break;
		}
		return new variable(nullptr);
	}

	var get(string key) {

		switch(type) {
			case 4:
				return (*(bs_object*)value)[key];
			break;

			default:
				//err(0,"%s don't support random access",{"undefined","integers","strings"}[type]);
			break;
		}

		return new variable(nullptr);
	}

};


int main() {

	int* temp1;
	var a = new variable(15);
	a->get(&temp1);
	cout << *temp1 << endl;

	a->set(70);
	cout << *a->getInt() << endl;


	var list = new variable(bs_list({new variable("Hello "), new variable("World!")}));
	cout << *list->get(0)->getStr() << endl;


	bs_object obj;
	obj["f"] = new variable(555);
	var vobj = new variable(obj);
	cout << *vobj->get("f")->getInt() << endl;

	return 0;
}
