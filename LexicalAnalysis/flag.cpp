#include "flag.hpp"

Flag::Flag() {
	this->clear();
}

void Flag::clear() {
	this->varType = false;
	this->varDec = false;
	this->varColon = false;
	this->funcType = false;
	this->funcDec = false;
	this->funcColon = false;
}

void Flag::funcWasColoned() {
	this->funcDec = false;
	this->funcColon = true;
}

void Flag::funcWasTyped() {
	this->funcColon = false;
	this->funcType = true;
}

void Flag::varWasColoned() {
	this->varDec = false;
	this->varColon = true;
}

void Flag::varWasTyped() {
	this->varColon = false;
	this->varType = true;
}
