//
//  NetContext.hpp
//  AutoPerf
//
//  Created by Mina Tahmasbi Arashloo on 4/2/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#ifndef NetContext_hpp
#define NetContext_hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"

#include "z3++.h"

#pragma clang diagnostic pop

using namespace z3;

class NetContext {
public:
    NetContext();

    expr pkt_const(char const* name);
    expr pkt2val(expr pkt);
    expr pkt2meta1(expr pkt);
    expr pkt2meta2(expr pkt);

    expr pkt_val(bool val, int meta1, int meta2);

    expr null_pkt();

    expr int_const(char* name);
    expr int_val(int n);

    expr bool_const(char* name);
    expr bool_val(bool b);

    context& z3_ctx();

    unsigned long get_bool_var_cnt();
    unsigned long get_int_var_cnt();

private:
    context ctx;
    // Packets
    const char* names[3] = {"val", "meta1", "meta2"};
    sort sorts[3] = {ctx.bool_sort(), ctx.int_sort(), ctx.int_sort()};
    func_decl_vector* ptup_projs = new func_decl_vector(ctx);
    func_decl pkt_sort = ctx.tuple_sort("pkt_tuple", 3, names, sorts, *ptup_projs);
    func_decl ptup = pkt_sort;


    const unsigned int VAL_IND = 0;
    const unsigned int META1_IND = 1;
    const unsigned int META2_IND = 2;

    /* ********* Stats ********** */
    unsigned long bool_var_cnt = 0;
    unsigned long int_var_cnt = 0;
};

#endif /* NetContext_hpp */
