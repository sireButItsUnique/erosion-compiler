#include "ParseNode.hpp"

ParseNode::ParseNode(Lexer* lexer) {
	// actual lexer
	this->lexer = lexer;

	// variables for the lex token it represents
	type = "<program>";
	val = "";

	// variables to help during construction of tree
	complete = false;
	children = vector<ParseNode*>(0);
	whitelist = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
}

ParseNode::ParseNode(Lexer* lexer, string type) {
	// actual lexer
	this->lexer = lexer;

	// variables for the lex token it represents
	this->type = type;
	val = "";

	// variables to help during construction of tree
	complete = false;
	children = vector<ParseNode*>(0);
	whitelist = vector<int>(0);

	for (int i = 0; i < rules.at(type).size(); i++) {
		whitelist.push_back(i);
	}
}

ParseNode::ParseNode(Lexer* lexer, string type, string val) {
	// actual lexer
	this->lexer = lexer;

	// variables for the lex token it represents
	this->type = type;
	this->val = val;

	// variables to help during construction of tree
	complete = true;
	children = vector<ParseNode*>(0);
	whitelist = vector<int>(0);
}

ParseNode::~ParseNode() {
	for (int i = 0; i < children.size(); i++) {
		delete children[i];
	}
}

void ParseNode::updateWhitelist() {

	// looping through each term currently under the node
	for (int i = 0; i < children.size(); i++) {

		// looping through each term in the remaining whitelist for the current node
		// in for (...;...;...) form in order to use std::vector.erase() method
		for (int j = whitelist.size() - 1; j >= 0; j--) {

			// current rule variation that's currently still considered valid in the whitelist
			int variation = whitelist[j];

			// checking if there are more terms under the node than there are terms in the variation, if so remove
			if (children.size() > rules.at(type)[variation].size()) {
				whitelist.erase(whitelist.begin() + j);
			}

			// test whether or not the terms under the node and the terms in the variation match
			else {
				string ruleType = rules.at(type)[variation][i];
				string childType = children[i]->type;

				if (ruleType[0] != '<') {
					if (ruleType != children[i]->val) {
						whitelist.erase(whitelist.begin() + j);
					}
				} else {
					if (ruleType != childType) {
						whitelist.erase(whitelist.begin() + j);
					}
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
			// child->updateCompleteness();

			// break out if the aforementioned child is still not complete
			if (!child->complete) {
				complete = false;
				return;
			}
		}
	}

	if (type == "<program>" || type == "<statements>") {
		complete = true;
		return;
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
		complete = false;
		return;
	}

	// marking as complete
	complete = true;
}

// TODO: fix infinite recursion bug
bool ParseNode::findPath(SyntaxToken* token, stack<int>& res, string type, int depth) {
	if (depth >= MAX_DEPTH) return false;

	//setting correct whitelist if its not lost its virginity yet
	vector<int>* tmpWhitelist = !depth ? &whitelist : new vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	int idx = children.size();

	if (depth || this->type == "<program>" || this->type == "<statements>") {
		idx = 0;
	}

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
			if (findPath(token, res, currTerm, depth + 1)) {
				res.push(variationIdx);
				if (depth) {
					delete tmpWhitelist;
				}
				return true;
			}
		} else if (currTerm == "TERMINAL_OP") {
			if (!depth) {
				delete tmpWhitelist;
			}
			return type == token->tokenCodeStringify();
		} else {
			if (currTerm == token->text) {
				res.push(variationIdx);
				if (depth) {
					delete tmpWhitelist;
				}
				return true;
			}
		}
	}
	if (depth) {
		delete tmpWhitelist;
	}
	return false;
}

void ParseNode::constructPath(SyntaxToken* token, stack<int>& path) {
	if (path.size() == 1) {
		children.emplace_back(new ParseNode(lexer, token->tokenCodeStringify(), token->text));
		return;
	}
	// If the variation has only 1 term, then we pick that one, otherwise we pick whichever node we are on
	// prevents it from picking one past the last term instead of the first term of a new variation
	int idx = min(children.size(), rules.at(type)[path.top()].size() - 1);
	children.emplace_back(new ParseNode(lexer, rules.at(type)[path.top()][idx]));
	path.pop();
	children.back()->constructPath(token, path);
}

bool ParseNode::handleToken(SyntaxToken* token) {

	// create new node
	if (children.empty() || children.back()->complete) {

		// Check if there is a completed child
		if (!children.empty() && children.back()->complete) {
			// If it is either of these types, we can try to extend it
			if (children.back()->type == "<statements>") {
				if (children.back()->handleToken(token)) {
					updateCompleteness();
					return true;
				}
			} else if (children.back()->type == "<expression>") {
				if (token->type == op) {
					ParseNode* tmp1 = children.back();
					children.back() = new ParseNode(lexer, "<expression>");
					ParseNode* tmp2 = new ParseNode(lexer, "<binaryExpression>");
					children.back()->children.push_back(tmp2);
					tmp2->children.push_back(tmp1);
					tmp2->children.push_back(new ParseNode(lexer, "<op>", token->text));
					return true;
				} else if (token->text == "(") {
					ParseNode* tmp1 = new ParseNode(lexer, "<expression>");
					tmp1->children.push_back(new ParseNode(lexer, "<roundBracket>", "("));
					ParseNode* tmp2 = children.back()->children.back();
					if (tmp2->type != "<binaryExpression>" || tmp2->children.size() != 2) {
						delete tmp1;
						return false;
					}
					tmp2->children.push_back(tmp1);
					return true;
				}
			}
		}

		stack<int> validPath;
		if (findPath(token, validPath, type)) {
			constructPath(token, validPath);
			if (type != "<program>" && type != "<statements>") {
				updateWhitelist();
			}
			updateCompleteness();
			return true;
		} else {
			return false;
		}
	}

	// go into latest child node
	else {
		if (!children.back()->handleToken(token)) {
			return false;
		}
		if (type != "<program>" && type != "<statements>") {
			updateWhitelist();
		}
		updateCompleteness();
		return true;
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

	updateCompleteness();
	if (!complete) {
		cerr << "Syntax error—fix ur code bro" << endl;
		return false;
	}
	return true;
}

void ParseNode::print() {
	for (int i = 0; i < 0; i++) {
		std::cout << "  ";
	}
	cout << type;
	if (val != "") {
		cout << ": " << val;
	}

	if (complete) {
		cout << " (complete)";
	}

	cout << endl;
	for (auto& c : children) {
		c->print(0 + 1);
	}
}

void ParseNode::print(int depth) {
	for (int i = 0; i < depth; i++) {
		cout << "  ";
	}

	std::cout << type;
	if (val != "") {
		cout << ": " << val;
	}

	if (complete) {
		cout << " (complete)";
	}

	cout << endl;
	for (auto& c : children) {
		c->print(depth + 1);
	}

	return;
}