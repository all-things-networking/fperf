//
// Created by Amir Hossein Seyhani on 7/9/25.
//

#include "wl_parser.hpp"

#include "constr_extractor.hpp"

#include <ANTLRInputStream.h>
#include <format>
#include <vector>

#include "fperfLexer.h"
#include "fperfParser.h"

using namespace antlr4;

WorkloadParser::WorkloadParser(int queue_cnt, int total_time): queue_cnt(queue_cnt), total_time(total_time) {

}


void WorkloadParser::parse(string wl_line) {
    cout << "Parsing:" << wl_line << endl;
    ANTLRInputStream inputStream(wl_line);
    fperfLexer lexer(&inputStream);
    CommonTokenStream tokens(&lexer);
    fperfParser parser(&tokens);
    auto tree = parser.con();
    cout << wl_line << endl;
    ConstrExtractor* visitor = new ConstrExtractor(wl);
    visitor->visit(tree);
}

void WorkloadParser::parse(vector<string> wl_lines) {
    wl = new Workload(1000, queue_cnt, total_time);
    for (int i = 0; i < wl_lines.size(); ++i) {
        string line = wl_lines[i];
        parse(line);
    }
    cout << "Parsed workload:" << endl;
    cout << wl << endl;
}
