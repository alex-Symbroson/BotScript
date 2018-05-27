
//$ g++ -std=c++11 -O3 -c Variables.cpp

#include "variables.hpp"

#define CUSTOM_BEGIN
#include "macros.hpp"

#define BEGIN(...) BEGIN_1("Variables", __VA_ARGS__)
#define END(...) END_1("Variables", __VA_ARGS__)

namespace Variables {

    // list of instances of all created Variables
    vector<var> all;

    // returns the name of the type of a Variable
    const char* getType(uint8_t t) {
        BEGIN("uint8_t t=%i", t);
        END();
        switch (t) {
            case T_NIL: return "null";
            case T_VAR: return "name";
            case T_INT: return "integer";
            case T_FLT: return "float";
            case T_STR: return "string";
            case T_LST: return "list";
            case T_OBJ: return "object";
            case T_PIN: return "pin";
            case T_TRM: return "term";
            case T_FNC: return "function";
            default: return "unknown";
        }
    }

    // convert variable to string equivalent
    void _stringify(var value, string* str, bool mark, bool expand_func) {
        // 		BEGIN("var value,string*str=\"%s\",bool mark=%s", str->c_str(),
        // mark?"true":"false");
        if (value->builtin) {
            // printf("tried to stringify builtin");
            *str += " [builtin] ";
            return;
        }

        switch (value->type) {
            case T_PIN:
            case T_INT:
            case T_FLT:
                // BEGIN("_stringify case INT");
                if (value->type == T_PIN) *str += "Pin ";
                *str += to_string(getInt(value));
                break;

            case T_STR:
            case T_VAR:
                // BEGIN("_stringify case STR");
                if (value->type == T_VAR) *str += "Var ";

                if (mark)
                    *str += "\"" + getStr(value) + "\"";
                else
                    *str += getStr(value);
                break;

            case T_LST:
            case T_TRM: {
                // BEGIN("_stringify case LST");
                if (mark) {
                    if (value->type == T_LST)
                        *str += "[";
                    else
                        *str += "(";
                }

                var_lst::iterator first(getLst(value).begin());
                for (var val : getLst(value)) {
                    if (val != *first) *str += ",";
                    _stringify(val, str, mark, false);
                }

                if (mark) {
                    if (value->type == T_LST)
                        *str += "]";
                    else
                        *str += ")";
                }
            } break;

            case T_FNC: {
                // BEGIN("_stringify case FNC");
                if (expand_func) {
                    var_obj fnc = getObj(value);
                    *str += getStr(fnc["name"]) + "(" + stringify(fnc["args"]) +
                            ")" + "{" + stringify(fnc["content"]) + "}";
                } else
                    *str += getStr(getObj(value)["name"]);
            } break;

            case T_OBJ: {
                // BEGIN("_stringify case OBJ");
                *str += "[";
                var_obj::iterator first = getObj(value).begin();
                for (pair<string, var> kvp : getObj(value)) {
                    if (kvp.first != first->first) *str += ",";
                    *str += "\"" + kvp.first + "\":";
                    _stringify(kvp.second, str, mark);
                }
                *str += "]";
            } break;

            case T_NIL: *str += "null"; break;

            default: // T_NIL
                // BEGIN("_stringify case NIL");
                *str += "undefined";
        }
        // 		END()
    }

    // direct stringify function
    string stringify(var value, bool mark, bool expand_func) {
        // BEGIN("var value,bool mark");

        string s = "";
        _stringify(value, &s, mark, expand_func);

        // END();
        return s;
    }

    string stringify(char** list, uint len) {
        // BEGIN("char**list,uint len=%u",len);
        string s = "";
        while (len--)
            if (len)
                s = string(", ") + list[len] + s;
            else
                s = list[len] + s;
        // END();
        return s;
    }

    string stringify(int value) {
        // BEGIN("int");
        // END();
        return to_string(value);
    }

    string stringify(int* value, uint len) {
        // BEGIN("int*, uint");
        string s = "[";
        for (uint i = 0; i < len; i++)
            if (len)
                s += value[i];
            else
                s += value[i] + ", ";
        // END();
        return (s + ']');
    }

    string stringify(var_lst value, const char brackets[]) {
        // BEGIN("var_lst");
        string s = "";
        for (var v : value) s += stringify(v, false) + ", ";
        // END();
        return brackets[0] + s + brackets[1];
    }

