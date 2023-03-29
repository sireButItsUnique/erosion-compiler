#pragma once

class Flag {
public:
	bool varDec = false;
	bool varColon = false;
	bool varType = false;
	bool funcDec = false;
	bool funcColon = false;
	bool funcType = false;

	Flag();

	void clear();

	void funcWasColoned();

	void funcWasTyped();

	void varWasColoned();

	void varWasTyped();
};
