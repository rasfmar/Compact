#ifndef COMPACT_PARSER
#define COMPACT_PARSER

#include <string>
#include <iostream>
#include "Compact_State.hpp"
#include "Compact_Token.hpp"
#include "Compact_Object.hpp"
#include "Compact_Lexer.hpp"

void cmp_dump(cmp_state* C) {
    for(const auto& d : *(C->def)) {
        std::cout << d.first << ' ' << (d.second)->str() << '\n';
    }
}

cmp_object* cmpP_expression(cmp_state* C, std::vector<cmp_token*>* tokens) {
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

                return cmpP_expression(C,tokens);
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

                return cmpP_expression(C,tokens);
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
                return cmpP_expression(C,tokens);
            }
            case cmp_ostring: {
                delete tokens->at(i);
                tokens->at(i) = new cmp_token(new cmp_object(new std::string(*(std::string*)(C->getdef(id)->val))));
                return cmpP_expression(C,tokens);
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

void cmpP_statement(cmp_state* C, std::string& str) {
    std::vector<cmp_token*>* tokens = cmpL_lexer(C,str);
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
            std::cout << cmpP_expression(C,tokens)->valStr() << '\n';
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

                cmp_object* inition = cmpP_expression(C,tokens);
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