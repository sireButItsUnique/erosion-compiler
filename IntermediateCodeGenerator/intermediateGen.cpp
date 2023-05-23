#include "intermediateGen.hpp"

const unordered_map<string, string> ops = {
	{"-u", "neg"},	{"!u", "not"},	 {"~u", "cmt"}, {"*", "mul"},  {"/", "div"},	{"%", "mod"},  {"+", "add"},
	{"-", "sub"},	{">>", "shr"},	 {"<<", "shl"}, {"<", "cmpl"}, {"<=", "cmple"}, {">", "cmpg"}, {">=", "cmpge"},
	{"==", "cmpe"}, {"!=", "cmpne"}, {"&", "and"},	{"^", "xor"},  {"|", "or"},		{"&&", "and"}, {"||", "or"},
};

string IRGenerator::labelify() {
	return ".L" + to_string(labelCnt++);
}

string IRGenerator::findVar(string var) {
	for (auto v : bss) {
		if (v.first == var) {
			return "V(" + to_string(v.second) + ')' + var;
		}
	}
	for (auto v : data) {
		if (v.first == var) {
			return "V(" + to_string(v.second.size) + ')' + var;
		}
	}
	int i = 0;
	for (auto v : st) {
		if (v.first == var) {
			return "S(" + to_string(v.second) + ')' + to_string(i);
		}
		i += v.second;
	}
	throw out_of_range("\x1b[91mYou fucked up big time\x1b[0m");
	terminate();
}

void IRGenerator::printStack() {
	for (auto it = st.rbegin(); it < st.rend(); it++) {
		cout << "\x1b[33m" << it->first << ' ' << it->second << "\x1b[0m\n";
	}
	cout << endl;
}

