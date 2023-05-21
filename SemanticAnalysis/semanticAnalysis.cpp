#include "semanticAnalysis.hpp"
#include "../SyntaxAnalysis/grammar.hpp"

bool Diagnoser::diagnose(ParseNode* root) {
	// remove nonsense
	if (!check(root)) {
		ParseNode* node = root;
		while (node->children.size()) {
			node = node->children[0];
		}
		error = "Line " + to_string(node->line) + ": " + error;
		return false;
	}

	// clean
	clean(root);

	// hoist
	hoist(root);

	// recurse
	for (auto child : root->children) {
		if (!diagnose(child)) {
			return false;
		}
	}

	return true;
}

bool Diagnoser::check(ParseNode* root) {
	if (root->type == "<func>") {
		string name = root->children[3]->val; // name of function

		if (args.count(name)) {
			error = "Function \"" + name + "\" already exists";
			return false;
		}

		ParseNode* argDefs = root->children[5]; // argument definitions

		if (argDefs->type == "<argDefs>") {
			for (auto child : argDefs->children) {
				args[name].push_back(child->children[2]->val);
			}
		}
	}

	else if (root->type == "<def>") {
		string name = root->children[3]->val; // name of variable

		if (vars.count(name)) {
			error = "Variable \"" + name + "\" already exists";
			return false;
		}

		string type = root->children[2]->val; // type of variable

		if (type == "void") {
			error = "Variable \"" + name + "\" cannot be of type void";
			return false;
		}

		vars[name] = type;
	}

	else if (root->type == "<for>") {
		if (root->whitelist[0] == 1) {
			// make sure idx 4 is an array
		} else if (root->whitelist[0] == 2) {
			// make sure idx 6 is an array
		}
	}

	else if (root->type == "<functionCall>") {
		ParseNode* callArgs = root->children[2]; // arguments in function call
		
		if (callArgs->type == "<args>") {
			if (callArgs->children.size() != args[root->children[0]->val].size()) {
				error = "Function \"" + root->children[0]->val + "\" requires " + to_string(args[root->children[0]->val].size()) + " arguments, " + to_string(callArgs->children.size()) + " provided";
				return false;
			}
			vector<string>& argTypes = args[root->children[0]->val]; // types of arguments in function definition
			for (int i = 0; i < argTypes.size(); i++) {
				ParseNode* child = callArgs->children[i];
				if (child->children[0]->type == "<variable>") {
					if (vars[child->children[0]->val] != argTypes[i]) {
						error = "Argument \"" + child->children[0]->val + "\" is of type \"" + vars[child->children[0]->val] + "\" but should be of type \"" + argTypes[i] + "\"";
						return false;
					}
				}
				else if (child->children[0]->type == "<literal>") {
					string type = child->children[0]->children[0]->type;

					// should be string but isnt
					if (type == "<booleanLiteral>" && argTypes[i] == "string") {
						error = "Argument \"" + child->children[0]->children[0]->val + "\" is of type \"bool\" but should be of type \"string\"";
						return false;
					}

					else if (type == "<integerLiteral>" && argTypes[i] == "string") {
						error = "Argument \"" + child->children[0]->children[0]->val + "\" is of type \"int\" but should be of type \"string\"";
						return false;
					}

					// got string but it shouldn't be
					else if (type == "<stringLiteral>" && argTypes[i] != "string") {
						error = "Argument " + child->children[0]->children[0]->val + " is of type \"string\" but should be of type \"" + argTypes[i] + "\"";
						return false;
					}
				}
				// must either be correct or an expression, in which case we let them suffer because its too hard to check
			}
		} else {
			if (args[root->children[0]->val].size() != 0) {
				error = "Function \"" + root->children[0]->val + "\" requires " + to_string(args[root->children[0]->val].size()) + " arguments, 0 provided";
				return false;
			}
		}
	}

	else if (root->type == "<type>") {
		if (!types.count(root->val)) {
			error = "Type \"" + root->val + "\" is undefined";
			return false;
		}
	}

	else if (root->type == "<variable>") {
		if (!vars.count(root->val)) {
			error = "Variable \"" + root->val + "\" is undefined";
			return false;
		}
	}

	else if (root->type == "<function>") {
		if (!args.count(root->val)) {
			error = "Function \"" + root->val + "\" is undefined";
			return false;
		}
	}

	return true;
}

void Diagnoser::hoist(ParseNode* root) {
	for (int i = 0; i < root->children.size(); i++) {
		ParseNode* child = root->children[i];

		if (child->type == "<statements>" || child->type == "<args>" || child->type == "<argDefs>" || child->type == "<conditional>") {

			// erase the child from the root
			root->children.erase(root->children.begin() + i);

			// move everything from the child that was just erased into the root
			root->children.insert(root->children.begin() + i, child->children.begin(), child->children.end());
		}
	}
}

void Diagnoser::clean(ParseNode* root) {
	for (int i = 0; i < root->children.size(); i++) {
		ParseNode* child = root->children[i];

		if (child->val.size() && rules.find(child->type) == rules.end()) {
			root->children.erase(root->children.begin() + i);
			i--;
		}
	}
}

// .Recurse:
// 	Check for nonsense
// 	Remove unneeded stuff
// 	Hoist
// jmp .Recurse
