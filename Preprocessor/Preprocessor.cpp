#include "Preprocessor.hpp"

stringstream* preprocess(string fileName) {
	fstream source(fileName);
	if (!source) {
		cerr << "Invalid file" << endl;
		return nullptr;
	}

	stringstream* res = new stringstream();
	string buffer;

	while (getline(source, buffer)) {
		size_t commentPos = -1;
		do {
			commentPos = buffer.find("//", commentPos + 1);
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
		} while (commentPos != string::npos);

		if (!buffer.empty()) {
			*res << buffer << '\n';
		}
	}

	source.close();

	return res;
}
