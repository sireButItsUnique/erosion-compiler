#include "ParseNode.hpp"

ParseNode::ParseNode(Lexer* lexer) {
    // actual lexer
    this->lexer = lexer;

    // variables for the lex token it represents
    this->type = "<program>";
    this->val = "";

    // variables to help during construction of tree
    this->complete = false;
    this->children = vector<ParseNode*>(0);
    this->whitelist = {0, 1, 2, 3}; 
}

ParseNode::ParseNode(Lexer* lexer, string type) {
    // actual lexer
    this->lexer = lexer;

    // variables for the lex token it represents
    this->type = type;
    this->val = "";

    // variables to help during construction of tree
    this->complete = false;
    this->children = vector<ParseNode*>(0);
    this->whitelist = vector<int>(0);

    for (int i = 0; i < rules.at(type).size(); i++) {
        this->whitelist.push_back(i);
    }
}

ParseNode::ParseNode(Lexer* lexer, string type, string val) {
    // actual lexer
    this->lexer = lexer;

    // variables for the lex token it represents
    this->type = type;
    this->val = val;

    // variables to help during construction of tree
    this->complete = false;
    this->children = vector<ParseNode*>(0);
    this->whitelist = vector<int>(0); 

    for (int i = 0; i < rules[type].size(); i++) {
        this->whitelist.push_back(i);
    }
}

void ParseNode::updateWhitelist() {
    for (int i = 0; i < this->children.size(); i++) {
        for (int j = 0; j < this->whitelist.size(); j++) {
            int variation = this->whitelist[j];
            
            if (this->children.size() > rules[this->type][variation].size()) {
                
                this->children.erase(this->children.begin() + j);
            } else {

                string ruleType = rules[this->type][variation][i];
                string childType = this->children[i]->type;

                if (ruleType != childType) {
                    this->children.erase(this->children.begin() + j);
                } 
            }
        }
    }
}

void ParseNode::updateCompleteness() {
    
}

bool ParseNode::findPath(SyntaxToken* token, vector<int>& res) {

}

void ParseNode::constructPath(SyntaxToken* token, vector<int>& path) {

}

bool ParseNode::handleToken(SyntaxToken* token) {

}

void ParseNode::build() {

}

void ParseNode::print() {
    for (int i = 0; i < 0; i++) {
		cout << "  ";
	}
	cout << this->type;
	if (this->val != "") {
		cout << ": " << this->val;
	}

	if (this->complete) {
		cout << " (complete)";
	}

	cout << endl;
	for (auto& c : this->children) {
		c->print(0 + 1);
	}
}

void ParseNode::print(int depth) {
    for (int i = 0; i < depth; i++) {
        cout << "  ";
    }
    
    cout << this->type;
    if (this->val != "") {
        cout << ": " << this->val;
    }

    if (this->complete) {
        cout << " (complete)";
    }

    cout << endl;
    for (auto& c : this->children) {
        c->print(depth + 1);
    }

	return;
}