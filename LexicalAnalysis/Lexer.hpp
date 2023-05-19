#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "syntaxToken.hpp"
#include "flag.hpp"

using namespace std;

class Lexer {
private:
	string source;
	stringstream *sourceFile;
	Flag flags;
	unordered_map<string, TokenCode> declared;
	int pos;

public:
	/**
	 * @brief Construct a new Lexer object with the source file
	 * @param sourceFile The stringstream containing the source file
	 */
	Lexer(stringstream*);

	~Lexer();

	/**
	 * @brief Get the next token
	 * @return The next token to be parsed
	 */
	SyntaxToken* nextToken();

	/**
	 * @brief The the line number of the current token
	 * @return The line number
	 */
	int getLinenum();
};
