
#include "builtins.hpp"

bool isSymbol(char c) {
    // BEGIN("%c", c);
    // characters interpreted as symbols
    static const string s_symbols("/\\():,.;<>~!@#$%^&*|+=[]{}`?-…");
    // END();
    return (s_symbols.find(c) + 1);
}

bool isWhitespace(char c) {
    // BEGIN("%c", c);
    // whitespace characters
    static const string s_whitespace(" \t\r\n");
    // END();
    return (s_whitespace.find(c) + 1);
}

bool isOperator(char c) {
    // BEGIN("%c", c);
    // characters interpreted as operators (op and op=)
    static const string s_operators("<>!%^&*|+=");
    // END();
    return (s_operators.find(c) + 1);
}

bool isOperator(string s) {
    // BEGIN("%s", s.c_str());
    END();
    return operators.find(s) != operators.end();
}

// delay in milliseonds
void delay(int time) {
    BEGIN("int time=%i", time);
    time = clock() + time * 1000;
    while (clock() < time)
        ;
    END();
}

// replace all in string
string replace(string str, string src, string ovr) {
    BEGIN(
        "string*str=\"%s\",string src=\"%s\",string ovr=\"%s\"", str.c_str(),
        src.c_str(), ovr.c_str());
    int start = 0;
    while ((start = str.find(src, start)) + 1) {
        str.replace(start, src.size(), ovr);
        start += ovr.size(); // case 'ovr' is substring of 'src'
    }
    END("-> %s", str.c_str());
    return str;
}

#define REPLACE1(a) replace(s, a)
#define REPLACE2(a, b) replace(REPLACE1(REP(a)), b)
#define REPLACE3(a, b, c) replace(REPLACE2(REP(a), REP(b)), c)
#define REPLACE4(a, b, c, d) replace(REPLACE3(REP(a), REP(b), REP(c)), d)
#define REP(a, b) a COMMA b

// replace some escape sequences
string unescape(string s) {
    BEGIN("string*s=\"%s\"", s.c_str());
    if (s.size()) {
        s = REPLACE4(
            REP("\\n", "\n"), REP("\\t", "\t"), REP("\\033", "\033"),
            REP("\\\\", "\\") /*must be last*/);
    }
    return s;
    END();
}

// undo replacement of escape sequences
string escape(string s) {
    BEGIN("string*s=\"%s\"", s.c_str());
    if (s.size()) {
        s = REPLACE4(
            REP("\\", "\\\\"), /* must be first*/
            REP("\n", "\\n"), REP("\t", "\\t"), REP("\033", "\\033"));
    }
    return s;
    END();
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
            while (c != EOF && isWhitespace(c))
                c = fgetc(f);

            // strings
            if (c == '"') {
                do {
                    content += c;
                    // don't change the cond. order
                } while ((c = fgetc(f)) != EOF && c != '"');
            }

            // comments
            if (c == '/') {
                c = fgetc(f);
                // line
                if (c == '/') {
                    do {
                        c = fgetc(f);
                    } while (c != EOF && c != '\n');
                }
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

            if (c == EOF)
                break;
            else
                content += c;
        }
    } else {
        while ((c = fgetc(f)) != EOF)
            content += c;
    }
    END();
    return content;
}
