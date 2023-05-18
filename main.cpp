#include "./LexicalAnalysis/Lexer.hpp"
#include "./Preprocessor/Preprocessor.hpp"
#include "./SyntaxAnalysis/ParseNode.hpp"

#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {
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

	Preprocessor preprocessor = Preprocessor(argv[1]);
	preprocessor.process();

	Lexer lexer = Lexer("preprocessed.cor");

	ParseNode *ast = new ParseNode(&lexer);
	if (!ast->build()) {
		ast->print();
		delete ast;
		remove("preprocessed.cor");
		return 1;
	}
	ast->print();

	delete ast;

	remove("preprocessed.cor");
	return 0;
}