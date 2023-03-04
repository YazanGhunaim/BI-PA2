#include <iostream>
#include <cassert>
#include <algorithm>
#include <numeric>

using namespace std;

class MyString
{
private:
    string str;

public:
    MyString(string data = "") : str(data) {}
    void setString(string set_str)
    {
        str = move(set_str);
    }
    string getString()
    {
        return str;
    }
    bool startsWith(const string &start_text) const;
    bool endsWith(const string &end_text) const;
    MyString toUpper() const;
    MyString toLower() const;
    unsigned CountLetter(char c) const;
};

bool MyString::startsWith(const string &start_text) const
{
    size_t length = start_text.size();
    if (str.size() < length || length == 0)
    {
        return false;
    }
    for (size_t i = 0; i < length; ++i)
    {
        if (str[i] != start_text[i])
        {
            return false;
        }
    }
    return true;
}

bool MyString::endsWith(const string &end_text) const
{
    size_t length = end_text.size();
    if (str.size() < length || length == 0)
    {
        return false;
    }

    size_t temp = length - 1;
    for (size_t i = str.length() - 1; i > (str.length() - length); --i)
    {
        if (str[i] != end_text[temp--])
        {
            return false;
        }
    }
    return true;
}

MyString MyString::toUpper() const
{
    string tmp = str;
    transform(tmp.begin(), tmp.end(), tmp.begin(), static_cast<int (*)(int)>(&std::toupper));
    return MyString(move(tmp));
}

MyString MyString::toLower() const
{
    string tmp = str;
    transform(tmp.begin(), tmp.end(), tmp.begin(), static_cast<int (*)(int)>(&std::tolower));
    return MyString(move(tmp));
}

unsigned MyString::CountLetter(char c) const
{
    return std::accumulate(str.begin(), str.end(), 0u, [=](unsigned val, char current)
                           { return val + (tolower(current) == c); });
}

int main()
{
    MyString str;

    str.setString("yazan");
    assert(str.getString() == "yazan");

    assert(str.startsWith("ya") == true);
    assert(str.startsWith("") == false);
    assert(str.startsWith(" ") == false);
    assert(str.startsWith("azan") == false);

    assert(str.endsWith("n") == true);
    assert(str.endsWith("an") == true);
    assert(str.endsWith("zan") == true);
    assert(str.endsWith("yazan") == true);
    assert(str.endsWith("yazan") == true);
    assert(str.endsWith("") == false);
    assert(str.endsWith("yaz") == false);
    assert(str.endsWith("yaza") == false);

    MyString upper = str.toUpper();
    assert(upper.getString() == "YAZAN");

    MyString lower = upper.toLower();
    assert(lower.getString() == str.getString());

    assert(upper.CountLetter('y') == 1);
    assert(upper.CountLetter('z') == 1);
    assert(upper.CountLetter('a') == 2);
    assert(upper.CountLetter('l') == 0);
    return 0;
}