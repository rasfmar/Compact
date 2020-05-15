#ifndef COMPACT_PARSER
#define COMPACT_PARSER

#include <string>
#include <iostream>
#include "Compact_State.hpp"
#include "Compact_Token.hpp"
#include "Compact_Object.hpp"
#include "Compact_Lexer.hpp"

void cmpP_dump(cmp_state* C) {
    for(const auto& d : *(C->def)) {
        std::cout << d.first << ' ' << (d.second)->str() << '\n';
    }
}

cmp_object* cmpP_expression(cmp_state* C, std::vector<cmp_token*>* tokens) {
    unsigned int i = 0;
    // multiplication and division
    for (; i < tokens->size();) {
        switch(tokens->at(i)->type) {
        case cmp_top_multiply: case cmp_top_divide: {
            // we want to get the subexpressions immediately before and after.
            // on the right, we can include a minus sign
            // also, i can't be 0. if it is, throw exception
            if(i > 0) {
_3:
                // let's look at the argument to the left of the op
                switch(tokens->at(i-1)->type) {
                case cmp_tint: { // can only be this!
                    int* result = new int(
                        *(int*)(tokens->at(i-1)->obj->val)
                    );
_2:
                    // now the argument to the right
                    switch(tokens->at(i+1)->type) {
                    case cmp_tint: { // yes!
                        if(tokens->at(i)->type == cmp_top_multiply)
                            *result *= *(int*)(tokens->at(i+1)->obj->val);
                        else if(tokens->at(i)->type == cmp_top_divide)
                            *result /= *(int*)(tokens->at(i+1)->obj->val);
                        
                        for(int k = -1; k <= 1; k++)
                            delete tokens->at(i+k);
                        
                        tokens->erase(tokens->begin()+i,tokens->begin()+i+2);
                        tokens->at(i-1) = new cmp_token(new cmp_object(result));

                        i -= 2;
                        break;
                    }
                    case cmp_top_plus: case cmp_top_minus: {
                        // well, let's make sure the thing after this is an int. otherwise, throw error
_1:
                        switch(tokens->at(i+2)->type) {
                        case cmp_tint: {
                            // let's convert this whole thing to an int without the operator prefix
                            // should probably use the subexpression function but don't want to instantiate new tokens vector
                            int sign = tokens->at(i + 1)->type == cmp_top_minus ? -1 : 1;
                            delete tokens->at(i+1);
                            tokens->erase(tokens->begin()+i+1,tokens->begin()+i+2);
                            *(int*)(tokens->at(i+1)->obj->val) *= sign;
                            goto _2;
                        }
                        case cmp_tidentifier: {
                            std::string id = *(std::string*)(tokens->at(i+2)->obj->val);
                            if(C->isdef(id)) {
                                switch(C->getdef(id)->type) {
                                case cmp_oint: {
                                    delete tokens->at(i+2);
                                    tokens->at(i+2) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                                    goto _1;
                                }
                                default: // this can't happen, it can't be anything else
                                    C->error("Identifier as second argument in mul/div operation is not an integer");
                                    goto finish;
                                }
                            } else {
                                C->error("Identifier not defined");
                                goto finish;
                            }
                            break;
                        }
                        default:
                            C->error("Cannot add sign to non-integer");
                            goto finish;
                        }
                    }
                    case cmp_tidentifier: {
                        std::string id = *(std::string*)(tokens->at(i+1)->obj->val);
                        if(C->isdef(id)) {
                            switch(C->getdef(id)->type) {
                            case cmp_oint: {
                                // then let's replace this token's value and recall this entire function
                                delete tokens->at(i+1);
                                tokens->at(i+1) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                                goto _2;
                            }
                            default: // this can't happen, it can't be anything else
                                C->error("Identifier as first argument in mul/div operation is not an integer");
                                goto finish;
                            }
                        } else {
                            C->error("Identifier not defined");
                            goto finish;
                        }
                        break;
                    }
                    default:
                        C->error("Invalid or missing second argument for multiplication");
                        goto finish;
                    }
                    break;
                }
                case cmp_tidentifier: {
                    std::string id = *(std::string*)(tokens->at(i-1)->obj->val);
                    if(C->isdef(id)) {
                        switch(C->getdef(id)->type) {
                        case cmp_oint: {
                            // then let's replace this token's value and recall this entire function
                            delete tokens->at(i-1);
                            tokens->at(i-1) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                            goto _3;
                        }
                        default: // this can't happen, it can't be anything else
                            C->error("Identifier as first argument in mul/div operation is not an integer");
                            goto finish;
                        }
                    } else {
                        C->error("Identifier not defined");
                        goto finish;
                    }
                    break;
                }
                default:
                    C->error("Invalid first argument for multiplication");
                    goto finish;
                }
            } else {
                // we have a multiplication or division operator at the beginning of the expression! impossible
                C->error("Broad expression begins with binary operator");
                goto finish;
            }
            break;
        }
        default:
            break;
        }
        i++;
    }

    i = 0;
    // now addition, subtraction
    for(; i < tokens->size(); ) {
        switch(tokens->at(i)->type) {
        case cmp_top_plus: case cmp_top_minus: {
            if(i == 0) {
                // we're gonna take this sign and convert the next thing
_5:
                switch(tokens->at(i+1)->type) {
                case cmp_tint: {
                    int sign = tokens->at(i)->type == cmp_top_minus ? -1 : 1;
                    delete tokens->at(i);
                    tokens->erase(tokens->begin() + i, tokens->begin() + i + 1);
                    *(int *)(tokens->at(i)->obj->val) *= sign;
                    i -= 1;
                    break;
                }
                case cmp_tidentifier: {
                    std::string id = *(std::string*)(tokens->at(i+1)->obj->val);
                    if(C->isdef(id)) {
                        switch(C->getdef(id)->type) {
                        case cmp_oint: {
                            delete tokens->at(i+1);
                            tokens->at(i+1) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                            goto _5;
                        }
                        default:
                            C->error("Identifier as argument in unary operation is not an integer");
                            goto finish;
                        }
                    } else {
                        C->error("Identifier not defined");
                        goto finish;
                    }
                    break;  
                }
                default:
                    C->error("Can't add sign to non-integer");
                    goto finish;
                }
            } else {
                // then let's check the first argument
_6:
                switch(tokens->at(i-1)->type) {
                case cmp_tint: {
                    int* result = new int(*(int*)(tokens->at(i-1)->obj->val));
                    // now the argument to the right
_7:
                    switch(tokens->at(i+1)->type) {
                    case cmp_tint: { // yes!
                        if(tokens->at(i)->type == cmp_top_plus)
                            *result += *(int*)(tokens->at(i+1)->obj->val);
                        else if(tokens->at(i)->type == cmp_top_minus)
                            *result -= *(int*)(tokens->at(i+1)->obj->val);
                        
                        for(int k = -1; k <= 1; k++)
                            delete tokens->at(i+k);
                        
                        tokens->erase(tokens->begin()+i,tokens->begin()+i+2);
                        tokens->at(i-1) = new cmp_token(new cmp_object(result));

                        i -= 2;
                        break;
                    }
                    case cmp_top_plus: case cmp_top_minus: {
                        // well, let's make sure the thing after this is an int. otherwise, throw error
_8:
                        switch(tokens->at(i+2)->type) {
                        case cmp_tint: {
                            // let's convert this whole thing to an int without the operator prefix
                            // should probably use the subexpression function but don't want to instantiate new tokens vector
                            int sign = tokens->at(i + 1)->type == cmp_top_minus ? -1 : 1;
                            delete tokens->at(i+1);
                            tokens->erase(tokens->begin()+i+1,tokens->begin()+i+2);
                            *(int*)(tokens->at(i+1)->obj->val) *= sign;
                            goto _7;
                        }
                        case cmp_tidentifier: {
                            std::string id = *(std::string*)(tokens->at(i+2)->obj->val);
                            if(C->isdef(id)) {
                                switch(C->getdef(id)->type) {
                                case cmp_oint: {
                                    delete tokens->at(i+2);
                                    tokens->at(i+2) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                                    goto _8;
                                }
                                default: // this can't happen, it can't be anything else
                                    C->error("Identifier as second argument in binary operation is not an integer");
                                    goto finish;
                                }
                            } else {
                                C->error("Identifier not defined");
                                goto finish;
                            }
                            break;
                        }
                        default:
                            C->error("Cannot add sign to non-integer");
                            goto finish;
                        }
                    }
                    case cmp_tidentifier: {
                        std::string id = *(std::string*)(tokens->at(i+1)->obj->val);
                        if(C->isdef(id)) {
                            switch(C->getdef(id)->type) {
                            case cmp_oint: {
                                // then let's replace this token's value and recall this entire function
                                delete tokens->at(i+1);
                                tokens->at(i+1) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                                goto _7;
                            }
                            default: // this can't happen, it can't be anything else
                                C->error("Identifier as first argument in mul/div operation is not an integer");
                                goto finish;
                            }
                        } else {
                            C->error("Identifier not defined");
                            goto finish;
                        }
                        break;
                    }
                    default:
                        C->error("Invalid or missing second argument for multiplication");
                        goto finish;
                    }
                    break;
                }
                case cmp_tstring: {
                    // okay, the first argument is a string. we're concatenating
                    // let's make sure the operation is +
                    if(tokens->at(i)->type != cmp_top_plus) {
                        C->error("Binary operator invalid for string as first argument");
                        goto finish;
                    }
                    std::string* result = new std::string(*(std::string*)(tokens->at(i-1)->obj->val));
_9:
                    // let's check the second argument
                    switch(tokens->at(i+1)->type) {
                    case cmp_tstring: { // yes!
                        *result += *(std::string*)(tokens->at(i+1)->obj->val);

                        for(int k = -1; k <= 1; k++)
                            delete tokens->at(i+k);
                        
                        tokens->erase(tokens->begin()+i,tokens->begin()+i+2);
                        tokens->at(i-1) = new cmp_token(new cmp_object(result));

                        i -= 2;
                        break;
                    }
                    case cmp_tint: {
                        std::string* val = new std::string(tokens->at(i+1)->obj->valStr());
                        delete tokens->at(i+1);
                        tokens->at(i+1) = new cmp_token(new cmp_object(val));
                        goto _9;
                    }
                    case cmp_tidentifier: {
                        std::string id = *(std::string*)(tokens->at(i+1)->obj->val);
                        if(C->isdef(id)) {
                            switch(C->getdef(id)->type) {
                            case cmp_ostring: {
                                // then let's replace this token's value and recall this entire function
                                delete tokens->at(i+1);
                                tokens->at(i+1) = new cmp_token(new cmp_object(new std::string(*(std::string*)(C->getdef(id)->val))));
                                goto _9;
                            }
                            case cmp_oint: {
                                delete tokens->at(i+1);
                                tokens->at(i+1) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                                goto _9;
                            }
                            default: // this can't happen, it can't be anything else
                                C->error("Identifier as first argument in mul/div operation is not an integer");
                                goto finish;
                            }
                        } else {
                            C->error("Identifier not defined");
                            goto finish;
                        }
                        break;
                    }
                    default:
                        C->error("Invalid or missing second argument for string concatenation");
                        goto finish;
                    }
                    break;
                    // done with string concat!
                }
                case cmp_tidentifier: {
                    std::string id = *(std::string*)(tokens->at(i-1)->obj->val);
                    if(C->isdef(id)) {
                        switch(C->getdef(id)->type) {
                        case cmp_oint: {
                            delete tokens->at(i-1);
                            tokens->at(i-1) = new cmp_token(new cmp_object(new int(*(int*)(C->getdef(id)->val))));
                            goto _6;
                        }
                        case cmp_ostring: {
                            delete tokens->at(i-1);
                            tokens->at(i-1) = new cmp_token(new cmp_object(new std::string(*(std::string*)(C->getdef(id)->val))));
                            goto _6;
                        }
                        default:
                            C->error("Identifier as argument in unary operation is not an integer");
                            goto finish;
                        }
                    } else {
                        C->error("Identifier not defined");
                        goto finish;
                    }
                    break;
                }
                default:
                    C->error("Invalid first argument to binary operator");
                    goto finish;
                }
            }
            break;
        }
        default:
            break;
        }
        i++;
    }

    // we've done all the arithmetic operations... what's left?
    // if all went well, there's two last tokens left: something and cmp_teof
end:
    switch(tokens->at(0)->type) {
    case cmp_tint: {
        int val = *(int*)(tokens->at(0)->obj->val);
        for(auto& token: *tokens)
            delete token;
        tokens->clear();
        return new cmp_object(new int(val));
    }
    case cmp_tstring: {
        std::string val = *(std::string*)(tokens->at(0)->obj->val);
        for(auto& token: *tokens)
            delete token;
        tokens->clear();
        return new cmp_object(new std::string(val));
    }
    case cmp_tidentifier: {
        std::string id = *(std::string *)(tokens->at(0)->obj->val);
        if(C->isdef(id)) {
            switch(C->getdef(id)->type) {
            case cmp_oint: {
                delete tokens->at(0);
                tokens->at(0) = new cmp_token(new cmp_object(new int(
                    *(int*)(C->getdef(id)->val)
                )));
                goto end;
            }
            case cmp_ostring: {
                delete tokens->at(0);
                tokens->at(0) = new cmp_token(new cmp_object(new std::string(
                    *(std::string*)(C->getdef(id)->val)
                )));
                goto end;
            }
            default:
                goto finish;
            }
        } else {
            C->error("Identifier not defined");
            goto finish;
        }
        break;
    }
    default:
        goto finish;
    }
finish:
    for(auto& token: *tokens)
        delete token;
    tokens->clear();
    return new cmp_object();
}

