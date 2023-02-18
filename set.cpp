#include "set.h"

using namespace set;

IBool::operator bool() const {
    return boolean()->cast<Bool>().value();
}

std::unique_ptr<Bool> IBool::operator&(IBool const &rhs) const {
    return std::make_unique<Bool>(operator bool() & rhs.operator bool());
}

std::unique_ptr<Bool> IBool::operator&&(IBool const &rhs) const {
    return std::make_unique<Bool>(operator bool() && rhs.operator bool());
}

std::unique_ptr<Bool> IBool::operator|(IBool const &rhs) const {
    return std::make_unique<Bool>(operator bool() | rhs.operator bool());
}

std::unique_ptr<Bool> IBool::operator||(IBool const &rhs) const {
    return std::make_unique<Bool>(operator bool() || rhs.operator bool());
}

std::unique_ptr<Bool> IBool::operator!() const {
    return std::make_unique<Bool>(!operator bool());
}

std::unique_ptr<Bool> Bool::boolean() const {
    return std::make_unique<Bool>(value());
}

std::unique_ptr<Int> Int::operator+(Int const &rhs) const {
    return std::make_unique<Int>(value() + rhs.value());
}

std::unique_ptr<Int> Int::operator-(Int const &rhs) const {
    return std::make_unique<Int>(value() - rhs.value());
}

std::unique_ptr<Int> Int::operator*(Int const &rhs) const {
    return std::make_unique<Int>(value() * rhs.value());
}

std::unique_ptr<Int> Int::operator/(Int const &rhs) const {
    return std::make_unique<Int>(value() / rhs.value());
}

std::unique_ptr<Bool> Int::operator==(Int const &rhs) const {
    return std::make_unique<Bool>(value() == rhs.value());
}

std::unique_ptr<Bool> Int::operator!=(Int const &rhs) const {
    return std::make_unique<Bool>(value() != rhs.value());
}

std::unique_ptr<Bool> Int::operator<(Int const &rhs) const {
    return std::make_unique<Bool>(value() < rhs.value());
}

std::unique_ptr<Bool> Int::operator>(Int const &rhs) const {
    return std::make_unique<Bool>(value() > rhs.value());
}

std::unique_ptr<Int> Int::operator-() const {
    return std::make_unique<Int>(-value());
}
