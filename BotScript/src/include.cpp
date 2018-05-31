
//$ g++ -std=c++11 -O3 -c include.cpp

#include "builtins.hpp"

bool isSymbol(char c) {
    return (symbols.find(c) + 1);
}

bool isWhitespace(char c) {
    return (whitespace.find(c) + 1);
}

bool isOperator(char c) {
    return (operators.find(c) + 1);
}

bool isOperator(string s) {
    if (s[1] == '=') return isOperator(s[0]);
    return false;
}

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
string format(string& s) {
    BEGIN("string*s=\"%s\"", s->c_str());
    replace(&s, "\\n", "\n");
    replace(&s, "\\t", "\t");
    replace(&s, "\\033", "\033");
    replace(&s, "\\\\", "\\"); // must be last!!
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
