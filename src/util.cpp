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
    set<unsigned int> res;
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

bool satisfies(unsigned int q, qset_t qset) {
    return qset.find(q) != qset.end();
}


//************************************* CID *************************************//

cid_t get_unique_id(cid_t module_id, cid_t queue_id) {
    return module_id + "." + queue_id;
}

//************************************* COMP *************************************//

Op::Op() {}
Op::Op(Op::Type op) : type(op) {}

Op::Type Op::get_type() const {
    return type;
}

ostream& operator<<(ostream& os, const Op& op) {
    switch (op.get_type()) {
        case Op::Type::GT: os << ">"; break;
        case Op::Type::GE: os << ">="; break;
        case Op::Type::LT: os << "<"; break;
        case Op::Type::LE: os << "<="; break;
        case Op::Type::EQ: os << "=="; break;
    }
    return os;
}

bool Op::eval(unsigned int lhs_val, const Op& op, unsigned int rhs_val) {
    switch (op.get_type()) {
        case Type::GT: return lhs_val > rhs_val;
        case Type::GE: return lhs_val >= rhs_val;
        case Type::LT: return lhs_val < rhs_val;
        case Type::LE: return lhs_val <= rhs_val;
        case Type::EQ: return lhs_val == rhs_val;
        default: throw std::invalid_argument("Unknown operation");
    }
}

void Op::neg() {
    switch (type) {
        case Type::GT: type = Type::LE; break;
        case Type::GE: type = Type::LT; break;
        case Type::LT: type = Type::GE; break;
        case Type::LE: type = Type::GT; break;
        case Type::EQ: break;
        default: throw std::invalid_argument("Unknown operation");
    }
}

bool Op::operator==(const Op& other) const {
    return type == other.type;
}

bool Op::operator<(const Op& other) const {
    return type < other.type;
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
