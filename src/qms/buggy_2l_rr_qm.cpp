//
//  2l_rr_qm.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 7/30/21.
//  Copyright © 2021 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "buggy_2l_rr_qm.hpp"

Buggy2LRRQM::Buggy2LRRQM(cid_t id,
               unsigned int total_time,
               std::vector<QueueInfo> in_queue_info,
               QueueInfo out_queue_info,
               NetContext& net_ctx):
QueuingModule(id, total_time, in_queue_info,
              std::vector<QueueInfo>{out_queue_info},
              net_ctx)
{
    new_queues = new Queue(id, "new_queues", 
                           in_queues.size() + 2,
                           in_queues.size(),
                           1,
                           total_time,
                           net_ctx);
    
    old_queues = new Queue(id, "old_queues", 
                           in_queues.size() + 2,
                           1,
                           1,
                           total_time,
                           net_ctx);

    inactive_ = new vector<expr>[in_queues.size()];
    
    init(net_ctx);
}

void Buggy2LRRQM::add_proc_vars(NetContext& net_ctx){
    char vname[100];
    
    for (unsigned int q = 0; q < in_queues.size(); q++){
        for (unsigned int t = 0; t < total_time; t++){
            std::sprintf(vname, "%s_inactive[%d][%d]", id.c_str(), q, t);
            inactive_[q].push_back(net_ctx.bool_const(vname));
        }
    }
}

