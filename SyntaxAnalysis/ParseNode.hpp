// change this later to make code more optimized
#include <bits/stdc++.h>

#include "../LexicalAnalysis/Lexer.hpp"
#include "grammar.hpp"
using namespace std;

class ParseNode {
public:
    // actual lexer
    Lexer* lexer;

    // variables for the lex token it represents
    string type;
    string val;

    // variables to help during construction of tree
    bool complete;
    vector<ParseNode*> children;

    /**
     * @brief holds a vector of integers (indices of whitelisted types in each rule)
    */
    vector<int> whitelist; 

    /**
     * @brief constructs a new ParseNode that will be the root
     * @param lexer the lexer for the tree
    */
    ParseNode(Lexer*);

    /**
     * @brief constructs a new ParseNode that will be an intermediary node, given a type
     * @param lexer the lexer for the tree
     * @param type what type the node will be
    */
    ParseNode(Lexer*, string type);

    /**
     * @brief constructs a new ParseNode that will be a leaf node, given a type and a val
     * @param lexer the lexer for the tree
     * @param type what type the token will be
     * @param val what the actual piece of code the node is representing
    */
    ParseNode(Lexer*, string type, string val);

    /**
     * @brief constructs the ast using the lexer given
    */
    void build();

    /**
     * @brief prints the nodes in the ast, will be removed in prod
    */
    void print();
    void print(int);

private:

    /**
     * @brief finds theoretical leaf node corresponding to token + branches out tree to reach it, returns false if it the token doesn't fit
     * @param SyntaxToken the token that is added as a leaf node to the tree
    */
    bool handleToken(SyntaxToken*);

    /**
     * @brief recursively breaks down rules to find leaf node, returns false if it can't find a valid path
     * @param SyntaxToken the token that must find its place in the tree
     * @param Path the resulting path that's acceptable for the token
    */
    bool findPath(SyntaxToken*, vector<int>&);

    /**
     * @brief recursively constructs a path based on path given + places token as leaf node
     * @param SyntaxToken 
     * @param 
    */
    void constructPath(SyntaxToken*, vector<int>&);
    
    /**
     * @brief updates whitelist based on the current children of the node
    */
    void updateWhitelist();

    /**
     * @brief updates completeness status of the node and all of its children recursively
    */
    void updateCompleteness();
};