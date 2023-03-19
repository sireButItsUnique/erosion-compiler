#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Format: erosion <filename>\n";
        return 0;
    }

    fstream source(argv[1]);
    fstream res;
    res.open("preprocessed.cor", ios::out);
    string buffer;
    if (!source) {
        cout << "Invalid file\n";
    }

    if (!res) {
        return 0;
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
    return 0;
}