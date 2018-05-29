
//$ g++ -std=c++11 -O3 -c include.cpp

#include "include.hpp"
#include "builtins.hpp"
#include "macros.hpp"

#include <math.h> //pow

// copied (and modified) from stackoverflow, "ausercomment"
double stod(string s, uint8_t radix) {
    BEGIN("string s=\"%s\",uint8_t=%i", s.c_str(), radix);
    double n  = 0;
    uint8_t x = s.size(), y = 0;
    while (x)
        if (!(s[--x] ^ '.'))
            n /= pow(radix, s.size() - 1 - x), y += s.size() - x;
        else
            n +=
                ((s[x] - (s[x] <= '9' ? '0' : '7')) *
                 pow(radix, s.size() - 1 - x - y));
    END();
    return n;
}

// replace all in string
void replace(string* str, string src, string ovr) {
    BEGIN(
        "string*str=\"%s\",string src=\"%s\",string ovr=\"%s\"", str->c_str(),
        src.c_str(), ovr.c_str());
    int start = 0;
    while ((start = str->find(src, start)) + 1) {
        str->replace(start, src.length(), ovr);
        start += ovr.length(); // case 'ovr' is substring of 'src'
    }
    END();
}

// delay in milliseonds
void delay(int time) {
    BEGIN("int time=%i", time);
    time = clock() + time * 1000;
    while (clock() < time)
        ;
    END();
}

// replace some \ placeholders
void format(string* s) {
    BEGIN("string*s=\"%s\"", s->c_str());
    replace(s, "\\n", "\n");
    replace(s, "\\t", "\t");
    replace(s, "\\033", "\033");
    replace(s, "\\\\", "\\"); // must be last!!
    END();
}

// converts scope string to term
var_lst toFunction(string::iterator* c, string::iterator end) {
    BEGIN("string::it*c=\"%c\",string::it end", **c);
    var_lst block(0);
    while (**c != ')' && **c != '}' && *c < end) {
        switch (**c) {
            case '"': {
                string word = "";
                while (*++*c != '"' && *c < end) word += **c;
                printf("%s: %s\n", word.c_str(), typeName(T_STR));
            } break;
            case '(':
            case '{': {
                var_lst scope;
                uint8_t type = **c == '(' ? T_TRM : T_FNC;
                ++*c;
                scope = toFunction(c, end);
                printf("%s: %s\n", TLst(scope).toStr().c_str(), typeName(type));
            } break;
            default:
                string word = "";
                if (symbols.find(**c) + 1) do
                        word += **c;
                    while (symbols.find(*++*c) + 1 && *c < end);
                else
                    do
                        word += **c;
                    while (!(symbols.find(*++*c) + 1) && *c < end);
                /*
                if (Builtins::exists(word))
                    printf("%s: %s\n", word.c_str(), "builtin");
                else
                */
                printf("%s: %s\n", word.c_str(), typeName(T_VAR));
                continue;
        }
        ++*c;
    }
    // END("%s", Variables::sstringify(block));
    return block;
}

// start recursive conversation from string to term
var_lst toCode(string* code) {
    BEGIN("string*code=\"%s\"", code->c_str());
    string::iterator c = code->begin();
    END();
    return toFunction(&c, code->end());
}

// ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore) {
    BEGIN("char*path=\"%s\",bool ignore=%i", path, ignore);
    // file buffer
    FILE* f = fopen(path, "r");

    if (!f) error("file \"%s\" does not exist!", path);

    // character from file
    int c;
    string content = "";
    if (ignore) {
        while ((c = fgetc(f)) != EOF) {
            // whitespace
            if (whitespace.find(c) < 3) {
                if (c == '\n' && content[content.size() - 1] != ';')
                    content += ';';
                continue;
            }

            // strings
            if (c == '"') {
                do
                    content += c;
                while ((c = fgetc(f)) != EOF && c != '"');
                if (c == EOF) break;
            }

            // comments
            if (c == '/') {
                c = fgetc(f);
                // line
                if (c == '/') do
                        c = fgetc(f);
                    while (c != EOF && c != '\n');
                // block
                else if (c == '*') {
                    bool brk = false;
                    while ((c = fgetc(f)) != EOF) {
                        if (brk && c == '/') break;
                        brk = (c == '*');
                    }
                }
                continue;
            }
            content += c;
        }
    } else
        while ((c = fgetc(f)) != EOF) content += c;
    END();
    return content;
}