    string stringify(var_obj value) {
        // BEGIN("var_obj value");
        string s = "[";
        for (pair<string, var> v : value)
            s += "\"" + v.first + "\":" + stringify(v.second, false) + ", ";
        s += ']';
        // END();
        return s;
    }
    /*
    var parse(string str) {
        return _parse(&str);
    }*/

    // note: when using create(*ptr, int) instead of
    // create(val) the .out file will be smaller
    // undefined / null
    var create(void* value, uint8_t type, bool builtin) {
        BEGIN(
            "void*,uint8 type=%i(%s),bool %i", type, Variables::getType(type),
            builtin);
        all.push_back(new Variable(value, type));
        END();
        return all.back();
    }

    // integers
    forward_list<var_int> integers;
    var create(var_int value, uint8_t type, bool builtin) {
        BEGIN(
            "var_int %li,uint8 type=%i(%s),bool builtin=%i", value, type,
            Variables::getType(type), builtin);
        all.push_back(new Variable(&value, type));
        END();
        return all.back();
    }
    var_int* addInt(var_int* v) {
        BEGIN("var_int* %li", *v);
        integers.push_front(*v);
        END();
        return &integers.front();
    }

    // floating points
    forward_list<var_flt> floats;
    var create(var_flt value, uint8_t type, bool builtin) {
        BEGIN(
            "flt value=%.15f,uint8 type=%i(%s),bool builtin=%i", value, type,
            Variables::getType(type), builtin);
        all.push_back(new Variable(&value, type));
        END();
        return all.back();
    }
    var_flt* addFlt(var_flt* v) {
        BEGIN("var_flt* %.15f", *v);
        floats.push_front(*v);
        END();
        return &floats.front();
    }

    // strings
    forward_list<var_str> strings;
    var create(var_str value, uint8_t type, bool builtin) {
        BEGIN(
            "var_str %s, uint8_t type=%i(%s),bool builtin=%i", value.c_str(),
            type, Variables::getType(type), builtin);
        all.push_back(new Variable(&value, type));
        END();
        return all.back();
    }
    var_str* addStr(var_str* v) {
        BEGIN("var_str* %s", v->c_str());
        strings.push_front(*v);
        END();
        return &strings.front();
    }

    // lists
    forward_list<var_lst> lists;
    var create(var_lst value, uint8_t type, bool builtin) {
        BEGIN(
            "var_lst value,uint8 type=%i(%s),bool builtin=%i", type,
            Variables::getType(type), builtin);
        all.push_back(new Variable(&value, type));
        END();
        return all.back();
    }
    var_lst* addLst(var_lst* v) {
        BEGIN("var_lst*");
        lists.push_front(*v);
        END();
        return &lists.front();
    }

    // objects
    forward_list<var_obj> objects;
    var create(var_obj value, uint8_t type, bool builtin) {
        BEGIN(
            "var_obj value,uint8 type=%i(%s),bool bultin=%i", type,
            Variables::getType(type), builtin);
        all.push_back(new Variable(&value, type));
        END();
        return all.back();
    }
    var_obj* addObj(var_obj* v) {
        BEGIN("var_obj*");
        objects.push_front(*v);
        END();
        return &objects.front();
    }

    // free space -> deletes ALL Variables
    void free() {
        BEGIN("");
        while (all.size()) delete all.front();
        END();
    }
} // namespace Variables

/* * * * * * * * * *
 * class Variable  *
 * * * * * * * * * */

Variable::Variable(void* v, uint8_t type, bool builtin) {
    BEGIN(
        "void* v,uint8 type=%i(%s),bool builtin=%i", type,
        Variables::getType(type), builtin);
    this->type    = type;
    this->builtin = builtin;
    if (!type) return;

    switch (type) {
        case T_INT:
        case T_PIN: value = Variables::addInt((var_int*)v); break;
        case T_FLT: value = Variables::addFlt((var_flt*)v); break;
        case T_STR:
        case T_VAR: value = Variables::addStr((var_str*)v); break;
        case T_LST:
        case T_TRM: value = Variables::addLst((var_lst*)v); break;
        case T_OBJ:
        case T_FNC: value = Variables::addObj((var_obj*)v); break;
    }
    END();
}

