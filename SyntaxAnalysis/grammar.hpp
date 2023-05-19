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
		}
	},
	{"<func>",
		{
			{"func", ":", "<type>", "<function>", "(", "<arg>", ")", "{", "<statements>", "}"},
			{"func", ":", "<type>", "<function>", "(", ")", "{", "<statements>", "}"},
		}
	},
	{"<arg>",
		{
			{"var", ":", "<type>", "<variable>"},
			{"var", ":", "<type>", "<variable>", "=" ,"<literal>"},
			{"var", ":", "<type>", "<variable>", ", ", "<arg>"},
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
			{"return", "<literal>", "<newLine>"},
			{"return", "<variable>", "<newLine>"},
			{"return", "<function>", "(", "<arg>", ")", "<newLine>"},
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
			{"<variable>", "=", "<variable>", "<newLine>"},
			{"<variable>", "=", "<literal>", "<newLine>"},
		}
	},
	{"<expression>",
		{
			{"<variable>", "<op>", "<variable>"},
			{"<variable>", "<op>", "<literal>"},
			{"<literal>", "<op>", "<literal>"},
			{"<literal>", "<op>", "<variable>"},
			{"<literal>", "<op>", "<expression>"},
			{"<variable>", "<op>", "<expression>"},
			{"<expression>", "<op>", "<literal>"},
			{"<expression>", "<op>", "<variable>"},
			{"<expression>", "<op>", "<expression>"},
			{"(", "<expression>", ")"},
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
