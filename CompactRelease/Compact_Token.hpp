#ifndef COMPACT_TOKEN
#define COMPACT_TOKEN

#include "Compact_Object.hpp"

enum cmp_ttype {
    cmp_tnull,
    cmp_tint,
    cmp_tstring,
    cmp_tidentifier,
    cmp_top_plus,
    cmp_top_minus,
    cmp_top_equals,
    cmp_teof
};

std::string cmp_ttypeString(cmp_ttype t) {
    switch(t) {
    case cmp_tnull: return "cmp_tnull";
    case cmp_tint: return "cmp_tint";
    case cmp_tstring: return "cmp_tstring";
    case cmp_tidentifier: return "cmp_tidentifier";
    case cmp_top_plus: return "cmp_top_plus";
    case cmp_top_minus: return "cmp_top_minus";
    case cmp_top_equals: return "cmp_top_equals";
    case cmp_teof: return "cmp_teof";
    }
    return "";
}

struct cmp_token {
    cmp_ttype type;
    cmp_object* obj;
    std::string str();
    cmp_token(cmp_ttype t, cmp_object* o);
    cmp_token(cmp_ttype t);
    cmp_token(cmp_object* o);
    ~cmp_token();
};

std::string cmp_token::str() {
    std::string ret = cmp_ttypeString(this->type);
    ret += ':';
    ret += this->obj->str();
    return ret;
}

cmp_token::cmp_token(cmp_ttype t, cmp_object* o) {
    this->type = t;
    this->obj = o;
}

cmp_token::cmp_token(cmp_ttype t)
: cmp_token(t,new cmp_object()) {}

cmp_token::cmp_token(cmp_object* o)
: cmp_token((cmp_ttype)(o->type),o) {}

cmp_token::~cmp_token() {
    delete this->obj;
}

#endif