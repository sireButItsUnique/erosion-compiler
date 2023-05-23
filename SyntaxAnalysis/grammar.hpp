#pragma once

#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

/**
 * @brief rules[type][variation][term]
*/
static const unordered_map<string, vector<vector<string>>> rules{
	{"<program>",
		{
			{"<func>"},
			{"<statement>"},
		}
	},
	{"<statements>",
		{{"<statement>"}},
	},
	{"<statement>",
		{
			{"<declaration>"},
			{"<conditional>"},
			{"<return>"},
			{"<assignment>"},
			{"<functionCall>", "<newLine>"},
		}
	},
	{"<func>",
		{
			{"func", ":", "<type>", "<function>", "(", "<argDefs>", ")", "{", "<statements>", "}"},
			{"func", ":", "<type>", "<function>", "(", ")", "{", "<statements>", "}"},
		}
	},
	{"<functions>",
		{{"<expression>"}},
	},
	{"<argDefs>",
		{{"<def>"}},
	},
	{"<def>",
		{
			{"var", ":", "<type>", "<variable>"},
			{"var", ":", "<type>", "<variable>", "=" ,"<expression>"},
		}
	},
	{"<conditional>",
		{
			{"<while>"},
			{"<for>"},
			{"<if>"},
		}
	},
	{"<return>",
		{
			{"return", "<newLine>"},
			{"return", "<expression>", "<newLine>"},
		}
	},
	{"<declaration>",
		{
			{"<def>", "<newLine>"},
		}
	},
	{"<if>",
		{
			{"if", "(", "<expression>", ")", "{", "<statements>", "}"},
		}
	},
	{"<while>",
		{
			{"while", "(", "<expression>", ")", "{", "<statements>", "}"},
		}
	},
	{"<for>",
		{
			{"for", "(", "<declaration>", "<expression>", "<newLine>", "<statement>", ")", "{", "<statements>", "}"},
			{"for", "(", "<declaration>", "in", "<variable>", ")" "{", "<statements>", "}"}, //"in" var must be array
			{"for", "(", "<declaration>", ",", "<declaration>", "in", "<variable>", ")", "{", "<statements>", "}"},
		}
	},
	{"<assignment>",
		{
			{"<variable>", "=", "<expression>", "<newLine>"},
		}
	},
	{"<expression>",
		{
			{"(", "<expression>", ")"},
			{"<literal>"},
			{"<variable>"},
			{"<functionCall>"},
			{"<unaryExpression>"},
			{"<binaryExpression>"},
		}
	},
	{"<unaryExpression>",
		{
			{"<op>", "<expression>"},
		}
	},
	{"<binaryExpression>",
		{
			{"<expression>", "<op>", "<expression>"},
		}
	},
	{"<functionCall>",
		{
			{"<function>", "(", ")"},
			{"<function>", "(", "<functions>", ")"},
		}
	},
	{"<literal>",
		{
			{"<booleanLiteral>"},
			{"<stringLiteral>"},
			{"<floatingLiteral>"},
			{"<integerLiteral>"},
		}
	},
	{"<type>", {{"TERMINAL_OP"}}}, //note: deal with <type>:<type> later
	{"<newLine>", {{"TERMINAL_OP"}}}, // semicolon
	{"<variable>", {{"TERMINAL_OP"}}},
	{"<function>", {{"TERMINAL_OP"}}},
	{"<keyword>", {{"TERMINAL_OP"}}},
	{"<op>", {{"TERMINAL_OP"}}},
	{"<booleanLiteral>", {{"TERMINAL_OP"}}},
	{"<stringLiteral>", {{"TERMINAL_OP"}}},
	{"<floatingLiteral>", {{"TERMINAL_OP"}}},
	{"<integerLiteral>", {{"TERMINAL_OP"}}},
};
