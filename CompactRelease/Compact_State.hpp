#ifndef COMPACT_STATE
#define COMPACT_STATE

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "Compact_Object.hpp"

struct cmp_state {
    bool ok = true;
    std::map<std::string,cmp_object*>* def;
    std::ostringstream* out;
    std::istringstream* in;
    void define(std::string, cmp_object*);
    void error(std::string);
    bool isdef(std::string);
    cmp_object* getdef(std::string);
    cmp_state();
    ~cmp_state();
};

void cmp_state::define(std::string s, cmp_object* o) {
    if(this->ok) {
        bool doesntExist = true;
        for(auto& pair : *(this->def))
            if(pair.first == s) {
                doesntExist = false;
                pair.second = o;
            }
        if(doesntExist)
            this->def->insert({s,o});
    }
}

void cmp_state::error(std::string s) {
    *(this->out) << s;
    this->ok = false;
}

bool cmp_state::isdef(std::string s) {
    return this->def->count(s) > 0;
}

cmp_object* cmp_state::getdef(std::string s) {
    return this->def->operator[](s);
}

cmp_state::cmp_state() {
    this->def = new std::map<std::string,cmp_object*>();
    this->out = new std::ostringstream();
    this->in = new std::istringstream();
}

cmp_state::~cmp_state() {
    for(const auto& pair : *(this->def))
        delete pair.second;
    delete this->def;
    delete this->out;
    delete this->in;
}

#endif