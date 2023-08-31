//
//  example.cpp
//  FPerf
//
//  Created by Mina Tahmasbi Arashloo on 12/15/20.
//  Copyright © 2020 Mina Tahmasbi Arashloo. All rights reserved.
//

#include "example.hpp"

#include <iostream>

ostream& operator<<(ostream& os, const Example& eg) {
    os << "total time: " << eg.total_time << endl;
    os << eg.query_qid << endl;

    os << "enqs:" << endl;
    for (map<cid_t, vector<unsigned int>>::const_iterator it = eg.enqs.begin(); it != eg.enqs.end();
         it++) {
        os << it->first << " ";
        for (unsigned int t = 0; t < it->second.size(); t++) {
            os << it->second[t] << " ";
        }
        os << std::endl;
    }

    os << "deqs:" << endl;
    for (map<cid_t, vector<unsigned int>>::const_iterator it = eg.deqs.begin(); it != eg.deqs.end();
         it++) {
        os << it->first << " ";
        for (unsigned int t = 0; t < it->second.size(); t++) {
            os << it->second[t] << " ";
        }
        os << std::endl;
    }

    os << "pkts metadata: " << endl;
    for (map<cid_t, vector<vector<int>>>::const_iterator it = eg.enqs_meta1.begin();
         it != eg.enqs_meta1.end();
         it++) {
        cid_t qid = it->first;
        os << qid << " ";
        for (unsigned int t = 0; t < it->second.size(); t++) {
            vector<int> meta1 = it->second[t];
            vector<int> meta2 = eg.enqs_meta2.at(qid)[t];
            for (unsigned int p = 0; p < meta1.size(); p++) {
                os << "[" << meta1[p] << ", " << meta2[p] << "] ";
            }
            os << " | ";
        }
        os << std::endl;
    }
    return os;
}


void write_examples_to_file(deque<Example*>& examples, string fname) {
    ofstream out;
    out.open(fname);

    for (unsigned int i = 0; i < examples.size(); i++) {
        Example* eg = examples[i];

        out << eg->query_qid << " ";
        out << eg->total_time << " ";
        out << eg->enqs.size() << std::endl;

        for (map<cid_t, vector<unsigned int>>::iterator it = eg->enqs.begin(); it != eg->enqs.end();
             it++) {
            out << it->first << " ";
            for (unsigned int t = 0; t < it->second.size(); t++) {
                out << it->second[t] << " ";
            }
            out << std::endl;
        }

        for (map<cid_t, vector<vector<int>>>::iterator it = eg->enqs_meta1.begin();
             it != eg->enqs_meta1.end();
             it++) {
            out << it->first << " ";
            for (unsigned int t = 0; t < it->second.size(); t++) {
                vector<int> meta = it->second[t];
                for (unsigned int p = 0; p < meta.size(); p++) {
                    out << meta[p] << " ";
                }
                out << "| ";
            }
            out << std::endl;
        }

        for (map<cid_t, vector<vector<int>>>::iterator it = eg->enqs_meta2.begin();
             it != eg->enqs_meta2.end();
             it++) {
            out << it->first << " ";
            for (unsigned int t = 0; t < it->second.size(); t++) {
                vector<int> meta = it->second[t];
                for (unsigned int p = 0; p < meta.size(); p++) {
                    out << meta[p] << " ";
                }
                out << "| ";
            }
            out << std::endl;
        }

        for (map<cid_t, vector<unsigned int>>::iterator it = eg->deqs.begin(); it != eg->deqs.end();
             it++) {
            out << it->first << " ";
            for (unsigned int t = 0; t < it->second.size(); t++) {
                out << it->second[t] << " ";
            }
            out << std::endl;
        }
    }
    out.close();
}

vector<unsigned int> u_split_line(string line) {
    vector<unsigned int> parts;

    size_t next_part = line.find(" ", 0);
    while (next_part != string::npos) {
        string part = line.substr(0, next_part);
        if (part.size() > 0) parts.push_back((unsigned int) stoi(part));
        line = line.substr(next_part + 1);
        next_part = line.find(" ", 0);
    }
    if (line.size() > 0) parts.push_back((unsigned int) stoi(line));
    return parts;
}

vector<int> s_split_line(string line) {
    vector<int> parts;

    size_t next_part = line.find(" ", 0);
    while (next_part != string::npos) {
        string part = line.substr(0, next_part);
        if (part.size() > 0) parts.push_back(stoi(part));
        line = line.substr(next_part + 1);
        next_part = line.find(" ", 0);
    }
    if (line.size() > 0) parts.push_back(stoi(line));
    return parts;
}


/*
 File format:
 query_qid total_time num_queues
 num_queue lines for enqs:
 qid enq1 enq2 ...
 num_queue lines for enqs_meta1:
 qid meta00 meta01 meta02 ... | meta10 meta11 ...
 num_queue lines in the format above for enqs_meta2
 num_queue lines for deqs:
 qid deq1 deq2 ...
 */
