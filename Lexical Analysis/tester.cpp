#include "lexicalAnalysis.cpp"

int main() {
    Lexer* lexer = new Lexer("test.cor");
    while (lexer->nextToken()) {

    }
    return 0;
}