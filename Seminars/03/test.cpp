#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
using namespace std;

class CTimeTester;
#endif /* __PROGTEST__ */

class CTime
{
private:
    int m_Hour;
    int m_Minute;
    int m_Second;

    bool valid_time(int hour, int min, int sec)
    {
        return hour >= 0 && hour <= 23 && min >= 0 && min < 60 && sec >= 0 && sec < 60;
    }

public:
    // constructor, destructor
    CTime() : m_Hour(0), m_Minute(0), m_Second(0) {}
    CTime(int hour, int min)
    {
        if (!valid_time(hour, min, 0))
        {
            throw std::invalid_argument("Time not valid.");
        }
        m_Hour = hour;
        m_Minute = min;
        m_Second = 0;
    }
    CTime(int hour, int min, int sec)
    {
        if (!valid_time(hour, min, sec))
        {
            throw std::invalid_argument("Time not valid.");
        }
        m_Hour = hour;
        m_Minute = min;
        m_Second = sec;
    }
    ~CTime() {}
    // arithmetic operators
    CTime operator+(int sec) const
    {
        int total_minutes_added = (sec + m_Second) / 60;
        int total_minutes = m_Hour * 60 + m_Minute + total_minutes_added;
        int hour = (total_minutes / 60) % 24;
        int minutes = total_minutes % 60;
        int seconds = (sec + m_Second) % 60;
        return CTime(hour, minutes, seconds);
    }
    CTime &operator+=(int sec)
    {
        *this = *this + sec;
        return *this;
    }
    CTime operator-(int sec) const
    {
        int total_seconds = m_Hour * 3600 + m_Minute * 60 + m_Second - sec;
        if (total_seconds < 0)
        {
            total_seconds += 86400; // add a day's worth of seconds
        }
        int new_hours = (total_seconds / 3600) % 24;
        int remaining_seconds = total_seconds % 3600;
        int new_minutes = (remaining_seconds / 60) % 60;
        int new_seconds = remaining_seconds % 60;
        return CTime(new_hours, new_minutes, new_seconds);
    }
    CTime &operator-=(int seconds)
    {
        *this = *this - seconds;
        return *this;
    }
    int operator-(const CTime &other) const
    {
        int thisSeconds = m_Hour * 3600 + m_Minute * 60 + m_Second;
        int otherSeconds = other.m_Hour * 3600 + other.m_Minute * 60 + other.m_Second;
        int diffSeconds = thisSeconds - otherSeconds;
        if (diffSeconds < 0)
        {
            diffSeconds += 86400; // 86400 seconds in a day
        }
        return diffSeconds;
    }

    // prefix
    CTime &operator++()
    {
        return (*this += 1);
    }
    CTime &operator--()
    {
        return (*this -= 1);
    }
    // postfix
    CTime operator++(int _)
    {
        CTime temp{*this};
        ++*this;
        return temp;
    }
    CTime operator--(int _)
    {
        CTime temp{*this};
        --*this;
        return temp;
    }

    // comparison operators
    bool operator<(const CTime &other) const
    {
        return std::tie(m_Hour, m_Minute, m_Second) < std::tie(other.m_Hour, other.m_Minute, other.m_Second);
    }
    bool operator<=(const CTime &other) const
    {
        return std::tie(m_Hour, m_Minute, m_Second) <= std::tie(other.m_Hour, other.m_Minute, other.m_Second);
    }
    bool operator>(const CTime &other) const
    {
        if (*this == other)
        {
            return false;
        }
        return !(*this < other);
    }
    bool operator>=(const CTime &other) const
    {
        return !(*this < other);
    }
    bool operator==(const CTime &other) const
    {
        return std::tie(m_Hour, m_Minute, m_Second) == std::tie(other.m_Hour, other.m_Minute, other.m_Second);
    }
    bool operator!=(const CTime &other) const
    {
        return !(*this == other);
    }
    // output operator
    friend std::ostream &operator<<(std::ostream &os, const CTime &time);
    friend class ::CTimeTester;
};

std::ostream &operator<<(std::ostream &os, const CTime &time)
{
    os << std::setfill(' ') << std::setw(2) << time.m_Hour << ":"
       << std::setfill('0') << std::setw(2) << time.m_Minute << ":"
       << std::setw(2) << time.m_Second;
    return os;
}

#ifndef __PROGTEST__
struct CTimeTester
{
    static bool test(const CTime &time, int hour, int minute, int second)
    {
        return time.m_Hour == hour && time.m_Minute == minute && time.m_Second == second;
    }
};

#include <cassert>
#include <sstream>

int main()
{
    CTime a{12, 30};
    assert(CTimeTester::test(a, 12, 30, 0));

    CTime b{13, 30};
    assert(CTimeTester::test(b, 13, 30, 0));

    assert(b - a == 3600);

    assert(CTimeTester::test(a + 60, 12, 31, 0));
    assert(CTimeTester::test(a - 60, 12, 29, 0));
    assert(a < b);
    assert(a <= b);
    assert(a != b);
    assert(!(a > b));
    assert(!(a >= b));
    assert(!(a == b));

    while (++a != b)
        ;
    assert(a == b);

    std::ostringstream output;
    assert(static_cast<bool>(output << a));
    assert(output.str() == "13:30:00");

    assert(a++ == b++);
    assert(a == b);

    assert(--a == --b);
    assert(a == b);

    assert(a-- == b--);
    assert(a == b);
    return 0;
}
#endif /* __PROGTEST__ */
