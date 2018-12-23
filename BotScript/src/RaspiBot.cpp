
#if ISBOT

#    include <python2.7/Python.h>

#    define CUSTOM_BEGIN
#    include "RaspiBot.hpp"

#    define BEGIN(...) BEGIN_1("RaspiBot::", __func__, __VA_ARGS__)
#    define END(...) END_1("RaspiBot::", __func__, __VA_ARGS__)


namespace RaspiBot {
    PVar toBSVar(PyObject *v);
    PyObject *toPyVar(PVar &v);

    PyObject *pModule, *pFunc;
    PVar res     = NULL;
    bool freeing = false;

    PVar &getRes() {
        return res;
    }

    bool Init() {
        BEGIN();
        PyObject *pName;
        Py_Initialize();

        PyObject *sys  = PyImport_ImportModule("sys");
        PyObject *path = PyObject_GetAttrString(sys, "path");
        PyList_Append(path, PyUnicode_FromString("res"));

        pName   = PyString_FromString("RaspiBot");
        pModule = PyImport_Import(pName);
        Py_DECREF(pName);

        if (pModule == NULL) {
            PyErr_Print();
            error_exit("Failed to load \"RaspiBot\"");
            END();
            return true;
        }

        pFunc = PyObject_GetAttrString(pModule, "callMethod");
        res   = incRef(newNil());

        if (pFunc == NULL || !PyCallable_Check(pFunc)) {
            if (PyErr_Occurred()) PyErr_Print();
            error_exit("Couldn't find def \"callMethod\"");
            END();
            return true;
        }

        END();
        return false;
    }

    bool Call(const char *func, var_lst args) {
        BEGIN("func=%s,args=%s", func, TOSTR(args));
        PyObject *pArgs, *pValue;
        int argc = args.size(), i;

        pArgs = PyTuple_New(argc + 1);

        // method name
        pValue = PyString_FromString(func);
        PyTuple_SetItem(pArgs, 0, pValue);

        for (i = 0; i < argc; ++i) {
            pValue = toPyVar(args[i]);
            if (!pValue) {
                Py_DECREF(pArgs);
                error_exit(
                    "couldn't convert argument %i for \"%s\": %s", i, func,
                    TOSTR(args[i]));
                END();
                return true;
            }
            PyTuple_SetItem(pArgs, i + 1, pValue);
        }

        pValue = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);

        if (pValue != NULL) {
            if (res == NULL)
                res = incRef(toBSVar(pValue));
            else
                REPVAR(res, toBSVar(pValue));
            Py_DECREF(pValue);
        } else {
            PyErr_Print();
            error_exit("Call of \"%s\" failed", func);
            END();
            return true;
        }
        END();
        return false;
    }

    PyObject *toPyVar(PVar &v) {
        switch (getType(v)) {
        case T_NIL: return Py_None;
        case T_BIN: return getBin(v) ? Py_True : Py_False;
        case T_INT: return PyLong_FromLong(getInt(v));
        case T_PIN: return PyLong_FromLong(getPin(v));
        case T_FLT: return PyFloat_FromDouble(getFlt(v));
        case T_STR: return PyString_FromString(getStr(v).c_str());

        case K_NIL: return Py_None;
        case K_TRU: return Py_True;
        case K_FLS: return Py_False;

        default:
            error_exit(
                "cannot pass %s to python variable", typeName(getType(v)));
            return NULL;
        }
    }

    PVar toBSVar(PyObject *v) {
        if (PyInt_CheckExact(v))
            return newInt(PyInt_AsLong(v));
        else if (PyLong_CheckExact(v))
            return newInt(PyLong_AsLong(v));
        else if (PyString_CheckExact(v))
            return newStr(PyString_AsString(v));
        else if (PyFloat_CheckExact(v))
            return newFlt(PyFloat_AsDouble(v));
        else if (v == Py_True)
            return newBin(true);
        else if (v == Py_False)
            return newBin(false);
        else if (v == Py_None)
            return newNil();
        else
            error_exit(
                "cannot convert %s to BotScript variable", Py_TYPE(v)->tp_name);
        return newNil();
    }

    void Free() {
        BEGIN();
        freeing = true;
        if (pFunc) {
            Call("cleanup", {});
            if (res) decRef(res);
            Py_DECREF(pFunc);
        }
        if (pModule) Py_DECREF(pModule);
        Py_Finalize();
        END();
    }
} // namespace RaspiBot

#endif // ISBOT
