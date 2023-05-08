#pragma once

#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class Preprocessor {
public:
	string fileName;
	
	/**
	 * @brief Construct a new Preprocessor object
	 * @param fileName The name of the file to be preprocessed
	 */
	Preprocessor(string);

	/**
	 * @brief Preprocess the file
	 */
	void process();
};
