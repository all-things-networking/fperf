//
//  util.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "util.hpp"

#include <iostream>
#include <sstream>
#include <vector>

//************************************* QSET *************************************//
bool is_superset(qset_t qset1, qset_t qset2) {
    set<cid_t> res;
    set_difference(
        qset2.begin(), qset2.end(), qset1.begin(), qset1.end(), inserter(res, res.begin()));
    return res.size() == 0;
}

ostream& operator<<(ostream& os, const qset_t& qset) {
    os << "{";
    for (qset_t::iterator it = qset.begin(); it != qset.end(); it++) {
        os << *it << ", ";
    }
    os << "}";
    return os;
}

bool satisfies(cid_t q, qset_t qset) {
    return qset.find(q) != qset.end();
}


//************************************* CID *************************************//

cid_t get_unique_id(cid_t module_id, cid_t queue_id) {
    return module_id + "." + queue_id;
}

//************************************* COMP *************************************//

ostream& operator<<(ostream& os, const op_t& comp) {
    switch (comp) {
        case (op_t::GT): os << ">"; break;
        case (op_t::GE): os << ">="; break;
        case (op_t::LT): os << "<"; break;
        case (op_t::LE): os << "<="; break;
        case (op_t::EQ): os << "="; break;
    }
    return os;
}

bool eval_op(unsigned int lhs_val, op_t op, unsigned int rhs_val) {
    switch (op) {
        case (op_t::GT): return lhs_val > rhs_val;
        case (op_t::GE): return lhs_val >= rhs_val;
        case (op_t::LT): return lhs_val < rhs_val;
        case (op_t::LE): return lhs_val <= rhs_val;
        case (op_t::EQ): return lhs_val == rhs_val;
    }
    cout << "eval_comp: should not reach here" << endl;
    return false;
}

op_t neg_op(op_t op) {
    switch (op) {
        case (op_t::GT): return op_t::LT;
        case (op_t::GE): return op_t::LE;
        case (op_t::LT): return op_t::GT;
        case (op_t::LE): return op_t::GE;
        case (op_t::EQ): return op_t::EQ;
    }
    cout << "neg_comp: should not reach here" << endl;
    return op;
}

//************************************* TIME RANGE *************************************//
bool is_superset(time_range_t t1, time_range_t t2) {
    return t1.first <= t2.first && t1.second >= t2.second;
}

bool includes(time_range_t time_range, unsigned int t) {
    bool res = time_range.first <= t && t <= time_range.second;
    return res;
}

ostream& operator<<(ostream& os, const time_range_t& time_range) {
    os << "[" << (time_range.first + 1) << ", " << (time_range.second + 1) << "]";
    return os;
}
//************************************* TIME *************************************//

time_typ noww() {
    return chrono::high_resolution_clock::now();
}

unsigned long long int get_diff_sec(time_typ start, time_typ end) {
    auto duration = chrono::duration_cast<chrono::seconds>(end - start);
    return duration.count();
}

unsigned long long int get_diff_millisec(time_typ start, time_typ end) {
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    return duration.count();
}

unsigned long long int get_diff_microsec(time_typ start, time_typ end) {
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    return duration.count();
}

//************************************* OTHER *************************************//
string banner(string b) {
    stringstream ss;
    ss << "**********************************************";
    ss << "**********************************************" << endl;
    ss << "**********************************************";
    ss << "**********************************************" << endl;
    ss << b << endl;
    ss << "**********************************************";
    ss << "**********************************************" << endl;
    ss << "**********************************************";
    ss << "**********************************************" << endl;
    return ss.str();
}
