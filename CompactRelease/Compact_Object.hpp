#ifndef COMPACT_OBJECT
#define COMPACT_OBJECT

#include <string>

enum cmp_otype {
    cmp_onull,
    cmp_oint,
    cmp_ostring
};

std::string cmp_otypeString(cmp_otype t) {
    switch(t) {
    case cmp_onull: return "cmp_onull";
    case cmp_oint: return "cmp_oint";
    case cmp_ostring: return "cmp_ostring";
    }
    return "";
}

struct cmp_object {
    cmp_otype type;
    void* val;
    std::string str();
    std::string valStr();
    cmp_object(cmp_otype t, void* v);
    cmp_object(int* v);
    cmp_object(std::string* v);
    cmp_object();
    ~cmp_object();
};

std::string cmp_object::str() {
    std::string ret = cmp_otypeString(this->type);
    ret += '<';
    ret += this->valStr();
    return ret;
}

std::string cmp_object::valStr() {
    switch(this->type) {
    case cmp_onull: return "null";
    case cmp_oint: return std::to_string(*(int*)(this->val));
    case cmp_ostring: return *(std::string*)(this->val);
    }
    return "";
}

cmp_object::cmp_object(cmp_otype t, void* v) {
    this->type = t;
    this->val = v;
}

cmp_object::cmp_object(int* v)
: cmp_object(cmp_oint,(void*)v) {}

cmp_object::cmp_object(std::string* v)
: cmp_object(cmp_ostring,(void*)v) {}

cmp_object::cmp_object()
: cmp_object(cmp_onull,nullptr) {}

cmp_object::~cmp_object() {
    delete this->val;
}

#endif