// destructor: search Variable value in Variables and erase it
Variable::~Variable() {
    BEGIN("");
    vector<var>::iterator it = Variables::all.begin();
    while (*it != this) it++;
    Variables::all.erase(it);
    // cout << "deleted " << this << " " << stringify(this) << endl;
    END();
}

// assign value v to the Variable
void Variable::set(void* v, uint8_t type) {
    BEGIN("void*v,uint8 type=%i(%s)", type, Variables::getType(type));

    if (!type) type = this->type;

    // Variable and v has different types -> incompatible types error
    if (this->type && this->type != type)
        err_ict(Variables::getType(this->type), Variables::getType(type));

    // assign v to Variable
    switch (type) {
        case T_NIL: Variable(v, type); break;
        case T_INT:
        case T_PIN: *(var_int*)value = *(var_int*)v; break;
        case T_FLT: *(var_flt*)value = *(var_flt*)v; break;
        case T_STR: *(var_str*)value = *(var_str*)v; break;
        case T_LST:
        case T_TRM: *(var_lst*)value = *(var_lst*)v; break;
        case T_OBJ:
        case T_FNC: *(var_obj*)value = *(var_obj*)v; break;
    }
    END();
}
/*
var Variable::keys() {
    BEGIN("");
    if(type == T_OBJ) {
        var_lst keys;
        for(pair<string, var> kvp : *getObj()) keys.push_back(new
Variable(kvp.first)); return new Variable(keys);
    }
    err_imu(getType(type), "keys");
    END();
}

var Variable::values() {
    BEGIN();
    if(type == T_OBJ) {
        var_lst vals;
        for(pair<string,var> kvp : *getObj()) vals.push_back(kvp.second);
        return new Variable(vals);
    }
    err_imu(getType(type), "values");
    END();
}*/

// random access for strings and lists
var Variable::at(int i) {
    BEGIN("int i=%i", i);

    if (type == T_LST)
        return (*(var_lst*)value)[i];
    else if (type == T_FLT) {
        char c = ((var_str*)value)->at(i);
        return Variables::create(string(&c));
    }
    error("%ss don't support random access", Variables::getType(type));
    END();
    return V_NULL;
}

// random access for objects
var Variable::at(string key) {
    BEGIN("\"%s\"", key.c_str());

    if (type == T_OBJ) return (*(var_obj*)value)[key];
    error("%s don't support random access", Variables::getType(type));
    END();
    return V_NULL;
}

// creates a new Variable instance
var Variable::copy() {
    BEGIN();
    END();
    return Variables::create(value, type);
}

// assign Variable instance to Variable
void Variable::operator=(var v) {
    BEGIN("var v");
    if (type == v->type)
        value = v->value;
    else
        err_ict(Variables::getType(type), Variables::getType(v->type));
    END();
}

// add Variable instance to Variable
void Variable::operator+=(var v) {
    BEGIN("var v");
    switch (type) {
        case T_NIL:
            err_iop(
                "undefined",
                "+="); // can't add smth to null -> invalid operator
        case T_INT:
            if (v->type == T_INT)
                *(var_int*)value += *(var_int*)v->value;
            else
                goto err;
            break;

        case T_FLT:
            if (v->type == T_FLT)
                *(var_flt*)value += *(var_flt*)v->value;
            else
                goto err;
            break;

        case T_STR:
            if (v->type == T_STR)
                *(var_str*)value += *(var_str*)v->value;
            else
                goto err;
            break;

        case T_LST:
            if (v->type == T_LST)
                ((var_lst*)value)
                    ->insert(
                        ((var_lst*)value)->end(), getLst(v).begin(),
                        getLst(v).end());
            else
                goto err;
            break;

        case T_OBJ:
            if (v->type == T_OBJ)
                ((var_obj*)value)->insert(getObj(v).begin(), getObj(v).end());
            else
                goto err;
            break;

        default: err_iop(Variables::getType(type), "+=");
    }
    END();
    return;
err:
    err_ict(Variables::getType(type), Variables::getType(v->type));
}

// substract Variable instance from integer Variable
void Variable::operator-=(var v) {
    BEGIN("");
    if (type == T_INT) {
        if (v->type == T_INT)
            *(var_int*)value += *(var_int*)v->value;
        else
            err_ict("integer", Variables::getType(v->type));
    } else
        err_iop(Variables::getType(type), "-=");
    END();
}

// void _parse(...) {}
