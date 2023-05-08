#include "ParseNode.hpp"

ParseNode::ParseNode(Lexer* lexer) {
    
}

ParseNode::ParseNode(Lexer* lexer, string type) {

}

ParseNode::ParseNode(Lexer* lexer, string type, string val) {

}

void ParseNode::updateWhitelist() {

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