void read_examples_from_file(std::deque<Example*>& examples, std::string fname) {
    ifstream in;
    in.open(fname);

    if (!in.is_open()) {
        std::cout << "Cannot open file: " << fname << std::endl;
        return;
    }
    string line;
    while (getline(in, line)) {
        Example* eg = new Example();

        size_t next_part = line.find(" ", 0);
        eg->query_qid = line.substr(0, next_part);
        line = line.substr(next_part + 1);

        vector<unsigned int> u_parts = u_split_line(line);
        if (u_parts.size() != 2) {
            std::cout << "Bad input file format" << std::endl;
            in.close();
            return;
        }

        eg->total_time = u_parts[0];
        unsigned int num_queues = u_parts[1];

        for (unsigned int i = 0; i < num_queues; i++) {
            getline(in, line);

            size_t next_part = line.find(" ", 0);
            cid_t qid = line.substr(0, next_part);
            line = line.substr(next_part + 1);

            u_parts = u_split_line(line);

            if (u_parts.size() != eg->total_time) {
                std::cout << "Bad input file format" << std::endl;
                in.close();
                return;
            }

            eg->enqs[qid] = u_parts;
        }

        for (unsigned int i = 0; i < num_queues; i++) {
            getline(in, line);
            size_t next_part = line.find(" ", 0);
            cid_t qid = line.substr(0, next_part);
            line = line.substr(next_part + 1);

            vector<vector<int>> enqs_meta1;
            for (unsigned int t = 0; t < eg->total_time; t++) {
                size_t next_part = line.find("|", 0);

                if (next_part == string::npos) {
                    std::cout << "Bad input file format" << std::endl;
                    in.close();
                    return;
                }

                string enqs_meta_str = line.substr(0, next_part);
                line = line.substr(next_part + 1);

                vector<int> s_parts = s_split_line(enqs_meta_str);
                enqs_meta1.push_back(s_parts);
            }

            eg->enqs_meta1[qid] = enqs_meta1;
        }

        for (unsigned int i = 0; i < num_queues; i++) {
            getline(in, line);
            size_t next_part = line.find(" ", 0);
            cid_t qid = line.substr(0, next_part);
            line = line.substr(next_part + 1);

            vector<vector<int>> enqs_meta2;
            for (unsigned int t = 0; t < eg->total_time; t++) {
                size_t next_part = line.find("|", 0);

                if (next_part == string::npos) {
                    std::cout << "Bad input file format" << std::endl;
                    in.close();
                    return;
                }

                string enqs_meta_str = line.substr(0, next_part);
                line = line.substr(next_part + 1);

                vector<int> s_parts = s_split_line(enqs_meta_str);
                enqs_meta2.push_back(s_parts);
            }

            eg->enqs_meta2[qid] = enqs_meta2;
        }

        for (unsigned int i = 0; i < num_queues; i++) {
            getline(in, line);

            size_t next_part = line.find(" ", 0);
            cid_t qid = line.substr(0, next_part);
            line = line.substr(next_part + 1);

            u_parts = u_split_line(line);

            if (u_parts.size() != eg->total_time) {
                std::cout << "Bad input file format" << std::endl;
                in.close();
                return;
            }

            eg->deqs[qid] = u_parts;
        }

        examples.push_back(eg);
    }
}

ostream& operator<<(ostream& os, const IndexedExample& eg) {
    os << "total time: " << eg.total_time << endl;
    os << eg.query_qid << endl;

    os << "enqs:" << endl;
    for (unsigned int i = 0; i < eg.enqs.size(); i++) {
        os << i << " ";
        for (unsigned int t = 0; t < eg.enqs[i].size(); t++) {
            os << eg.enqs[i][t] << " ";
        }
        os << std::endl;
    }

    os << "deqs:" << endl;
    for (unsigned int i = 0; i < eg.deqs.size(); i++) {
        os << i << " ";
        for (unsigned int t = 0; t < eg.deqs[i].size(); t++) {
            os << eg.deqs[i][t] << " ";
        }
        os << std::endl;
    }

    os << "pkts metadata: " << endl;
    for (unsigned int i = 0; i < eg.enqs_meta1.size(); i++) {
        os << i << " ";
        for (unsigned int t = 0; t < eg.enqs_meta1[i].size(); t++) {
            vector<int> meta1 = eg.enqs_meta1[i][t];
            vector<int> meta2 = eg.enqs_meta2[i][t];
            for (unsigned int p = 0; p < meta1.size(); p++) {
                os << "[" << meta1[p] << ", " << meta2[p] << "] ";
            }
            os << " | ";
        }
        os << std::endl;
    }
    return os;
}

ostream& operator<<(ostream& os, const Trace& tr) {
    os << "total time: " << tr.total_time << endl;

    os << "enqs:" << endl;
    for (unsigned int i = 0; i < tr.enqs.size(); i++) {
        os << i << " ";
        for (unsigned int t = 0; t < tr.enqs[i].size(); t++) {
            os << tr.enqs[i][t] << " ";
        }
        os << std::endl;
    }
    return os;
}
