
#ifndef _VARIABLES_HPP_
#define _VARIABLES_HPP_

#include <string>
#include <vector>        //alternative array based list container
#include <forward_list>  //alternative linked list container
#include <unordered_map> //k:v object container

#define uint8_t unsigned char

	//type id's
#define T_NIL 0
#define T_VAR 1
#define T_INT 2
#define T_FLT 3
#define T_STR 4
#define T_LST 5
#define T_OBJ 6
#define T_PIN 7
#define T_TRM 8
#define T_FNC 9

	//types
#define var Variable*
#define var_int long
#define var_flt double
#define var_str string
#define var_lst vector<var>
#define var_obj unordered_map<string, var>

	//macros for getting var instance values
	//-> prevents bigger code by defining functions
#define getIntP(var) ((var_int*)var->value)
#define getInt(var) (*(var_int*)var->value)
#define getFltP(var) ((var_flt*)var->value)
#define getFlt(var) (*(var_flt*)var->value)
#define getStrP(var) ((var_str*)var->value)
#define getStr(var) (*(var_str*)var->value)
#define getLstP(var) ((var_lst*)var->value)
#define getLst(var) (*(var_lst*)var->value)
#define getObjP(var) ((var_obj*)var->value)
#define getObj(var) (*(var_obj*)var->value)

#define V_NULL new Variable(nullptr, T_NIL, false)

using namespace std;

	//link Variable class so that the Variables namespace knows it
class Variable;

namespace Variables {

		//returns the name of the type of a Variable
	const char* getType(uint8_t t);

		//recursive _stringify
	void _stringify(var value, string* str, bool mark = true);

		//returns stringified Variable
	string stringify(var value, bool mark = true);
	string stringify(char** list, uint len);
	string stringify(int value);
	string stringify(int* value, uint len);
	string stringify(var_lst value, const char brackets[] = "[]");
	string stringify(var_obj value);
	#define sstringify(...) stringify(__VA_ARGS__).c_str()


//	var parse(string str);

/*
 * forward_list<type> saves the Variable values
 * overloaded create functions for different types
 * the add<Type> function pushes Variables into the specific forward_list
 *  -> returns the Variable instance
 */
		//undefined / null Variables
	var create(void* value, uint8_t type = T_NIL, bool builtin = false);

		//integers
 	var create(var_int value, uint8_t type = T_INT, bool builtin = false);
	var_int* addInt(var_int* v);

		//integers
 	var create(var_flt value, uint8_t type = T_FLT, bool builtin = false);
	var_flt* addFlt(var_flt* v);

		//strings
	var create(var_str value, uint8_t type = T_STR, bool builtin = false);
	var_str* addStr(var_str* v);

		//lists
	var create(var_lst value, uint8_t type = T_LST, bool builtin = false);
	var_lst* addLst(var_lst* v);

		//objects
	var create(var_obj value, uint8_t type = T_OBJ, bool builtin = false);
	var_obj* addObj(var_obj* v);

		//link free function
	void free();
};

	//Variable object
class Variable {
public:
	uint8_t type; //type id
	//uint8_t spec; //special information (not used yet)
	bool builtin;
	void* value;  //pointer to Variable value

		//constructor: add v to value list in Variables::
		//and define the pointer to value
	Variable(void* v, uint8_t type, bool builtin = false);

		//destructor: search Variable value in Variables::Variables and erase it
	~Variable();

		//assign value v to the Variable
	void set(void* v, uint8_t type = 0);
/*
	var keys();

	var values();*/

		//random access for strings and lists
	var at(int i);

		//random access for objects
	var at(string key);

		//creates a new Variable instance
	var copy();

		//assign Variable instance to Variable
	void operator = (var v);

		//add Variable instance to Variable
	void operator += (var v);

		//substract Variable instance from integer Variable
	void operator -= (var v);
};

#endif //_T_VARIABLES_CPP_

/*
#if MAIN == _T_VARIABLES_CPP_

int main() {

	var a = Variables::create(5);
	cout << getInt(a) << endl;

	var list = Variables::create(var_lst( {
		Variables::create("Hello "),
		Variables::create("World! "),
		Variables::create(123)
	} ));
	cout << *list->at(0)->getStr() << getStr(list->at(1)) << getInt(list->at(2)) << endl;
	*list += list; //list->getLst()->insert(list->getLst()->end(), list->getLst()->begin(), list->getLst()->end());

	*list->at(2) = a; //reference
	int temp = 15;
	a->set(&temp);
	cout << *list->at(2)->getInt() << endl;

	temp = 20;
	*list->at(2) = a->copy(); //real copy
	a->set(&temp);
	cout << *list->at(2)->getInt() << endl;

	var_obj obj = {
		{"a", Variables::create(254)},
		{"b", Variables::create("eof") }
	};
	obj["f"] = Variables::create("reference");
	var vobj = Variables::create(&obj, T_OBJ);
	(*vobj->getObj())["list"] = list->copy();

	cout << Variables::stringify(vobj) << endl;

	Variables::free();
	return 0;
}

#endif //MAIN == _T_VARIABLES_CPP_
*/