void IRGenerator::generateIR(ParseNode* root, vector<string>& code) {
	if (root->type == "<func>") {
		st.push_front({PLACEHOLDER, 8}); // return address
		st.push_front({PLACEHOLDER, 8}); // old rbp
		code.push_back(root->children[1]->val + ":");
		code.push_back("prologue");
		int size = 0;
		if (root->children[2]->type == "<argDefs>") {
			auto children = root->children[2]->children;
			for (auto it = children.rbegin(); it < children.rend(); it++) {
				st.push_front({(*it)->children[1]->val, sizes.at((*it)->children[0]->val)});
				code.push_back("push A(" + (*it)->children[0]->val + ')' + (*it)->children[1]->val);
			}
		}
		printStack();
		for (auto child : root->children) {
			generateIR(child, code);
		}
	}

	else if (root->type == "<def>") {
		if (st.empty()) { // global scope
			if (root->children.size() == 2) { // declaration only
				bss.push_back({root->children[1]->val, sizes.at(root->children[0]->val)});
			} else {
				// TODO: Make semantics error if its not a literal
				Thing thing{};
				// TODO: Parse string escape sequences somewhere earlier
				ParseNode* child = root->children[3]->children[0];
				if (child->type == "<stringLiteral>") {
					thing.size = child->val.size() - 1;
					thing.data = malloc(thing.size);
					memcpy(thing.data, child->val.c_str() + 1, thing.size - 1);
					((char*)thing.data)[thing.size - 1] = '\0';
				} else {
					thing.size = sizes[root->children[0]->val];
					thing.data = malloc(thing.size);
					if (root->children[0]->val == "float") {
						*(float*)thing.data = stof(child->val);
					} else if (root->children[0]->val == "double") {
						*(double*)thing.data = stod(child->val);
					} else if (root->children[0]->val == "bool") {
						*(char*)thing.data = child->val == "true";
					} else {
						uint64_t tmp;
						if (child->val[0] == '-') {
							tmp = stoll(child->val);
						} else {
							tmp = stoull(child->val);
						}
						memcpy(thing.data, &tmp, thing.size);
					}
				}
				data.push_back({root->children[1]->val, thing});
			}
		} else {
			if (root->children.size() != 2) { // definition also
				ParseNode* child = root->children[3]->children[0];
				if (child->type == "<stringLiteral>") {
					Thing thing{};
					thing.size = child->val.size() - 1;
					thing.data = malloc(thing.size);
					memcpy(thing.data, child->val.c_str() + 1, thing.size - 1);
					((char*)thing.data)[thing.size - 1] = '\0';
					data.push_back({root->children[1]->val, thing});
				} else {
					uint64_t tmp;
					if (root->children[0]->val == "float") {
						*(float*)&tmp = stof(child->val);
					} else if (root->children[0]->val == "double") {
						*(double*)&tmp = stod(child->val);
					} else if (root->children[0]->val == "bool") {
						*(char*)&tmp = child->val == "true";
					} else {
						if (child->val[0] == '-') {
							tmp = stoll(child->val);
						} else {
							tmp = stoull(child->val);
						}
					}
					st.push_front({root->children[1]->val, tmp});
					code.push_back("push C(" + to_string(sizes.at(root->children[0]->val)) + ')' + to_string(tmp));
				}
			} else {
				st.push_front({root->children[1]->val, sizes.at(root->children[0]->val)});
				code.push_back("alloc " + to_string(sizes.at(root->children[0]->val)));
			}
		}
	}

	else if (root->type == "<return>") {
		if (root->children.size() == 0) {
			code.push_back("ret -");
		} else {
			if (root->children[0]->type == "<literal>") {
				code.push_back("ret C" + root->children[0]->children[0]->val);
			} else {
				generateIR(root->children[0], code);
				code.push_back("ret S0");
			}
		}

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (funcName, 0)
		while (st.front().first != currentLabel) {
			size += st.front().second;
			st.pop_front();
		}
		st.pop_front();
	}

	else if (root->type == "<literal>") {
		if (root->children[0]) {}

		// for string literals:
		// a = "abc"
		// put it into data
		// everything else:
		// push onto stack (st) the placeholder of length 8 bytes

		// once literal base case here is handled
		
		// inside <assignment>
		// e.g. var:int a = 5 + (2 * 3)
		// steps:
		// 1) offload right hand side (expression) recursively into unary / binExp
		// 2) push this: "pop " + findVar("a"); // returns "pop ...(4)a" to code
		// lastly, pop from st
	}

	else if (root->type == "<if>") {

		// enter scope
		st.push_front({PLACEHOLDER, 0});

		// gen jmp
		string label = labelify();
		generateIR(root->children[0], code);
		st.pop_front();
		code.push_back("; IF start");
		code.push_back("jz " + label);

		// gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}

		// gen label
		code.push_back(label + ": ; IF end");

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (PLACEHOLDER, 0)
		while (st.front().second != 0) {
			size += st.front().second;
			st.pop_front();
		}
		st.pop_front();
		if (size != 0) {
			code.push_back("dealloc " + to_string(size));
		}
	}

	else if (root->type == "<while>") {

		// enter scope
		st.push_front({PLACEHOLDER, 0});

		// gen jmp
		string first = labelify();
		string second = labelify();
		code.push_back(first + ": ; WHILE start");
		generateIR(root->children[0], code);
		st.pop_front();
		code.push_back("jz " + second);

		// gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}

		// gen loop jmp
		code.push_back("jmp " + first);
		code.push_back(second + ": ; WHILE end");

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (PLACEHOLDER, 0)
		while (st.front().second != 0) {
			size += st.front().second;
			st.pop_front();
		}
		st.pop_front();
		if (size != 0) {
			code.push_back("dealloc " + to_string(size));
		}
	}

	else if (root->type == "<for>") {

		// enter scope
		st.push_front({PLACEHOLDER, 0});

		// gen declar
		generateIR(root->children[0], code);

		// gen label
		string first = labelify();
		string second = labelify();
		code.push_back(first + ": ; FOR start");

		// gen jmp
		generateIR(root->children[1], code);
		st.pop_front();
		code.push_back("jz " + second);

		// gen contents
		for (int i = 3; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}

		// gen update
		generateIR(root->children[2], code);

		// gen loop jmp
		code.push_back("jmp " + first);

		// gen escape label
		code.push_back(second + ": ; FOR end");

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (PLACEHOLDER, 0)
		while (st.front().second != 0) {
			size += st.front().second;
			st.pop_front();
		}
		st.pop_front();
		if (size != 0) {
			code.push_back("dealloc " + to_string(size));
		}
	}

	else if (root->type == "<assignment>") {
		
	}

	else if (root->type == "<unaryExpression>") {

		code.push_back("; start unary expression");

		// load operand
		if (root->children[1]->type == "<literal>") {
			code.push_back("push C" + root->children[1]->children[0]->val);
			st.push_front({PLACEHOLDER, 8});
		} else if (root->children[1]->type == "<variable>") {
			code.push_back("push " + findVar(root->children[1]->val));
			st.push_front({PLACEHOLDER, 8});
		} else {
			generateIR(root->children[1], code);
		}
		st.pop_front();

		code.push_back(ops.at(root->children[0]->val + 'u'));
		st.push_front({PLACEHOLDER, 8});
		code.push_back("; end unary expression");
	}

	else if (root->type == "<binaryExpression>") {

		code.push_back("; start binary expression");

		// first operand
		if (root->children[0]->type == "<literal>") {
			code.push_back("push C" + root->children[0]->children[0]->val);
			st.push_front({PLACEHOLDER, 8});
		} else if (root->children[0]->type == "<variable>") {
			code.push_back("push " + findVar(root->children[0]->val));
			st.push_front({PLACEHOLDER, 8});
		} else {
			generateIR(root->children[0], code);
		}

		// second operand
		if (root->children[2]->type == "<literal>") {
			code.push_back("push C" + root->children[2]->children[0]->val);
			st.push_front({PLACEHOLDER, 8});
		} else if (root->children[2]->type == "<variable>") {
			code.push_back("push " + findVar(root->children[2]->val));
			st.push_front({PLACEHOLDER, 8});
		} else {
			generateIR(root->children[2], code);
		}
		st.pop_front();
		st.pop_front();

		code.push_back(ops.at(root->children[1]->val));
		st.push_front({PLACEHOLDER, 8});
		code.push_back("; end binary expression");
	}

	else if (root->type == "<functionCall>") {
		string funcName = root->children[0]->val;

		// no arguments

		if (root->children.size() == 1) {
			code.push_back("call " + funcName);
			st.push_front({PLACEHOLDER, 8});
		}

		// TODO: Add ways to handle functionCall arguments
	}

	else if (root->type == "<program>") {
		for (int i = 0; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}
	}

	return;
}
