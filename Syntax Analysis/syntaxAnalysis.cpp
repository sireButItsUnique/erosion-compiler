// change this later to make code more optimized
#include <bits/stdc++.h>

#include "../Lexical Analysis/lexicalAnalysis.cpp"
#include "grammar.cpp"
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
        this->val = "";

        return;
    }
};

class ParseTree {
   private:
    ParseNode* root;  // root of current parse tree
    vector<ParseTree*> children;  // children of the root node of the current parse tree
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
        cout << "called on rule: " << rule << endl;
        bool found = false;

        for (int i = 0; i < this->rules->rules[rule].size(); i++) {  // iterate thru all possible rule syntaxes
            cout << "found variation: " << i << endl;
            
            vector<string> ruleVariation = this->rules->rules[rule][i];
            string currTerm = ruleVariation[0];

            // testing if currTerm is a terminal operator (leaf node)
            if (currTerm == "TERMINAL_OP") {
                if (rule == token->tokenCodeStringify()) {
                    path.push_back(i);

                    return true;
                }

                return false;
            }

            // testing if can break down more
            else if (currTerm[0] == '<') {
                found = this->breakDown(currTerm, token, path);

                if (found) {
                    path.push_back(i);
                    return found;
                }
            }

            // have reached constant term (leaf node)
            else {
                cout << "reached constant: " << currTerm << endl;
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
        string currTerm = rule;

        //adding child
        ParseNode* newChild = new ParseNode(currTerm);
        this->children.push_back(new ParseTree(newChild, this->rules));
        
        // we know its complete when it has equal num of children as num of
        // terms in the rule variation
        if (this->children.size() == this->rules->rules[rule][path[i]].size()) {
            this->complete = true;
        }

        i--;
        if (i < 0) {
            cout << "leaf\n";
            // reached leaf node
            newChild = new ParseNode(token->tokenCodeStringify(), token->text);

            this->children.back()->children.push_back(new ParseTree(newChild, this->rules));
            this->children.back()->children.back()->complete = true;
            return;
        }

        //calling from new child
        currTerm = this->rules->rules[rule][path[i]][0];
        this->children.back()->buildUp(currTerm, token, path, i);
        
        return;
    }

    void matchToken(SyntaxToken* token) {
        if (this->children.empty() || this->children[this->children.size() - 1]->complete) {
            
            // looping thru rule variations
            for (auto ruleVariation : this->rules->rules[this->root->type]) {  
                
                // checking if there are enough terms left in this specific rule variation
                if (ruleVariation.size() > this->children.size()) {
                    string currTerm = ruleVariation[this->children.size()];
                    
                    if (currTerm[0] == '<') {
                        vector<int> path;

                        if (this->breakDown(currTerm, token, path)) {
                            cout << "starting build of size: " << path.size() << endl;
                            this->buildUp(currTerm, token, path, path.size() - 1);
                        }
                    } else {
                        if (token->text == currTerm) {
                            this->children.push_back(new ParseTree(new ParseNode(token->tokenCodeStringify(), token->text), this->rules));
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
        cout << this->root->type;
        if (this->root->val != "") {
            cout << ": " << this->root->val;
        }
        cout << endl;
        for (auto& c : this->children) {
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