#pragma once

inline std::string filename2module(std::string const &filename) {
    return filename.substr(0, filename.size() - 4);
}

template <size_t N> struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }
    char value[N]{};
};