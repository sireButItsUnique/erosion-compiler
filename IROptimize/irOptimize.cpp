#include "irOptimize.hpp"

void parseIR(string& line, vector<string>& args, string& instr) {
	instr = line.substr(0, line.find(' '));
	size_t j = line.find(' ');
	do {
		size_t k = j + 1;
		j = line.find(' ', k);
		args.push_back(line.substr(k, j - k));
	} while (j != string::npos);
}

void optimizeIR(deque<string>& code) {
	for (auto it = code.begin(); it + 1 != code.end(); it++) {
		if (it + 1 == code.end()) {
			return;
		}
		string instr, nextInstr;
		vector<string> args, nextArgs;
		parseIR(*it, args, instr);
		parseIR(*(it + 1), nextArgs, nextInstr);
		if (instr == "alloc" && nextInstr == "alloc") {
			*it = "alloc " + to_string(stoi(args[0]) + stoi(nextArgs[0]));
			code.erase(it + 1);
		}
		if (instr == "push" && nextInstr == "pop") {
			*it = "mov " + nextArgs[0] + ' ' + args[0];
			code.erase(it + 1);
		}
	}
}
