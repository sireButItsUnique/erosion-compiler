#pragma once
#include "../SyntaxAnalysis/ParseNode.hpp"

class IntermediateCodeGenerator {
private:
    // pair:
    // string is the variable name, etc. and int is the size of that variable
	stack<pair<string, int>> s;

public:
	/**
	 * @brief 
	 * @param root 
	 * @return vector<string> 
	 */
	vector<string> intermediateGen(ParseNode *root);
};
