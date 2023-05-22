#pragma once
#include "../SyntaxAnalysis/ParseNode.hpp"

#define SENTINEL "驔"

struct Thing {
	int size;
	void *data;
};

class IRGenerator {
private:
	/**
	 * @brief stack of variables and their sizes
	 * @brief .first is the variable name, and .second is the size of that variable
	 */
	stack<pair<string, int>> st;
	vector<pair<string, int>> bss;
	vector<pair<string, Thing>> data;
	int labelCnt = 0;
	unordered_map<string, int> sizes = {{"bool", 1}, {"short", 2}, {"int", 4}, {"float", 4}, {"long", 8}, {"double", 8}, {"void", 0}};

	/*
	 * @brief generates current label and increments
	*/
	string labelify();

public:
	/**
	 * @brief generates intermediary representation of the program
	 * @param root the root of the tree
	 * @param code output vector representing our code
	 * @return how much stuff got pushed
	 */
	int generateIR(ParseNode *root, vector<string>&);
};
