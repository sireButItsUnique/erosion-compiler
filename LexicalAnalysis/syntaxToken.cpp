#include "syntaxToken.hpp"

string SyntaxToken::tokenCodeStringify() {
	vector<string> _tokenCodeStringify = {
		"<op>",
		"<keyword>",
		"<type>",
		"<variable>",
		"<function>",
		"<declarator>",
		"<integerLiteral>",
		"<booleanLiteral>",
		"<floatingLiteral>",
		"<stringLiteral>",
		"<curlyBracket>",
		"<roundBracket>",
		"<squareBracket>",
		"<ofType>",
		"<separator>",
		"<newLine>",
		"<lexicalError>",
	};
	return _tokenCodeStringify[type];
}

SyntaxToken::SyntaxToken(int start, int len, string text, TokenCode type, int lineNum) {
		this->start = start;
		this->length = len;
		this->type = type;
		this->text = text;
		this->line = lineNum;
		cout << "Line: " << lineNum << ", startPos: " << start << ", Length: " << len << ", Text: " << text << ", of type: " << tokenCodeStringify() << endl;
		return;
}
