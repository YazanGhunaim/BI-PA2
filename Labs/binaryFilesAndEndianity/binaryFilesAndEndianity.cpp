#include <fstream>
#include <iostream>
#include <iomanip>

bool read(int &number)
{
    return (bool)(std::cin >> number);
}

bool read(std::string &token)
{
    return (bool)(std::cin >> token);
}

bool read(double &number)
{
    return (bool)(std::cin >> number);
}

bool isComputerBigEndian()
{
    int x = 0xAA000000;
    return *(unsigned char *)&x == 0xAA;
}

int swapEndianity(unsigned number)
{
    return (number << 24) | ((number << 8) & 0x00FF0000) | ((number >> 8) & 0x0000FF00) | (number >> 24);
}

void writeNumber447BinaryToFileAsBigEndian()
{
    std::ofstream file("number-big-endian.bin", std::ios::binary);
    int number = 447;
    // if computer is little endian we swap endianity to big endian
    if (isComputerBigEndian() == false)
    {
        number = swapEndianity(number);
    }

    // (char *)&number is pointer to first byte and we write 4 bytes to the binary file
    file.write((char *)&number, 4);
}

bool readBinaryBigEndianFromFile(std::ifstream &file, int &number)
{
    file.read((char *)&number, 4);
    if (isComputerBigEndian() == false)
    {
        number = swapEndianity(number);
    }
    return (bool)file;
}

int main()
{
    if (isComputerBigEndian())
    {
        std::cout << "Youre computer is big endian." << std::endl;
    }
    else
    {
        std::cout << "Youre computer is little endian." << std::endl;
    }

    // create binary file with encoded 447 as 4bytes integerin big endian in it
    writeNumber447BinaryToFileAsBigEndian();
    int number;
    std::string filename;
    std::cout << "Enter the filename and number" << std::endl;
    if (!read(filename))
    {
        std::cout << "Invalid input." << std::endl;
        return 1;
    }
    if (!read(number))
    {
        std::cout << "Invalid input." << std::endl;
        return 1;
    }
    std::ifstream file(filename, std::ios::binary);
    if (file)
    { // if there is everything OK with the file.
        int numberInFile;
        readBinaryBigEndianFromFile(file, numberInFile);
        if (!(file))
        {
            std::cout << "There is no number in the file\n";
            return 2;
        }
        if (number == numberInFile)
        {
            std::cout << "Numbers are same\n";
        }
        else
        {
            std::cout << "Numbers are different\n";
        }
    }
    else
    {
        std::cout << "Something is wrong with the file.\n";
    }

    return 0;
}