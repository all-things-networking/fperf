//
//  visitor.hpp
//  FPerf
//
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "visitor.hpp"

void Visitor::set_time_range(time_range_t time_range) {
    this->time_range = time_range;
}

ExpressionGeneratorVisitor::ExpressionGeneratorVisitor(NetContext& net_ctx, const vector<Queue*>& in_queues) : net_ctx(net_ctx), in_queues(in_queues), workload_expr(net_ctx.z3_ctx().bool_val(true)), wlspec_expr(net_ctx.z3_ctx().bool_val(false)) {}

expr ExpressionGeneratorVisitor::get_workload_expr() {
    return workload_expr;
}

void ExpressionGeneratorVisitor::visit(Workload& workload) {
    workload_expr = net_ctx.z3_ctx().bool_val(true);
    while (!timedSpecs_exprs.empty()) {
        workload_expr = workload_expr && timedSpecs_exprs.top();
        timedSpecs_exprs.pop();
    }
}

void ExpressionGeneratorVisitor::visit(TimedSpec& timedSpec) {
    WlSpec* wl_spec = timedSpec.get_wl_spec();
    timedSpecs_exprs.push(wlspec_expr);
}

void ExpressionGeneratorVisitor::visit(Comp& comp) {
    expr_vector res(net_ctx.z3_ctx());
    for(unsigned int t = time_range.first; t <= time_range.second; t++) {
        m_val_expr_t lhs = lhs_exprs[t - time_range.first];
        m_val_expr_t rhs = rhs_exprs[t - time_range.first];
        Op op = operations[t - time_range.first];
        res.push_back(lhs.first && rhs.first && mk_op(lhs.second, op, rhs.second));
    }

    wlspec_expr = mk_and(res);
}

void ExpressionGeneratorVisitor::visit(Same& same) {
    Queue* queue = in_queues[same.get_queue()];
    Metric* metric = queue->get_metric(same.get_metric());
    m_val_expr_t initial_val_expr = metric->val(time_range.first);

    expr valid_expr = initial_val_expr.first;
    expr init_value_expr = initial_val_expr.second;

    expr_vector res(net_ctx.z3_ctx());
    res.push_back(valid_expr);
    expr_vector no_enq(net_ctx.z3_ctx());

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        m_val_expr_t val_expr = metric->val(t);
        res.push_back(val_expr.first);
        res.push_back(val_expr.second == init_value_expr);
        no_enq.push_back(queue->enq_cnt(t) == 0);
    }

    expr on_enq = mk_and(res);
    wlspec_expr = ite(mk_and(no_enq), net_ctx.bool_val(true), on_enq);
}

void ExpressionGeneratorVisitor::visit(Unique& uniq) {
    qset_t qset = uniq.get_qset();
    metric_t metric = uniq.get_metric();
    vector<unsigned int> queues(qset.begin(), qset.end());
    unsigned int start = time_range.first;
    unsigned int end = time_range.second;

    expr_vector unique_metric(net_ctx.z3_ctx());

    for (unsigned int t = start; t <= end; t++) {
        for (unsigned int i = 0; i < queues.size(); i++) {
            unsigned int q = queues[i];
            Queue* queue = in_queues[q];
            Metric* metric1 = queue->get_metric(metric);
            m_val_expr_t metric1_val_expr = metric1->val(t);
            expr m1_valid = metric1_val_expr.first;
            expr m1_value = metric1_val_expr.second;

            for (unsigned int j = i + 1; j < queues.size(); j++) {
                unsigned int q2 = queues[j];
                if (q2 == q) continue;
                Queue* queue2 = in_queues[q2];
                Metric* metric2 = queue2->get_metric(metric);
                m_val_expr_t metric2_val_expr = metric2->val(t);
                expr m2_valid = metric2_val_expr.first;
                expr m2_value = metric2_val_expr.second;
                unique_metric.push_back(implies(m1_valid && m2_valid, m1_value != m2_value));
            }
        }
    }
    expr constr = mk_and(unique_metric);
    wlspec_expr = constr;
}

void ExpressionGeneratorVisitor::visit(Incr& incr) {
    Queue* queue = in_queues[incr.get_queue()];
    Metric* metric = queue->get_metric(incr.get_metric());
    m_val_expr_t initial_val_expr = metric->val(time_range.first);

    expr valid_expr = initial_val_expr.first;
    expr value_expr = initial_val_expr.second;

    expr_vector res(net_ctx.z3_ctx());
    res.push_back(valid_expr);

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        m_val_expr_t val_expr = metric->val(t);
        res.push_back(val_expr.first);
        res.push_back(val_expr.second > value_expr);
        value_expr = val_expr.second;
    }

    wlspec_expr = mk_and(res);
}

void ExpressionGeneratorVisitor::visit(Decr& decr) {
    Queue* queue = in_queues[decr.get_queue()];
    Metric* metric = queue->get_metric(decr.get_metric());
    m_val_expr_t initial_val_expr = metric->val(time_range.first);

    expr valid_expr = initial_val_expr.first;
    expr value_expr = initial_val_expr.second;

    expr_vector res(net_ctx.z3_ctx());
    res.push_back(valid_expr);

    for (unsigned int t = time_range.first + 1; t <= time_range.second; t++) {
        m_val_expr_t val_expr = metric->val(t);
        res.push_back(val_expr.first);
        res.push_back(val_expr.second < value_expr);
        value_expr = val_expr.second;
    }

    wlspec_expr = mk_and(res);
}

void ExpressionGeneratorVisitor::visit(Constant& c) {
    for(unsigned int t = time_range.first; t <= time_range.second; t++) {
        rhs_exprs.push_back(m_val_expr_t(net_ctx.bool_val(true), net_ctx.int_val(c.get_value())));
    }
}

void ExpressionGeneratorVisitor::visit(Time& time) {
    for(unsigned int t = time_range.first; t <= time_range.second; t++) {
        rhs_exprs.push_back(m_val_expr_t(net_ctx.bool_val(true), net_ctx.int_val(time.get_coeff() * (t + 1))));
    }
}

void ExpressionGeneratorVisitor::visit(Indiv& indiv) {
    for(unsigned int t = time_range.first; t <= time_range.second; t++) {
        Queue* queue = in_queues[indiv.get_queue()];
        Metric* metric = queue->get_metric(indiv.get_metric());
        // TODO: This a temporary fix, after parametrizing search based on metrics we need to throw
        // exception and stop the search if metric not exists
        if (metric != nullptr)
            rhs_exprs.push_back(metric->val(t));
        else
            rhs_exprs.push_back({net_ctx.bool_val(false), net_ctx.int_val(0)});
    }
}

void ExpressionGeneratorVisitor::visit(QSum& qsum) {
    for(unsigned int t = time_range.first; t <= time_range.second; t++) {
        expr res = net_ctx.int_val(0);
        expr_vector valid(net_ctx.z3_ctx());

        qset_t qset = qsum.get_qset();
        for (qset_t::iterator it = qset.begin(); it != qset.end(); it++) {
            unsigned int q = *it;
            m_val_expr_t val_expr = in_queues[q]->get_metric(qsum.get_metric())->val(t);
            valid.push_back(val_expr.first);
            res = res + val_expr.second;
        }
        rhs_exprs.push_back(m_val_expr_t(mk_and(valid), res));
    }
}

void ExpressionGeneratorVisitor::visit(Op& op) {
    operations.push_back(op);
}
