//remember to add flags after colon and "var" or "func"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

enum TokenCode {
    op,             //0
    keyword,        //1
    type,           //2
    variable,       //3
    function,       //4
    declarator,     //5

    integerLiteral, //6
    booleanLiteral, //7
    floatingLiteral,//8
    stringLiteral,  //9

    curlyBracket,   //10
    roundBracket,   //11
    squareBracket,  //12

    ofType,         //13
    newLine,        //14

    lexicalError,   //15
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

//flags = what just happened before -> have a sense of what to expect
class Flag {
public:
    bool varDec;
    bool varColon;
    bool varType;
    bool funcDec;
    bool funcColon;
    bool funcType;

    Flag() {
        this->varType = false;
        this->varDec = false;
        this->varColon = false;
        this->funcType = false;
        this->funcDec = false;
        this->funcColon = false;
    }

    void clear() {
        this->varType = false;
        this->varDec = false;
        this->varColon = false;
        this->funcType = false;
        this->funcDec = false;
        this->funcColon = false;
    }

    void funcWasColoned() {
        this->funcDec = false;
        this->funcColon = true;
    }

    void funcWasTyped() {
        this->funcColon = false;
        this->funcType = true;
    } 

    void varWasColoned() {
        this->varDec = false;
        this->varColon = true;
    }

    void varWasTyped() {
        this->varColon = false;
        this->varType = true;
    }
};

class Lexer {
public:
    fstream sourceFile;
    string source;
    Flag flags;
    unordered_map<string, TokenCode> declared; 
    int pos;
    
    Lexer(string source) {
        this->pos = 0;
        this->source = "";
        this->flags = Flag();

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
        
        //"of type" found
        if (this->flags.funcDec || this->flags.varDec) {
            if (source[pos] != ':') {
                return new SyntaxToken(pos, pos, "\":\" expected after declaring", lexicalError);
            } else {
                this->flags.funcDec ? this->flags.funcWasColoned() : this->flags.varWasColoned();
                pos++;
                return new SyntaxToken(pos - 1, pos - 1, ":", ofType);
            }
        }

        //number found
        else if (isdigit(source[pos])) {
            int start = pos;
            while (isdigit(source[pos])) {
                pos++;
            }
            int end = pos;
            string text = source.substr(start, end - start);

            return new SyntaxToken(start, end - start, text, integerLiteral);
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
            if (text == "func") { //declare func
                this->flags.funcDec = true;
                return new SyntaxToken(start, end - start, text, declarator);
            }

            if (text == "var") { //declare var
                this->flags.varDec = true;
                return new SyntaxToken(start, end - start, text, declarator);
            }

            if (this->flags.funcColon) { //declare type
                this->flags.funcWasTyped();
                return new SyntaxToken(start, end - start, text, type);
            }

            if (this->flags.funcType) { //declare name
                this->flags.clear();
                this->declared[text] = function;
                return new SyntaxToken(start, end - start, text, function);
            }

            if (this->flags.varColon) { //declare type
                this->flags.varWasTyped();
                return new SyntaxToken(start, end - start, text, type);
            }

            if (this->flags.varType) { //declare name
                this->flags.clear();
                this->declared[text] = variable;
                return new SyntaxToken(start, end - start, text, variable);
            }

            if (this->declared.find(text) != this->declared.end()) { //already declared var/func
                return new SyntaxToken(start, end - start, text, this->declared[text]);
            }
            return new SyntaxToken(start, end - start, text, keyword);
        }

        //special character
        else {
            //brackets
            if (source[pos] == '{' || source[pos] == '}') {
                pos++;
                return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, roundBracket);
            }
            
            else if (source[pos] == '[' || source[pos] == ']') {
                pos++;
                return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, roundBracket);
            }
            
            else if (source[pos] == '(' || source[pos] == ')') {
                pos++;
                return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, roundBracket);
            } 
            
            //semicolon
            else if (source[pos] == ';') {
                this->pos++;
                return new SyntaxToken(pos - 1, 1, ";", newLine);
            } 

            else if (source[pos] == ':') {
                if (this->flags.funcDec || this->flags.funcType) {
                    this->flags.funcWasColoned();    
                } else if (this->flags.varDec || this->flags.varType) {
                    this->flags.varWasColoned();
                }

                this->pos++;
                return new SyntaxToken(pos - 1, 1, ":", ofType);
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