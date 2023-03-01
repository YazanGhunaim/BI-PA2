#include <iostream>
#include <fstream>

int main()
{
    std::fstream myFile;
    myFile.open("output.txt", std::ios::out);

    int data = 0;
    while (std::cin >> data)
    {
        myFile << data;
    }
    if (std::cin.eof())
    {
        std::cout << "Operation successful" << std::endl;
        myFile.close();
        return 0;
    }
    std::cout << "Operation Failed." << std::endl;
    myFile.close();
    return 0;
}

/*
Implement a program that reads the standard input and
stores the input in a text file without remembering the whole input.
The input is terminated with an EOF.
*/