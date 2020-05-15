#ifndef COMPACT_LEXER
#define COMPACT_LEXER

#include <string>
#include "Compact_State.hpp"
#include "Compact_Token.hpp"
#include "Compact_Object.hpp"

std::vector<cmp_token*>* cmpL_lexer(cmp_state* C, std::string& strng) {
    const char* str = strng.c_str();
    cmp_ttype t = cmp_tnull;
    std::string* z = new std::string("");
    std::vector<cmp_token*>* ret = new std::vector<cmp_token*>();

    for(unsigned int i = 0; i < strlen(str) + 1; ) {
        switch(t) {
        case cmp_tnull: {
            if(isdigit(str[i])) {
                t = cmp_tint; // there is nothing else it could be
                *z += str[i];
            } else if(isalpha(str[i])) {
                t = cmp_tidentifier; // there is nothing else it could be, again
                *z += str[i];
            } else {
                switch(str[i]) {
                case '"': { // a new string
                    t = cmp_tstring;
                    *z += str[i];
                    break;
                }
                case '+': {
                    ret->push_back(new cmp_token(cmp_top_plus));
                    break;
                }
                case '-': {
                    ret->push_back(new cmp_token(cmp_top_minus));
                    break;
                }
                case '*': {
                    ret->push_back(new cmp_token(cmp_top_multiply));
                    break;
                }
                case '/': {
                    ret->push_back(new cmp_token(cmp_top_divide));
                    break;
                }
                case '=': {
                    ret->push_back(new cmp_token(cmp_top_equals));
                    break;
                }
                default: {
                    break;
                }
                }
            }
            break;
        }
        case cmp_tint: {
            if(isdigit(str[i])) {
                *z += str[i];
            } else {
                // we obviously broke the integer, we're done
                int* v = new int(std::stoi(*z));
                ret->push_back(new cmp_token(new cmp_object(v)));
                *z = "";
                t = cmp_tnull;
                i--;
            }
            break;
        }
        case cmp_tstring: {
            if(str[i] != '"' && str[i] != '\0') {
                *z += str[i];
            } else {
                // we reached another quote, we're done
                std::string* v = new std::string(*z);
                v->erase(0,1);
                ret->push_back(new cmp_token(new cmp_object(v)));
                *z = "";
                t = cmp_tnull;
            }
            break;
        }
        case cmp_tidentifier: {
            if(isalnum(str[i])) {
                *z += str[i];
            } else {
                // it definitely can't be an identifier at this point
                std::string* v = new std::string(*z);
                ret->push_back(new cmp_token(t,new cmp_object(v)));
                *z = "";
                t = cmp_tnull;
                i--;
            }
            break;
        }
        default: break;
        }
        if(str[i] == '\0') {
            // check if we weren't done earlier in the code, that means malformed expression
            ret->push_back(new cmp_token(cmp_teof));
            break;
        }
        i++;
    }

    delete z;
    return ret;
}

#endif