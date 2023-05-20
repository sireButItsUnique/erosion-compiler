#pragma once

#include <fstream>
#include <list>
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
	list<SyntaxToken*> expressionTokens;
	int pos;

public:
	/**
	 * @brief Construct a new Lexer object with the source file
	 * @param sourceFile The stringstream containing the source file
	 */
	Lexer(stringstream*);

	~Lexer();

	//for expressifier: peek token until closingCnt > openingCnt || semicolon found
	//then instead of asking lexer for next token ask expressifier for next token
	/**
	 * @brief Get the next token, if there are expression tokens waiting it will prioritize those instead
	 * @return The next token to be parsed
	 */
	SyntaxToken* nextToken(bool = false);

	/**
	 * @brief Get the next token without moving where you are
	 * @return Stop peeking you pervert
	 */
	SyntaxToken* peekToken();

	/**
	 * @brief Orders the following tokens for order of operations; stores said tokens in expessionTokens to be given in the correct order when nextToken is called
	*/
	void expressify();

	/**
	 * @brief The the line number of the current token
	 * @return The line number
	 */
	int getLineNum();
};
