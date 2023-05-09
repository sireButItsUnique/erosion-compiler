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

	for (int i = 0; i < rules.at(type).size(); i++) {
		this->whitelist.push_back(i);
	}
}

void ParseNode::updateWhitelist() {

	// looping through each term currently under the node
	for (int i = 0; i < this->children.size(); i++) {
		
		// looping through each term in the remaining whitelist for the current node
		// in for (...;...;...) form in order to use std::vector.erase() method
		for (int j = 0; j < this->whitelist.size(); j++) {
			
			// current rule variation that's currently still considered valid in the whitelist
			int variation = this->whitelist[j];
			
			// checking if there are more terms under the node than there are terms in the variation, if so remove
			if (this->children.size() > rules.at(this->type)[variation].size()) {
				this->whitelist.erase(this->whitelist.begin() + j);
			} 
			
			// test whether or not the terms under the node and the terms in the variation match
			else {
				string ruleType = rules.at(this->type)[variation][i];
				string childType = this->children[i]->type;

				if (ruleType != childType) {
					this->whitelist.erase(this->whitelist.begin() + j);
				} 
			}
		}
	}
}

void ParseNode::updateCompleteness() {
	// whether all the children are complete
	for (const auto &child : children) {
		if (!child->complete) {
			child->updateCompleteness();
			if (!child->complete) {
				return;
			}
		}
	}

	// whether it has the correct number of children/terms
	if (rules.at(type)[whitelist[0]].size() != children.size()) {
		return;
	}

	// marking as complete
	complete = true; 
	return;   
}

bool ParseNode::findPath(SyntaxToken* token, stack<int>& res, string type, bool first) {
	// issue: it always looks at the first thing, for example when trying to parse the ":" after "var" in <declaration> it dies
	// it always tries to match against "var" because its first in the list
	for (const auto &variationIdx : whitelist) {
		if (variationIdx >= rules.at(type).size()) {
			continue;
		}

		const vector<string>& variation = rules.at(type)[variationIdx];

		if ((variation.size() - 1) < first * children.size()) {
			continue;
		}

		const string currTerm = variation[first * children.size()];


		if (currTerm[0] == '<') {
			if (findPath(token, res, currTerm, false)) {
				res.push(variationIdx);
				return true;
			}
		} else {
			if (currTerm == token->text) {
				res.push(variationIdx);
				return true;
			}
		}
	}
	return false;
}

void ParseNode::constructPath(SyntaxToken* token, stack<int>& path) {
	//children.emplace_back(new ParseNode(lexer, rules.at(type)[path.top()][0]));

}

bool ParseNode::handleToken(SyntaxToken* token) {
	
	// create new node
	if (children.empty() || children.back()->complete) {
		
		stack<int> validPath;
		if (findPath(token, validPath, type)) {
			constructPath(token, validPath);
			updateWhitelist();
			updateCompleteness();
		} else {
			return false;
		}
	}

	// go into latest child node
	else {
		return children.back()->handleToken(token);
	}
}

void ParseNode::build() {
	SyntaxToken* next = lexer->nextToken();

	while (next) {
		handleToken(next);
		next = lexer->nextToken();
	}
}

void ParseNode::print() {
	for (int i = 0; i < 0; i++) {
		std::cout << "  ";
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
	
	std::cout << this->type;
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