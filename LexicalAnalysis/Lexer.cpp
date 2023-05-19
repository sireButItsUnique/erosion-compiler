#include "Lexer.hpp"

//remember to add flags after colon and "var" or "func"

Lexer::Lexer(stringstream* sourceFile) {
	pos = 0;
	this->source = "";
	this->sourceFile = sourceFile;
	flags = Flag();

	string buffer;

	while (*sourceFile >> buffer) {
		this->source += buffer + (char)sourceFile->peek();
	}

	return;
}

SyntaxToken* Lexer::nextToken() {
	if (pos >= source.size()) {
		return nullptr;
	}

	while (source[pos] == ' ' || source[pos] == '\n') {
		pos++;
	}

	if (pos >= source.size()) {
		return nullptr;
	}

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
		string text;
		int start = pos;
		pos++;
		while (source[pos] != '\"' || source[pos - 1] == '\\') {
			pos++;
		}
		int end = pos;
		pos++;
		text = source.substr(start, end - start + 1);

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
				!(curChar == ';' || curChar == ' ' || curChar == '\n' || curChar == '\"') &&
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
	int cnt = count(source.begin(), source.begin() + max(pos - 1, 0), '\n');
	if (cnt == string::npos) {
		return 1;
	}
	return cnt + 1;
}

Lexer::~Lexer() {
	delete sourceFile;
}
