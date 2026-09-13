#pragma once
#include <string>
#include <charconv>
#include <system_error>
#include <optional>
//My utilities

const char* StripPath(const char* path);

template <typename T>
std::optional<T> FromChars(const std::string& str, int base = 10)
{
    T fcVal{};
    std::from_chars_result fcRes;

    //Check if T is integer type or floating point type, only pass base to integer types.
    if constexpr(std::is_integral_v<T>)
        fcRes = std::from_chars(str.data(), str.data() + str.size(), fcVal, base);
    else if constexpr(std::is_floating_point_v<T>)
        fcRes = std::from_chars(str.data(), str.data() + str.size(), fcVal);
    else
    {
        static_assert(sizeof(T) == 0, 
                "FromChars only supports integer and floating point types.");
    }

    //Return optional with no result (nullopt) in the case of error.
    if(fcRes.ec != std::errc{} || fcRes.ptr != str.data() + str.size())
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

