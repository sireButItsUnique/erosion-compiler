// change this later to make code more optimized
#include <bits/stdc++.h>
#include "grammar.cpp"
#include "../Lexical Analysis/lexicalAnalysis.cpp"
using namespace std;

class ParseNode {
public:
    // name & value of node (according to grammar)
    string type, val;

    ParseNode(string type, string val) {
        this->type = type;
        this->val = val;

        return;
    }

    ParseNode(string type) {
        this->type = type;
        
        return;
    }
};

class ParseTree {
private:
    ParseNode* root; // root of current parse tree
    vector<ParseTree*> children; // children of the root node of the current parse tree
    Lexer* lexer;
    Grammar* rules;
    bool complete = false;

    // adding children
    void addChildren(vector<string>& children) {
        for (auto child : children) {
            this->children.push_back(new ParseTree(child, rules));
        }

        return;
    }

    void addChild(string child) {
        this->children.push_back(new ParseTree(child, rules));

        return;
    }
    
    // go down the tree + add nodes to tree recursively
    // return after reaching terminal operator
    // return syntax error if missing a token (e.g. a bracket)
    bool breakDown(string rule, SyntaxToken* token, vector<int>& path) {
        bool found = false;
        
        for (int i = 0; i < this->rules->rules[rule].size() - 1; i++) { //iterate thru all possible rule syntaxes
            vector<string> ruleVariation =  this->rules->rules[rule][i];
            string currTerm = ruleVariation[0];
            
            //testing if currTerm is a terminal operator (leaf node)
            if (currTerm == "TERMINAL_OP") {
                if (rule == token->tokenCodeStringify()) {
                    path.push_back(i);
                    return true;
                } 
                
                return false;
            }

            //testing if can break down more
            else if (currTerm[0] == '<') {
                found = this->breakDown(currTerm.substr(1, currTerm.back() - 1), token, path);
                
                if (found) {
                    path.push_back(i);
                    return found;
                } 
            } 
            
            //have reached constant term (leaf node)
            else {
                if (token->text == currTerm) {
                    path.push_back(i);
                    return true;
                }

                return false;
            }
        }

        return found;
    }

    void buildUp(string rule, SyntaxToken* token, vector<int>& path, int i) {
        string rawCurrTerm = this->rules->rules[rule][path[i]][0];
        string currTerm = rawCurrTerm.substr(1, rawCurrTerm.back() - 1); // slice off first and last index
        
        ParseNode* newChild = new ParseNode(currTerm);
        this->children.push_back(new ParseTree(newChild, this->rules));

        i--;
        if (!i) {
            //reached leaf node
            newChild = new ParseNode(token->tokenCodeStringify(), token->text);
            this->children.back()->children.push_back(new ParseTree(newChild, this->rules));
            this->children.back()->children.back()->complete = true;
            return;
        }
        
        this->children.back()->buildUp(currTerm, token, path, i + 1);

        //we know its complete when it has equal num of children as num of terms in the rule variation
        if (this->children.size() == this->rules->rules[rule][path[i]].size()) {
            this->complete = true;
        }
        return;
    }

    void matchToken(SyntaxToken* token) {
        if (this->children.empty() || this->children[this->children.size() - 1]->complete) {
            //add new child
            for (auto ruleVariation : this->rules->rules[this->root->type]) { //iterate thru all possible rule syntaxes
                
                //if <expression> {<statement>}
                if (ruleVariation.size() >= this->children.size()) {
                    string currTerm = ruleVariation[this->children.size()];
                    
                    if (currTerm[0] == '<') {
                        currTerm = currTerm.substr(1, currTerm.back() - 1);
                        vector<int> path;
                        
                        if (this->breakDown(currTerm, token, path)) {
                            cout << "true\n";
                            this->buildUp(this->root->type, token, path, path.size() - 1);
                        }
                    }
                }
            }
        } else {
            this->children[this->children.size() - 1]->matchToken(token);
        }
    }

public:
    ParseTree(Lexer* lexer) {
        this->lexer = lexer;
        this->root = new ParseNode("<program>");
        this->rules = new Grammar();

        return;
    }

    ParseTree(ParseNode* node, Grammar* grammar) {
        this->root = node;
        this->rules = grammar;
    }

    ParseTree(string root, Grammar* grammar) {
        this->root = new ParseNode(root);
        this->rules = grammar;

        return;
    }

    void build() {
        SyntaxToken* next = lexer->nextToken();

        while (next) {
            this->matchToken(next);
            
            next = lexer->nextToken();
        }
    }

    // for testing (remove in prod)
    void print(int depth) {
        for (int i = 0; i < depth; i++) {
            cout << "  ";
        }
        cout << this->root->type << '\n';
        for (auto& c: this->children) {
            c->print(depth + 1);
        }

        return;
    }

};


int main() {
    Lexer* lexer = new Lexer("test.cor");
    ParseTree* ast = new ParseTree(lexer);
    ast->build();
    ast->print(0);
    
    return 0;
}