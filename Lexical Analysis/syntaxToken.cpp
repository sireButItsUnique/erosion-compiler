#include <string>
#include <iostream>
#include <vector>
#include "tokenCode.hpp"

using namespace std;

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

    string tokenCodeStringify() {
        vector<string> tokenCodeStringify = {
            "op",
            "keyword",
            "type",
            "variable",
            "function",
            "declarator",
            "integerLiteral",
            "booleanLiteral",             
            "floatingLiteral",            
            "stringLiteral",  
            "curlyBracket",   
            "roundBracket",   
            "squareBracket",  
            "ofType",         
            "separator",      
            "newLine",        
            "lexicalError",   
        };
        
        return tokenCodeStringify[this->type];
    }
};
