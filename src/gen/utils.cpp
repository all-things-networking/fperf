//
// Created by Amir Hossein Seyhani on 7/7/25.
//

#include "utils.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

vector<vector<string> > read_wl_file(string file_path) {
    ifstream file(file_path);
    if (!file) {
        cerr << "Error: could not open file" << endl;
    }
    string line;
    vector<vector<string> > sections;
    vector<string> current;

    while (getline(file, line)) {
        if (line.rfind("###", 0) == 0) {
            if (!current.empty()) {
                sections.push_back(current);
                current.clear();
            }

            if (line.rfind("UNSAT") != -1)
                current.emplace_back("UNSAT");
            else
                current.emplace_back("SAT");
        } else {
            if (!line.empty() && line.find_first_not_of(" \t\r\n") != string::npos) {
                current.push_back(line);
            }
        }
    }

    if (!current.empty()) {
        sections.push_back(current);
    }

    return sections;
}

string join_vec(const vector<int> &v) {
    ostringstream oss;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) oss << ",";
        oss << v[i];
    }
    return oss.str();
}
