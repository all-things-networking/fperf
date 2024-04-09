#ifndef GLOBALVARS_H
#define GLOBALVARS_H

#include <vector>
#include <string>

extern std::vector<std::string> globalArgs;

// Map to count how many times each optimization step improves the solution
extern std::map<std::string, unsigned int> opt_count;

#endif // GLOBALVARS_H
