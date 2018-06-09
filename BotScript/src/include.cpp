
#include "builtins.hpp"

bool isSymbol(char c) {
    BEGIN();
    END();
    return (symbols.find(c) + 1);
}

bool isWhitespace(char c) {
    BEGIN();
    END();
    return (whitespace.find(c) + 1);
}

bool isOperator(char c) {
    BEGIN();
    END();
    return (operators.find(c) + 1);
}

bool isOperator(string s) {
    BEGIN();
    END();
    if (s[1] == '=') return isOperator(s[0]);
    return false;
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
        str.replace(start, src.length(), ovr);
        start += ovr.length(); // case 'ovr' is substring of 'src'
    }
    END("-> %s", str.c_str());
    return str;
}

// replace some escape sequences
string format(string s) {
    BEGIN("string*s=\"%s\"", s.c_str());
    if (s.size()) {
        s = replace(s, "\\n", "\n");
        s = replace(s, "\\t", "\t");
        s = replace(s, "\\033", "\033");
        s = replace(s, "\\\\", "\\"); // must be last
    }
    return s;
    END();
}

// undo replace of escape sequences
string unformat(string s) {
    BEGIN("string*s=\"%s\"", s.c_str());
    if (s.size()) {
        s = replace(s, "\\", "\\\\"); // must be first
        s = replace(s, "\n", "\\n");
        s = replace(s, "\t", "\\t");
        s = replace(s, "\033", "\\033");
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
            while (c != EOF && whitespace.find(c) + 1) {
                // if (c == '\n' && content[content.size() - 1] != ';')
                //     content += ';';
                c = fgetc(f);
            }
            if (c == EOF) break;

            // strings
            if (c == '"') {
                do {
                    content += c;
                    // don't change the cond. order
                } while ((c = fgetc(f)) != EOF && c != '"');
                if (c == EOF) break;
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
            if (c != EOF) content += c;
        }
    } else {
        while ((c = fgetc(f)) != EOF)
            content += c;
    }
    END();
    return content;
}
