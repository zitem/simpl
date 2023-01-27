#pragma once
#include "utils.h"

namespace style {
constexpr char const reset[] = "\033[0m";

constexpr char const black[] = "\033[30m";
constexpr char const red[] = "\033[31m";
constexpr char const green[] = "\033[32m";
constexpr char const yellow[] = "\033[33m";
constexpr char const blue[] = "\033[34m";
constexpr char const purple[] = "\033[35m";
constexpr char const cyan[] = "\033[36m";
constexpr char const white[] = "\033[37m";

constexpr char const underline[] = "\033[4m";
} // namespace style

enum class Verbosity { Quiet, Minimal, Normal, Detailed, Diagnostic };

Verbosity getVerbosity();
void setVerbosity(Verbosity verbo);

template <Verbosity V, StringLiteral Style> struct OutputTemplate {
    using self = OutputTemplate<V, Style>;
    self &operator,(auto &&out) {
        if (getVerbosity() >= V) {
            if constexpr ((sizeof Style.value) > 1) {
                std::cout << Style.value;
            }
            std::cout << std::forward<decltype(out)>(out);
            if constexpr ((sizeof Style.value) > 1) {
                std::cout << style::reset;
            }
        }
        return *this;
    }
};

template <StringLiteral Style = ""> struct Quiet : OutputTemplate<Verbosity::Quiet, Style> {};
template <StringLiteral Style = ""> struct Minim : OutputTemplate<Verbosity::Minimal, Style> {};
template <StringLiteral Style = ""> struct Norma : OutputTemplate<Verbosity::Normal, Style> {};
template <StringLiteral Style = ""> struct Detai : OutputTemplate<Verbosity::Detailed, Style> {};
template <StringLiteral Style = ""> struct Diagn : OutputTemplate<Verbosity::Diagnostic, Style> {};