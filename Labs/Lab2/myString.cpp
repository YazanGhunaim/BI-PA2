#include <iostream>
#include <iomanip>

class myString
{
private:
    std::string str;

public:
    void setString(std::string str_set)
    {
        str = str_set;
    }
    bool startsWith(std::string text)
    {
        if (text.size() > str.size())
        {
            std::cout << "no it dont" << endl;
            return false;
        }
        for (auto &i : text)
        {
            if (str[i] != text[i])
            {
                std::cout << "no it dont" << endl;
                return false;
            }
        }
        std::cout << "yes it does" << endl;
    }

    bool endsWith(std::string text)
    {
    }
};