//
// Created by Amir Hossein Seyhani on 7/9/25.
//

#ifndef WL_PARSER_HPP
#define WL_PARSER_HPP
#include <vector>


class Workload;
using namespace std;


class WorkloadParser {
public:
    WorkloadParser(int queue_cnt, int total_time);

    void parse(string wl_line);

    void parse(vector<string> wl);
    Workload* wl;

private:
    int queue_cnt;
    int total_time;
};


#endif // WL_PARSER_HPP
