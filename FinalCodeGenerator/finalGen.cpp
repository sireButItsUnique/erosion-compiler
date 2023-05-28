#include "finalGen.hpp"

void parseArg(string instr, string& arg, vector<string>& output) {
	if (arg[0] == 'A') {
		output.push_back(instr + " r15");
	} else if (arg[0] == 'S') {
		string res;
		if (arg[2] == '1') {
			if (instr == "push") {
				if (arg[4] == '0') {
					output.push_back("movzx ecx,byte [rsp]");
				} else {
					output.push_back("movzx ecx,byte [rsp+" + arg.substr(4) + ']');
				}
				output.push_back("sub rsp,1");
				output.push_back("mov [rsp],cl");
				return;
			} else if (instr == "pop") {
				output.push_back("movzx ecx,byte [rsp]");
				if (arg[4] == '0') {
					output.push_back("mov [rsp],cl");
				} else {
					output.push_back("mov [rsp+" + arg.substr(4) + "],cl");
				}
				output.push_back("add rsp,1");
				return;
			} else {
				res = " byte ";
			}
		} else if (arg[2] == '2') {
			if (instr == "push") {
				if (arg[4] == '0') {
					output.push_back("movzx ecx,word [rsp]");
				} else {
					output.push_back("movzx ecx,word [rsp+" + arg.substr(4) + ']');
				}
				output.push_back("sub rsp,2");
				output.push_back("mov [rsp],cx");
				return;
			} else if (instr == "pop") {
				output.push_back("movzx ecx,word [rsp]");
				if (arg[4] == '0') {
					output.push_back("mov [rsp],cx");
				} else {
					output.push_back("mov [rsp+" + arg.substr(4) + "],cx");
				}
				output.push_back("add rsp,2");
				return;
			} else {
				res = " word ";
			}
		} else if (arg[2] == '4') {
			if (instr == "push") {
				if (arg[4] == '0') {
					output.push_back("mov ecx,[rsp]");
				} else {
					output.push_back("mov ecx,[rsp+" + arg.substr(4) + ']');
				}
				output.push_back("sub rsp,4");
				output.push_back("mov [rsp],ecx");
				return;
			} else if (instr == "pop") {
				output.push_back("mov ecx,[rsp]");
				if (arg[4] == '0') {
					output.push_back("mov [rsp],ecx");
				} else {
					output.push_back("mov [rsp+" + arg.substr(4) + "],ecx");
				}
				output.push_back("add rsp,4");
				return;
			} else {
				res = " dword ";
			}
		} else if (arg[2] == '8') {
			if (instr == "push") {
				if (arg[4] == '0') {
					output.push_back("mov rcx,[rsp]");
				} else {
					output.push_back("mov rcx,[rsp+" + arg.substr(4) + ']');
				}
				output.push_back("sub rsp,8");
				output.push_back("mov [rsp],rcx");
				return;
			} else if (instr == "pop") {
				output.push_back("mov rcx,[rsp]");
				if (arg[4] == '0') {
					output.push_back("mov [rsp],rcx");
				} else {
					output.push_back("mov [rsp+" + arg.substr(4) + "],rcx");
				}
				output.push_back("add rsp,8");
				return;
			} else {
				res = " qword ";
			}
		}
		if (arg[4] == '0') {
			output.push_back(instr + res + "[rsp]");
		} else {
			output.push_back(instr + res + "[rsp+" + arg.substr(4) + ']');
		}
	} else if (arg[0] == 'C') {
		if (instr == "push") {
			output.push_back("sub rsp,8");
			output.push_back("mov qword [rsp]," + arg.substr(1));
		} else {
			output.push_back(instr + arg.substr(1));
		}
	} else if (arg[0] == 'V') {
		string res;
		if (arg[2] == '1') {
			if (instr == "push") {
				output.push_back("movzx ecx,byte [rel " + arg.substr(1) + ']');
				output.push_back("sub rsp,1");
				output.push_back("mov [rsp],cl");
				return;
			} else if (instr == "pop") {
				output.push_back("movzx ecx,byte [rsp]");
				output.push_back("mov [rel " + arg.substr(1) + "],cl");
				output.push_back("add rsp,1");
				return;
			} else {
				res = " byte ";
			}
		} else if (arg[2] == '2') {
			res = " word ";
		} else if (arg[2] == '4') {
			if (instr == "push") {
				output.push_back("mov ecx,[rel " + arg.substr(1) + ']');
				output.push_back("sub rsp,4");
				output.push_back("mov [rsp],ecx");
				return;
			} else if (instr == "pop") {
				output.push_back("mov ecx,[rsp]");
				output.push_back("mov [rel " + arg.substr(1) + "],ecx");
				output.push_back("add rsp,4");
				return;
			} else {
				res = " dword ";
			}
		} else if (arg[2] == '8') {
			res = " qword ";
		}
		output.push_back(instr + res + " [rel " + arg.substr(1) + "]");
	} else {
		output.push_back(instr + " r14");
	}
}

