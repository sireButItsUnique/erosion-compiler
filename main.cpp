#include "./IntermediateCodeGenerator/intermediateGen.hpp"
#include "./LexicalAnalysis/Lexer.hpp"
#include "./Preprocessor/Preprocessor.hpp"
#include "./SemanticAnalysis/semanticAnalysis.hpp"
#include "./SyntaxAnalysis/ParseNode.hpp"

#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Format: erosion <filename>\n";
		return 1;
	}

	string extension = string(argv[1]);
	if (extension.size() < 4) {
		cout << "Error: <filename> must be a .cor file\n";
		return 1;
	}
	if (extension.substr(extension.size() - 4, 4) != ".cor") {
		cout << "Error: <filename> must be a .cor file\n";
		return 1;
	}

	stringstream* str = preprocess(argv[1]);
	if (str == nullptr) {
		return 1;
	}

	Lexer lexer = Lexer(str);

	// lexer.expressify();

	ParseNode* ast = new ParseNode(&lexer);
	cerr << "\x1b[31m";
	if (!ast->build()) {
		cerr << "\x1b[0m";
		ast->print();
		delete ast;
		return 1;
	}
	cerr << "\x1b[0m";

	Diagnoser diagnoser = Diagnoser();
	if (!diagnoser.diagnose(ast)) {
		ast->print();
		delete ast;
		cerr << "\x1b[31m" << diagnoser.error << "\x1b[0m" << endl;
		return 1;
	}

	ast->print();

	IRGenerator irGen = IRGenerator();
	vector<string> ir;
	irGen.generateIR(ast, ir);
	for (auto line : ir) {
		cout << line << endl;
	}

	delete ast;

	return 0;
}