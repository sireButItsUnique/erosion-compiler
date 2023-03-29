#pragma once

#include <string>
#include <iostream>
#include <vector>

using namespace std;

typedef enum TokenCode {
	op,				//0
	keyword,		//1
	type,			//2
	variable,		//3
	function,		//4
	declarator,		//5

	integerLiteral,	//6
	booleanLiteral,	//7
	floatingLiteral,//8
	stringLiteral,	//9

	curlyBracket,	//10
	roundBracket,	//11
	squareBracket,	//12

	ofType,			//13
	separator,		//14
	newLine,		//15

	lexicalError,	//16
} TokenCode;

class SyntaxToken {
public:
	int start;
	int length;
	string text;
	TokenCode type;

	/**
	 * @brief Construct a new Syntax Token object
	 * @param start Start position of the token
	 * @param len Length of the token
	 * @param text Text of the token
	 * @param type Type of the token
	 */
	SyntaxToken(int, int, string, TokenCode);

	/**
	 * @brief Stringify the token
	 * @return The stringified token
	 */
	string tokenCodeStringify();
};
