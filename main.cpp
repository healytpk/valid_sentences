#include <string_view> // string_view
#include <iostream>    // cout, endl

bool IsValid(std::string_view const &s);

int main(void)
{
    std::string_view const strs[] = {
        "The quick brown fox said \"hello Mr lazy dog\".",
        "The quick brown fox said hello Mr lazy dog.",
        "One lazy dog is too few, 13 is too many.",     
        "One lazy dog is too few, thirteen is too many.",
        "How many \"lazy dogs\" are there?",
        "The quick brown fox said \"hello Mr. lazy dog\".",
        "the quick brown fox said \"hello Mr lazy dog\"",
        "\"The quick brown fox said \"hello Mr lazy dog.\"",
        "One lazy dog is too few, 12 is too many.",
        "Are there 11, 12, or 13 lazy dogs?",
        "There is no punctuation in this sentence"
    };

    for (auto const &s : strs)
    {
    	std::cout << (IsValid(s) ? "valid" : "INVALID") << std::endl;
    }
}

#include "generator.hpp" /* supporting type for coroutines */

Generator<std::uintmax_t> ExtractDecimalNumbers(std::string_view const &s);

#include <cstdint>     /* uintmax_t */
#include <cctype>      /* isalpha, islower, isdigit */
#include <cstring>     /* strchr */
#include <charconv>    /* from_chars */
#include <algorithm>   /* count */

bool IsValid(std::string_view const &s)
{
    using std::isalpha;
    using std::islower;
    
    // 1: String starts with a capital letter.
    if ( isalpha(s[0u]) && islower(s[0u]) ) return false;
    
    // 2: String has an even number of quotation marks.
    if ( 1u & std::count(s.cbegin(), s.cend(), '"' ) ) return false;

    // 3: String ends with one of the following sentence termination characters: ".", "?", "!"
    if ( nullptr ==  std::strchr(".?!", s.back()) ) return false;

    // 4: String has no period characters other than the last character
    if ( s.find_first_of('.') < (s.size() - 1u) ) return false;

    // 5: Numbers below 13 are spelled out

    auto gen = ExtractDecimalNumbers(s);

    while ( gen )
    {
        if ( gen() < 13u ) return false;
    }

    return true;
}

Generator<std::uintmax_t> ExtractDecimalNumbers(std::string_view const &s)
{
    using std::isdigit;

    char const *p = &s.front();

    for (;;)
    {
        while ( p <= &s.back() && !isdigit(*p) ) ++p;

        if ( &s.back() < p ) co_return;

        char const *const pfirst = p;

        while ( p <= &s.back() && isdigit(*p) ) ++p;

        if ( &s.back() < p ) --p;

        std::uintmax_t number;
    
        std::from_chars( pfirst, p, number, 10u );

        co_yield number;
    }
}
