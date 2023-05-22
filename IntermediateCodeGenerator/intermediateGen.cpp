#include "intermediateGen.hpp"

const unordered_map<string, string> ops = {
	{"-u","neg"},
	{"!u","not"},
	{"~u","cmt"},
	{"*","mul"},
	{"/","div"},
	{"%","mod"},
	{"+","add"},
	{"-","sub"},
	{">>","shr"},
	{"<<","shl"},
	{"<","cmpl"},
	{"<=","cmple"},
	{">","cmpg"},
	{">=","cmpge"},
	{"==","cmpe"},
	{"!=","cmpne"},
	{"&","and"},
	{"^","xor"},
	{"|","or"},
	{"&&","and"},
	{"||","or"},
};

string IRGenerator::labelify() {
	return ".L" + to_string(labelCnt++);
}

int IRGenerator::generateIR(ParseNode *root, vector<string>& code) {
	if (root->type == "<func>") {
		st.push({SENTINEL, 8});
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
					((char *)thing.data)[thing.size - 1] = '\0';
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
					((char *)thing.data)[thing.size - 1] = '\0';
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
					st.push({root->children[1]->val, tmp});
					code.push_back("push L(" + to_string(sizes.at(root->children[0]->val)) + ')' + to_string(tmp));
				}
			}
		}
	}

	else if (root->type == "<return>") {
		st.pop();
		code.push_back("pop 0");
		code.push_back("ret 0");
	}

	else if (root->type == "<if>") {

		// New scope
		st.push({SENTINEL, 0});

		// gen jmp
		string label = labelify();
		generateIR(root->children[0], code);
		st.pop();
		code.push_back("pop 0 ; IF start");
		code.push_back("test 0 0");
		code.push_back("jnz " + label);

		// gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}

		// gen label
		code.push_back(label + ": ; IF end");

		// exit scope
		st.pop();
	}

	else if (root->type == "<while>") {

		// New scope
		st.push({SENTINEL, 0});

		// gen jmp
		string first = labelify();
		string second = labelify();
		code.push_back(first + ": ; WHILE start");
		generateIR(root->children[0], code);
		st.pop();
		code.push_back("pop 0");
		code.push_back("test 0 0");
		code.push_back("jz " + second);

		// gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}

		// gen loop jmp
		code.push_back("jmp " + first);
		code.push_back(second + ": ; WHILE end");

		// exit scope
		st.pop();
	}

	else if (root->type == "<for>") {

		// New scope
		st.push({SENTINEL, 0});

		// gen declar
		generateIR(root->children[0], code);

		// gen label
		string first = labelify();
		string second = labelify();
		code.push_back(first + ": ; FOR start");

		// gen jmp
		generateIR(root->children[1], code);
		st.pop();
		code.push_back("pop 0");
		code.push_back("test 0 0");
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
		st.pop();
	}

	else if (root->type == "<assignment>") {

	}

	else if (root->type == "<unaryExpression>") {

		code.push_back("; start unary expression");

		// load operand
		if (root->children[1]->type == "<literal>") {
			code.push_back("mov 0 L" + root->children[1]->children[0]->val);
		}
		else if (root->children[1]->type == "<variable>") {
			code.push_back("mov 0 V" + root->children[1]->val);
		}
		else {
			generateIR(root->children[1], code);

			st.pop();
			code.push_back("pop 0"); // move output into register
		}

		code.push_back(ops.at(root->children[0]->val + 'u'));
		st.push({SENTINEL, 8});
		code.push_back("push 0"); // push output onto stack
		code.push_back("; end unary expression");
	}

	else if (root->type == "<binaryExpression>") {

		code.push_back("; start binary expression");

		// first operand
		if (root->children[0]->type == "<literal>") {
			code.push_back("mov 0 L" + root->children[0]->children[0]->val);
		}
		else if (root->children[0]->type == "<variable>") {
			code.push_back("mov 0 V" + root->children[0]->val);
		}
		else {
			generateIR(root->children[0], code);

			st.pop();
			code.push_back("pop 0"); // move output into register
		}

		// second operand
		if (root->children[2]->type == "<literal>") {
			code.push_back("mov 1 L" + root->children[2]->children[0]->val);
		}
		else if (root->children[2]->type == "<variable>") {
			code.push_back("mov 1 V" + root->children[2]->val);
		}
		else {
			generateIR(root->children[2], code);

			st.pop();
			code.push_back("pop 1"); // move output into register
		}

		code.push_back(ops.at(root->children[1]->val));
		st.push({SENTINEL, 8});
		code.push_back("push 0"); // push output onto stack
		code.push_back("; end binary expression");
	}

	else if (root->type == "<functionCall>") {

	}

	else if (root->type == "<program>") {
		for (int i = 0; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}
	}

	else {

	}

	return 0;
}
