#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

class anagramChecker
{
private:
public:
    static bool isAnagram(string str1, string str2)
    {
        map<char, int> freq_map;
        for (char c : str1)
        {
            freq_map[c] += 1;
        }
        for (char c : str2)
        {
            freq_map[c] -= 1;
        }

        for (auto &i : freq_map)
        {
            if (i.second != 0)
            {
                cout << "no" << endl;
                return false;
            }
        }
        cout << "yes" << endl;
        return true;
    }
};

int main()
{
    anagramChecker::isAnagram("manya", "asodaso");
    return 0;
}