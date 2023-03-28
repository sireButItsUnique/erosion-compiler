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
	vector<int> whitelist;

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

	bool constructWhitelist(string rule, SyntaxToken* token, string currTerm, int idx) {
		// check for the same things as in the breakDown function
		if (currTerm == "TERMINAL_OP" && rule == '<' + token->tokenCodeStringify() + '>') {
			this->whitelist.push_back(idx);
			return true;
		} else if (currTerm[0] == '<') {
			for (auto variation : this->rules->rules[currTerm]) {
				if (this->constructWhitelist(currTerm, token, variation[0], idx)) {
					return true;
				}
			}
			return false;
		} else {
			if (token->text == currTerm) {
				this->whitelist.push_back(idx);
				return true;
			}
		}

        return false;
	}

	// go down the tree + add nodes to tree recursively
	// return after reaching terminal operator
	// return syntax error if missing a token (e.g. a bracket)
	bool breakDown(string rule, SyntaxToken* token, vector<int>& path) {
		cout << "called on rule: " << rule << endl;
		bool found = false;

		for (int i = 0; i < this->rules->rules[rule].size(); i++) {  // iterate thru all possible rule syntaxes
			vector<string> ruleVariation = this->rules->rules[rule][i];
			string currTerm = ruleVariation[0];

			// testing if currTerm is a terminal operator (leaf node)
			if (currTerm == "TERMINAL_OP") {
				if (rule == '<' + token->tokenCodeStringify() + '>') {
					this->whitelist.push_back(i);
					path.push_back(i);

					return true;
				}

				return false;
			}

			// testing if can break down more
			else if (currTerm[0] == '<') {
				found = this->breakDown(currTerm, token, path);

				if (found) {
					this->whitelist.push_back(i);
					for (int j = i + 1; j < this->rules->rules[rule].size(); j++) {
						//call whitelist -> returns a nth, if u can match token to the variation j add to whitelist
						//params: rule, token, (technically the ) currTerm

						this->constructWhitelist(rule, token, this->rules->rules[rule][j][0], j);
					}
					path.push_back(i);
					return found;
				}
			}

			// have reached constant term (leaf node)
			else {
				if (token->text == currTerm) {
					path.push_back(i);
					this->whitelist.push_back(i);
					return true;
				}

				return false;
			}
		}

		return found;
	}

	// traverse thru tree and mark nodes that should be complete as complete
	void markComplete() {
		cout << "checking completeness of " << this->root->type << ", "
			 << this->root->val << endl;
		if (this->complete) {
			return;
		}

		for (auto c : this->children) {
			if (!c->complete) {
				c->markComplete();
			}
		}

		// check that all children are complete
		if (this->children.back()->complete) {
			// find which variation of the rule the current thing is
			for (auto variation : this->rules->rules[this->root->type]) {
				if (this->children.size() == variation.size()) {
					for (int i = 0; i < variation.size(); i++) {
						// checking if the term doesnt match
						if (!(variation[i] == this->children[i]->root->type ||
							  variation[i] == this->children[i]->root->val ||
							  variation[i] == "TERMINAL_OP")) {
							cout << "dont work, var[i] = " << variation[i]
								 << " (child is "
								 << this->children[i]->root->type << ", "
								 << this->children[i]->root->val << ")\n";
							break;
						}

						// everything worked for this variation
						if (i == variation.size() - 1) {
							this->complete = true;
							return;
						}
					}
				}
			}
		}

		return;
	}

	void buildUp(string rule, SyntaxToken* token, vector<int>& path, int i) {
		string currTerm = rule;

		// adding child
		ParseNode* newChild = new ParseNode(currTerm);
		this->children.push_back(new ParseTree(newChild, this->rules));

		if (i <= 0) {
			// reached leaf node
			newChild = new ParseNode(token->tokenCodeStringify(), token->text);

			this->children.back()->children.push_back(
				new ParseTree(newChild, this->rules));
			this->children.back()->children.back()->complete = true;
			return;
		}

		// calling from new child
		currTerm = this->rules->rules[rule][path[i--]][0];
		this->children.back()->buildUp(currTerm, token, path, i);

		return;
	}

	void matchToken(SyntaxToken* token) {
		if (this->children.empty() ||
			this->children[this->children.size() - 1]->complete) {
			// looping thru rule variations
			for (auto ruleVariation : this->rules->rules[this->root->type]) {
				if (this->root->type == "<program>") {
					string currTerm = ruleVariation[0];

					if (currTerm[0] == '<') {
						vector<int> path;

						if (this->breakDown(currTerm, token, path)) {
							cout << "path: ";
							for (auto p : path) {
								cout << p << ", ";
							}
							cout << '\n';

							cout << "starting build of size: " << path.size()
								 << " with token " << token->text << endl;
							this->buildUp(currTerm, token, path,
										  path.size() - 1);
							this->markComplete();

							break;
						}
					} else {
						if (token->text == currTerm ||
							currTerm == "TERMINAL_OP") {
							this->children.push_back(new ParseTree(
								new ParseNode(token->tokenCodeStringify(),
											  token->text),
								this->rules));
							this->children.back()->complete = true;
							this->markComplete();
							break;
						}
					}
				}

				// checking if there are enough terms left in this specific rule
				// variation
				else if (ruleVariation.size() > this->children.size()) {
					string currTerm = ruleVariation[this->children.size()];

					if (currTerm[0] == '<') {
						cout << currTerm << " can be broken" << '\n';

						vector<int> path;

						if (this->breakDown(currTerm, token, path)) {
							cout << "path: ";
							for (auto p : path) {
								cout << p << ", ";
							}
							cout << '\n';

							cout << "starting build of size: " << path.size()
								 << endl;
							cout << "token: " << token->text << '\n';
							this->buildUp(currTerm, token, path,
										  path.size() - 1);
							this->markComplete();

							break;
						}
					} else {
						if (token->text == currTerm ||
							currTerm == "TERMINAL_OP") {
							this->children.push_back(new ParseTree(
								new ParseNode(token->tokenCodeStringify(),
											  token->text),
								this->rules));
							this->children.back()->complete = true;
							this->markComplete();

							break;
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
		this->whitelist = vector<int>();

		return;
	}

	ParseTree(ParseNode* node, Grammar* grammar) {
		this->root = node;

		this->rules = grammar;
		this->whitelist = vector<int>();
	}

	ParseTree(string root, Grammar* grammar) {
		this->root = new ParseNode(root);

		this->rules = grammar;
		this->whitelist = vector<int>();
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
	void print() {
		for (int i = 0; i < 0; i++) {
			cout << "  ";
		}
		cout << this->root->type;
		if (this->root->val != "") {
			cout << ": " << this->root->val;
		}

		if (this->complete) {
			cout << " (complete)";
		}

		cout << endl;
		for (auto& c : this->children) {
			c->print(0 + 1);
		}
	}

	void print(int depth) {
		for (int i = 0; i < depth; i++) {
			cout << "  ";
		}
		cout << this->root->type;
		if (this->root->val != "") {
			cout << ": " << this->root->val;
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
};

// int main() {
//     Lexer* lexer = new Lexer("test.cor");
//     ParseTree* ast = new ParseTree(lexer);
//     ast->build();
//     ast->print();

//     return 0;
// }