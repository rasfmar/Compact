#ifndef COMPACT_INTERPRET
#define COMPACT_INTERPRET

#include <string>
#include <iostream>
#include "Compact_State.hpp"
#include "Compact_Token.hpp"
#include "Compact_Object.hpp"

std::vector<cmp_token*>* cmp_interpret(cmp_state* C, std::string& strng) {
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

void cmp_dump(cmp_state* C) {
    for(const auto& d : *(C->def)) {
        std::cout << d.first << ' ' << (d.second)->str() << '\n';
    }
}

cmp_object* cmp_express(cmp_state* C, std::vector<cmp_token*>* tokens) {
    unsigned int i = 0;
    switch(tokens->at(i)->type) {
    case cmp_tnull: break; // not possible
    case cmp_tint: {
        int* val = new int(*(int*)(tokens->at(i)->obj->val));
        cmp_ttype op = tokens->at(i+1)->type;
        switch(op) {
        case cmp_top_plus: case cmp_top_minus: {
            // this is all it can really be
            switch(tokens->at(i+2)->type) {
            case cmp_tint: {
                // correct
_0:
                if(op == cmp_top_plus)
                    *val += *(int*)(tokens->at(i+2)->obj->val);
                else if(op == cmp_top_minus)
                    *val -= *(int*)(tokens->at(i+2)->obj->val);
                
                for(int i = 0; i < 3; i++)
                    delete tokens->at(i);

                tokens->erase(tokens->begin(),tokens->begin()+2);
                tokens->at(0) = new cmp_token(new cmp_object(val));

                return cmp_express(C,tokens);
            }
            case cmp_tidentifier: {
                std::string id = *(std::string*)(tokens->at(i+2)->obj->val);
                if(C->isdef(id) && C->getdef(id)->type == cmp_oint) {
                    delete tokens->at(i+2);
                    tokens->at(i+2) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                    goto _0;
                } else // the identifier is not defined
                    C->error("Identifier expression invalid for second argument of expression operation");
                break;
            }
            default:
                C->error("Invalid or missing second argument for expression operator");
                break;
            }
        }
        case cmp_teof: {
            return new cmp_object(val);
        }
        default:
            C->error("Invalid or missing expression operator");
            break;
        }
        break;
    }
    case cmp_tstring: {
        std::string* val = new std::string(*(std::string*)(tokens->at(i)->obj->val));
        cmp_ttype op = tokens->at(i+1)->type;
        switch(op) {
        case cmp_top_plus: {
            switch(tokens->at(i+2)->type) {
            case cmp_tstring: {
_1:
                *val += *(std::string*)(tokens->at(i+2)->obj->val);
                
                for(int i = 0; i < 3; i++)
                    delete tokens->at(i);

                tokens->erase(tokens->begin(),tokens->begin()+2);
                tokens->at(0) = new cmp_token(new cmp_object(val));

                return cmp_express(C,tokens);
            }
            case cmp_tidentifier: {
                std::string id = *(std::string *)(tokens->at(i + 2)->obj->val);
                if(C->isdef(id) && C->getdef(id)->type == cmp_ostring) {
                    delete tokens->at(i+2);
                    tokens->at(i+2) = new cmp_token(new cmp_object(new std::string(*(std::string*)(C->getdef(id)->val))));
                    goto _1;
                } else // the identifier is not defined
                    C->error("Identifier expression invalid for second argument of expression operation");
                break;
            }
            default:
                C->error("Invalid or missing second argument for expression operator");
                break;
            }
        }
        case cmp_teof: {
            return new cmp_object(val);
        }
        default:
            C->error("Invalid or missing expression operator");
            break;
        }
        break;
    }
    case cmp_tidentifier: {
        // first, let's make sure this is defined
        std::string id = *(std::string*)(tokens->at(i)->obj->val);
        if(C->isdef(id)) {
            switch(C->getdef(id)->type) {
            case cmp_oint: {
                // then let's replace this token's value and recall this entire function
                delete tokens->at(i);
                tokens->at(i) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                return cmp_express(C,tokens);
            }
            case cmp_ostring: {
                delete tokens->at(i);
                tokens->at(i) = new cmp_token(new cmp_object(new std::string(*(std::string*)(C->getdef(id)->val))));
                return cmp_express(C,tokens);
            }
            default: // this can't happen, it can't be anything else
                break;
            }
        } else
            C->error("Identifier not defined");
        break;
    }
    // at this point its just operators and eof
    case cmp_teof:
        break;
    default: {
        C->error("Invalid expression");
        break;
    }
    }
    return new cmp_object();
}

void cmp_do(cmp_state* C, std::string& str) {
    std::vector<cmp_token*>* tokens = cmp_interpret(C,str);
    unsigned int i = 0;
    switch(tokens->at(i)->type) {
    case cmp_tnull: break; // this isn't possible
    case cmp_tint: case cmp_tstring: break; // this is just an expression doing nothing, no other way around it. i sho
    case cmp_tidentifier: {
        std::string name = tokens->at(i)->obj->valStr();
        if(name == "dump") {
            cmp_dump(C);
            goto finish;
        } else if(name == "carve") {
            delete tokens->at(i);
            tokens->erase(tokens->begin(),tokens->begin()+1);
            std::cout << cmp_express(C,tokens)->valStr() << '\n';
        } else if(name == "absorb") {
            switch(tokens->at(i+1)->type) {
            case cmp_tidentifier: {
                std::string id = *(std::string*)tokens->at(i+1)->obj->val;

                for(int i = 0; i < tokens->size(); i++)
                    delete tokens->at(i);
                tokens->clear();

                std::string* input = new std::string("");
                std::getline(std::cin, *input);

                C->define(id,new cmp_object(input));
                break;
            }
            // what else could we possibly put the input inside? only option is an identifier. otherwise...
            default:
                C->error("Input destination must be identifier");
                break;
            }
        } else {
            switch(tokens->at(i+1)->type) {
            case cmp_top_equals: {
                for(int i = 0; i < 2; i++)
                    delete tokens->at(i);
                tokens->erase(tokens->begin(),tokens->begin()+2);

                cmp_object* inition = cmp_express(C,tokens);
                if(C->ok) {
                    C->define(name,inition);
                }
                goto finish;
            }
            default: {
                C->error("Invalid syntax\n");
                goto finish;
            }
            }
        }
        break;
    }
    case cmp_teof: {
        goto finish;
        break;
    }
    // at this point, it's just operators, and hence impossible
    default: {
        C->error("Binary operator missing first argument\n");
        goto finish;
    }
    }
finish:
    return;
}

#endif