void Buggy2LRRQM::add_constrs(NetContext& net_ctx,
                                    std::map<std::string, expr>& constr_map){
    new_queues->add_constrs(net_ctx, constr_map);
    old_queues->add_constrs(net_ctx, constr_map);
    
    char constr_name[100];
    unsigned long in_queue_cnt = in_queues.size();
    
    Queue* outq = out_queues[0];
    
    // Init for time zero
    for (unsigned int q = 0; q < in_queue_cnt; q++){
        sprintf(constr_name, "%s_%d_inactive_at_0", id.c_str(), q);
        expr constr_expr = inactive_[q][0];
        constr_map.insert(named_constr(constr_name, constr_expr));
    }
    
    for (unsigned int t = 0; t < total_time; t++){
        expr new_queues_head = new_queues->elem(0)[t];
        expr old_queues_head = old_queues->elem(0)[t];
        
        /* *************** Activate ***************** */

        // If a queue was inactive and will be getting packets this
        // timestep, it should go into new queues
        if (t > 0){
            unsigned int prev_t = t - 1;
            for (unsigned int q = 0; q < in_queue_cnt; q++){
                Queue* queue = in_queues[q];
                expr had_enq = net_ctx.pkt2val(queue->enqs(0)[prev_t]);

                sprintf(constr_name, "%s_activate_%d_at_%d", id.c_str(), q, t);
                expr constr_expr = implies(inactive_[q][prev_t] && had_enq, !inactive_[q][t]);
                constr_map.insert(named_constr(constr_name, constr_expr));

                // keep inactive
                sprintf(constr_name, "%s_keep_inactive_%d_at_%d", id.c_str(), q, t);
                constr_expr = implies(inactive_[q][prev_t] && !had_enq, inactive_[q][t]);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
        
        expr_vector activated_vec(net_ctx.z3_ctx());
        expr_vector prev_unavailable(net_ctx.z3_ctx());
        prev_unavailable.push_back(net_ctx.bool_val(true));
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            Queue* queue = in_queues[q];
            expr has_enq = net_ctx.pkt2val(queue->enqs(0)[t]);
            
            expr_vector insert_vec(net_ctx.z3_ctx());
            for (unsigned int ind = 0; ind <= q; ind++){
                expr meta_pkt = new_queues->enqs(ind)[t];
                insert_vec.push_back(implies(prev_unavailable, 
                                     net_ctx.pkt2val(meta_pkt) && net_ctx.pkt2meta1(meta_pkt) == (int) q));

            }
            sprintf(constr_name, "%s_insert_%d_into_new_at_%d", id.c_str(), q, t);
            expr constr_expr = implies(inactive_[q][t] && has_enq, mk_or(insert_vec));
            constr_map.insert(named_constr(constr_name, constr_expr));
           
            prev_unavailable.push_back(prev_unavailable[q] && net_ctx.pkt2val(new_queues->enqs(q)[t])); 

            activated_vec.push_back(ite(inactive_[q][t] && has_enq,
                                        net_ctx.int_val(1),
                                        net_ctx.int_val(0)));
        }
        
        
        // Don't allow extra random enqs
        for (unsigned int ind = 0; ind < in_queue_cnt; ind++){
            sprintf(constr_name, "%s_new_queues_enq_cnt_bounds1_%d_at_%d",
                    id.c_str(), ind, t);
            expr constr_expr = implies(net_ctx.pkt2val(new_queues->enqs(ind)[t]),
                                       sum(activated_vec) > (int) ind);
            constr_map.insert(named_constr(constr_name, constr_expr));
            
            sprintf(constr_name, "%s_new_queues_enq_cnt_bounds2_%d_at_%d",
                    id.c_str(), ind, t);
            constr_expr = implies(sum(activated_vec) <= (int) ind,
                                       new_queues->enqs(ind)[t] == net_ctx.null_pkt());
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // no holes in enq for new queues
        for (unsigned int ind = 0; ind < in_queue_cnt - 1; ind++){
            expr enq1_val = net_ctx.pkt2val(new_queues->enqs(ind)[t]);
            expr enq2_val = net_ctx.pkt2val(new_queues->enqs(ind + 1)[t]);

            sprintf(constr_name, "%s_new_no_enq_holes_ind_%d_at_%d", id.c_str(), ind, t);
            expr constr_expr = enq1_val || !enq2_val;
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // Unique enqs for new queues
        for (unsigned int i = 0; i < in_queue_cnt - 1; i++){
            for (unsigned int j = i + 1; j < in_queue_cnt; j++){
                expr pkt1 = new_queues->enqs(i)[t];
                expr pkt2 = new_queues->enqs(j)[t];

                sprintf(constr_name, "%s_new_%d_and_%d_unique_at_%d", id.c_str(), i, j, t);
                expr constr_expr = implies(net_ctx.pkt2val(pkt1) && net_ctx.pkt2val(pkt2),
                                           net_ctx.pkt2meta1(pkt1) != net_ctx.pkt2meta1(pkt2));
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }

        /* *************** Dequeue from New Queues  ***************** */
        // Check new queues for dequeue
        
        expr_vector get_from_new_queues(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            expr deq_criteria = net_ctx.pkt2val(new_queues_head) &&
                                net_ctx.pkt2meta1(new_queues_head) == (int) q;
            expr per_queue_expr = implies(deq_criteria,
                                          outq->enqs(0)[t] == in_queues[q]->elem(0)[t]);

            get_from_new_queues.push_back(per_queue_expr);

            // Set this queue's deq_cnt to 1
            per_queue_expr = implies(deq_criteria, in_queues[q]->deq_cnt(t) == 1);
            get_from_new_queues.push_back(per_queue_expr);
        }
        sprintf(constr_name, "%s_deq_from_new_queues_at_%d", id.c_str(), t);
        constr_map.insert(named_constr(constr_name, mk_and(get_from_new_queues)));

        // set deq_cnt for new queues
        sprintf(constr_name, "%s_new_queues_deq_cnt_is_1_at_%d", id.c_str(), t);
        expr constr_expr = implies(net_ctx.pkt2val(new_queues_head), new_queues->deq_cnt(t) == 1);
        constr_map.insert(named_constr(constr_name, constr_expr));

        sprintf(constr_name, "%s_new_queues_deq_cnt_is_0_at_%d", id.c_str(), t);
        constr_expr = implies(!net_ctx.pkt2val(new_queues_head), new_queues->deq_cnt(t) == 0);
        constr_map.insert(named_constr(constr_name, constr_expr));

        /* *************** From new to old  ***************** */
        
        expr_vector from_new_to_old(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            expr dequeued_from_q = net_ctx.pkt2val(new_queues_head) &&
                                   net_ctx.pkt2meta1(new_queues_head) == (int) q;
            
            expr not_empty_next_round = net_ctx.pkt2val(in_queues[q]->elem(1)[t]) ||
                                        net_ctx.pkt2val(in_queues[q]->enqs(0)[t]);
            
            expr insert_into_old = net_ctx.pkt2val(old_queues->enqs(0)[t]) &&
                                   net_ctx.pkt2meta1(old_queues->enqs(0)[t]) == (int) q;
            
            expr per_queue_expr = implies(dequeued_from_q && not_empty_next_round,
                                          insert_into_old);
            
            from_new_to_old.push_back(per_queue_expr);
        }
        sprintf(constr_name, "%s_from_new_to_old_at_%d", id.c_str(), t);
        constr_expr = mk_and(from_new_to_old);
        constr_map.insert(named_constr(constr_name, constr_expr));
        
        // No enqs into old queues
        sprintf(constr_name, "%s_no_enqs_into_old_queues_at_%d", id.c_str(), t);
        expr_vector new_head_queue_empty_vec(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            new_head_queue_empty_vec.push_back(net_ctx.pkt2meta1(new_queues_head) == (int) q &&
                                               !net_ctx.pkt2val(in_queues[q]->elem(1)[t]) &&
                                               !net_ctx.pkt2val(in_queues[q]->enqs(0)[t]));
        }
        
        expr_vector old_head_queue_empty_vec(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            old_head_queue_empty_vec.push_back(net_ctx.pkt2meta1(old_queues_head) == (int) q &&
                                               !net_ctx.pkt2val(in_queues[q]->elem(1)[t]) &&
                                               !net_ctx.pkt2val(in_queues[q]->enqs(0)[t]));
        }
        
        expr no_old_enq1 = !net_ctx.pkt2val(new_queues_head) &&
                           !net_ctx.pkt2val(old_queues_head);
        
        expr no_old_enq2 = (net_ctx.pkt2val(new_queues_head) &&
                            mk_or(new_head_queue_empty_vec));
        
        expr no_old_enq3 = (!net_ctx.pkt2val(new_queues_head) &&
                            net_ctx.pkt2val(old_queues_head) &&
                            mk_or(old_head_queue_empty_vec));
        
        expr no_old_enq_critiera = no_old_enq1 || no_old_enq2 || no_old_enq3;
        
        constr_expr = implies(no_old_enq_critiera, old_queues->enqs(0)[t] == net_ctx.null_pkt());
        
        constr_map.insert(named_constr(constr_name, constr_expr));
        
        /* *************** Dequeue from Old  ***************** */
        // Check old queues for dequeue
        expr_vector get_from_old_queues(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            expr deq_criteria = !net_ctx.pkt2val(new_queues_head) &&
                                net_ctx.pkt2val(old_queues_head) &&
                                net_ctx.pkt2meta1(old_queues_head) == (int) q;
            expr per_queue_expr = implies(deq_criteria,
                                          outq->enqs(0)[t] == in_queues[q]->elem(0)[t]);

            get_from_old_queues.push_back(per_queue_expr);

            // Set this queue's deq_cnt to 1
            per_queue_expr = implies(deq_criteria, in_queues[q]->deq_cnt(t) == 1);
            get_from_old_queues.push_back(per_queue_expr);
        }
        sprintf(constr_name, "%s_deq_from_old_queues_at_%d", id.c_str(), t);
        constr_map.insert(named_constr(constr_name, mk_and(get_from_old_queues)));

        // set deq_cnt for old queues
        sprintf(constr_name, "%s_old_queues_deq_cnt_is_1_at_%d", id.c_str(), t);
        constr_expr = implies(!net_ctx.pkt2val(new_queues_head) &&
                              net_ctx.pkt2val(old_queues_head),
                                        old_queues->deq_cnt(t) == 1);
        constr_map.insert(named_constr(constr_name, constr_expr));

        sprintf(constr_name, "%s_old_queues_deq_cnt_is_0_at_%d", id.c_str(), t);
        constr_expr = implies(net_ctx.pkt2val(new_queues_head) ||
                              !net_ctx.pkt2val(old_queues_head),
                                        old_queues->deq_cnt(t) == 0);
        constr_map.insert(named_constr(constr_name, constr_expr));
        
        /* *************** From old to old  ***************** */
        expr_vector from_old_to_old(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            expr dequeued_from_q = !net_ctx.pkt2val(new_queues_head) &&
                                   net_ctx.pkt2val(old_queues_head) &&
                                   net_ctx.pkt2meta1(old_queues_head) == (int) q;
            
            expr not_empty_next_round = net_ctx.pkt2val(in_queues[q]->elem(1)[t]) ||
                                        net_ctx.pkt2val(in_queues[q]->enqs(0)[t]);
            
            expr insert_into_old = net_ctx.pkt2val(old_queues->enqs(0)[t]) &&
                                   net_ctx.pkt2meta1(old_queues->enqs(0)[t]) == (int) q;
            
            expr per_queue_expr = implies(dequeued_from_q && not_empty_next_round,
                                          insert_into_old);
            
            from_old_to_old.push_back(per_queue_expr);
        }
        sprintf(constr_name, "%s_from_old_to_old_at_%d", id.c_str(), t);
        constr_expr = mk_and(from_old_to_old);
        constr_map.insert(named_constr(constr_name, constr_expr));
        
        /* *************** Other ***************** */
        // If dequeued from and will be empty next round, make the queue inactive
        if (t < total_time - 1){
            for (unsigned int q = 0; q < in_queue_cnt; q++){
                expr dequeued_from_new = net_ctx.pkt2val(new_queues_head) &&
                                         net_ctx.pkt2meta1(new_queues_head) == (int) q;
                expr dequeued_from_old = !net_ctx.pkt2val(new_queues_head) &&
                                         net_ctx.pkt2val(old_queues_head) &&
                                         net_ctx.pkt2meta1(old_queues_head) == (int) q;
                
                sprintf(constr_name, "%s_%d_becomes_inactive_at_%d", id.c_str(), q, t + 1);
                constr_expr = implies((dequeued_from_new || dequeued_from_old) &&
                                       !net_ctx.pkt2val(in_queues[q]->elem(1)[t]) &&
                                       !net_ctx.pkt2val(in_queues[q]->enqs(0)[t]),
                                           inactive_[q][t + 1]);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
    
        // If a queue is active and
        //    (1) doesn't dequeue, or
        //    (2) dequeues and still has more packets
        // it should stay active
        if (t > 0){
            unsigned int prev_t = t - 1;
            for (unsigned int q = 0; q < in_queue_cnt; q++){
                expr_vector somewhere_in_queues(net_ctx.z3_ctx());
                for (unsigned int ind = 1; ind < new_queues->size(); ind++){
                    expr meta_pkt = new_queues->elem(ind)[prev_t];
                    somewhere_in_queues.push_back(net_ctx.pkt2val(meta_pkt) &&
                                                  net_ctx.pkt2meta1(meta_pkt) == (int) q);
                }
                for (unsigned int ind = 0; ind < new_queues->max_enq(); ind++){
                    expr meta_pkt = new_queues->enqs(ind)[prev_t];
                    somewhere_in_queues.push_back(net_ctx.pkt2val(meta_pkt) &&
                                                  net_ctx.pkt2meta1(meta_pkt) == (int) q);
                }
                
                for (unsigned int ind = 1; ind < old_queues->size(); ind++){
                    expr meta_pkt = old_queues->elem(ind)[prev_t];
                    somewhere_in_queues.push_back(net_ctx.pkt2val(meta_pkt) &&
                                                  net_ctx.pkt2meta1(meta_pkt) == (int) q);
                }
                for (unsigned int ind = 0; ind < old_queues->max_enq(); ind++){
                    expr meta_pkt = old_queues->enqs(ind)[prev_t];
                    somewhere_in_queues.push_back(net_ctx.pkt2val(meta_pkt) &&
                                                  net_ctx.pkt2meta1(meta_pkt) == (int) q);
                }
            
                sprintf(constr_name, "%s_keep_%d_active1_at_%d", id.c_str(), q, t);
                constr_expr = implies(!inactive_[q][prev_t] &&
                                      mk_or(somewhere_in_queues),
                                      !inactive_[q][t]);
                constr_map.insert(named_constr(constr_name, constr_expr));
                
                sprintf(constr_name, "%s_keep_%d_active2_at_%d", id.c_str(), q, t);
                constr_expr = implies(net_ctx.pkt2val(new_queues->elem(0)[prev_t]) &&
                                      net_ctx.pkt2val(old_queues->elem(0)[prev_t]) &&
                                      net_ctx.pkt2meta1(old_queues->elem(0)[prev_t]) == (int) q,
                                      !inactive_[q][t]);
                constr_map.insert(named_constr(constr_name, constr_expr));
            }
        }
        
        // Set deq_cnt to zero for all queues not dequeued from
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            expr not_chosen1 = net_ctx.pkt2val(new_queues_head) &&
                               net_ctx.pkt2meta1(new_queues_head) != (int)q;

            expr not_chosen2 = !net_ctx.pkt2val(new_queues_head) &&
                                net_ctx.pkt2val(old_queues_head) &&
                                net_ctx.pkt2meta1(old_queues_head) != (int) q;

            expr not_chosen3 = !net_ctx.pkt2val(new_queues_head) &&
                               !net_ctx.pkt2val(old_queues_head);
            
            sprintf(constr_name, "%s_deq_cnt_for_%d_is_0_at_%d", id.c_str(), q, t);
            constr_expr = implies(not_chosen1 || not_chosen2 || not_chosen3,
                                  in_queues[q]->deq_cnt(t) == 0);
            constr_map.insert(named_constr(constr_name, constr_expr));
        }

        // If every queue is inactive
        expr_vector inactive_vec(net_ctx.z3_ctx());
        for (unsigned int q = 0; q < in_queue_cnt; q++){
            inactive_vec.push_back(inactive_[q][t]);
        }
        sprintf(constr_name, "%s_if_all_inactive_at_%d", id.c_str(), t);
        constr_expr = implies(mk_and(inactive_vec),
                              outq->enqs(0)[t] == net_ctx.null_pkt());
        constr_map.insert(named_constr(constr_name, constr_expr));

        // Make sure nothing else gets pushed to the output queue
        for (unsigned int i = 1; i < outq->max_enq(); i++){
            sprintf(constr_name, "%s_only_one_output_%d_%d", id.c_str(), i, t);
            expr constr_expr = outq->enqs(i)[t] == net_ctx.null_pkt();
            constr_map.insert(named_constr(constr_name, constr_expr));
        }
    }
}

Queue* Buggy2LRRQM::new_fifo(){
    return new_queues;
}

Queue* Buggy2LRRQM::old_fifo(){
    return old_queues;
}

expr Buggy2LRRQM::inactive(unsigned int q, unsigned int t){
    return inactive_[q][t];
}
