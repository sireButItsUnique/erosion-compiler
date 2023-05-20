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
			{"<declaration>"},
			{"<return>"},
			{"<func>"},
			{"<conditional>"},
			{"<assignment>"},
			{"<functionCall>", "<newLine>"},
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
	{"<args>",
		{{"<arg>"}},
	},
	{"<arg>",
		{
			{"<expression>"},
		}
	},
	{"<argDefs>",
		{{"<argDef>"}},
	},
	{"<argDef>",
		{
			{"var", ":", "<type>", "<variable>"},
			{"var", ":", "<type>", "<variable>", "=" ,"<literal>"},
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
			{"var", ":", "<type>", "<variable>", "<newLine>"},
			{"var", ":", "<type>", "<variable>", "=" ,"<literal>", "<newLine>"},
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
			{"for", "(", "<declaration>", "<expression>", "<statement>", ")" "{", "<statements>", "}"},
			{"for", "(", "<declaration>", "in", "<variable>", ")" "{", "<statements>", "}"}, //"in" var must be array
			{"for", "(", "<declaration>", "<declaration>", "in", "<variable>", ")", "{", "<statements>", "}"},
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
			{"<variable>"},
			{"<literal>"},
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
			{"<function>", "(", "<args>", ")"},
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
