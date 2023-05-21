#pragma once
#include "../SyntaxAnalysis/ParseNode.hpp"

class IRGenerator {
private:
	/**
	 * @brief stack of variables and their sizes
	 * @brief .first is the variable name, and .second is the size of that variable
	 */
	stack<pair<string, int>> st;
	int labelCnt = 0;

public:
	/**
	 * @brief generates intermediary representation of the program
	 * @param root the root of the tree
	 * @param code output vector representing our code
	 * @return how much stuff got pushed
	 */
	int generateIR(ParseNode *root, vector<string>&);
};
