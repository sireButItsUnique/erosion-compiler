#include "semanticAnalysis.hpp"
#include "../SyntaxAnalysis/grammar.hpp"

Diagnoser::Diagnoser() {
	scopes.push_front({});
}

bool Diagnoser::diagnose(ParseNode* root) {

	// scope info
	bool newScope = false;
	for (auto child : root->children) {
		if (child->val == "{") {
			newScope = true;
			scopes.push_front({});
			break;
		}
	}

	// function info
	if (root->type == "<func>") {
		if (!insideFunc.empty()) {
			error = "Functions must be defined in the global scope";
			return false;
		}
		insideFunc = root->children[3]->val;
	}

	// check for nonsense
	if (!check(root)) {
		ParseNode* node = root;
		while (node->children.size()) {
			node = node->children[0];
		}
		error = "Line " + to_string(node->line) + ": " + error;
		return false;
	}

	// hoist
	hoist(root);

	// clean
	clean(root);

	// recurse
	string prev = "";

	// penetrating thru all the lil kids* i play genshin bro (okay fine)
	for (int i = 0; i < root->children.size(); i++) {
		ParseNode* child = root->children[i];

		if (child->type == "<elif>" || child->type == "<else>") {
			if (prev != "<elif>" && prev != "<if>") {
				error = "Invalid \"elif\"/\"else\" without an initial if";
				return false;
			}

			// TODO: attempt to bundle
			if (true) {
			} else if (child->type == "<else>" || (i == root->children.size() - 1) || (root->children[i + 1]->type != "<elif>" && root->children[i + 1]->type != "<else>")) {
			}
		}

		// diagnosing child
		if (!diagnose(child)) {
			return false;
		}
		prev = child->type;
	}

	// check returns
	if (root->type == "<func>") {
		if (checkReturns(root) != functions.at(insideFunc)[0]) {
			error = "Function \"" + insideFunc + "\" must return a value of type \"" + functions.at(insideFunc)[0] + "\"";
			return false;
		}
	}

	// scope stuff
	if (newScope) {
		scopes.pop_front();
	}

	if (root->type == "<func>") {
		insideFunc.clear();
	}

	if (root->type == "<program>") {
		if (!functions.count("main")) {
			error = "Function \"main\" does not exist";
			return false;
		}

		// check if main returns an int

		if (functions.at("main")[0] != "int") {
			error = "Main function must return an integer";

			return false;
		}
	}

	return true;
}

string Diagnoser::queryVar(string var) {
	for (auto scope : scopes) {
		auto tmp = scope.find(var);

		if (tmp != scope.end()) {
			return tmp->second;
		}
	}
	return "";
}

bool Diagnoser::checkDefine(ParseNode* root, string varName) {
	if (root->type == "TERMINAL_OP" || root->type == "<variable>") {
		return (root->val != varName);
	}

	for (int i = 0; i < root->children.size(); i++) {
		return checkDefine(root->children[i], varName);
	}

	return true;
}

// TODO: When we really support custom types we need to make a casting checker
// because right now we are just trusting that anything other than a string can be casted to each other
string Diagnoser::checkReturns(ParseNode* root) {
	for (auto it = root->children.begin() + 2; it < root->children.end(); it++) {
		ParseNode* node = *it;

		if (node->type == "<return>") {
			if (node->children.size() == 2) {
				return "void";
			}

			ParseNode* child = node->children[0];

			if (child->type == "<literal>") {
				if (child->children[0]->type == "<stringLiteral>") {
					return "string";
				} else {
					return functions.at(insideFunc)[0];
				}
			} else if (child->type == "<variable>") {
				if (queryVar(child->val) == "string") {
					return "string";
				} else {
					return functions.at(insideFunc)[0];
				}
			} else if (child->type == "<binaryExpression>" || child->type == "<unaryExpression>") {
				// TODO: check if expression is a string
				return functions.at(insideFunc)[0];
			} else if (child->type == "<functionCall>") {
				if (functions.at(child->children[0]->val)[0] == "string") {
					return "string";
				} else {
					return functions.at(insideFunc)[0];
				}
			}
		}
	}

	return "void";
}

