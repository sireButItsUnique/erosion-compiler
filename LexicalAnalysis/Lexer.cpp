#include "Lexer.hpp"

//remember to add flags after colon and "var" or "func"

Lexer::Lexer(string source) {
	pos = 0;
	this->source = "";
	flags = Flag();

	sourceFile.open(source);
	string buffer;
	int lastCharPos = 0; // Used to find chained newlines
	while (sourceFile >> buffer) {
		
		this->source += buffer;
		this->source += ' ';
	}

	sourceFile.close();
	return;
}

SyntaxToken* Lexer::nextToken() {
	int& pos = pos;
	if (pos >= source.size()) {
		return nullptr;
	}

	while (source[pos] == ' ') {
		pos++;
	}

	if (pos >= source.size()) {
		return nullptr;
	}

	string& source = source;

	//"of type" found
	if (flags.funcDec || flags.varDec) {
		if (source[pos] != ':') {
			return new SyntaxToken(pos, pos, "\":\" expected after declaring", lexicalError);
		} else {
			flags.funcDec ? flags.funcWasColoned() : flags.varWasColoned();
			pos++;
			return new SyntaxToken(pos - 1, 1, ":", ofType);
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
			flags.funcDec = true;
			return new SyntaxToken(start, end - start, text, declarator);
		}

		if (text == "var") { //declare var
			flags.varDec = true;
			return new SyntaxToken(start, end - start, text, declarator);
		}

		if (flags.funcColon) { //declare type
			flags.funcWasTyped();
			return new SyntaxToken(start, end - start, text, type);
		}

		if (flags.funcType) { //declare name
			flags.clear();
			declared[text] = TokenCode::function;
			return new SyntaxToken(start, end - start, text, TokenCode::function);
		}

		if (flags.varColon) { //declare type
			flags.varWasTyped();
			return new SyntaxToken(start, end - start, text, type);
		}

		if (flags.varType) { //declare name
			flags.clear();
			declared[text] = variable;
			return new SyntaxToken(start, end - start, text, variable);
		}

		if (declared.find(text) != declared.end()) { //already declared var/func
			return new SyntaxToken(start, end - start, text, declared[text]);
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
			pos++;
			return new SyntaxToken(pos - 1, 1, ";", newLine);
		}

		else if (source[pos] == ':') {
			if (flags.funcDec || flags.funcType) {
				flags.funcWasColoned();
			} else if (flags.varDec || flags.varType) {
				flags.varWasColoned();
			}

			pos++;
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

int Lexer::getLinenum() {
	
}
