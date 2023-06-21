#pragma once
#include "../SyntaxAnalysis/ParseNode.hpp"

class Diagnoser {
private:
	unordered_set<string> types = {"int", "short", "float", "double", "bool", "string", "void"};
	unordered_map<string, vector<string>> functions; // name, types; holds all the functions currently defined in the program
	// Stack of scopes, front = current scope, back = global scope
	deque<unordered_map<string, string>> scopes = {}; // name, type
	string insideFunc = "";

	/**
	 * @brief queries the global and local variables for a variable
	 * @param var the name of the variable
	 * @return the type of the variable, or "" if it doesn't exist
	 */
	string queryVar(string);

	/**
	 * @brief recursively searches for return statement in a function to find if it exists and the type it returns
	 * @param root the parent node
	 */
	string checkReturns(ParseNode*);

	/**
	 * @brief check the validity of a variable definition
	 * @details see if the definition references itself
	 * @param root the parent node
	 * @param varName the name of the variable being defined
	 * @return true if a variable definition is valid
	 */
	bool checkDefine(ParseNode*, string);

	/**
	 * @brief checks for nonsense in the current node
	 * @param root the parent node
	 * @return true if the node is valid, false otherwise
	 */
	bool check(ParseNode*);

	/**
	 * @brief attempts to hoist the grandchildren up and removes the child
	 * @param root the parent node
	 */
	void hoist(ParseNode*);

	/**
	 * @brief Does cleanup on the current node
	 * @param root the root of the subtree to be cleaned
	 *
	 */
	void clean(ParseNode*);

public:
	string error;

	Diagnoser();

	/**
	 * @brief recursively traverses tree to find points of nonsense, erroring if it doesn't make sense and cleaning up redundant nodes
	 * @param root the root of the ast
	 * @return true if the ast is valid, false otherwise
	 */
	bool diagnose(ParseNode*);
};
