#pragma once
 
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

/**
 * @brief rules[type][variation][term]
*/
unordered_map<string, vector<vector<string>>> rules{
	{"<program>",
		{
			{"<declaration>"},
			{"<return>"},
			{"<func>"},
			{"<conditional>"}
		}
	},
	{"<statement>",
		{
			{"<declaration>"},
			{"<conditional>"},
			{"<return>"},
		}
	},
	{"<func>",
		{
			{"func", ":", "<type>", "<function>", "(", "<arg>", ")", "{", "<statement>", "}"},
			{"func", ":", "<type>", "<function>", "(", ")", "{", "<statement>", "}"}
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
			{"return", "<literal>", "<newLine>"},
			{"return", "<variable>", "<newLine>"},
			{"return", "<function>", "(", "<arg>", ")", "<newLine>"}
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
			{"if", "<expression>", "{", "<statement>", "}"}
		}
	},
	{"<while>", 
		{
			{"while", "<expression>", "{", "<statement>", "}"},
		}
	},
	{"<for>", 
		{
			{"for", "(", "<declaration>", "<expression>", "<statement>", ")" "{", "<statement>", "}"}, 
			{"for", "(", "<declaration>", "in", "<variable>", ")" "{", "<statement>", "}"}, //"in" var must be array
			{"for", "(", "<declaration>", "<declaration>", "in", "<variable>", ")", "{", "<statement>", "}"}
		}
	},
	{"<expression>", 
		{
			{"(", "<expression>", ")"}, 
			{"<variable>", "<op>", "<expression>"},
			{"<variable>", "<op>", "<variable>"}, 
			{"<variable>", "<op>", "<literal>"},
			{"<literal>", "<op>", "<expression>"},
			{"<literal>", "<op>", "<literal>"}, 
			{"<literal>", "<op>", "<variable>"},
			{"<expression>", "<op>", "<expression>"},
			{"<expression>", "<op>", "<literal>"},
			{"<expression>", "<op>", "<variable>"},
		}
	},
	{"<literal>", 
		{
			{"<booleanLiteral>"}, 
			{"<stringLiteral>"}, 
			{"<floatingLiteral>"}, 
			{"<integerLiteral>"}
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
