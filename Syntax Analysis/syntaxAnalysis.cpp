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
    void matchToken(SyntaxToken* token) {
        
        if (this->children[this->children.size() - 1]->complete) {
            //add new child
            for (auto rule : this->rules->rules[this->root->type]) { //iterate thru all possible rule syntaxes

            //if <expression> {<statement>}
                if (rule.size() >= this->children.size()) {
                    if (rule[this->children.size()][0] == '<') {

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

    ParseTree(string root, Grammar* grammar) {
        this->root = new ParseNode(root);
        this->rules = grammar;

        return;
    }

    void build() {
        SyntaxToken* next = lexer->nextToken();

        while (next) {
            this->matchToken(next);

            if (next->type == keyword) {
                string text = next->text;

            }
            
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