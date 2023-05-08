#include "./Preprocessor/preprocessor.hpp"
#include "./LexicalAnalysis/lexicalAnalysis.hpp"
#include "./SyntaxAnalysis/syntaxAnalysis.hpp"

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

    Preprocessor* preprocessor = new Preprocessor(argv[1]);
    preprocessor->process();

    Lexer* lexer = new Lexer("preprocessed.cor");

    ParseTree* ast = new ParseTree(lexer);
    ast->build();
    ast->print();

    remove("preprocessed.cor");
    return 0;
}