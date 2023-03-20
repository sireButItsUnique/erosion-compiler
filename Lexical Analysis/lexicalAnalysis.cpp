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
    string text;
    TokenCode type;

    SyntaxToken(int start, int len, string text, TokenCode type) {
        cout << "startPos: " << start << ", Length: " << len << ", Text: " << text << ", of type: " << type << endl;
        this->start = start;
        this->length = len;
        this->type = type;
        this->text = text;
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
            this->source += ' ';
        }
        cout << this->source << endl;
        this->sourceFile.close();
        return;
    }

    SyntaxToken* nextToken() {
        int& pos = this->pos;
        if (pos >= this->source.size()) {
            return nullptr;
        }

        while (this->source[pos] == ' ') {
            pos++;
        }

        string& source = this->source;

        //number found
        if (isdigit(source[pos])) {
            int start = pos;
            while (isdigit(source[pos])) {
                pos++;
            }
            int end = pos;
            string text = source.substr(start, end);

            return new SyntaxToken(start, end - start, text, integer);
        } 
        
        //string found
        else if (source[pos] == '\"') {
            int start = pos;
            pos++;
            while (source[pos] != '\"') {
                pos++;
            }
            int end = pos;
            pos++;
            string text = source.substr(start, end);

            return new SyntaxToken(start, end - start + 1, text, stringLiteral);
        }

        //keyword, variable, or function found
        else if (isalnum(source[pos])) {
            int start = pos;
            while (isalnum(source[pos])) {
                pos++;
            }
            int end = pos;
            string text = source.substr(start, end);

            //check here which one it is
            return new SyntaxToken(start, end - start, text, keyword);
        }

        //special character
        else {
            //brackets
            if (source[pos] == '{' || source[pos] == '}' || source[pos] == '[' || source[pos] == ']' || source[pos] == '(' || source[pos] == ')') {
                pos++;
                return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, bracket);
            } 
            
            //semicolon
            else if (source[pos] == ';') {
                this->pos++;
                return new SyntaxToken(pos - 1, 1, ";", newLine);
            } 
            
            //operator
            else {
                int start = pos;
                char curChar = source[pos];
                while (
                    !isalnum(curChar) && 
                    !(curChar == ';' || curChar == ' ' || curChar == '\"') && 
                    !(curChar == '{' || curChar == '}' || curChar == '[' || curChar == ']' || curChar == '(' || curChar == ')')
                ) {
                    pos++;
                }
                int end = pos;
                string text = source.substr(start, end);

                return new SyntaxToken(start, end - start, text, op);
            }
        }
    }
};
//
int main() {
    Lexer* lexer = new Lexer("test.cor");
    lexer->nextToken();
    lexer->nextToken();
    lexer->nextToken();
    return 0;
}