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

int IRGenerator::generateIR(ParseNode *root, vector<string>& code) {
	if (root->type == "<func>") {
	
	}
	
	else if (root->type == "<def>") {

	}
	
	else if (root->type == "<return>") {

	}
	
	else if (root->type == "<if>") {
		
		//gen jmp
		string label = 'L' + to_string(labelCnt);
		labelCnt++;
		generateIR(root->children[0], code);
		code.push_back("pop 0");
		code.push_back("test 0 0");
		code.push_back("jnz ." + label);

		//gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}
		
		//gen label
		code.push_back(label + ":");
	}

	else if (root->type == "<while>") {
		
		//gen jmp
		string label = 'L' + to_string(labelCnt);
		labelCnt++;
		generateIR(root->children[0], code);
		code.push_back("pop 0");
		code.push_back("test 0 0");
		code.push_back("jz ." + label);

		//gen label
		code.push_back(label + ":");

		//gen contents
		for (int i = 1; i < root->children.size(); i++) {
			generateIR(root->children[i], code);
		}
		
		//gen loop jmp
		label = 'L' + to_string(labelCnt);
		labelCnt++;
		generateIR(root->children[0], code);
		code.push_back("pop 0");
		code.push_back("test 0 0");
		code.push_back("jnz ." + label);
	}
	
	else if (root->type == "<for>") {
		
	}
	
	else if (root->type == "<assignment>") {
		
	}

	else if (root->type == "<unaryExpression>") {

		//first operand
		if (root->children[0]->type == "<binaryExpression>" || root->children[0]->type == "<unaryExpression>") {
			generateIR(root->children[0], code);

			code.push_back("pop 0"); // move output into register
		} else {
			code.push_back("mov 0 " + root->children[0]->val); // move constant into register
		}

		code.push_back(ops.at(root->children[1]->val + 'u'));
		code.push_back("push 0"); // push output onto stack
	}
	
	else if (root->type == "<binaryExpression>") {

		//first operand
		if (root->children[0]->type == "<binaryExpression>" || root->children[0]->type == "<unaryExpression>") {
			generateIR(root->children[0], code);

			code.push_back("pop 0"); // move output into register
		} else {
			code.push_back("mov 0 " + root->children[0]->val); // move constant into register
		}
		
		//second operand
		if (root->children[2]->type == "<binaryExpression>" || root->children[2]->type == "<unaryExpression>") {
			generateIR(root->children[2], code); 
			
			code.push_back("pop 1"); // move output into register
		} else {
			code.push_back("mov 1 " + root->children[2]->val); // move constant into register
		}

		code.push_back(ops.at(root->children[1]->val));
		code.push_back("push 0"); // push output onto stack
	}

	else if (root->type == "<functionCall>") {

	}

	else {

	}

	return 0;
}
