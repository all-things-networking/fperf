//
//  NetContext.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 4/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "net_context.hpp"
#include <cstring>

NetContext::NetContext() {
}

expr NetContext::pkt_const(char const* name) {
    char val_vname[100];
    strcpy(val_vname, name);
    strcat(val_vname, "_val");
    expr val_var = ctx.bool_const(val_vname);

    char meta1_vname[100];
    strcpy(meta1_vname, name);
    strcat(meta1_vname, "_meta1");
    expr meta1_var = ctx.int_const(meta1_vname);

    char meta2_vname[100];
    strcpy(meta2_vname, name);
    strcat(meta2_vname, "_meta2");
    expr meta2_var = ctx.int_const(meta2_vname);

    bool_var_cnt += 1;
    int_var_cnt += 2;
    return (ptup) (val_var, meta1_var, meta2_var);
}

expr NetContext::pkt2val(expr pkt) {
    return (*ptup_projs)[VAL_IND](pkt);
}

expr NetContext::pkt2meta1(expr pkt) {
    return (*ptup_projs)[META1_IND](pkt);
}

expr NetContext::pkt2meta2(expr pkt) {
    return (*ptup_projs)[META2_IND](pkt);
}

expr NetContext::pkt_val(bool val, int meta1, int meta2) {
    expr vval = ctx.bool_val(val);
    expr vmeta1 = ctx.int_val(meta1);
    expr vmeta2 = ctx.int_val(meta2);
    return (ptup) (vval, vmeta1, vmeta2);
}

expr NetContext::null_pkt() {
    expr null_val = ctx.bool_val(false);
    expr null_meta1 = ctx.int_val(0);
    expr null_meta2 = ctx.int_val(0);
    return (ptup) (null_val, null_meta1, null_meta2);
}

expr NetContext::int_const(char* name) {
    int_var_cnt++;
    return ctx.int_const(name);
}

expr NetContext::int_val(int n) {
    return ctx.int_val(n);
}

expr NetContext::bool_const(char* name) {
    bool_var_cnt++;
    return ctx.bool_const(name);
}

expr NetContext::bool_val(bool b) {
    return ctx.bool_val(b);
}

context& NetContext::z3_ctx() {
    return ctx;
}

unsigned long NetContext::get_bool_var_cnt() {
    return bool_var_cnt;
}

unsigned long NetContext::get_int_var_cnt() {
    return int_var_cnt;
}
