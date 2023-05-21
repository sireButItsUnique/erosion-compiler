#include "Preprocessor.hpp"

stringstream* preprocess(string fileName) {
	fstream source(fileName);
	if (!source) {
		cerr << "Invalid file" << endl;
		return nullptr;
	}

	stringstream* res = new stringstream();
	string buffer;

	int lineCnt = 0;
	while (getline(source, buffer)) {
		lineCnt++;
		size_t commentPos = buffer.find("//");
		while (commentPos != string::npos) {
			size_t quoteCnt = 0;
			for (auto it = buffer.begin(); it < buffer.begin() + commentPos; it++) {
				if (it == buffer.begin() || *(it - 1) != '\\') {
					if (*it == '\"') {
						quoteCnt++;
					}
				}
			}
			if (commentPos != string::npos && quoteCnt % 2 == 0) {
				buffer.erase(commentPos);
				break;
			}
			commentPos = buffer.find("//", commentPos + 1);
		}
		
		if (!buffer.empty()) {
			*res << "爨" << lineCnt << " " << buffer << endl; // dont question it
		}
	}

	source.close();

	return res;
}
