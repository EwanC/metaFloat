#include <bitset>
#include <iomanip>
#include <iostream>

#ifndef FLOAT_VAL
#define FLOAT_VAL 0xc32d3be7 // -173.234
//#define FLOAT_VAL 0x00000000 // +0
//#define FLOAT_VAL 0x80000000 // -0
//#define FLOAT_VAL 0x7f800000 // +inf
//#define FLOAT_VAL 0xff800000 // -inf
//#define FLOAT_VAL 0xffc20200 // NaN
#endif

namespace
{ // anonymous

// sign * 2^exponent * mantissa
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

static const uint16_t buffer_size = 256;

enum colour_codes_e
{
    console_red = 31,
    console_green = 32,
    console_blue = 34,
    console_purple = 35,
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

} // end anonymous namespace

template <uint32_t sign, uint32_t exponent, uint32_t mantissa> class IEEE_754
{
  private:
    void print_int_as_string(uint32_t x) const
    {
        while (x != 0)
        {
            int rem = x % 10;
            s_printable_buffer[s_buffer_index++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
            x = x / 10;
        }
    }

    static char s_printable_buffer[buffer_size];
    static unsigned int s_buffer_index;

  public:
    constexpr IEEE_754()
    {
        if (sign)
            s_printable_buffer[s_buffer_index++] = '-';

        // Exponent is a 2's complement number
        // exponent has a bias of 127, which we must subtract
        const int32_t adjusted_exp = static_cast<int32_t>(exponent) - 127;

        //// Hidden 24th bit is 1 in mantissa
        const uint32_t full_mantissa = mantissa | (1 << mantissa_bits);

        const uint32_t adjustment = mantissa_bits - adjusted_exp;

        uint32_t integer_component = full_mantissa >> adjustment;

        print_int_as_string(integer_component);
        s_printable_buffer[s_buffer_index++] = '.';

        //// Top of the fraction
        uint32_t frac = full_mantissa & ((1 << adjustment) - 1);

        //// Base of the fraction, must be bigger than frac
        uint32_t base = 1 << adjustment;

        int decimal_places = 0;
        const int max_digits_to_print = 12;
        const int base_10 = 10;
        while (frac != 0 && decimal_places++ < max_digits_to_print)
        {
            frac *= base_10;
            print_int_as_string(frac / base);
            frac %= base;
        }

        s_printable_buffer[s_buffer_index++] = '\0';
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

        print_colour<console_purple>("Float: ");
        print_colour<console_purple>(s_printable_buffer);
        std::cout << std::endl;
    }
};

template <uint32_t sign, uint32_t exponent, uint32_t mantissa>
unsigned int IEEE_754<sign, exponent, mantissa>::s_buffer_index = 0;

template <uint32_t sign, uint32_t exponent, uint32_t mantissa>
char IEEE_754<sign, exponent, mantissa>::s_printable_buffer[buffer_size] = {0};

template <uint32_t sign> struct IEEE_754<sign, 0u, 0>
{
    void print()
    {
        print_colour<console_red>("Sign ");
        print_colour<console_blue>("Exponent ");
        print_colour<console_green>("Mantissa ");
        std::cout << std::endl;

        print_colour<console_red, sign_bits>(std::bitset<sign_bits>(sign));
        print_colour<console_blue, exponent_bits>(std::bitset<exponent_bits>(0));
        print_colour<console_green, mantissa_bits>(std::bitset<mantissa_bits>(0));
        std::cout << std::endl;

        if (sign)
            print_colour<console_purple>("Float: -0");
        else
            print_colour<console_purple>("Float: +0");
        std::cout << std::endl;
    }
};

template <uint32_t sign> struct IEEE_754<sign, 255u, 0u>
{
    void print()
    {
        print_colour<console_red>("Sign ");
        print_colour<console_blue>("Exponent ");
        print_colour<console_green>("Mantissa ");
        std::cout << std::endl;

        print_colour<console_red, sign_bits>(std::bitset<sign_bits>(sign));
        print_colour<console_blue, exponent_bits>(std::bitset<exponent_bits>(255u));
        print_colour<console_green, mantissa_bits>(std::bitset<mantissa_bits>(0u));
        std::cout << std::endl;

        if (sign)
            print_colour<console_purple>("Float: -INF");
        else
            print_colour<console_purple>("Float: +INF");
        std::cout << std::endl;
    }
};

template <uint32_t sign, uint32_t mantissa> struct IEEE_754<sign, 255u, mantissa>
{
    void print()
    {
        print_colour<console_red>("Sign ");
        print_colour<console_blue>("Exponent ");
        print_colour<console_green>("Mantissa ");
        std::cout << std::endl;

        print_colour<console_red, sign_bits>(std::bitset<sign_bits>(sign));
        print_colour<console_blue, exponent_bits>(std::bitset<exponent_bits>(255u));
        print_colour<console_green, mantissa_bits>(std::bitset<mantissa_bits>(mantissa));
        std::cout << std::endl;

        print_colour<console_purple>("Float: NaN");
        std::cout << std::endl;
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

    // Get 23 mantissa bits
    // Instantiate print helper class
    IEEE_754<sign, exponent, mantissa> printer;

    // Print to stdout at runtime
    printer.print();

    return 0;
}
