#include "finalGen.hpp"

void parseArg(string instr, string arg, deque<string>& output) {
	if (arg[0] == 'A') {
		output.push_back(instr + " r15");
	} else if (arg[0] == 'S' || arg[0] == 'V') {
		string thing;
		if (arg[0] == 'S') {
			thing = "[rbp";
			int off = stoi(arg.substr(4));
			if (off) {
				thing += to_string(-off);
			}
			thing += "]";
		} else {
			thing = "[rel " + arg.substr(4) + "]";
		}
		if (arg[2] == '1') {
			if (instr == "push") {
				output.push_back("movzx cx,byte " + thing);
				output.push_back("push cx");
			} else if (instr == "pop") {
				output.push_back("pop cx");
				output.push_back("mov " + thing + ",cl");
			} else {
				output.push_back(instr + " byte " + thing);
			}
		} else if (arg[2] == '2') {
			if (instr == "push") {
				output.push_back("mov cx," + thing);
				output.push_back("push cx");
			} else if (instr == "pop") {
				output.push_back("pop cx");
				output.push_back("mov " + thing + ",cx");
			} else {
				output.push_back(instr + " word " + thing);
			}
		} else if (arg[2] == '4') {
			if (instr == "push") {
				output.push_back("mov ecx,dword " + thing);
				output.push_back("push rcx");
			} else if (instr == "pop") {
				output.push_back("pop rcx");
				output.push_back("mov " + thing + ",ecx");
			} else {
				output.push_back(instr + " dword " + thing);
			}
		} else {
			if (instr == "push") {
				output.push_back("mov rcx," + thing);
				output.push_back("push rcx");
			} else if (instr == "pop") {
				output.push_back("pop rcx");
				output.push_back("mov qword " + thing + ",rcx");
			} else {
				output.push_back(instr + " qword " + thing);
			}
		}
	} else if (arg[0] == 'C') {
		if (instr == "push") {
			if (arg[2] == '1') {
				output.push_back("sub rsp,1");
				output.push_back("mov byte [rsp]," + arg.substr(4));
			} else if (arg[2] == '2') {
				output.push_back("sub rsp,2");
				output.push_back("mov word [rsp]," + arg.substr(4));
			} else if (arg[2] == '4') {
				output.push_back("sub rsp,4");
				output.push_back("mov dword [rsp]," + arg.substr(4));
			} else if (arg[2] == '8') {
				output.push_back("mov rcx," + arg.substr(4));
				output.push_back("push rcx");
			}
		} else {
			output.push_back(instr + ' ' + arg.substr(4));
		}
	}
}

void parseMov(string dst, string src, deque<string>& output) {
	parseArg("", dst, output);
	string d = output.back();
	output.pop_back();
	parseArg("", src, output);
	string s = output.back();
	output.pop_back();
	string regs[] = {" cl", " cx", " ecx", "", " rcx"};
	if (((dst[0] == 'S' || dst[0] == 'V') && (src[0] == 'S' || src[0] == 'V')) || (dst[2] != src[2] && src[0] != 'C')) { // both memory or not same size
		output.push_back("mov" + regs[(src[2] >> 1) - '0' / 2] + ',' + s);
		string reg = regs[(dst[2] >> 1) - '0' / 2];
		output.push_back("mov" + d + ',' + reg);
		return;
	}
	output.push_back("mov" + d + ',' + s);
}

void CodeGenerator::generatex86(deque<string>& output, deque<string>& ir) {
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
			output.push_back("pop rcx");
			output.push_back("pop rax");
			output.push_back("cqo");
			output.push_back("idiv rcx");
			output.push_back("push rax");
		} else if (tokens[0] == "mod") {
			output.push_back("pop rcx");
			output.push_back("pop rax");
			output.push_back("cqo");
			output.push_back("idiv rcx");
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
			output.push_back("setle al");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "cmpg") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cmp rax,rdx");
			output.push_back("setg al");
			output.push_back("movzx eax,al");
			output.push_back("push rax");
		} else if (tokens[0] == "cmpge") {
			output.push_back("pop rdx");
			output.push_back("pop rax");
			output.push_back("cmp rax,rdx");
			output.push_back("setge al");
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
		} else if (tokens[0] == "mov") {
			parseMov(tokens[1], tokens[2], output);
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
		} else if (tokens[0] == "alloc") {
			output.push_back("sub rsp," + tokens[1]);
		} else if (tokens[0][0] == 'j') {
			output.push_back("pop rax");
			if (tokens[0][1] == 'z') {
				output.push_back("cmp rax,0");
			} else {
				output.push_back("cmp rax,rax");
			}
			output.push_back(tokens[0] + " " + tokens[1]);
		}
	}

	output.push_back("global _start");
	output.push_back("_start:");
	output.push_back("call main");
	output.push_back("mov edi,eax");
	output.push_back("mov eax,60");
	output.push_back("syscall");
}

void parseX86(string& line, vector<string>& args, string& instr) {
	instr = line.substr(0, line.find(' '));
	size_t j = -1;
	do {
		size_t k = j + 1;
		j = line.find(',', k);
		args.push_back(line.substr(k, j - k));
	} while (j != string::npos);
}

void optimize(deque<string>& code) {
	// for (auto it = code.begin(); it != code.end(); it++) {
	// 	string &line = *it;
	// 	string instr, nextInstr;
	// 	vector<string> args, nextArgs;
	// 	parse(line, args, instr);
	// 	if (it + 1 == code.end()) {
	// 		return;
	// 	}
	// 	parse(*(it + 1), nextArgs, nextInstr);
	// 	if (instr == "mov" && nextInstr == "mov") {
	// 		if (args[0] == nextArgs[1]) {
	// 			it = code.erase(it);
	// 			*it = "mov " + nextArgs[0] + "," + args[1];
	// 			it--;
	// 		}
	// 	}
	// }
}
