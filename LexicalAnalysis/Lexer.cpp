#include "Lexer.hpp"

Lexer::Lexer(stringstream* sourceFile) {
	pos = 0;
	lineNum = 1;
	this->source = "";
	this->sourceFile = sourceFile;
	flags = Flag();

	string buffer;

	while (*sourceFile >> buffer) {
		this->source += buffer + (char)sourceFile->peek();
	}


	return;
}

SyntaxToken* Lexer::nextToken(bool expressifying) {
	if (!expressifying && !expressionTokens.empty()) {
		SyntaxToken* token = expressionTokens.front();
		expressionTokens.pop_front();
		return token;
	}

	if (pos >= source.size()) {
		return nullptr;
	}

	while (source[pos] == ' ' || source[pos] == '\n') {
		if (source[pos] == '\n') {
			lineNum++;
		}
		pos++;
	}

	if (pos >= source.size()) {
		return nullptr;
	}

	if (source.substr(pos, 3) == "爨") {
		pos += 3;
		lineNum = stoi(source.substr(pos, source.find(' ', pos) - pos));
		pos = source.find(' ', pos) + 1;
	}

	//"of type" found
	if (flags.funcDec || flags.varDec) {
		if (source[pos] != ':') {
			return new SyntaxToken(pos, pos, "\":\" expected after declaring", lexicalError, lineNum);
		} else {
			flags.funcDec ? flags.funcWasColoned() : flags.varWasColoned();
			pos++;
			return new SyntaxToken(pos - 1, 1, ":", ofType, lineNum);
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

		return new SyntaxToken(start, end - start, text, integerLiteral, lineNum);
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

		return new SyntaxToken(start, end - start + 1, text, stringLiteral, lineNum);
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
		if (text == "true" || text == "false") {
			return new SyntaxToken(start, end - start, text, booleanLiteral, lineNum);
		}

		if (text == "func") { //declare func
			flags.funcDec = true;
			return new SyntaxToken(start, end - start, text, declarator, lineNum);
		}

		if (text == "var") { //declare var
			flags.varDec = true;
			return new SyntaxToken(start, end - start, text, declarator, lineNum);
		}

		if (flags.funcColon) { //declare type
			flags.funcWasTyped();
			return new SyntaxToken(start, end - start, text, type, lineNum);
		}

		if (flags.funcType) { //declare name
			flags.clear();
			declared[text] = function;
			return new SyntaxToken(start, end - start, text, function, lineNum);
		}

		if (flags.varColon) { //declare type
			flags.varWasTyped();
			return new SyntaxToken(start, end - start, text, type, lineNum);
		}

		if (flags.varType) { //declare name
			flags.clear();
			declared[text] = variable;
			return new SyntaxToken(start, end - start, text, variable, lineNum);
		}

		if (declared.find(text) != declared.end()) { //already declared var/func
			return new SyntaxToken(start, end - start, text, declared[text], lineNum);
		}
		return new SyntaxToken(start, end - start, text, keyword, lineNum);
	}

	//special character
	else {
		//brackets
		if (source[pos] == '{' || source[pos] == '}') {
			pos++;
			return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, curlyBracket, lineNum);
		}

		else if (source[pos] == '[' || source[pos] == ']') {
			pos++;
			return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, squareBracket, lineNum);
		}

		else if (source[pos] == '(' || source[pos] == ')') {
			pos++;
			return new SyntaxToken(pos - 1, 1, string{source[pos - 1]}, roundBracket, lineNum);
		}

		//comma
		else if (source[pos] == ',') {
			pos++;
			return new SyntaxToken(pos - 1, 1, ",", separator, lineNum);
		}

		//semicolon
		else if (source[pos] == ';') {
			pos++;
			return new SyntaxToken(pos - 1, 1, ";", newLine, lineNum);
		}

		else if (source[pos] == ':') {
			if (flags.funcDec || flags.funcType) {
				flags.funcWasColoned();
			} else if (flags.varDec || flags.varType) {
				flags.varWasColoned();
			}

			pos++;
			return new SyntaxToken(pos - 1, 1, ":", ofType, lineNum);
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

			return new SyntaxToken(start, end - start, text, op, lineNum);
		}
	}
}

Lexer::~Lexer() {
	delete sourceFile;
}
