#include "intermediateGen.hpp"

const unordered_map<string, string> ops = {
	{"-u", "neg"},	{"!u", "not"},	 {"~u", "cmt"}, {"*", "mul"},  {"/", "div"},	{"%", "mod"},  {"+", "add"},
	{"-", "sub"},	{">>", "shr"},	 {"<<", "shl"}, {"<", "cmpl"}, {"<=", "cmple"}, {">", "cmpg"}, {">=", "cmpge"},
	{"==", "cmpe"}, {"!=", "cmpne"}, {"&", "and"},	{"^", "xor"},  {"|", "or"},		{"&&", "and"}, {"||", "or"},
};

string CodeGenerator::labelify() {
	return ".L" + to_string(labelCnt++);
}

string CodeGenerator::findVar(string var) {
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
	int i = -8;
	bool inStack = false;
	for (auto v : st) {
		if (v.first == PLACEHOLDER "old_rbp") {
			inStack = true;
		} else if (!inStack) {
			continue;
		}
		i += v.second;
		if (v.first == var) {
			return "S(" + to_string(v.second) + ')' + to_string(i);
		}
	}
	throw out_of_range("\x1b[91mYou fucked up big time\x1b[0m");
	terminate();
}

void CodeGenerator::printStack() {
	for (auto it = st.rbegin(); it < st.rend(); it++) {
		cout << "\x1b[33m" << it->first << ' ' << it->second << "\x1b[0m\n";
	}
	cout << endl;
}

