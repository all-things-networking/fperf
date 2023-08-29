//
//  switch_xbar_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/13/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "switch_xbar_qm.hpp"

#include <iostream>

SwitchXBarQM::SwitchXBarQM(cid_t id,
                           unsigned int total_time,
                           std::vector<unsigned int> voq_input_map,
                           std::vector<unsigned int> voq_output_map,
                           std::vector<QueueInfo> in_queue_info,
                           std::vector<QueueInfo> out_queue_info,
                           NetContext& net_ctx):
QueuingModule(id, total_time, in_queue_info, out_queue_info, net_ctx),
voq_input_map(voq_input_map),
voq_output_map(voq_output_map) {
    port_cnt = (unsigned int) out_queues.size();

    for (unsigned int i = 0; i < port_cnt; i++) {
        input_voq_map[i] = vector<unsigned int>();
        output_voq_map[i] = vector<unsigned int>();
    }

    for (unsigned int i = 0; i < voq_input_map.size(); i++) {
        unsigned int input_port = voq_input_map[i];
        input_voq_map[input_port].push_back(i);
    }

    for (unsigned int i = 0; i < voq_output_map.size(); i++) {
        unsigned int output_port = voq_output_map[i];
        output_voq_map[output_port].push_back(i);
    }

    /*
    cout << id << endl;
    cout << "voq_input_map " << endl;
    for (unsigned int i = 0; i < voq_input_map.size(); i++){
        cout << i << "->" << voq_input_map[i] << endl;
    }
    cout << "input_voq_map" << endl;
    for (map<unsigned int, vector<unsigned int>>::iterator it = input_voq_map.begin();
         it != input_voq_map.end(); it++){
        cout << it -> first << endl;
        vector<unsigned int> voqs = it->second;
        for (unsigned int i = 0; i < voqs.size(); i++){
            cout << voqs[i] << " ";
        }
        cout << endl;
    }
    cout << "voq_output_map " << endl;
    for (unsigned int i = 0; i < voq_output_map.size(); i++){
        cout << i << "->" << voq_output_map[i] << endl;
    }
    cout << "output_voq_map" << endl;
    for (map<unsigned int, vector<unsigned int>>::iterator it = output_voq_map.begin();
         it != output_voq_map.end(); it++){
        cout << it -> first << endl;
        vector<unsigned int> voqs = it->second;
        for (unsigned int i = 0; i < voqs.size(); i++){
            cout << voqs[i] << " ";
        }
        cout << endl;
    }
    */

    init(net_ctx);
}


void SwitchXBarQM::add_proc_vars(NetContext& net_ctx) {
    in_to_out_ = new vector<vector<expr>>[port_cnt];
    out_from_in_ = new vector<vector<expr>>[port_cnt];
    in_prio_head_ = new vector<vector<expr>>[port_cnt];
    out_prio_head_ = new vector<vector<expr>>[port_cnt];

    char vname[100];
    for (unsigned int q1 = 0; q1 < port_cnt; q1++) {
        for (unsigned int q2 = 0; q2 < input_voq_map[q1].size(); q2++) {
            in_to_out_[q1].push_back(vector<expr>());
            for (unsigned int t = 0; t < total_time; t++) {
                std::sprintf(vname, "%s_in_to_out[%d][%d][%d]", id.c_str(), q1, q2, t);
                in_to_out_[q1][q2].push_back(net_ctx.bool_const(vname));
            }
        }
    }

    for (unsigned int q1 = 0; q1 < port_cnt; q1++) {
        for (unsigned int q2 = 0; q2 < output_voq_map[q1].size(); q2++) {
            out_from_in_[q1].push_back(vector<expr>());
            for (unsigned int t = 0; t < total_time; t++) {
                std::sprintf(vname, "%s_out_from_in[%d][%d][%d]", id.c_str(), q1, q2, t);
                out_from_in_[q1][q2].push_back(net_ctx.bool_const(vname));
            }
        }
    }

    for (unsigned int q1 = 0; q1 < port_cnt; q1++) {
        for (unsigned int q2 = 0; q2 < input_voq_map[q1].size(); q2++) {
            in_prio_head_[q1].push_back(vector<expr>());
            for (unsigned int t = 0; t < total_time; t++) {
                std::sprintf(vname, "%s_in_prio_head[%d][%d][%d]", id.c_str(), q1, q2, t);
                in_prio_head_[q1][q2].push_back(net_ctx.bool_const(vname));
            }
        }
    }

    for (unsigned int q1 = 0; q1 < port_cnt; q1++) {
        for (unsigned int q2 = 0; q2 < output_voq_map[q1].size(); q2++) {
            out_prio_head_[q1].push_back(vector<expr>());
            for (unsigned int t = 0; t < total_time; t++) {
                std::sprintf(vname, "%s_out_prio_head[%d][%d][%d]", id.c_str(), q1, q2, t);
                out_prio_head_[q1][q2].push_back(net_ctx.bool_const(vname));
            }
        }
    }
}

