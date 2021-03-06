#include <bitset>
#include <iomanip>
#include <iostream>

// The input float value to print can be set at compile time
// with CXX_FLAGS=-DFLOAT_VALUE=0x.....
// Otherwise a predefined value will be used.
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

// -------------------------------------------------
// 32 Bit layout of IEEE 754 single precision float
// -------------------------------------------------
// SEEEEEEE EMMMMMMM MMMMMMMM MMMMMMMM
// 80       00       00       00       Sign MASK
// 7F       80       00       00       Exponent MASK
// 00       7F       FF       FF       Mantissa MASK
// -------------------------------------------------
static const uint8_t g_sign_bits = 1;
static const uint8_t g_exponent_bits = 8;
static const uint8_t g_mantissa_bits = 23;

static const uint32_t g_sign_mask = 0x80000000;
static const uint32_t g_exponent_mask = 0x7F800000;
static const uint32_t g_mantissa_mask = 0x007FFFFF;

// ANSI colour codes for UNIX consoles
enum colour_codes_e
{
    console_red = 31,
    console_green = 32,
    console_blue = 34,
    console_purple = 35,
    console_default = 39
};

// Prints object of type T to stdout with templated ANSI colour
template <colour_codes_e colour, typename T>
void print_colour(T str)
{
    std::cout << "\033[" << colour << "m";
    std::cout << str;
    std::cout << "\033[" << console_default << "m";
}

// Pretty prints IEEE 754 bits
template <uint32_t sign, uint32_t exponent, uint32_t mantissa>
void print_bit_layout()
{
    print_colour<console_red>("Sign ");
    print_colour<console_blue>("Exponent ");
    print_colour<console_green>("Mantissa ");
    std::cout << std::endl;

    print_colour<console_red>(std::bitset<g_sign_bits>(sign));
    print_colour<console_blue>(std::bitset<g_exponent_bits>(exponent));
    print_colour<console_green>(std::bitset<g_mantissa_bits>(mantissa));
    std::cout << std::endl;
}

} // end anonymous namespace

template <uint32_t sign, uint32_t exponent, uint32_t mantissa>
struct IEEE_754
{
    static void print()
    {
        print_bit_layout<sign, exponent, mantissa>();
        print_colour<console_purple>("Float: ");

        if (sign)
            print_colour<console_purple>('-');

        // Exponent is a  2's complement number with a bias of 127
        const int32_t adjusted_exp = static_cast<int32_t>(exponent) - 127;

        // Since the mantissa is normalized the first bit will always be 1.
        // And is therefore hidden to save space, but we need to add
        // it back here as the 24th.
        const uint32_t full_mantissa = mantissa | (1 << g_mantissa_bits);

        // Offset used to denormalize number
        const uint32_t adjustment = g_mantissa_bits - adjusted_exp;

        // Print whole number followed by decimal point
        constexpr uint32_t integer_component = full_mantissa >> adjustment;
        print_colour<console_purple>(integer_component);
        print_colour<console_purple>('.');

        // Print fractional component
        //
        // We consider the floating point as a fraction
        // with a top and bottom.
        //
        // Each digit is printed by multiplying the top
        // of the fraction by the base we want
        // to print, i.e. 10. Then printing the quotient
        // of the new fraction.
        //
        // Finally to prepare the fraction for printing the
        // next digit, we use the fraction.

        // Top of the fraction
        uint32_t frac = full_mantissa & ((1 << adjustment) - 1);

        // Bottom of the fraction
        uint32_t base = 1 << adjustment;

        // We want 12 decimal digits of precision
        const int max_digits_to_print = 12;
        const int base_10 = 10;

        // Iterate over each digit we want to print
        int decimal_places = 0;
        while (frac != 0 && decimal_places++ < max_digits_to_print)
        {
            frac *= base_10;
            print_colour<console_purple>(frac / base);
            frac %= base;
        }
        std::cout << std::endl;
    }
};

// Partial specialization for -/+ 0
template <uint32_t sign>
struct IEEE_754<sign, 0u, 0u>
{
    static void print()
    {
        print_bit_layout<sign, 0u, 0u>();
        if (sign)
            print_colour<console_purple>("Float: -0");
        else
            print_colour<console_purple>("Float: +0");
        std::cout << std::endl;
    }
};

// Partial specialization for -/+ infinity
template <uint32_t sign>
struct IEEE_754<sign, 255u, 0u>
{
    static void print()
    {
        print_bit_layout<sign, 255u, 0u>();
        if (sign)
            print_colour<console_purple>("Float: -INF");
        else
            print_colour<console_purple>("Float: +INF");
        std::cout << std::endl;
    }
};

// Partial specialization for NaN values
template <uint32_t sign, uint32_t mantissa>
struct IEEE_754<sign, 255u, mantissa>
{
    static void print()
    {
        print_bit_layout<sign, 255u, mantissa>();
        print_colour<console_purple>("Float: NaN");
        std::cout << std::endl;
    }
};

int main()
{
    // Assign macro to a local variable for convenience
    constexpr const uint32_t float_bits = FLOAT_VAL;

    // Get sign bit
    const uint32_t sign = (float_bits & g_sign_mask) >> (32 - g_sign_bits);

    // Get 8 exponents bits
    const uint32_t exponent = (float_bits & g_exponent_mask) >> g_mantissa_bits;

    // Get 23 mantissa bits
    const uint32_t mantissa = float_bits & g_mantissa_mask;

    // Print float to stdout at runtime
    IEEE_754<sign, exponent, mantissa>::print();

    return 0;
}
