#include <iostream>
#include <iomanip>
#include <bitset>

#ifndef FLOAT_VAL
#define FLOAT_VAL 1.753f;
#endif


template <uint32_t sign, uint32_t exponent, uint32_t mantissa>
class IEEE_754 { 
 private:
    enum
    colour_codes_e {
        console_red = 31,
        console_green = 32,
        console_blue = 34,
        console_default = 39
    };

    template<colour_codes_e colour>
    void print_colour(const char* c_str)
    {
       std::cout << "\033[" << colour << "m";
       std::cout << c_str;
       std::cout << "\033[" << console_default << "m";
    }

    template<colour_codes_e colour, size_t bitset_size>
    void print_colour(std::bitset<bitset_size> bitset)
    {
       std::cout << "\033[" << colour << "m";
       std::cout << bitset;
       std::cout << "\033[" << console_default << "m";
    } 
 public:
    static const uint8_t sign_bits = 1;
    static const uint8_t exponent_bits = 8;
    static const uint8_t mantissa_bits = 23;
    
    void print() {

       print_colour<console_red>("Sign ");
       print_colour<console_blue>("Exponent ");
       print_colour<console_green>("Mantissa ");
       std::cout << std::endl;

       print_colour<console_red, sign_bits>(std::bitset<sign_bits>(sign >> (32 - sign_bits)));
       print_colour<console_blue, exponent_bits>(std::bitset<exponent_bits>(exponent >> mantissa_bits));
       print_colour<console_green, mantissa_bits>(std::bitset<mantissa_bits>(mantissa));
       std::cout << std::endl;

    }
};


// SEEEEEEE EMMMMMMM MMMMMMMM MMMMMMMM
// 00       7F       FF       FF       Mantissa MASK
// 80       00       00       00       Sign MASK
// 7F       80       00       00       Exponent MASK
//
int main()
{
   constexpr const float f = FLOAT_VAL;

   std::cout  << f << std::endl;
   //constexpr const uint32_t cast = static_cast<uint32_t>(f);
   const uint32_t cast = 0xbf9df3b6; // -1.234

   const uint32_t exponent = cast & (0x7F800000);
   const uint32_t sign = cast & (0x80000000);
   const uint32_t mantissa = cast & (0x007FFFFF);

   IEEE_754<sign, exponent, mantissa> printer;
   printer.print();
}
