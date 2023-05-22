#pragma once
#include "../SyntaxAnalysis/ParseNode.hpp"

class Diagnoser {
private:
	unordered_set<string> types = {"int", "float", "double", "long", "short", "bool", "string", "void"};
	unordered_map<string, vector<string>> args; // name, types
	deque<unordered_map<string, string>> scopes{}; // name, type
	bool insideFunc = false;

	/**
	 * @brief queries the global and local variables for a variable
	 * @param var the name of the variable
	 * @return the type of the variable, or "" if it doesn't exist
	 */
	string queryVar(string);

	/**
	 * @brief recursively searches for return statement in a function to find if it exists and the type it returns
	 * @param root the parent node
	 * @param typeMatch a reference to a boolean which becomes true if the return statement type is same as declared function type 
	*/
	bool functionReturnType(ParseNode*, bool&);

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
