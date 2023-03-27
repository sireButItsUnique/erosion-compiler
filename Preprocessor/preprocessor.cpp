#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class Preprocessor {
public:
    string fileName;
    
    Preprocessor(string fileName) {
        this->fileName = fileName;
    }

    void process() {
        fstream source(this->fileName);
        fstream res;
        string buffer;

        res.open("preprocessed.cor", ios::out);
        if (!source) {
            cout << "Invalid file\n";
        }

        while (getline(source, buffer)) {
            int commentPos = buffer.find("//");
            if (commentPos != string::npos) {
                buffer.erase(commentPos);
            }
            
            if (!buffer.empty()) {
                res << buffer << endl;
            } 
        }

        source.close();
        res.close();
    }
};