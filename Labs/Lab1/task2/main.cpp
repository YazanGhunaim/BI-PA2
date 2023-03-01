#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main()
{
    vector<int> nums;
    int data = 0;
    while (cin >> data)
    {
        nums.push_back(data);
    }

    ofstream myFile;
    myFile.open("numbers.bin", ios::binary);
    if (myFile.is_open())
    {
        int size = nums.size();
        myFile.write((const char *)&size, sizeof(size));
        myFile.write((const char *)nums.data(), size * sizeof(int));
        myFile.close();
    }
    else
    {
        cout << "couldnt open file." << endl;
        return 1;
    }

    // reading
    vector<int> numsT;
    ifstream infile("numbers.bin", ios::binary);
    if (infile.is_open())
    {
        int size;
        infile.read((char *)&size, sizeof(size));
        numsT.resize(size);
        infile.read((char *)numsT.data(), size * sizeof(int));
        infile.close();
    }
    else
    {
        cout << "could not be read" << endl;
        return 1;
    }

    // Print data from file to standard output
    cout << "Numbers from file: ";
    for (auto i : numsT)
    {
        cout << i << endl;
    }
    cout << endl;
    return 0;
}
