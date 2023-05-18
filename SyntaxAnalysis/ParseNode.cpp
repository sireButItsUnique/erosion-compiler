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
	this->complete = true;
	this->children = vector<ParseNode*>(0);
	this->whitelist = vector<int>(0); 
}

ParseNode::~ParseNode() {
	for (int i = 0; i < this->children.size(); i++) {
		delete this->children[i];
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

			// update completeness value of each child
			child->updateCompleteness();

			// break out if the aforementioned child is still not complete
			if (!child->complete) {
				return;
			}
		}
	}

	// whether it has the correct number of children/terms
	bool passed = false;
	for (auto variationIdx: whitelist) {
		if (rules.at(type)[variationIdx].size() == children.size()) {
			passed = true;
			break;
		}
	}
	if (!passed) {
		return;
	}

	// marking as complete
	complete = true; 
	return;   
}

bool ParseNode::findPath(SyntaxToken* token, stack<int>& res, string type, bool first) {
	
	//setting correct whitelist if its not lost its virginity yet
	vector<int>* tmpWhitelist = first ? &whitelist : new vector<int>;
	if (!first) {
		for (int i = 0; i < rules.at(type).size(); i++) {
			tmpWhitelist->push_back(i);
		}
	}

	int idx = children.size();
	if (!first || this->type == "<program>")
		idx = 0;

	for (const auto &variationIdx : *tmpWhitelist) {
		if (variationIdx >= rules.at(type).size()) {
			continue;
		}

		const vector<string>& variation = rules.at(type)[variationIdx];

		if (idx >= variation.size()) {
			continue;
		}

		const string currTerm = variation[idx];

		if (currTerm[0] == '<') {
			if (findPath(token, res, currTerm, false)) {
				res.push(variationIdx);
				if (!first) {
					delete tmpWhitelist;
				}
				return true;
			}
		} else if (currTerm == "TERMINAL_OP") {
			if (!first) {
				delete tmpWhitelist;
			}
			return type.substr(1, type.size() - 2) == token->tokenCodeStringify();
		} else {
			if (currTerm == token->text) {
				res.push(variationIdx);
				if (!first) {
					delete tmpWhitelist;
				}
				return true;
			}
		}
	}
	if (!first) {
		delete tmpWhitelist;
	}
	return false;
}

void ParseNode::constructPath(SyntaxToken* token, stack<int>& path) {
	// TODO: fix this section. The error is triggered by jsut a normal if statement
	// the following line does not work properly to check if the node is terminal, so it thinks the expression is just (
	// and tries to evaluate the next part (variable) as the { which obviously fails
	if (rules.at(type)[path.top()][0][0] != '<') {
		children.emplace_back(new ParseNode(lexer, token->tokenCodeStringify(), token->text));
		return;
	}
	children.emplace_back(new ParseNode(lexer, rules.at(type)[path.top()][0]));
	path.pop();
	children.back()->constructPath(token, path);
}

bool ParseNode::handleToken(SyntaxToken* token) {
	
	// create new node
	if (children.empty() || children.back()->complete) {
		
		stack<int> validPath;
		if (findPath(token, validPath, type)) {
			constructPath(token, validPath);
			//updateWhitelist();
			updateCompleteness();
			return true;
		} else {
			return false;
		}
	}

	// go into latest child node
	else {
		return children.back()->handleToken(token);
	}
}

bool ParseNode::build() {
	SyntaxToken* next = lexer->nextToken();

	while (next) {
		if (!handleToken(next)) {
			cerr << "Syntax error—fix ur code bro" << endl;
			delete next;
			return false;
		}
		delete next;
		next = lexer->nextToken();
	}

	return true;
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