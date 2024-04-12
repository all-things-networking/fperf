#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <vector>
#include <string>

extern std::vector<std::string> globalArgs;

// Map to count how many times each optimization step improves the solution
extern std::map<std::string, unsigned int> opt_count;

extern std::map<std::string, vector<unsigned int>> before_cost;
extern std::map<std::string, vector<unsigned int>> after_cost;

#endif // GLOBALVARS_H