/*
cmp_object* cmpP_literal(cmp_state* C, cmp_token* token) {
    std::vector<cmp_token*>* t = new std::vector<cmp_token*>();
    t->push_back(token);
    cmp_object* ret = cmpP_subexpression()
}
*/

/*

cmp_object* cmpP_subexpression(cmp_state* C, std::vector<cmp_token*>* tokens) {
    unsigned int i = 0;
    switch(tokens->at(i)->type) {
    case cmp_tnull: break; // not possible
    case cmp_tint: {
        int* val = new int(*(int*)(tokens->at(i)->obj->val));
        cmp_ttype op = tokens->at(i+1)->type;
        switch(op) {
        case cmp_top_plus: case cmp_top_minus: case cmp_top_multiply: case cmp_top_divide: {
            // this is all it can really be
            switch(tokens->at(i+2)->type) {
            case cmp_tint: {
                // correct
_0:
                if(op == cmp_top_plus)
                    *val += *(int*)(tokens->at(i+2)->obj->val);
                else if(op == cmp_top_minus)
                    *val -= *(int*)(tokens->at(i+2)->obj->val);
                else if(op == cmp_top_multiply)
                    *val *= *(int*)(tokens->at(i+2)->obj->val);
                else if(op == cmp_top_divide)
                    *val /= *(int*)(tokens->at(i+2)->obj->val);
                
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
    case cmp_top_plus: case cmp_top_minus: {
        switch(tokens->at(i+1)->type) {
        case cmp_tint: { // must be
            int sign = tokens->at(i)->type == cmp_top_plus ? 1 : -1;
            delete tokens->at(i);
            tokens->erase(tokens->begin(),tokens->begin()+1);
            *(int*)(tokens->at(i)->obj->val) *= sign;
            return cmpP_expression(C,tokens);
        }
        default:
            C->error("Binary operator missing arguments");
            break;
        }
        break;
    }
    case cmp_teof:
        break;
    default:
        C->error("Invalid expression");
        break;
    }
    return new cmp_object();
}

*/

void cmpP_statement(cmp_state* C, std::string& str) {
    std::vector<cmp_token*>* tokens = cmpL_lexer(C,str);
    unsigned int i = 0;
    switch(tokens->at(i)->type) {
    case cmp_tnull: break; // this isn't possible
    case cmp_tint: case cmp_tstring: break; // this is just a statement consisting of an expression doing nothing, no other way around it. i should calculate but i won't
    case cmp_tidentifier: {
        std::string name = tokens->at(i)->obj->valStr();
        if(name == "dump") {
            cmpP_dump(C);
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