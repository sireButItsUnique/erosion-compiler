// AKA parser

// change this later to make code more optimized
#include <bits/stdc++.h>
#include "../Lexical Analysis/lexicalAnalysis.cpp"
using namespace std;

class ParseNode {
public:
    // name & value of node (according to grammar)
    string name, val;

    ParseNode(string name, string val) {
        this->name = name;
        this->val = val;
        
        return;
    }

    ParseNode(string name) {
        this->name = name;
        
        return;
    }
};

class ParseTree {
private:
    ParseNode* root; // root of current parse tree
    vector<ParseTree*> children; // children of the root node of the current parse tree
    Lexer* lexer;

    // add a vector of children to current root node
    void addChildren(vector<string>& children) {
        for (auto child : children) {
            this->children.push_back(new ParseTree(child));
        }

        return;
    }

    // add a child to current root node
    void addChild(string child) {
        this->children.push_back(new ParseTree(child));
    
        return;
    }
    
public:
    ParseTree(Lexer* lexer) {
        this->lexer = lexer;

        return;
    }

    ParseTree(string root) {
        this->root = new ParseNode(root);

        return;
    }

    void build() {
        
    }

    // for testing (remove in prod)
    void print(int depth) {
        for (int i = 0; i < depth; i++) {
            cout << "  ";
        }
        cout << this->root->name << '\n';
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