
#include "builtins.hpp"
#include "interpret.hpp"

#define INF (unsigned)!((var_flt)0)

char* dtos2(long double num, uint8_t rad) {
    // BEGIN("num=%Lf,r=%i", num, rad);

    // check for zero
    if (!num) {
        // END("0.0");
        return strdup("0.0");
    }

    // check for infinite
    if (num == num + 1) {
        if (num > 0) {
            // END("inf");
            return strdup("inf");
        } else {
            // END("-inf");
            return strdup("-inf");
        }
    }

    // check for nan
    if (num != num) {
        if (num > 0) {
            // END("nan");
            return strdup("nan");
        } else {
            // END("-nan");
            return strdup("-nan");
        }
    }

    static const char* digits =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    int8_t digit = 0, i = 0, end = 0, zeros = 0, nines = 0;
    char res[70] = "", *begin, *d = res;
    bool isfloat = false;
    long double tnum;

    // check negative
    if (num < 0) {
        *d++ = '-';
        num  = -num;
    }

    // add radix
    if (rad != 10) {
        if (rad < 2 || rad > 62) error_exit("%Lf invalid radix %i", num, rad);
        if (rad > 9) *d++ = '0' + (rad / 10) % 10;
        *d++ = '0' + rad % 10;
        *d++ = 'x';
    }

    // add 0 for 0.x or 9999.. -> 10000..
    *d++ = '0';

    // count decimals with target radix
    tnum = num;
    while (tnum >= rad) {
        tnum /= rad;
        i++;
    }
    end = i - 64;

    // shift num to 0.x
    while (num >= rad) num /= rad;

    begin = d;

    for (; i >= end && (i > -2 || (double)num - trunc(num) > 0); i--, end++) {
        // write digit
        *d++ = digits[digit = (long)num % rad];
        // shift num
        num = (num - digit) * rad;

        if (isfloat) {
            // count zeros
            if (!digit) {
                nines = 0;
                if (++zeros >= 7) break;
            } else
                zeros = 0;

            // count nines of radix (rad - 1)
            if (digit == rad - 1) {
                // if over 7 nines - round
                if (++nines > 5) {
                    char* c = d - nines - 1;
                    if (*c == '.') {
                        c[1] = '0';
                        c[2] = 0;
                        c--;
                        while (*c == digits[rad - 1]) *c-- = '0';
                    } else
                        c[1] = 0;

                    switch (*c) {
                        case '9': *c = 'A'; break;
                        case 'z': *c = 'a'; break;
                        default: (*c)++;
                    }

                    break;
                }
            } else
                nines = 0;
        }

        // add floating point
        if (!i) {
            *d++    = '.';
            zeros   = -1; // leave one zero for integers
            isfloat = true;
        }
    }

    // crop zeros
    if (zeros) d[-zeros] = 0;

    // remove zero from begin
    if (begin[-1] == '0') {
        while (begin-- - 1 > res) { *begin = begin[-1]; }
        begin++;
    } else
        begin = res;

    // END("\"%s\"", begin);
    return strdup(begin);
}

long double stod2(const char* s) {
    // BEGIN("s=\"%s\"", s);

    static char digits[] =
        "------------------------------------------------"
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\t-------"
        "\n\x0b\x0c\r\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b"
        "\x1c\x1d\x1e\x1f !\"#------"
        "$%&\'()*+,-./0123456789:;<=";

    long double pot, res = 0;
    bool isneg   = false;
    int16_t d    = -1;
    uint16_t rad = 0;
    const char* c;

    if (*s == '-' || *s == '+') isneg = *s++ == '-';
    c = s;

    while (*c && *c != 'x') rad = rad * 10 + *c++ - '0';

    if (!*c) { //?
        if (s[0] == '0' && s[1] == 'b')
            s += rad = 2;
        else
            rad = 10;
    } else if (!rad) {
        rad = 16;
        s   = c + 1;
    } else if (rad < 2 || rad > 62) {
        error_exit("%s invalid radix %i", s, rad);
    } else {
        s = c + 1;
    }


    while (*s == '0') s++;
    c = s;
    while (*c && *c != '.') c++, d++;
    pot = pow(rad, d);
    while (*s) {
        if (*s != '.') {
            res += digits[(unsigned)*s] * pot;
            pot /= rad;
        }
        s++;
    }

    if (isneg) res = -res;
    // END("%Lf", res);
    return res;
}

// delay in milliseonds
void delay(long double time) {
    BEGIN("time=%Lf", time);
    time = clock() + round(time * 1000);
    while (clock() < time)
        ;
    END();
}

// replace all in string
string replace(string str, string src, string ovr) {
    /*BEGIN(
        "string*str=\"%s\",string src=\"%s\",string ovr=\"%s\"", str.c_str(),
        src.c_str(), ovr.c_str());*/
    int start = 0;
    while ((start = str.find(src, start)) + 1) {
        str.replace(start, src.size(), ovr);
        start += ovr.size(); // case 'ovr' is substring of 'src'
    }
    // END("%s", str.c_str());
    return str;
}

#define REPLACE1(a) replace(s, a)
#define REPLACE2(a, b) replace(REPLACE1(REP(a)), b)
#define REPLACE3(a, b, c) replace(REPLACE2(REP(a), REP(b)), c)
#define REPLACE4(a, b, c, d) replace(REPLACE3(REP(a), REP(b), REP(c)), d)
#define REPLACE5(a, b, c, d, e) \
    replace(REPLACE4(REP(a), REP(b), REP(c), REP(d)), e)
#define REP(a, b) a COMMA b

// replace some escape sequences
string unescape(string s) {
    // BEGIN("string*s=\"%s\"", s.c_str());
    if (!s.empty()) {
        s = REPLACE5(
            REP("\\\"", "\""), REP("\\n", "\n"), REP("\\t", "\t"),
            REP("\\033", "\033"), REP("\\\\", "\\") /*must be last*/);
    }
    // END();
    return s;
}

// undo replacement of escape sequences
string escape(string s) {
    // BEGIN("string*s=\"%s\"", s.c_str());
    if (!s.empty()) {
        s = REPLACE5(
            REP("\\", "\\\\"), /* must be first*/
            REP("\"", "\\\""), REP("\n", "\\n"), REP("\t", "\\t"),
            REP("\033", "\\033"));
    }
    // END();
    return s;
}

// ignore == true -> ignore useless whitespace
string readFile(const char* path, bool ignore) {
    BEGIN("char*path=\"%s\",bool ignore=%i", path, ignore);
    // file buffer
    FILE* f = fopen(path, "r");

    if (!f) error("file \"%s\" does not exist!", path);

    // character from file
    int c, lc = 0;
    string content = "";
    if (ignore) {
        while ((c = fgetc(f)) != EOF) {
            // whitespace
            if (isspace(c)) {
                do
                    c = fgetc(f);
                while (c != EOF && isspace(c));

                // keep two words separated
                if (isAlnum(lc) && isAlnum(c)) content += ' ';
            }

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
                    do { c = fgetc(f); } while (c != EOF && c != '\n');
                }
                // block
                else if (c == '*') {
                    bool brk = false;
                    while ((c = fgetc(f)) != EOF) {
                        if (brk && c == '/') break;
                        brk = (c == '*');
                    }
                }
                lc = c;
                continue;
            }

            if (c == EOF) break;
            content += lc = c;
        }
    } else {
        while ((c = fgetc(f)) != EOF) content += c;
    }
    END();
    return content;
}
