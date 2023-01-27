#include "lexer.h"
#include "outs.h"

node::Token Lexer::next(std::vector<std::shared_ptr<token::Base>> const &tokens) {
    auto tmp = tokens;

    for (; _now < _string.length();) {
        std::erase_if(tmp, [this](auto e) { return !e->match(_view()) && !e->over(_view()); });
        if (tmp.empty()) {
            auto v = _view();
            if (std::any_of(v.begin(), v.end(), [](char c) { return !token::isSpace(c); })) {
                Quiet<style::red>(), "undefined input '", v, "'\n";
            }
            _begin = ++_now;
            tmp = tokens;
        } else if (std::ranges::all_of(tmp, [this](auto ptr) { return ptr->over(_view()); })) {
            break; // greedy for the longest match
        } else {
            ++_now;
        }
    }
    if (_now == _string.length()) {
        ++_returnedEof;
        return {Kind::Eof, _view(0)};
    }
    auto v = _view(0);
    _begin = _now;
    return {tmp.front()->kind, v};
}