void CodeGenerator::generateIR(ParseNode* root, deque<string>& code) {
	if (root->type == "<func>") {
		st.push_back({PLACEHOLDER "ret_addr", 8}); // return address
		st.push_back({PLACEHOLDER "old_rbp", 8}); // old rbp
		code.push_back(root->children[1]->val + ":");
		code.push_back("prologue");
		int size = 0;
		if (root->children[2]->type == "<argDefs>") {
			auto children = root->children[2]->children;
			for (auto it = children.rbegin(); it < children.rend(); it++) {
				st.push_back({(*it)->children[1]->val, sizes.at((*it)->children[0]->val)});
				code.push_back("push A(" + (*it)->children[0]->val + ')' + (*it)->children[1]->val);
			}
		}
		for (auto child : root->children) {
			generateIR(child, code);
		}
		while (st.back().first != PLACEHOLDER "ret_addr") {
			st.pop_back();
		}
		st.pop_back();
	}

	else if (root->type == "<def>") {
		if (st.empty()) { // global scope
			if (root->children.size() == 2) { // declaration only
				bss.push_back({root->children[1]->val, sizes.at(root->children[0]->val)});
			} else {
				// TODO: Make semantics error if its not a literal
				Blob blob{};
				// TODO: Parse string escape sequences somewhere earlier
				ParseNode* child = root->children[3]->children[0];
				if (child->type == "<stringLiteral>") {
					blob.size = child->val.size() - 1;
					blob.data = malloc(blob.size);
					memcpy(blob.data, child->val.c_str() + 1, blob.size - 1);
					((char*)blob.data)[blob.size - 1] = '\0';
				} else {
					blob.size = sizes[root->children[0]->val];
					blob.data = malloc(blob.size);
					if (root->children[0]->val == "float") {
						*(float*)blob.data = stof(child->val);
					} else if (root->children[0]->val == "double") {
						*(double*)blob.data = stod(child->val);
					} else if (root->children[0]->val == "bool") {
						*(char*)blob.data = child->val == "true";
					} else {
						// int or long

						uint64_t tmp;
						tmp = stoll(child->val);
						memcpy(blob.data, &tmp, blob.size);
					}
				}
				data.push_back({root->children[1]->val, blob});
			}
		} else { // local scope
			if (root->children.size() != 2) { // definition also
				ParseNode* child = root->children[3]->children[0];
				if (child->type == "<stringLiteral>") {
					Blob blob{};
					blob.size = child->val.size() - 1;
					blob.data = malloc(blob.size);
					memcpy(blob.data, child->val.c_str() + 1, blob.size - 1);
					((char*)blob.data)[blob.size - 1] = '\0';
					data.push_back({root->children[1]->val, blob});
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
					st.push_back({root->children[1]->val, sizes.at(root->children[0]->val)});
					code.push_back("push C(" + to_string(sizes.at(root->children[0]->val)) + ')' + to_string(tmp));
				}
			} else {
				st.push_back({root->children[1]->val, sizes.at(root->children[0]->val)});
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
				code.push_back("ret pop");
			}
		}
	}

	else if (root->type == "<literal>") {
		ParseNode* child = root->children[0];

		// for strings

		if (child->type == "<stringLiteral>") {
			Blob blob = {};
			blob.size = child->val.size() - 1;
			blob.data = malloc(blob.size);

			memcpy(blob.data, child->val.c_str() + 1, blob.size - 1);

			data.push_back({child->val, blob});
		}

		// every other type of literal

		else {
			code.push_back("push C(8)" + child->val);
			st.push_back({PLACEHOLDER, 8});
		}
	}

	else if (root->type == "<variable>") {
		code.push_back("push " + findVar(root->val));
		st.push_back({root->val, 8});
	}

	else if (root->type == "<if>") {

		// enter scope
		st.push_back({PLACEHOLDER, 0});

		// gen jmp
		string label = labelify();
		generateIR(root->children[0], code);
		st.pop_back();
		code.push_back("jz " + label);

		// gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}

		string endLabel = labelify();
		code.push_back("jmp " + endLabel);

		// gen label
		code.push_back(label + ':');
		code.push_back(endLabel + ':');

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (PLACEHOLDER, 0)
		while (st.back().first != PLACEHOLDER || st.back().second != 0) {
			size += st.back().second;
			st.pop_back();
		}
		st.pop_back();
		if (size != 0) {
			code.push_back("dealloc " + to_string(size));
		}
	}

	else if (root->type == "<elif>") {
		string prevLabel = code.back();
		prevLabel = prevLabel.substr(0, prevLabel.size() - 1);
		code.pop_back();

		// enter scope
		st.push_back({PLACEHOLDER, 0});

		// gen jmp
		string label = labelify();
		generateIR(root->children[0], code);
		st.pop_back();
		code.push_back("jz " + label);

		// gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}

		code.push_back("jmp " + prevLabel);

		// gen label
		code.push_back(label + ':');
		code.push_back(prevLabel + ':');

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (PLACEHOLDER, 0)
		while (st.back().first != PLACEHOLDER || st.back().second != 0) {
			size += st.back().second;
			st.pop_back();
		}
		st.pop_back();
		if (size != 0) {
			code.push_back("dealloc " + to_string(size));
		}
	}

	else if (root->type == "<else>") {
		string prevLabel = code.back();
		prevLabel = prevLabel.substr(0, prevLabel.size() - 1);
		code.pop_back();

		// enter scope
		st.push_back({PLACEHOLDER, 0});

		// gen contents
		for (auto child : root->children) {
			generateIR(child, code);
		}
		code.push_back(prevLabel + ':');

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (PLACEHOLDER, 0)
		while (st.back().first != PLACEHOLDER || st.back().second != 0) {
			size += st.back().second;
			st.pop_back();
		}
		st.pop_back();
		if (size != 0) {
			code.push_back("dealloc " + to_string(size));
		}
	}

	else if (root->type == "<while>") {

		// enter scope
		st.push_back({PLACEHOLDER, 0});

		// gen jmp
		string first = labelify();
		string second = labelify();
		code.push_back(first + ':');
		generateIR(root->children[0], code);
		st.pop_back();
		code.push_back("jz " + second);

		// gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}

		// gen loop jmp
		code.push_back("jmp " + first);
		code.push_back(second + ':');

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (PLACEHOLDER, 0)
		while (st.back().second != 0) {
			size += st.back().second;
			st.pop_back();
		}
		st.pop_back();
		if (size != 0) {
			code.push_back("dealloc " + to_string(size));
		}
	}

	else if (root->type == "<for>") {

		// enter scope
		st.push_back({PLACEHOLDER, 0});

		// gen declar
		generateIR(root->children[0], code);

		// gen label
		string first = labelify();
		string second = labelify();
		code.push_back(first + ':');

		// gen jmp
		generateIR(root->children[1], code);
		st.pop_back();
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
		code.push_back(second + ':');

		// exit scope
		int size = 0;
		// keep going until we hit the scope marker (PLACEHOLDER, 0)
		while (st.back().second != 0) {
			size += st.back().second;
			st.pop_back();
		}
		st.pop_back();
		if (size != 0) {
			code.push_back("dealloc " + to_string(size));
		}
	}

	else if (root->type == "<assignment>") {
		generateIR(root->children[2], code);

		string name = root->children[0]->val;
		code.push_back("pop " + findVar(name));

		st.pop_back();
	}

	else if (root->type == "<unaryExpression>") {
		// load operand
		generateIR(root->children[1], code);

		st.pop_back();

		code.push_back(ops.at(root->children[0]->val + 'u'));
		st.push_back({PLACEHOLDER, 8});
	}

	else if (root->type == "<binaryExpression>") {
		printStack();
		// first operand
		generateIR(root->children[0], code);

		// second operand
		generateIR(root->children[2], code);

		st.pop_back();
		st.pop_back();

		code.push_back(ops.at(root->children[1]->val));
		st.push_back({PLACEHOLDER, 8});
	}

	else if (root->type == "<functionCall>") {
		string funcName = root->children[0]->val;

		// evaluate all arguments
		// start from the back; good convention because pushing onto a stack (puts it into the right order)
		for (int i = root->children.size() - 1; i > 0; i--) {
			generateIR(root->children[i], code);
		}

		int size = 0;
		auto it = st.begin();
		for (int i = 1; i < root->children.size(); i++) {
			code.push_back("arg S(8)" + to_string(size));
			size += (it++)->second;
		}

		code.push_back("call " + funcName);
	}

	else if (root->type == "<program>") {
		for (int i = 0; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}
	}

	return;
}
