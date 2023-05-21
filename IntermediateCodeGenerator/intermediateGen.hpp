#pragma once
#include "../SyntaxAnalysis/ParseNode.hpp"

class IRGenerator {
private:
	/**
	 * @brief stack of variables and their sizes
	 * @brief .first is the variable name, and .second is the size of that variable
	 */
	stack<pair<string, int>> st;

public:
	/**
	 * @brief generates intermediary representation of the program
	 * @param root 
	 * @return vector<string> 
	 */
	vector<string> generateIR(ParseNode *root);
};
