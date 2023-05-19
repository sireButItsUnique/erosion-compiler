#include "flag.hpp"

Flag::Flag() {
	clear();
}

void Flag::clear() {
	varType = false;
	varDec = false;
	varColon = false;
	funcType = false;
	funcDec = false;
	funcColon = false;
}

void Flag::funcWasColoned() {
	funcDec = false;
	funcColon = true;
}

void Flag::funcWasTyped() {
	funcColon = false;
	funcType = true;
}

void Flag::varWasColoned() {
	varDec = false;
	varColon = true;
}

void Flag::varWasTyped() {
	varColon = false;
	varType = true;
}
