#pragma once
#include "../SyntaxAnalysis/ParseNode.hpp"

#define PLACEHOLDER "驔"

struct Thing {
	int size;
	void* data;
};

class IRGenerator {
private:
	/**
	 * @brief stack of variables and their sizes
	 * @brief .first is the variable name, and .second is the size of that variable
	 */
	deque<pair<string, int>> st;
	vector<pair<string, int>> bss;
	vector<pair<string, Thing>> data;
	unordered_map<string, int> sizes = {{"bool", 1}, {"int", 4}, {"float", 4}, {"double", 8}, {"void", 0}};
	int labelCnt = 0;
	string currentLabel = "";

	/**
	 * @brief generates current label and increments
	 */
	string labelify();

	/**
	 * @brief finds a variable
	 * @param var the name of the variable
	 * @return a string addressing the variable 
	 */
	string findVar(string);

public:
	/**
	 * @brief print the internal stack (for debugging)
	 */
	void printStack();

	/**
	 * @brief generates intermediary representation of the program
	 * @param root the root of the tree
	 * @param code output vector representing our code
	 * @return how much stuff got pushed
	 */
	void generateIR(ParseNode* root, vector<string>&);
};
