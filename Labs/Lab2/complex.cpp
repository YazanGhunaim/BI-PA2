#include <iostream>
#include <cmath>

#define DBL_EPSILON 10e-12

class complex
{
private:
    double m_real;
    double m_image;

public:
    constexpr complex(double real = 0, double image = 0) : m_real(real), m_image(image)
    {
    }

    complex add(const complex &second) const
    {
        return complex(m_real + second.m_real, m_image + second.m_image);
    }
    complex sub(const complex &second) const
    {
        return complex(m_real - second.m_real, m_image - second.m_image);
    }
    complex mul(const complex &second) const
    {
        return complex(m_real * second.m_real - m_image * second.m_image, m_real * second.m_image + m_image * second.m_real);
    }
    complex div(const complex &second) const
    {
        complex flip(second.m_real, -second.m_image);
        double divisor = second.m_real * second.m_real + second.m_image * second.m_image;
        return this->mul(flip).div(divisor);
    }

    double abs() const;

    void print(std::ostream &os) const;

    bool compare(const complex &second) const;

private:
    complex div(double value) const
    {
        return complex(m_real / value, m_image / value);
    }
};

double complex::abs() const
{
    return std::sqrt(m_real * m_real + m_image * m_image);
}

void complex::print(std::ostream &os) const
{
    os << "( " << this->m_real << ", " << this->m_image << "i )";
}

bool complex::compare(const complex &second) const
{
    return std::abs(m_real - second.m_real) <= DBL_EPSILON * 100 * std::abs(m_real + second.m_real) &&
           std::abs(m_image - second.m_image) <= DBL_EPSILON * 100 * std::abs(m_image + second.m_image);
}

int main()
{
    assert(complex(2, 7).add(complex(3, -4)).compare(complex(5, 3)));
    assert(complex(9, 5).sub(complex(4, 7)).compare(complex(5, -2)));
    assert(complex(3, 2).mul(complex(5, 6)).compare(complex(3, 28)));

    complex(3, 2).div(complex(4, -5)).print(std::cout);
    std::cout << std::endl;
    complex(2 / 41., 23 / 41.).print(std::cout);
    std::cout << std::endl;

    assert(complex(3, 2).div(complex(4, -5)).compare(complex(2 / 41., 23 / 41.)));

    std::cout << complex(11, 60).abs() << std::endl;
    return 0;
}