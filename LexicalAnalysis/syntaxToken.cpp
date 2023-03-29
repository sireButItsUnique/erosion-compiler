#include "syntaxToken.hpp"

SyntaxToken::SyntaxToken(int start, int len, string text, TokenCode type) {
		cout << "startPos: " << start << ", Length: " << len << ", Text: " << text << ", of type: " << type << endl;
		this->start = start;
		this->length = len;
		this->type = type;
		this->text = text;
		return;
}

string SyntaxToken::tokenCodeStringify() {
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
