#pragma once
#include <string>
#include <charconv>
#include <system_error>
#include <optional>
//My utilities

const char* StripPath(const char* path);
int DetermineStringBasePrefix(const std::string& str);

template <typename T>
std::optional<T> FromChars(const std::string& str)
{
    T fcVal{};
    std::from_chars_result fcRes;
    //Before running std::from_chars, check for base prefix (ex. 0x), if present use 
    //this to determine base to call with std::from_chars.
    int base = DetermineStringBasePrefix(str);
    std::string final_str = str;
    //Note, base 10 always assumed to be without a prefix.
    if(base != 10)
        final_str = str.substr(2);

    //Check if T is integer type or floating point type, only pass base to integer types.
    if constexpr(std::is_integral_v<T>)
        fcRes = std::from_chars(final_str.data(), final_str.data() + final_str.size(), fcVal, base);
    else if constexpr(std::is_floating_point_v<T>)
        fcRes = std::from_chars(final_str.data(), final_str.data() + final_str.size(), fcVal);
    else
    {
        static_assert(sizeof(T) == 0, 
                "FromChars only supports integer and floating point types.");
    }

    //Return optional with no result (nullopt) in the case of error.
    if(fcRes.ec != std::errc{} || fcRes.ptr != final_str.data() + final_str.size())
        return std::nullopt;
    else
        return fcVal;
}

template <typename T>
inline void ParseHexCharToDecVal(const char& c, T& buffer)
{
    //Multiply by 16 (<<4), convert to lower case if letter.
    buffer = (buffer << 4) + (c <= '9' ? c - '0' : (c | 32) - 'a' + 10); 
}

