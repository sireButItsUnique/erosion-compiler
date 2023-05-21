// #include "Lexer.hpp"
// #include "syntaxToken.hpp"

// //remember to add flags after colon and "var" or "func"
// //for expressifier: peek token until closingCnt > openingCnt || semicolon found
// //then instead of asking lexer for next token ask expressifier for next token

// SyntaxToken* Lexer::peekToken() {
// 	int oldPos = pos;
// 	SyntaxToken* token = nextToken(true);
// 	pos = oldPos;

// 	return token;
// }

// // TODO: Implement this
// void process(list<SyntaxToken*>::iterator begin, list<SyntaxToken*>::iterator end) {
// 	// Process the tokens
// 	// e.g. [a, +, b, *, c] => [a, +, (, b, *, c, )]]

// 	// Precedence (order):
// 	// (...) (treat as a single token)
// 	// +a -a !a ~a *a &a
// 	// a*b a/b a%b
// 	// a+b a-b
// 	// a>>b a<<b
// 	// a<b a<=b a>b a>=b
// 	// a==b a!=b
// 	// a&b
// 	// a^b
// 	// a|b
// 	// a&&b
// 	// a||b

// 	// Find all parenthetical sub-expressions and recursively process them
// 	for (list<SyntaxToken*>::iterator it = begin; it != --end; it++) {
// 		if ((*it)->type == roundBracket) {
// 			int openingCnt = 0;
// 			int closingCnt = 0;
// 			list<SyntaxToken*>::iterator it2 = it;
// 			while (it2 != end) {
// 				if ((*it2)->text == "(") {
// 					openingCnt++;
// 				} else if ((*it2)->text == ")") {
// 					closingCnt++;
// 				}
// 				if (openingCnt == closingCnt) {
// 					break;
// 				}
// 				it2++;
// 			}
// 			process(++it, it2);
// 		}
// 	}

// 	vector<vector<string>> ops = {
// 		{"+", "-", "!", "~", "*"},
// 		{"*", "/", "%"},
// 		{"+", "-"},
// 		{">>", "<<"},
// 		{"<", "<=", ">", ">="},
// 		{"==", "!="},
// 		{"&"},
// 		{"^"},
// 		{"|"},
// 		{"&&"},
// 		{"||"},
// 	};

// 	for (auto currOps : ops) { // loop thru all "priority lvls"
// 		for (list<SyntaxToken*>::iterator it = begin; it != --end; it++) { // Go through all terms

// 			// (*it)->... e.g. (*it)->text in order to get the text of the current iterator position thing etc.
// 			if ((*it)->type == op) {
// 				if (count(currOps.begin(), currOps.end(), (*it)->text)) {
// 					auto left = --it;
// 					it++;
// 					auto right = ++it;
// 					it--;
// 					int openingCnt = 0;
// 				}
// 			} else if ((*it)->type == roundBracket) {
// 				// Skip all parenthetical sub-expressions
// 				int openingCnt = 0;
// 				int closingCnt = 0;
// 				list<SyntaxToken*>::iterator it2 = it;
// 				while (it2 != end) {
// 					if ((*it2)->text == "(") {
// 						openingCnt++;
// 					} else if ((*it2)->text == ")") {
// 						closingCnt++;
// 					}
// 					if (openingCnt == closingCnt) {
// 						break;
// 					}
// 					it2++;
// 				}
// 				it = ++it2;
// 			}
// 		}
// 	}
// }

// void Lexer::expressify() {
// 	int closingCnt = 0;
// 	int openingCnt = 0;

// 	SyntaxToken* token;

// 	while ((token = peekToken()) && token->text != ";" && closingCnt <= openingCnt) {
// 		if (token->text == "(") {
// 			openingCnt++;
// 		} else if (token->text == ")") {
// 			closingCnt++;
// 		}
// 		expressionTokens.push_back(nextToken(true));
// 	}

// 	expressionTokens.push_front(new SyntaxToken(0, 0, "(", roundBracket));

// 	process(++expressionTokens.begin(), expressionTokens.end());

// 	return;
// }
