// change this later to make code more optimized
#include <bits/stdc++.h>

#include "../LexicalAnalysis/lexicalAnalysis.hpp"
#include "grammar.hpp"
using namespace std;

class ParseNode {
public:
	// name & value of node (according to grammar)
	string type, val;

	/**
	 * @brief Construct a new ParseNode object
	 * @param type The type of the node
	 * @param val The value of the node
	 */
	ParseNode(string, string="");
};

class ParseTree {
private:
	ParseNode* root;  // root of current parse tree
	vector<ParseTree*> children;  // children of the root node of the current parse tree
	Lexer* lexer;
	bool complete = false;
	vector<int> whitelist;

	/**
	 * @brief Add a list of children to the current parse tree
	 * @param children The list of children to be added
	 */
	void addChildren(vector<string>&);

	/**
	 * @brief Add a child to the current parse tree
	 * @param child The child to be added
	 */
	void addChild(string);

	/**
	 * @brief Make a whitelist idk robert change this description
	 * @param rule ...
	 * @param token ...
	 * @param currTerm ...
	 * @param idx ...
	 * @return
	 */
	bool constructWhitelist(string, SyntaxToken*, string, int);

	/**
	 * @brief Recursively break down the rule
	 * @param rule The rule to be broken down
	 * @param token The current token
	 * @param path The current path
	 */
	bool breakDown(string, SyntaxToken*, vector<int>&);

	void markComplete();

	/**
	 * @brief Recursively build up the parse tree
	 * @param rule The current rule
	 * @param token The current token
	 * @param path The current path
	 * @param i The current index of the path
	 */
	void buildUp(string, SyntaxToken*, vector<int>&, int);

	/**
	 * @brief Match the token
	 * @param token The token to be matched
	 */
	void matchToken(SyntaxToken*);

public:
	ParseTree(Lexer*);

	ParseTree(ParseNode*);

	ParseTree(string);

	void build();

	// for testing (remove in prod)
	void print();

	void print(int);
};
