#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main() {
    int test = 42;

    // Write in file
    ofstream myfile("/media/virtualram/test.txt");

    if(myfile.is_open())
        myfile << test <<endl;
    else
        cout << "Cannot open file." << endl;

    myfile.close();

    while(true);

    return 0;
}
