#include "Preprocessor.hpp"

stringstream* preprocess(string fileName) {
	fstream source(fileName);
	if (!source) {
		cerr << "Invalid file" << endl;
		return nullptr;
	}

	stringstream* res = new stringstream();
	string buffer;

	string tmp;

	while (getline(source, buffer)) {
		int commentPos = buffer.find("//");
		if (commentPos != string::npos) {
			buffer.erase(commentPos);
		}

		if (!buffer.empty()) {
			*res << buffer << '\n';
		}
	}

	source.close();

	return res;
}
