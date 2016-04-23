#include <bitset>
#include <iomanip>
#include <iostream>

#ifndef FLOAT_VAL
// -1.234
#define FLOAT_VAL 0xbf9df3b6
#endif

namespace
{ // anonymous

static const uint8_t sign_bits = 1;
static const uint8_t exponent_bits = 8;
static const uint8_t mantissa_bits = 23;

// SEEEEEEE EMMMMMMM MMMMMMMM MMMMMMMM
// 00       7F       FF       FF       Mantissa MASK
// 80       00       00       00       Sign MASK
// 7F       80       00       00       Exponent MASK
static const uint32_t sign_mask = 0x80000000;
static const uint32_t exponent_mask = 0x7F800000;
static const uint32_t mantissa_mask = 0x007FFFFF;

} // end anonymous namespace

template <uint32_t sign, uint32_t exponent, uint32_t mantissa> class IEEE_754
{
  private:
    enum colour_codes_e
    {
        console_red = 31,
        console_green = 32,
        console_blue = 34,
        console_default = 39
    };

    template <colour_codes_e colour> void print_colour(const char* c_str)
    {
        std::cout << "\033[" << colour << "m";
        std::cout << c_str;
        std::cout << "\033[" << console_default << "m";
    }

    template <colour_codes_e colour, size_t bitset_size> void print_colour(std::bitset<bitset_size> bitset)
    {
        std::cout << "\033[" << colour << "m";
        std::cout << bitset;
        std::cout << "\033[" << console_default << "m";
    }

    // TODO: Implement this
    constexpr const char* calc_float_str() const
    {
        return "-1.234";
    }

    const char* m_float_str;

  public:
    constexpr IEEE_754()
    {
        m_float_str = calc_float_str();
    }

    void print()
    {
        print_colour<console_red>("Sign ");
        print_colour<console_blue>("Exponent ");
        print_colour<console_green>("Mantissa ");
        std::cout << std::endl;

        print_colour<console_red, sign_bits>(std::bitset<sign_bits>(sign));
        print_colour<console_blue, exponent_bits>(std::bitset<exponent_bits>(exponent));
        print_colour<console_green, mantissa_bits>(std::bitset<mantissa_bits>(mantissa));
        std::cout << std::endl;

        std::cout << m_float_str << std::endl;
    }
};

int main()
{
    constexpr const uint32_t float_rep = FLOAT_VAL;

    // Get sign bit
    const uint32_t sign = (float_rep & sign_mask) >> (32 - sign_bits);

    // Get 8 exponents bits
    const uint32_t exponent = (float_rep & exponent_mask) >> mantissa_bits;

    // Get 23 mantissa bits
    const uint32_t mantissa = float_rep & mantissa_mask;

    // Instantiate print helper class
    IEEE_754<sign, exponent, mantissa> printer;

    // Print to stdout at runtime
    printer.print();

    return 0;
}
