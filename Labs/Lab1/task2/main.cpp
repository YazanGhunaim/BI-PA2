#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    int array[15];
    int read[15];

    for (int i = 0; i < 15; ++i)
    {
        cin >> array[i];
    }

    fstream myFile;
    myFile.open("input.bin", ios::out | ios::binary | ios::in);

    for (int i = 0; i < 15; ++i)
    {
        myFile << array[i];
    }

    fstream readFile;
    readFile.open("output.bin", ios::in | ios::out | ios::binary);
    for (int i = 0; i < 15; ++i)
    {
        myFile >> read[i];
    }

    for (int i = 0; i < 15; ++i)
    {
        cout << hex << read[i] << endl;
    }

    myFile.close();
    readFile.close();
    return 0;
}