//remember to add flags after colon and "var" or "func"
#include <fstream>
#include <string>
#include <unordered_map>

#include "tokenCode.hpp"
#include "syntaxToken.cpp"
#include "flag.cpp"

using namespace std;

class Lexer {
private:
    fstream sourceFile;
    string source;
    Flag flags;
    unordered_map<string, TokenCode> declared; 
    int pos;
    
public:
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
                this->declared[text] = TokenCode::function;
                return new SyntaxToken(start, end - start, text, TokenCode::function);
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
                return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, curlyBracket);
            }
            
            else if (source[pos] == '[' || source[pos] == ']') {
                pos++;
                return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, squareBracket);
            }
            
            else if (source[pos] == '(' || source[pos] == ')') {
                pos++;
                return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, roundBracket);
            } 

            //comma
            else if (source[pos] == ',') {
                pos++;
                return new SyntaxToken(pos - 1, 1, ",", separator);
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
