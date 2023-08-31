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
#include <random>
#include <set>
#include <string>

#include "params.hpp"

typedef std::pair<unsigned int, unsigned int> qpair;
typedef std::pair<std::string, z3::expr> named_constr;

//************************************* QSET *************************************//

typedef std::set<unsigned int> qset_t;

bool is_superset(qset_t qset1, qset_t qset2);

std::ostream& operator<<(std::ostream& os, const qset_t& qset);

bool satisfies(unsigned int q, qset_t qset);

//************************************* CID *************************************//
typedef std::string cid_t;
typedef std::pair<cid_t, cid_t> cid_pair;

cid_t get_unique_id(cid_t module_id, cid_t queue_id);

//************************************* COMP *************************************//
enum class comp_t { GT = 0, GE, LT, LE, EQ };
std::ostream& operator<<(std::ostream& os, const comp_t& comp);
bool eval_comp(unsigned int lhs_val, comp_t comp, unsigned int rhs_val);
comp_t random_comp();
comp_t neg_comp(comp_t comp);

//************************************* TIME RANGE *************************************//
typedef std::pair<unsigned int, unsigned int> time_range_t;
bool is_superset(time_range_t t1, time_range_t t2);
bool includes(time_range_t time_range, unsigned int t);
std::ostream& operator<<(std::ostream& os, const time_range_t& time_range);

//************************************* TIME *************************************//
typedef std::chrono::time_point<std::chrono::high_resolution_clock> time_typ;
time_typ noww();
unsigned long long int get_diff_sec(time_typ start, time_typ end);
unsigned long long int get_diff_millisec(time_typ start, time_typ end);
unsigned long long int get_diff_microsec(time_typ start, time_typ end);

//************************************* OTHER *************************************//
std::string banner(std::string b);

#define DEBUG
#ifdef DEBUG
#define DEBUG_MSG(str)                                                                             \
    do {                                                                                           \
        std::cout << str;                                                                          \
    } while (false)
#else
#define DEBUG_MSG(str)                                                                             \
    do {                                                                                           \
    } while (false)
#endif

#endif /* util_hpp */
