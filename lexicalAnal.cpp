#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class SyntaxToken {

};

class Lexer {
    fstream sourceFile;
    string source;
    int pos;
    
    Lexer(string source) {
        this->pos = 0;
        this->source = "";

        this->sourceFile.open(source);
        string buffer;
        while (this->sourceFile >> buffer) {
            this->source += buffer;
        }
        this->sourceFile.close();
        return;
    }

    SyntaxToken nextToken() {
        if (isdigit(source[pos]) ) {

        }
    }
};

int main() {
    fstream source("testPreprocessed.cor");
    string buffer;

    enum TokenCode {
        op,
        variable,
        function,
        integer,
        floating,
        string,
        curlyBracket,
        squareBracket,
        roundBracket,
    };

    while (cin >> buffer) {

    }

    source.close();
    return 0;
}