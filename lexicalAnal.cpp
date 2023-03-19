#include <fstream>
#include <iostream>
#include <string>
using namespace std;

enum TokenCode {
    op,
    keyword,
    variable,
    function,
    integer,
    floating,
    stringLiteral,
    bracket,
    newLine,
};

class SyntaxToken {
public:
    int start;
    int length;
    TokenCode type;

    SyntaxToken(int start, int len, TokenCode type) {
        cout << "startPos: " << start << ", Length: " << len << ", of type: " << type << endl;
        this->start = start;
        this->length = len;
        this->type = type;
        return;
    }
};

class Lexer {
public:
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
        int& pos = this->pos;
        string& source = this->source;

        //number found
        if (isdigit(source[pos])) {
            int start = pos;
            while (isdigit(source[pos])) {
                pos++;
            }
            int end = pos - 1;
            return SyntaxToken(start, end - start + 1, integer);
        } 
        
        //string found
        else if (source[pos] == '\"') {
            int start = pos;
            while (source[pos] != '\"') {
                pos++;
            }
            int end = pos;
            return SyntaxToken(start, end - start + 1, stringLiteral);
        }

        //keyword, variable, or function found
        else if (isalnum(source[pos])) {
            int start = pos;
            while (isalnum(source[pos])) {
                pos++;
            }
            int end = pos - 1;
            string text = source.substr(start, end);
            
            //check here which one it is
            return SyntaxToken(start, end - start + 1, keyword);
        }

        //operation or bracket
        else {
            if (source[pos] == '{' || source[pos] == '}' || source[pos] == '[' || source[pos] == ']' || source[pos] == '(' || source[pos] == ')') {
                return SyntaxToken(pos, 1, bracket);
            } else if (source[pos] == ';') {
                return SyntaxToken(pos, 1, newLine);
            } else {
                return SyntaxToken(pos, 1, op);
            }
            this->pos++;
        }
    }
};

int main() {
    Lexer* lexer = new Lexer("preprocessed.cor");
    lexer->nextToken();
    lexer->nextToken();
    lexer->nextToken();
    lexer->nextToken();
    lexer->nextToken();
    return 0;
}