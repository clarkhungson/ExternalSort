// Author: Tong Hung Son
// email: hungsona93@gmail.com
// tel: +84164 944 5637

// Build command: g++ -o generator generator.cpp
// program: white noise generator

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
using namespace std;
#define NUMBER 50000000              // 50000000 ~ 1 GB in file size

int main() {
    srand(time(NULL));
    float value=0.0;
    ofstream myfile ("input2.txt");
    if (myfile.is_open())
    {
        for (unsigned int i = 0; i < NUMBER; ++i) 
        {
            value = rand(); 
            myfile << value << " " << i << "\n";
        }
        myfile.close();
    }
    else cout << "Unable to open file";
    return 0;
}