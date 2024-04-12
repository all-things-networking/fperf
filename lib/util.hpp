//
//  util.hpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/9/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef util_hpp
#define util_hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"

#include "z3++.h"

#pragma clang diagnostic pop

#include <chrono>
#include <ostream>
#include <random>
#include <set>
#include <string>

#include "params.hpp"

using namespace z3;
using namespace std::chrono;

typedef pair<unsigned int, unsigned int> qpair;
typedef pair<string, expr> named_constr;

//************************************* QSET *************************************//

typedef set<unsigned int> qset_t;

bool is_superset(qset_t qset1, qset_t qset2);

ostream& operator<<(ostream& os, const qset_t& qset);

bool satisfies(unsigned int q, qset_t qset);

//************************************* CID *************************************//
typedef string cid_t;
typedef pair<cid_t, cid_t> cid_pair;

cid_t get_unique_id(cid_t module_id, cid_t queue_id);

//************************************* OP *************************************//
class Op {
public:
    enum class Type { GT = 0, GE, LT, LE, EQ };

    Op();
    Op(Op::Type op);

    Type get_type() const;

    friend ostream& operator<<(ostream& os, const Op& op);

    static Op random_op();

    void neg();

    bool operator==(const Op& other) const;
    bool operator<(const Op& other) const;

private:
    Type type;
};

//************************************* TIME RANGE *************************************//
typedef pair<unsigned int, unsigned int> time_range_t;
bool is_superset(time_range_t t1, time_range_t t2);
bool includes(time_range_t time_range, unsigned int t);
ostream& operator<<(ostream& os, const time_range_t& time_range);

//************************************* TIME *************************************//
typedef time_point<high_resolution_clock> time_typ;
time_typ noww();
unsigned long long int get_diff_sec(time_typ start, time_typ end);
unsigned long long int get_diff_millisec(time_typ start, time_typ end);
unsigned long long int get_diff_microsec(time_typ start, time_typ end);

//************************************* OTHER *************************************//
string banner(string b);

template <typename... Args> string format_string(const string& format, Args... args) {
    char vname[100];
    snprintf(vname, 100, format.c_str(), args...);
    string s(vname);
    return s;
}

#ifdef DEBUG
#define DEBUG_MSG(str)                                                                             \
    do {                                                                                           \
        cout << str;                                                                               \
    } while (false)
#else
#define DEBUG_MSG(str)                                                                             \
    do {                                                                                           \
    } while (false)
#endif

#endif /* util_hpp */
