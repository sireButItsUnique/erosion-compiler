#pragma once
#include "../SyntaxAnalysis/ParseNode.hpp"

class Diagnoser {
private:
	unordered_set<string> types = {"int", "float", "bool", "string", "void"};
    unordered_map<string, vector<string>> args; // name, types
    unordered_map<string, string> vars; // name, type

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

    /**
    * @brief recursively traverses tree to find points of nonsense, erroring if it doesn't make sense and cleaning up redundant nodes (calls clean function)
    * @param root the root of the ast
	* @return true if the ast is valid, false otherwise
    */
    bool diagnose(ParseNode*);
};