void SwitchXBarQM::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {

    char constr_name[100];

    const int NONE = port_cnt;
    for (unsigned int t = 0; t < total_time; t++) {

        // DATA
        /*
        for (unsigned int q = 0; q < port_cnt; q++){
            sprintf(constr_name, "%s_in_prio_head_port_%d_at_%d", id.c_str(), q, t);
            expr constr_expr = in_prio_head_[q][t] >= 0 && in_prio_head_[q][t] < (int)
        input_voq_map[q].size(); constr_map.insert(named_constr(constr_name, constr_expr));

            sprintf(constr_name, "%s_out_prio_head_port_%d_at_%d", id.c_str(), q, t);
            constr_expr = out_prio_head_[q][t] >= 0 && out_prio_head_[q][t] < (int)
        output_voq_map[q].size(); constr_map.insert(named_constr(constr_name, constr_expr));
        }
        */

        // cout << 1 << endl;
        //  input and output match together
        for (unsigned int i = 0; i < port_cnt; i++) {
            for (unsigned int i_ind_of_j = 0; i_ind_of_j < input_voq_map[i].size(); i_ind_of_j++) {
                unsigned int voq_ind = input_voq_map[i][i_ind_of_j];
                unsigned int j = voq_output_map[voq_ind];
                unsigned int j_ind_of_i = NONE;

                for (unsigned int k = 0; k < output_voq_map[j].size(); k++) {
                    if (output_voq_map[j][k] == voq_ind) {
                        j_ind_of_i = k;
                        break;
                    }
                }

                sprintf(constr_name,
                        "%s_in_and_out_together_%d(q%d)_%d(q%d)_at_%d",
                        id.c_str(),
                        i,
                        i_ind_of_j,
                        j,
                        j_ind_of_i,
                        t);

                expr constr_expr = (implies(in_to_out_[i][i_ind_of_j][t],
                                            out_from_in_[j][j_ind_of_i][t]) &&
                                    implies(!in_to_out_[i][i_ind_of_j][t],
                                            !out_from_in_[j][j_ind_of_i][t]));

                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // cout << 2 << endl;

        // Matching priorities
        for (unsigned int i = 0; i < port_cnt; i++) {
            unsigned int i_voq_cnt = input_voq_map[i].size();
            for (unsigned int i_ind_of_j = 0; i_ind_of_j < i_voq_cnt; i_ind_of_j++) {
                unsigned int voq_ind = input_voq_map[i][i_ind_of_j];
                unsigned int j = voq_output_map[voq_ind];
                unsigned int j_ind_of_i = NONE;
                for (unsigned int k = 0; k < output_voq_map[j].size(); k++) {
                    if (output_voq_map[j][k] == voq_ind) {
                        j_ind_of_i = k;
                        break;
                    }
                }

                // no higher priority inputs
                expr_vector cond1(net_ctx.z3_ctx());
                unsigned int j_voq_cnt = output_voq_map[j].size();
                for (unsigned int j_ind_of_p = 0; j_ind_of_p < j_voq_cnt; j_ind_of_p++) {
                    expr_vector not_matched(net_ctx.z3_ctx());
                    not_matched.push_back(net_ctx.bool_val(true));
                    for (unsigned int j_ind_of_k = 0; j_ind_of_k < j_voq_cnt; j_ind_of_k++) {
                        unsigned int ind = (j_ind_of_p + 1 + j_ind_of_k) % j_voq_cnt;
                        if (ind == j_ind_of_i) break;
                        not_matched.push_back(!out_from_in_[j][ind][t]);
                    }
                    cond1.push_back(implies(out_prio_head_[j][j_ind_of_p][t], mk_and(not_matched)));
                }


                // no higher priority outputs
                expr_vector cond2(net_ctx.z3_ctx());
                for (unsigned int i_ind_of_p = 0; i_ind_of_p < i_voq_cnt; i_ind_of_p++) {
                    expr_vector not_matched(net_ctx.z3_ctx());
                    not_matched.push_back(net_ctx.bool_val(true));
                    for (unsigned int i_ind_of_k = 0; i_ind_of_k < i_voq_cnt; i_ind_of_k++) {
                        unsigned int ind = (i_ind_of_p + 1 + i_ind_of_k) % i_voq_cnt;
                        if (ind == i_ind_of_j) break;
                        not_matched.push_back(!in_to_out_[i][ind][t]);
                    }
                    cond2.push_back(implies(in_prio_head_[i][i_ind_of_p][t], mk_and(not_matched)));
                }

                // there is outstanding packets from i to j
                Queue* voq = in_queues[voq_ind];
                expr cond3 = net_ctx.pkt2val(voq->elem(0)[t]);

                expr match_cond = mk_and(cond1) && mk_and(cond2) && cond3;

                sprintf(constr_name, "%s_%d_matches_%d_at_%d", id.c_str(), i, j, t);
                expr constr_expr = implies(match_cond, in_to_out_[i][i_ind_of_j][t]);
                constr_map.insert(named_constr(constr_name, constr_expr));

                sprintf(constr_name, "%s_%d_doesnt_match_%d_at_%d", id.c_str(), i, j, t);
                constr_expr = implies(!match_cond, !in_to_out_[i][i_ind_of_j][t]);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // cout << 3 << endl;

        // No double matching in input
        for (unsigned int i = 0; i < port_cnt; i++) {
            for (unsigned int i_ind_of_j = 0; i_ind_of_j < input_voq_map[i].size(); i_ind_of_j++) {
                expr_vector others_zero(net_ctx.z3_ctx());
                for (unsigned int k = 0; k < in_to_out_[i].size(); k++) {
                    if (i_ind_of_j == k) continue;
                    others_zero.push_back(!in_to_out_[i][k][t]);
                }
                sprintf(constr_name,
                        "%s_in_port_%d_matches_only_voq_%d_at_%d",
                        id.c_str(),
                        i,
                        i_ind_of_j,
                        t);
                expr constr_expr = implies(in_to_out_[i][i_ind_of_j][t], mk_and(others_zero));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // cout << 4 << endl;

        // No double matching in output
        for (unsigned int j = 0; j < port_cnt; j++) {
            for (unsigned int j_ind_of_i = 0; j_ind_of_i < output_voq_map[j].size(); j_ind_of_i++) {
                expr_vector others_zero(net_ctx.z3_ctx());
                for (unsigned int k = 0; k < output_voq_map[j].size(); k++) {
                    if (j_ind_of_i == k) continue;
                    others_zero.push_back(!out_from_in_[j][k][t]);
                }
                sprintf(constr_name,
                        "%s_out_port_%d_matches_only_voq_%d_at_%d",
                        id.c_str(),
                        j,
                        j_ind_of_i,
                        t);
                expr constr_expr = implies(out_from_in_[j][j_ind_of_i][t], mk_and(others_zero));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // cout << 5 << endl;
        //  One of prios is always one, and only one is one
        for (unsigned int i = 0; i < port_cnt; i++) {
            if (input_voq_map[i].size() == 0) continue;

            expr_vector all_prios(net_ctx.z3_ctx());
            for (unsigned int p = 0; p < input_voq_map[i].size(); p++) {
                all_prios.push_back(in_prio_head_[i][p][t]);
            }
            sprintf(constr_name, "%s_at_least_one_in_prio_head_[%d]_at_%d", id.c_str(), i, t);
            expr constr_expr = mk_or(all_prios);
            constr_map.insert(named_constr(constr_name, constr_expr));

            for (unsigned int p = 0; p < input_voq_map[i].size(); p++) {
                expr_vector others_zero(net_ctx.z3_ctx());
                for (unsigned int k = 0; k < in_prio_head_[i].size(); k++) {
                    if (p == k) continue;
                    others_zero.push_back(!in_prio_head_[i][k][t]);
                }
                sprintf(
                    constr_name, "%s_only_in_prio_head[%d][%d][%d]_is_one", id.c_str(), i, p, t);
                expr constr_expr = implies(in_prio_head_[i][p][t], mk_and(others_zero));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // cout << 6 << endl;

        for (unsigned int i = 0; i < port_cnt; i++) {
            if (output_voq_map[i].size() == 0) continue;

            expr_vector all_prios(net_ctx.z3_ctx());
            for (unsigned int p = 0; p < output_voq_map[i].size(); p++) {
                all_prios.push_back(out_prio_head_[i][p][t]);
            }
            sprintf(constr_name, "%s_at_least_one_out_prio_head_[%d]_at_%d", id.c_str(), i, t);
            expr constr_expr = mk_or(all_prios);
            constr_map.insert(named_constr(constr_name, constr_expr));

            for (unsigned int p = 0; p < output_voq_map[i].size(); p++) {
                expr_vector others_zero(net_ctx.z3_ctx());
                for (unsigned int k = 0; k < out_prio_head_[i].size(); k++) {
                    if (p == k) continue;
                    others_zero.push_back(!out_prio_head_[i][k][t]);
                }
                sprintf(
                    constr_name, "%s_only_out_prio_head[%d][%d][%d]_is_one", id.c_str(), i, p, t);
                expr constr_expr = implies(out_prio_head_[i][p][t], mk_and(others_zero));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // cout << 7 << endl;

        // Update heads
        if (t == 0) {
            for (unsigned int i = 0; i < port_cnt; i++) {
                // Inputs

                if (input_voq_map[i].size() > 0) {
                    sprintf(constr_name, "%s_in_prio_head[%d][0][0]_is_one", id.c_str(), i);
                    expr constr_expr = in_prio_head_[i][0][t];
                    constr_map.insert(named_constr(constr_name, constr_expr));

                    for (unsigned int p = 1; p < input_voq_map[i].size(); p++) {
                        sprintf(
                            constr_name, "%s_in_prio_head[%d][%d][0]_is_zero", id.c_str(), i, p);
                        expr constr_expr = !in_prio_head_[i][p][t];
                        constr_map.insert(named_constr(constr_name, constr_expr));
                    }
                }

                // Outputs
                if (output_voq_map[i].size() > 0) {
                    sprintf(constr_name, "%s_out_prio_head[%d][0][0]_is_one", id.c_str(), i);
                    expr constr_expr = out_prio_head_[i][0][t];
                    constr_map.insert(named_constr(constr_name, constr_expr));

                    for (unsigned int p = 1; p < output_voq_map[i].size(); p++) {
                        sprintf(
                            constr_name, "%s_out_prio_head[%d][%d][0]_is_zero", id.c_str(), i, p);
                        constr_expr = !out_prio_head_[i][p][t];
                        constr_map.insert(named_constr(constr_name, constr_expr));
                    }
                }
            }
        } else {
            unsigned int prev_t = t - 1;
            for (unsigned int i = 0; i < port_cnt; i++) {
                for (unsigned int i_ind_of_j = 0; i_ind_of_j < input_voq_map[i].size();
                     i_ind_of_j++) {
                    // TODO: maybe add something to also set the other ones to zero
                    sprintf(constr_name,
                            "%s_update_in_prio_head[%d][%d][%d]_to_one",
                            id.c_str(),
                            i,
                            i_ind_of_j,
                            t);
                    expr constr_expr = implies(in_to_out_[i][i_ind_of_j][prev_t],
                                               in_prio_head_[i][i_ind_of_j][t]);
                    constr_map.insert(named_constr(constr_name, constr_expr));
                }

                expr_vector all_zero(net_ctx.z3_ctx());
                for (unsigned int k = 0; k < input_voq_map[i].size(); k++) {
                    all_zero.push_back(!in_to_out_[i][k][prev_t]);
                }
                for (unsigned int i_ind_of_j = 0; i_ind_of_j < input_voq_map[i].size();
                     i_ind_of_j++) {
                    sprintf(constr_name,
                            "%s_dont_update_in_prio_head[%d][%d][%d]",
                            id.c_str(),
                            i,
                            i_ind_of_j,
                            t);
                    expr constr_expr = implies(mk_and(all_zero),
                                               in_prio_head_[i][i_ind_of_j][t] ==
                                                   in_prio_head_[i][i_ind_of_j][prev_t]);
                    constr_map.insert(named_constr(constr_name, constr_expr));
                }
            }

            for (unsigned int j = 0; j < port_cnt; j++) {
                for (unsigned int j_ind_of_i = 0; j_ind_of_i < output_voq_map[j].size();
                     j_ind_of_i++) {
                    // TODO: maybe add something to also set the other ones to zero
                    sprintf(constr_name,
                            "%s_update_out_prio_head[%d][%d][%d]",
                            id.c_str(),
                            j,
                            j_ind_of_i,
                            t);
                    expr constr_expr = implies(out_from_in_[j][j_ind_of_i][prev_t],
                                               out_prio_head_[j][j_ind_of_i][t]);
                    constr_map.insert(named_constr(constr_name, constr_expr));
                }

                expr_vector all_zero(net_ctx.z3_ctx());
                for (unsigned int k = 0; k < output_voq_map[j].size(); k++) {
                    all_zero.push_back(!out_from_in_[j][k][prev_t]);
                }
                for (unsigned int j_ind_of_i = 0; j_ind_of_i < output_voq_map[j].size();
                     j_ind_of_i++) {
                    sprintf(constr_name,
                            "%s_dont_update_out_prio_head[%d][%d][%d]",
                            id.c_str(),
                            j,
                            j_ind_of_i,
                            t);
                    expr constr_expr = implies(mk_and(all_zero),
                                               out_prio_head_[j][j_ind_of_i][t] ==
                                                   out_prio_head_[j][j_ind_of_i][prev_t]);
                    constr_map.insert(named_constr(constr_name, constr_expr));
                }
            }
        }

        // cout << 8 << endl;
        //  Move packets
        for (unsigned int i = 0; i < port_cnt; i++) {
            for (unsigned int i_ind_of_j = 0; i_ind_of_j < input_voq_map[i].size(); i_ind_of_j++) {
                unsigned int voq_ind = input_voq_map[i][i_ind_of_j];
                unsigned int j = voq_output_map[voq_ind];

                // Enqueue the packet
                sprintf(constr_name, "%s_in_%d_packet_to_out_%d_at_%d", id.c_str(), i, j, t);
                expr constr_expr = implies(in_to_out_[i][i_ind_of_j][t],
                                           out_queues[j]->enqs(0)[t] ==
                                               in_queues[voq_ind]->elem(0)[t]);
                constr_map.insert(named_constr(constr_name, constr_expr));

                // Set input queues deq_cnt
                sprintf(constr_name, "%s_in_%d_deq_cnt[%d]_is_one", id.c_str(), voq_ind, t);
                constr_expr = implies(in_to_out_[i][i_ind_of_j][t],
                                      in_queues[voq_ind]->deq_cnt(t) == 1);
                constr_map.insert(named_constr(constr_name, constr_expr));

                sprintf(constr_name, "%s_in_%d_deq_cnt[%d]_is_zero", id.c_str(), voq_ind, t);
                constr_expr = implies(!in_to_out_[i][i_ind_of_j][t],
                                      in_queues[voq_ind]->deq_cnt(t) == 0);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        // cout << 9 << endl;

        for (unsigned int j = 0; j < port_cnt; j++) {
            // Ensure no enqueues to output if it is not matched
            expr_vector all_zeros(net_ctx.z3_ctx());
            all_zeros.push_back(net_ctx.bool_val(true));

            for (unsigned int k = 0; k < output_voq_map[j].size(); k++) {
                all_zeros.push_back(!out_from_in_[j][k][t]);
            }
            sprintf(constr_name, "%s_no_enqs_in_out_%d_at_%d", id.c_str(), j, t);
            expr constr_expr = implies(mk_and(all_zeros),
                                       out_queues[j]->enqs(0)[t] == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));

            // Make sure nothing else gets pushed to the output queue
            for (unsigned int e = 1; e < out_queues[j]->max_enq(); e++) {
                sprintf(constr_name, "%s_out_%d_elem[%d][%d]_is_null", id.c_str(), j, e, t);
                expr constr_expr = out_queues[j]->enqs(e)[t] == net_ctx.null_pkt();
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        /*
        // Bounds on meta3 (arrival time)
        for (unsigned int i = 0; i < in_queue_cnt(); i++){
            Queue* queue = in_queues[i];
            for (unsigned int e = 0; e < queue->max_enq(); e++){
                expr pkt = queue->enqs(e)[t];
                expr meta3 = net_ctx.pkt2meta3(pkt);

                sprintf(constr_name, "%s_in_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i, e, t);
                expr constr_expr = meta3 <= (int) t && meta3 >= 0;
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        for (unsigned int i = 0; i < out_queue_cnt(); i++){
            Queue* queue = out_queues[i];
            for (unsigned int e = 0; e < queue->max_enq(); e++){
                expr pkt = queue->enqs(e)[t];
                expr meta3 = net_ctx.pkt2meta3(pkt);

                sprintf(constr_name, "%s_out_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i, e, t);
                expr constr_expr = meta3 <= (int) t && meta3 >= 0;
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
        */
    }
}

bool SwitchXBarQM::connected(unsigned int in_port, unsigned int out_port) {
    for (unsigned int i_ind_of_j = 0; i_ind_of_j < input_voq_map[in_port].size(); i_ind_of_j++) {
        unsigned int voq_ind = input_voq_map[in_port][i_ind_of_j];
        if (voq_output_map[voq_ind] == out_port) return true;
    }
    return false;
}

expr SwitchXBarQM::in_to_out(unsigned int in_port, unsigned int out_port, unsigned int t) {
    for (unsigned int i_ind_of_j = 0; i_ind_of_j < input_voq_map[in_port].size(); i_ind_of_j++) {
        unsigned int voq_ind = input_voq_map[in_port][i_ind_of_j];
        if (voq_output_map[voq_ind] == out_port) {
            return in_to_out_[in_port][i_ind_of_j][t];
        }
    }
    cout << "SwitchXBarQM::in_to_out: ERROR: should not reach here if used properly" << endl;
    return in_to_out_[in_port][input_voq_map[in_port].size() - 1][t];
}

expr SwitchXBarQM::out_from_in(unsigned int out_port, unsigned int in_port, unsigned int t) {
    for (unsigned int j_ind_of_i = 0; j_ind_of_i < input_voq_map[out_port].size(); j_ind_of_i++) {
        unsigned int voq_ind = output_voq_map[out_port][j_ind_of_i];
        if (voq_input_map[voq_ind] == in_port) {
            return out_from_in_[out_port][j_ind_of_i][t];
        }
    }
    cout << "SwitchXBarQM::out_from_in: ERROR: should not reach here if used properly" << endl;
    return out_from_in_[out_port][output_voq_map[out_port].size() - 1][t];
}


expr SwitchXBarQM::in_prio_head(unsigned int in_port, unsigned int out_port, unsigned int t) {
    for (unsigned int i_ind_of_j = 0; i_ind_of_j < input_voq_map[in_port].size(); i_ind_of_j++) {
        unsigned int voq_ind = input_voq_map[in_port][i_ind_of_j];
        if (voq_output_map[voq_ind] == out_port) {
            return in_prio_head_[in_port][i_ind_of_j][t];
        }
    }
    cout << "SwitchXBarQM::in_prio_head_: ERROR: should not reach here if used properly" << endl;
    return in_prio_head_[in_port][input_voq_map[in_port].size() - 1][t];
}

expr SwitchXBarQM::out_prio_head(unsigned int out_port, unsigned int in_port, unsigned int t) {
    for (unsigned int j_ind_of_i = 0; j_ind_of_i < output_voq_map[out_port].size(); j_ind_of_i++) {
        unsigned int voq_ind = output_voq_map[out_port][j_ind_of_i];
        if (voq_input_map[voq_ind] == in_port) {
            return out_prio_head_[out_port][j_ind_of_i][t];
        }
    }
    cout << "SwitchXBarQM::out_prio_head_: ERROR: should not reach here if used properly" << endl;
    return out_prio_head_[out_port][output_voq_map[out_port].size() - 1][t];
}

/* ********** LeafXBar ************ */

LeafXBarQM::LeafXBarQM(cid_t id,
                       unsigned int leaf_id,
                       unsigned int servers_per_leaf,
                       unsigned int spine_cnt,
                       unsigned int total_time,
                       std::vector<unsigned int> voq_input_map,
                       std::vector<unsigned int> voq_output_map,
                       std::vector<QueueInfo> in_queue_info,
                       std::vector<QueueInfo> out_queue_info,
                       NetContext& net_ctx):
SwitchXBarQM(id, total_time, voq_input_map, voq_output_map, in_queue_info, out_queue_info, net_ctx),
leaf_id(leaf_id),
servers_per_leaf(servers_per_leaf),
spine_cnt(spine_cnt) {
}

void LeafXBarQM::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {
    SwitchXBarQM::add_constrs(net_ctx, constr_map);

    // cout << "done with normal constraints" << endl;
    char constr_name[100];

    // TODO: maybe do it for enqs? it seems to increase the runtime though.

    for (unsigned int t = 0; t < total_time; t++) {

        // cout << 0 << endl;
        for (unsigned int i = 0; i < servers_per_leaf; i++) {
            Queue* queue = out_queues[i];
            unsigned int dst = (leaf_id * servers_per_leaf) + i;
            for (unsigned int p = 0; p < queue->size(); p++) {
                expr pkt = queue->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta1(pkt);

                sprintf(constr_name, "%s_meta_bounds_outq%d[%d][%d]", id.c_str(), i, p, t);
                expr constr_expr = implies(pkt_val, pkt_meta == (int) dst);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            /*
            for (unsigned int p = 0; p < queue->max_enq(); p++){
                expr pkt = queue->enqs(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta(pkt);

                sprintf(constr_name, "%s_meta_bounds_outq%d[%d][%d]", id.c_str(), i, p, t);
                expr constr_expr = implies(pkt_val, pkt_meta == (int) dst);
                constr_map.insert(named_constr(constr_name, constr_expr));

            } */
        }

        // cout << 1 << endl;

        for (unsigned int i = 0; i < spine_cnt; i++) {
            unsigned int qid = servers_per_leaf + i;
            Queue* queue = out_queues[qid];

            for (unsigned int p = 0; p < queue->size(); p++) {
                expr pkt = queue->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta1(pkt);

                expr_vector bounds(net_ctx.z3_ctx());
                for (unsigned int d = 0; d < servers_per_leaf; d++) {
                    unsigned int dst = (leaf_id * servers_per_leaf) + d;
                    bounds.push_back(pkt_meta != (int) dst);
                }

                sprintf(constr_name, "%s_meta_bounds_outq%d[%d][%d]", id.c_str(), qid, p, t);
                expr constr_expr = implies(pkt_val, mk_and(bounds));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        /*
        // Bounds on meta3 (arrival time)
        for (unsigned int i = 0; i < in_queue_cnt(); i++){
            Queue* queue = in_queues[i];
            for (unsigned int e = 0; e < queue->max_enq(); e++){
                expr pkt = queue->enqs(e)[t];
                expr meta3 = net_ctx.pkt2meta3(pkt);

                sprintf(constr_name, "%s_in_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i, e, t);
                expr constr_expr = meta3 <= (int) t && meta3 >= 0;
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        for (unsigned int i = 0; i < out_queue_cnt(); i++){
            Queue* queue = out_queues[i];
            for (unsigned int e = 0; e < queue->max_enq(); e++){
                expr pkt = queue->enqs(e)[t];
                expr meta3 = net_ctx.pkt2meta3(pkt);

                sprintf(constr_name, "%s_out_%d_enqs[%d][%d]_meta3_bound", id.c_str(), i, e, t);
                expr constr_expr = meta3 <= (int) t && meta3 >= 0;
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
        */
    }
}

ReducedLeafXBarQM::ReducedLeafXBarQM(cid_t id,
                                     unsigned int leaf_id,
                                     unsigned int starting_qid,
                                     unsigned int servers_per_leaf,
                                     unsigned int spine_cnt,
                                     unsigned int total_time,
                                     std::vector<unsigned int> voq_input_map,
                                     std::vector<unsigned int> voq_output_map,
                                     std::vector<QueueInfo> in_queue_info,
                                     std::vector<QueueInfo> out_queue_info,
                                     NetContext& net_ctx):
SwitchXBarQM(id, total_time, voq_input_map, voq_output_map, in_queue_info, out_queue_info, net_ctx),
leaf_id(leaf_id),
starting_qid(starting_qid),
servers_per_leaf(servers_per_leaf),
spine_cnt(spine_cnt) {
}


void ReducedLeafXBarQM::add_constrs(NetContext& net_ctx, std::map<std::string, expr>& constr_map) {
    SwitchXBarQM::add_constrs(net_ctx, constr_map);

    char constr_name[100];

    // TODO: maybe do it for enqs? it seems to increase the runtime though.

    for (unsigned int t = 0; t < total_time; t++) {
        for (unsigned int i = 0; i < servers_per_leaf; i++) {
            Queue* queue = out_queues[i];
            unsigned int dst = starting_qid + i;
            for (unsigned int p = 0; p < queue->size(); p++) {
                expr pkt = queue->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta1(pkt);

                sprintf(constr_name, "%s_meta_bounds_outq%d[%d][%d]", id.c_str(), i, p, t);
                expr constr_expr = implies(pkt_val, pkt_meta == (int) dst);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }

            /*
            for (unsigned int p = 0; p < queue->max_enq(); p++){
                expr pkt = queue->enqs(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta(pkt);

                sprintf(constr_name, "%s_meta_bounds_outq%d[%d][%d]", id.c_str(), i, p, t);
                expr constr_expr = implies(pkt_val, pkt_meta == (int) dst);
                constr_map.insert(named_constr(constr_name, constr_expr));

            } */
        }


        for (unsigned int i = 0; i < spine_cnt; i++) {
            unsigned int qid = servers_per_leaf + i;
            Queue* queue = out_queues[qid];

            for (unsigned int p = 0; p < queue->size(); p++) {
                expr pkt = queue->elem(p)[t];
                expr pkt_val = net_ctx.pkt2val(pkt);
                expr pkt_meta = net_ctx.pkt2meta1(pkt);

                expr_vector bounds(net_ctx.z3_ctx());
                for (unsigned int d = 0; d < servers_per_leaf; d++) {
                    unsigned int dst = starting_qid + d;
                    bounds.push_back(pkt_meta != (int) dst);
                }

                sprintf(constr_name, "%s_meta_bounds_outq%d[%d][%d]", id.c_str(), qid, p, t);
                expr constr_expr = implies(pkt_val, mk_and(bounds));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
    }
}
