#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main() {
    fstream source("test.cor");
    fstream res;
    res.open("testPreprocessed.cor", ios::out);
    string buffer;

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