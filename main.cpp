#include "./FinalCodeGenerator/finalGen.hpp"
#include "./IntermediateCodeGenerator/intermediateGen.hpp"
#include "./LexicalAnalysis/Lexer.hpp"
#include "./Preprocessor/Preprocessor.hpp"
#include "./SemanticAnalysis/semanticAnalysis.hpp"
#include "./SyntaxAnalysis/ParseNode.hpp"

#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
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

	stringstream *str = preprocess(argv[1]);
	if (str == nullptr) {
		return 1;
	}

	Lexer lexer = Lexer(str);

	// lexer.expressify();

	ParseNode *ast = new ParseNode(&lexer);
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

	CodeGenerator irGen = CodeGenerator();
	vector<string> ir;
	irGen.generateIR(ast, ir);
	deque<string> output;
	irGen.generatex86(output, ir);

	ofstream file("out.asm");

	for (auto line : ir) {
		cout << line << endl;
	}

	cout << endl;

	optimize(output);

	for (auto line : output) {
		file << line << '\n';
		cout << line << endl;
	}

	file.close();
	delete ast;

	pid_t pid = fork();
	if (pid == -1) {
		// error
		cerr << "Could not make child" << endl;
	} else if (pid) {
		// parent
		waitpid(pid, NULL, 0);
		remove("out.asm");
	} else {
		// child
		execlp("sedimentation", "sedimentation", "-f", "elf", "out.asm", NULL);
		return 0;
	}

	pid = fork();
	if (pid == -1) {
		// error
		cerr << "Could not make child" << endl;
	} else if (pid) {
		// parent
		waitpid(pid, NULL, 0);
		remove("out.o");
	} else {
		// child
		execlp("ld", "ld", "out.o", NULL);
	}

	return 0;
}