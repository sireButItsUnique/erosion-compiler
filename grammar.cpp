#include <unordered_map>
#include <vector>
#include <string>
using namespace std;

class Grammar {
public:
    unordered_map<string, vector<string>> rules = {
        {"func",
            {
                "func:<type> <function>(<arg>) {<statement>}",
            }
        },
        {"arg", 
            {
                "<declaration>",
                "<declaration>, <arg>", 
                ""  
            }
        },
        {"statement",
            {
                "<statement> <newLine>",
                "<declaration>",
                "<func>,"
                "<conditional>",
                "<return>",
            }
        },

//         func:int main() {
        //     for (var:int i = 0, i < 69, i++) {
        //         if (i == 2) {
        //             console.print("matches!");
        //         }
        //     }
            
        //     return 0;
        // }

        {"conditional", 
            {
                "<while>",
                "<for>",
                "<if>"
            }
        },
        {"return", 
            {
                "return <literal>",
                "return <variable>"
                "return <function>(<args>)"
            }
        },
        {"declaration",
            {
                "var:<type> <variable>",
                "var:<type> <variable> = <literal>",
            }
        },
        {"if", 
            {
                "if <expression> {<statement>}"
            }
        },
        {"while", 
            {
                "while <expression> {<statement>}"
            }
        },
        {"for", 
            {
                "for (<declaration>, <expression>, <statement>) {<statement>}", 
                "for (<declaration> in <variable>) {<statement>}", //"in" var must be array
                "for (<declaration>, <declaration> in <variable>) {<statement>}"
            }
        },
        {"expression", 
            {
                "(<expression>)", 
                "<variable> <op> <expression>",
                "<variable> <op> <variable>", 
                "<variable> <op> <literal>",
                "<literal> <op> <expression>",
                "<literal> <op> <literal>", 
                "<literal> <op> <variable>",
                "<expression> <op> <expression>",
                "<expression> <op> <literal>",
                "<expression> <op> <variable>"
            }
        },
        {"literal", 
            {
                "<booleanLiteral>", 
                "<stringLiteral>", 
                "<floatingLiteral>", 
                "<integerLiteral>"
            }
        },
        {"type", 
            {
                "int", 
                "float",
                "string",
                "char", 
                "bool", 
                "<type>:<type>"
            }
        },
        {"function", {"TERMINAL_OP"}},
        {"keyword", {"TERMINAL_OP"}},
        {"op", {"TERMINAL_OP"}},
        {"booleanLiteral", {"TERMINAL_OP"}},
        {"stringLiteral", {"TERMINAL_OP"}},
        {"floatingLiteral", {"TERMINAL_OP"}},
        {"integerLiteral", {"TERMINAL_OP"}},
    };
    
    Grammar() {
        return;
    }

    bool match() {
        return false;
    }

    bool isTerminator(string type) {
        
        return false; // unless vector size is 1 and vector[0] == terminator
    }
};

/*
func:int main() {
    for (var:int i = 0, i < 69, i++) {
        if (i == 10) {
            return i;
        }
    }
    
    return 0;
}
*/