#ifndef COMPACT_UTIL
#define COMPACT_UTIL

#include "Compact_State.hpp"
#include "Compact_Parser.hpp"

void cmp_do(cmp_state* C, std::string& s) {
    cmpP_statement(C,s);
}

#endif