void CodeGenerator::generatex86(vector<string>& output, vector<string>& ir) {
	if (data.size()) {
		output.push_back("section .rodata");
	}
	for (auto& obj : data) {
		output.push_back(obj.first + ":");
		if (obj.second.type == "string") {
			output.back() += "db \"";
			for (int i = 0; i < obj.second.size; i++) {
				output.back() += ((char*)obj.second.data)[i];
			}
			output.back() += '"';
		} else if (obj.second.size == 1) {
			output.back() += "db ";
			output.back() += to_string(*(char*)obj.second.data);
		} else if (obj.second.size == 2) {
			output.back() += "dw ";
			output.back() += to_string(*(short*)obj.second.data);
		} else if (obj.second.size == 4) {
			output.back() += "dd ";
			output.back() += to_string(*(int*)obj.second.data);
		} else if (obj.second.size == 8) {
			output.back() += "dq ";
			output.back() += to_string(*(long long*)obj.second.data);
		}
	}
	if (bss.size()) {
		output.push_back("section .bss");
	}
	for (auto& obj : bss) {
		output.push_back(obj.first + ":");
		output.back() += "resb " + to_string(obj.second);
	}
	output.push_back("section .text");
	for (auto& line : ir) {
		if (line.back() == ':') {
			output.push_back(line);
			continue;
		}
		vector<string> tokens;
		size_t pos = -1;
		do {
			size_t prev = pos + 1;
			pos = line.find(' ', prev);
			tokens.push_back(line.substr(prev, pos - prev));
		} while (pos != string::npos);
		if (tokens[0] == "prologue") {
			output.push_back("push rbp");
			output.push_back("mov rbp,rsp");
		} else if (tokens[0] == "epilogue") {
			output.push_back("mov rsp,rbp");
			output.push_back("pop rbp");
		} else if (tokens[0] == "neg") {
			output.push_back("pop rax");
			output.push_back("neg rax");
			output.push_back("push rax");
		} else if (tokens[0] == "not") {
			output.push_back("pop rax");
			output.push_back("test rax,rax");
			output.push_back("setz al");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "cmt") {
			output.push_back("pop rax");
			output.push_back("not rax");
			output.push_back("push rax");
		} else if (tokens[0] == "mul") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("imul rdx");
			output.push_back("push rax");
		} else if (tokens[0] == "div") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cqo");
			output.push_back("idiv rdx");
			output.push_back("push rax");
		} else if (tokens[0] == "mod") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cqo");
			output.push_back("idiv rdx");
			output.push_back("push rdx");
		} else if (tokens[0] == "add") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("add rax,rdx");
			output.push_back("push rax");
		} else if (tokens[0] == "sub") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("sub rax,rdx");
			output.push_back("push rax");
		} else if (tokens[0] == "shr") {
			output.push_back("pop rcx");
			output.push_back("pop rax");
			output.push_back("shr rax,cl");
			output.push_back("push rax");
		} else if (tokens[0] == "shl") {
			output.push_back("pop rcx");
			output.push_back("pop rax");
			output.push_back("shl rax,cl");
			output.push_back("push rax");
		} else if (tokens[0] == "cmpl") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cmp rax,rdx");
			output.push_back("setl al");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "cmple") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cmp rax,rdx");
			output.push_back("setl ale");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "cmpg") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cmp rax,rdx");
			output.push_back("setl ag");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "cmpge") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cmp rax,rdx");
			output.push_back("setl age");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "cmpe") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cmp rax,rdx");
			output.push_back("sete al");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "cmpne") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cmp rax,rdx");
			output.push_back("setne al");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "and") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("and rax,rdx");
			output.push_back("push rax");
		} else if (tokens[0] == "xor") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("xor rax,rdx");
			output.push_back("push rax");
		} else if (tokens[0] == "or") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("or rax,rdx");
			output.push_back("push rax");
		} else if (tokens[0] == "push") {
			parseArg("push", tokens[1], output);
		} else if (tokens[0] == "pop") {
			parseArg("pop", tokens[1], output);
		} else if (tokens[0] == "call") {
			output.push_back(line);
		} else if (tokens[0] == "ret") {
			if (tokens[1] == "pop") {
				output.push_back("pop rax");
			} else {
				parseArg("mov rax,", tokens[1], output);
			}
			output.push_back("mov rsp,rbp");
			output.push_back("pop rbp");
			output.push_back("ret");
		} else if (tokens[0] == "jmp") {
			output.push_back(line);
		} else if (tokens[0][0] == 'j') {
			output.push_back("pop rax");
			if (tokens[0][1] == 'z') {
				output.push_back("cmp rax,0");
			} else {
				output.push_back("cmp rax,rax");
			}
			output.push_back(tokens[0] + " " + tokens[1]);
		} else if (tokens[0] == "alloc") {
			output.push_back("sub rsp," + tokens[1]);
		}
	}

	output.push_back("global _start");
	output.push_back("_start:");
	output.push_back("call main");
	output.push_back("mov edi,eax");
	output.push_back("mov eax,60");
	output.push_back("syscall");
}
