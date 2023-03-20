#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

enum TokenCode {
    op,
    keyword,
    type,
    variable,
    function,

    integer,
    boolean,
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
    string flag;
    unordered_map<string, TokenCode> declared; 
    int pos;
    
    Lexer(string source) {
        this->pos = 0;
        this->source = "";
        this->flag = "";

        this->sourceFile.open(source);
        string buffer;
        while (this->sourceFile >> buffer) {
            this->source += buffer;
            this->source += ' ';
        }

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
        
        if (pos >= this->source.size()) {
            return nullptr;
        }

        string& source = this->source;
        
        //number found
        if (isdigit(source[pos])) {
            int start = pos;
            while (isdigit(source[pos])) {
                pos++;
            }
            int end = pos;
            string text = source.substr(start, end - start);

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
            string text = source.substr(start, end - start + 1);

            return new SyntaxToken(start, end - start + 1, text, stringLiteral);
        }

        //keyword, variable, or function found
        else if (isalnum(source[pos])) {
            int start = pos;
            while (isalnum(source[pos])) {
                pos++;
            }
            int end = pos;
            string text = source.substr(start, end - start);

            //check here which one it is
            if (this->flag == "funcType") {
                this->flag = "funcName";
                return new SyntaxToken(start, end - start, text, type);
            }

            if (this->flag == "funcName") {
                this->flag = "";
                this->declared[text] = function;
                return new SyntaxToken(start, end - start, text, function);
            }

            if (this->flag == "varType") {
                this->flag = "varName";
                return new SyntaxToken(start, end - start, text, type);
            }

            if (this->flag == "varName") {
                this->flag = "";
                this->declared[text] = variable;
                return new SyntaxToken(start, end - start, text, variable);
            }

            if (this->declared.find(text) != this->declared.end()) {
                return new SyntaxToken(start, end - start, text, this->declared[text]);
            }
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
                    curChar = source[pos];
                }
                int end = pos;
                string text = source.substr(start, end - start);

                return new SyntaxToken(start, end - start, text, op);
            }
        }
    }
};
//
int main() {
    Lexer* lexer = new Lexer("test.cor");
    while (lexer->nextToken()) {

    }
    return 0;
}