bool Diagnoser::check(ParseNode* root) {
	if (root->type == "<return>") {
		if (insideFunc.empty()) {
			error = "Cannot return outside of a function";
			return false;
		}
	}

	else if (root->type == "<func>") {
		string name = root->children[3]->val; // name of function

		if (functions.count(name)) {
			error = "Function \"" + name + "\" already exists";
			return false;
		}

		ParseNode* argDefs = root->children[5]; // argument definitions

		string type = root->children[2]->val; // type of function
		functions[name] = {type};

		// Either <argDefs> or ) depending on if there are arguments
		if (argDefs->type == "<argDefs>") {
			for (auto child : argDefs->children) {
				functions[name].push_back(child->children[2]->val);
			}
		}
	}

	else if (root->type == "<def>") {
		string name = root->children[3]->val; // name of variable

		// variable in a scope CAN'T have the same name as another variable in a parent scope
		if (!queryVar(name).empty()) {
			error = "Variable \"" + name + "\" already exists";
			return false;
		}

		if (functions.count(name)) {
			error = "Variable \"" + name + "\" cannot have the same name as a function";
			return false;
		}

		string type = root->children[2]->val; // type of variable

		if (type == "void") {
			error = "Variable \"" + name + "\" cannot be of type void";
			return false;
		}

		// if the variable definition is an assignment (e.g. var: int a = 5)
		if (root->children.size() > 4) {
			ParseNode* expression = root->children.back();

			if (scopes.size() == 1) {
				if (root->children[5]->children[0]->type != "<literal>") {
					error = "Global variables must be initialized with a literal";
					return false;
				}
			}

			if (!checkDefine(expression, name)) {
				error = "Variable \"" + name + "\" must be defined prior to usage";

				return false;
			}
		}

		scopes.front()[name] = type;
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
		if (!functions.count(root->children[0]->val)) {
			error = "Function \"" + root->children[0]->val + "\" does not exist";
			return false;
		}
		if (callArgs->type == "<args>") {
			if (callArgs->children.size() != functions.at(root->children[0]->val).size() - 1) {
				error = "Function \"" + root->children[0]->val + "\" requires " + to_string(functions.at(root->children[0]->val).size()) + " arguments, " +
						to_string(callArgs->children.size()) + " provided";
				return false;
			}
			vector<string>& argTypes = functions.at(root->children[0]->val); // types of arguments in function definition
			for (int i = 0; i < callArgs->children.size(); i++) {
				ParseNode* child = callArgs->children[i];
				if (child->children[0]->type == "<variable>") {
					if (queryVar(child->children[0]->val).empty()) {
						error = "Variable \"" + child->children[0]->val + "\" does not exist";
						return false;
					}
					if (queryVar(child->children[0]->val) != argTypes[i]) {
						error = "Argument \"" + child->children[0]->val + "\" is of type \"" + queryVar(child->children[0]->val) +
								"\" but should be of type \"" + argTypes[i] + "\"";
						return false;
					}
				} else if (child->children[0]->type == "<literal>") {
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
			if (functions.at(root->children[0]->val).size() != 1) {
				error =
					"Function \"" + root->children[0]->val + "\" requires " + to_string(functions.at(root->children[0]->val).size()) + " arguments, 0 provided";
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

	else if (root->type == "<return>") {

		// What its supposed to return
		string type = functions.at(insideFunc)[0];
	}

	return true;
}

void Diagnoser::hoist(ParseNode* root) {
	for (int i = 0; i < root->children.size(); i++) {
		ParseNode* child = root->children[i];

		if (child->type == "<statements>" || child->type == "<args>" || child->type == "<conditional>" || child->type == "<expression>" ||
			child->type == "<statement>" || child->type == "<declaration>") {

			// erase the child from the root
			root->children.erase(root->children.begin() + i);

			// move everything from the child that was just erased into the root
			root->children.insert(root->children.begin() + i, child->children.begin(), child->children.end());
			i--;
		}
	}
}

void Diagnoser::clean(ParseNode* root) {
	for (int i = 0; i < root->children.size(); i++) {
		ParseNode* child = root->children[i];

		if (child->val == ";" || (child->val.size() && rules.find(child->type) == rules.end())) {
			root->children.erase(root->children.begin() + i);
			i--;
		}

		if (child->type == "<keyword>") {
			root->children.erase(root->children.begin() + i);
			i--;
		}
	}
}
