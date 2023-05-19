#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

#include "syntaxToken.hpp"
#include "flag.hpp"

using namespace std;

class Lexer {
private:
	fstream sourceFile;
	string source;
	Flag flags;
	unordered_map<string, TokenCode> declared;
	int pos;

public:
	/**
	 * @brief Construct a new Lexer object with the source file
	 * @param source The source file name
	 */
	Lexer(string);

	/**
	 * @brief Get the next token
	 * @return The next token to be parsed
	 */
	SyntaxToken* nextToken();
};
