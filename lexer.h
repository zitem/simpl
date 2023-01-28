#pragma once
#include "node.h"

namespace token {

inline bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

inline bool isAlpha(char ch) {
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z');
}

inline bool isHead(char ch) {
    return isAlpha(ch) || ch == '_';
}

inline bool isBody(char ch) {
    return isHead(ch) || isDigit(ch);
}

inline bool isEol(char ch) {
    return ch == '\n' || ch == '\r';
}

inline bool isSpace(char ch) {
    return ch == ' ' || ch == '\t' || isEol(ch);
}

struct Base {
    Base(Kind kind) : kind(kind) {}
    Kind kind;
    virtual ~Base() = default;
    [[nodiscard]] virtual std::string show() const = 0;
    // suppose all characters of the view have matched except the last, so check the last
    virtual bool match(std::string_view const &view) = 0;
    // suppose all characters of the view have matched, so check is not matched yet
    virtual bool over(std::string_view const &view) = 0;
};

struct Id : Base {
    using Base::Base;
    [[nodiscard]] std::string show() const override { return "id"; }
    bool match(std::string_view const &view) override { return (view.size() == 1 ? isHead : isBody)(view.back()); }
    bool over(std::string_view const &view) override { return view.size() > 1 && !isBody(view.back()); }
};

struct Number : Base {
    using Base::Base;
    [[nodiscard]] std::string show() const override { return "num"; }
    bool match(std::string_view const &view) override { return isDigit(view.back()) || view.back() == '.'; }
    bool over(std::string_view const &view) override { return view.size() > 1 && !match(view); }
};

struct StrRegion : Base {
    StrRegion(std::string head, std::string tail, Kind kind)
        : Base(kind), head(std::move(head)), tail(std::move(tail)) {}
    [[nodiscard]] std::string show() const override { return head + "..." + tail; }
    bool match(std::string_view const &view) override {
        return view.size() > head.size() || view.back() == head[view.size() - 1];
    }
    bool over(std::string_view const &view) override {
        return view.size() > 2 && view.substr(0, view.size() - 1).ends_with(tail);
    }
    std::string const head, tail;
};

struct CharRegion : Base {
    CharRegion(char left, char right, Kind kind) : Base(kind), left(left), right(right) {}
    [[nodiscard]] std::string show() const override { return std::format("{}...{}", left, right); }
    bool match(std::string_view const &view) override { return view.size() > 1 || view.front() == left; }
    bool over(std::string_view const &view) override { return view.size() > 2 && *(view.rbegin() - 1) == right; }
    char const left;
    char const right;
};

struct StrFixed : Base {
    StrFixed(std::string name, Kind kind) : Base(kind), name(std::move(name)) {}
    [[nodiscard]] std::string show() const override { return name; }
    bool match(std::string_view const &view) override {
        auto size = view.size() - 1;
        return size < name.size() && view.back() == name[size];
    }
    bool over(std::string_view const &view) override { return view.size() == name.size() + 1; }
    std::string const name;
};

struct CharFixed : Base {
    CharFixed(char name, Kind kind) : Base(kind), name(name) {}
    [[nodiscard]] std::string show() const override { return {name}; }
    bool match(std::string_view const &view) override { return view.back() == name; }
    bool over(std::string_view const &view) override { return view.size() == 2; }
    char const name;
};

struct Any : Base {
    using Base::Base;
    [[nodiscard]] std::string show() const override { return "any"; }
    bool match(std::string_view const &view) override { return !isSpace(view.back()); }
    bool over(std::string_view const &view) override { return view.size() > 1 && isSpace(view.back()); }
};

} // namespace token

class Lexer {
public:
    Lexer(std::string const &str) : _string(str) {}

    [[nodiscard]] bool empty() const { return _returnedEof > 1; }

    node::Token next(std::vector<std::shared_ptr<token::Base>> const &tokens);

private:
    [[nodiscard]] char _getNow() const { return _string[_now]; }
    [[nodiscard]] char _getBegin() const { return _string[_begin]; }
    [[nodiscard]] int _offset() const { return static_cast<int>(_now - _begin); }
    [[nodiscard]] std::string_view _view(uint32_t offset = 1) const {
        return {_string.begin() + _begin, _string.begin() + _now + offset};
    }

private:
    uint32_t _begin{}, _now{};
    std::string const &_string;
    int _returnedEof{};
};
