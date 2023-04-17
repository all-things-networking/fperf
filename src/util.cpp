//
//  util.cpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 11/12/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "util.hpp"

#include <vector>
#include <sstream>
#include <iostream>

//************************************* QSET *************************************//
bool is_superset(qset_t qset1, qset_t qset2){
    std::set<unsigned int> res;
    std::set_difference(qset2.begin(), qset2.end(),
                         qset1.begin(), qset1.end(),
                         std::inserter(res, res.begin()));
    return res.size() == 0;
}

std::ostream& operator<<(std::ostream& os, const qset_t& qset){
    os << "{" ;
    for (qset_t::iterator it = qset.begin();
         it != qset.end(); it++){
        os << *it << ", ";
    }
    os << "}";
    return os;
}

bool satisfies(unsigned int q, qset_t qset){
    return qset.find(q) != qset.end();
}


//************************************* CID *************************************//

cid_t get_unique_id(cid_t module_id,
                     cid_t queue_id){
    return module_id + "." + queue_id;
}

//************************************* COMP *************************************//

std::ostream& operator<<(std::ostream& os, const comp_t& comp){
    switch (comp) {
        case (comp_t::GT):
            os << ">";
            break;
        case (comp_t::GE):
            os << ">=";
            break;
        case (comp_t::LT):
            os << "<";
            break;
        case (comp_t::LE):
            os << "<=";
            break;
        case (comp_t::EQ):
            os << "=";
            break;
    }
    return os;
}

bool eval_comp(unsigned int lhs_val, comp_t comp, unsigned int rhs_val){
    switch (comp) {
        case (comp_t::GT): return lhs_val > rhs_val;
        case (comp_t::GE): return lhs_val >= rhs_val;
        case (comp_t::LT): return lhs_val < rhs_val;
        case (comp_t::LE): return lhs_val <= rhs_val;
        case (comp_t::EQ): return lhs_val == rhs_val;
    }
    std::cout << "eval_comp: should not reach here" << std::endl;
    return false;
}

comp_t neg_comp(comp_t comp){
    switch (comp) {
        case (comp_t::GT): return comp_t::LT;
        case (comp_t::GE): return comp_t::LE;
        case (comp_t::LT): return comp_t::GT;
        case (comp_t::LE): return comp_t::GE;
        case (comp_t::EQ): return comp_t::EQ;
    }
    std::cout << "neg_comp: should not reach here" << std::endl;
    return comp;
}

//************************************* TIME RANGE *************************************//
bool is_superset(time_range_t t1, time_range_t t2){
    return t1.first <= t2.first && t1.second >= t2.second;
    
}

bool includes(time_range_t time_range, unsigned int t){
    bool res = time_range.first <= t && t <= time_range.second;
    return res;
    
}

std::ostream& operator<<(std::ostream& os, const time_range_t& time_range){
    os << "[" << (time_range.first + 1) << ", " << (time_range.second + 1) << "]";
    return os;
}
//************************************* TIME *************************************//

time_typ noww(){
    return std::chrono::high_resolution_clock::now();
}

unsigned long long int get_diff_sec(time_typ start, time_typ end){
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    return duration.count();
}

unsigned long long int get_diff_millisec(time_typ start, time_typ end){
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count();
}

unsigned long long int get_diff_microsec(time_typ start, time_typ end){
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count();
}

//************************************* OTHER *************************************//
std::string banner(std::string b){
    std::stringstream ss;
    ss << "**********************************************";
    ss << "**********************************************" << std::endl;
    ss << "**********************************************";
    ss << "**********************************************" << std::endl;
    ss << b << std::endl;
    ss << "**********************************************";
    ss << "**********************************************" << std::endl;
    ss << "**********************************************";
    ss << "**********************************************" << std::endl;
    return ss.str();
}

