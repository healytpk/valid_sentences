#include <cstdint>       // uintmax_t
#include <cctype>        // isalpha, islower, isdigit
#include <cstring>       // strchr

#include <algorithm>     // count
#include <charconv>      // from_chars
#include <string_view>   // string_view
#include <iostream>      // cout, endl

#include "generator.hpp" // supporting class 'Generator' required for coroutines

using std::uintmax_t;
using std::string_view;

bool IsSentenceValid(string_view);

Generator<uintmax_t> ExtractDecimalNumbers(string_view);

string_view const g_test_strings[] = {
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

int main(void)
{
    for ( string_view s : g_test_strings )
    {
        std::cout << (IsSentenceValid(s) ? "valid" : "INVALID") << std::endl;
    }
}

bool IsSentenceValid(string_view const s)
{
    using std::isalpha;
    using std::islower;

    if ( s.empty() ) return false;  // otherwise s.front() might be UB

    // 1: String starts with a capital letter.
    if ( isalpha(s.front()) && islower(s.front()) ) return false;

    // 2: String has an even number of quotation marks.
    if ( std::count(s.cbegin(), s.cend(), '"' ) % 2u ) return false;

    // 3: String ends with one of the following sentence termination characters: . ? !
    if ( nullptr == std::strchr(".?!", s.back()) ) return false;

    // 4: String has no period characters other than the last character
    if ( s.find_first_of('.') < (s.size() - 1u) ) return false;  // string::npos == max value

    // 5: Numbers below 13 are spelled out
    auto gen = ExtractDecimalNumbers(s);

    while ( gen )
    {
        if ( gen() < 13u ) return false;
    }

    return true;
}

Generator<uintmax_t> ExtractDecimalNumbers(string_view const s)
{
    using std::isdigit;

    for ( decltype(s)::const_iterator p{s.cbegin()} ; ; )
    {
        // Step 1: Find the first decimal digit in the number

        while ( p != s.cend() && !isdigit(*p) ) ++p;

        if ( s.cend() == p ) co_return;  // no more numbers

        // If control reaches here, we have at least one digit

        decltype(s)::const_iterator const pfirst{p};

	// Step 2: Increment 'p' until we find a non-digit (or end of string)

        while ( p != s.cend() && isdigit(*p) ) ++p;

        // Now 'p' either points to a non-digit or one-past-the-last

        uintmax_t number;

        std::from_chars(pfirst, p, number, 10u /* radix = 10 = decimal */ );

        co_yield number;
    }
}
