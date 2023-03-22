// AKA parser

// change this later to make code more optimized
#include <bits/stdc++.h>
using namespace std;

class Node {
public:
    string val;

    Node(string val) {
        this->val = val;
        
        return;
    }
};

class ParseTree {
public:
    Node* root;
    vector<ParseTree*> children;

    ParseTree(string root) {
        this->root = new Node(root);
    
        return;
    }

    void addChildren(vector<string>* children) {
        for (auto child : *children) {
            this->children.push_back(new ParseTree(child));
        }

        return;
    }

    void addChild(string child) {
        this->children.push_back(new ParseTree(child));
    
        return;
    }

    // for testing

    // void print() {
    //     cout << "root: " << root->val << '\n';
    //     cout << "children: ";

    //     for (auto child : children) {
    //         cout << child->root->val << ', '; 
    //     }

    //     cout << '\n';

    //     return;
    // }
};

int main() {
    ParseTree* ast = new ParseTree("if_statement");

    vector<string> children = {"expression", "statement"};
    ast->addChildren(&children);

    // ast->print();

    return 0;
}