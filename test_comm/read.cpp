#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main() {
    string line;
    int p_test;
    ifstream file("/media/virtualram/test.txt");

    if(file.is_open()){
        while(getline(file, line)){
            cout << line << endl;
            p_test = stoul(line, nullptr, 10);
        }

	cout << p_test << endl;
    } else {
        cout << "Cannot read" << endl;
    }

    file.close();

    return 0;
}
