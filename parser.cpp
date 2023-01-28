#include "parser.h"
#include "outs.h"

using namespace node;

void Parser::setupGramma(CFG cfg, Kind root, Kind epsilon) {
    reset();

    _cfg = std::move(cfg);
    _root = root;
    _epsilon = epsilon;
    _stack.push({Kind::Eof});
    _stack.push({_root});

    for (auto const &g : _cfg) {
        _nonterms[g.first.value()] = true;
    }

    Diagn(), "\n> construct first sets\n";
    _first = _makeFirstSets();
    _dump(_first);
    Diagn(), '\n';

    Diagn(), "> construct follow sets\n";
    _follow = _makeFollowSets();
    _dump(_follow);
    Diagn(), '\n';

    Diagn(), "> solve reference\n";
    for (auto const &pair : _cfg) {
        _solveReference(pair.first);
    }
    _dump(_follow);
    Diagn(), '\n';
}

std::stack<std::unique_ptr<Token>> const &Parser::getCST() const {
    return _cst;
}

void Parser::reset() {
    _root = Kind::Root;
    _epsilon = Kind::Epsilon;
    _nonterms = {};
    _cst = {};
    _stack = {};
}

void Parser::_pop() {
    auto &stack = _stack.top().stack;
    while (!stack.empty()) {
        auto count = stack.top()->size;
        std::vector<std::unique_ptr<Token>> tmp;
        for (auto i = 0; i < count; ++i) {
            tmp.push_back(std::move(_cst.top()));
            _cst.pop();
        }
        for (auto const &t : tmp | std::views::reverse) {
            Diagn(), t->show(), " ";
        }
        stack.top()->cast<Nonterm>().pushArgs(std::move(tmp));
        _cst.push(std::move(stack.top()));
        Diagn(), '\n';
        stack.pop();
    }
    _stack.pop();
};

int Parser::parse(Token const &input) {
    if (input.kind == _epsilon || input.kind == Kind::Comment) return 0;
    int err{};
    while (!_stack.empty() && !_isTerm(_stack.top().kind)) {
        auto const &fiEle = _first[_stack.top().kind];
        auto findrule = fiEle.find(input.kind);
        if (findrule != fiEle.end()) {
            auto rule = findrule->second;
            Kind const grammaKind = _stack.top().kind;
            auto const &gramma = _cfg.at(grammaKind);
            auto top = std::move(_stack.top().stack);
            _stack.pop();
            Diagn(), "⏪";
            _dump(_stack);
            Diagn(), "🔄️";
            for (auto it = gramma[rule].rbegin(); it != gramma[rule].rend(); ++it) {
                if (it == gramma[rule].rbegin()) {
                    auto token = std::make_unique<Nonterm>(grammaKind);
                    token->size = (int)gramma[rule].size();
                    top.push(std::move(token));
                    _stack.push({*it, std::move(top)});
                } else {
                    _stack.push({*it});
                }
            }
            _dump(_stack);
        } else if (fiEle.find(_epsilon) != fiEle.end()) {
            if (_canfollow(_follow.find(_stack.top().kind)->second, input.kind)) {
                _cst.push(std::make_unique<Token>(_epsilon, std::string_view{input.view.begin(), input.view.begin()}));
                _pop();
                Diagn(), "⏪";
                _dump(_stack);
            } else {
                ++err;
                Diagn(), "⏪";
                // try to recover code here
                break;
            }
        } else {
            ++err;
            Diagn(), "😖\n";
            return err;
        }
    }

    if (_isTerm(_stack.top().kind)) {
        if (_stack.top().kind != input.kind) {
            ++err;
        } else {
            Diagn(), "⏪";
            _cst.push(std::make_unique<Token>(input));
            _pop();
            _dump(_stack);
        }
    }

    if (err > 0) {
        Quiet(), style::red;
        Quiet(), "input is '", input.show(), "' \n";
        Quiet(), "expected ";
        if (_isTerm(_stack.top().kind)) {
            Quiet(), '\'', _stack.top().kind.show(), '\'';
        }
        for (auto const &ff : _first[_stack.top().kind]) {
            Quiet(), '\'', ff.first.show(), '\'', ' ';
        }
        Quiet(), "\n\n", style::reset;
        return err;
    }

    for (auto const &k : _cst._Get_container()) {
        Diagn(), k->show(), " ";
    }
    Diagn(), "\n";

    Diagn(), "\n";
    return err;
}

bool Parser::_isTerm(Kind k) const {
    return !_nonterms[k.value()];
}

bool Parser::_isReference(FoKind k) const {
    return !_isTerm(k.kind);
}

std::vector<Kind> Parser::_getTerm(Kind k) const {
    if (_isTerm(k)) return {k};
    std::vector<Kind> res;
    for (auto const &vec : _cfg.at(k)) {
        Kind front = vec.front();
        if (front == k) {
            Diagn(), "!left recursion: ", k.show(), "\n";
            continue;
        }
        if (_isTerm(front)) {
            res.push_back(front);
        } else {
            for (auto t : _getTerm(front)) {
                res.push_back(t);
            }
        }
    }
    return res;
}

std::map<Kind, int> Parser::_makeFirstSet(Kind kind) const {
    std::map<Kind, int> res;
    auto const &gramma = _cfg.at(kind);
    for (size_t i{}; i < gramma.size(); ++i) {
        auto const &option = gramma[i];
        auto addition = _getTerm(option.front());
        for (auto a : addition) {
            auto find = res.find(a);
            if (find != res.end()) {
                auto fi = find->first.show();
                constexpr auto txt = "'input {} -> {}.rule[{}]' is replaced by 'input {} -> {}.rule[{}]'\n";
                auto msg = std::format(txt, fi, kind.show(), find->second, a.show(), kind.show(), int(i));
                Quiet<style::red>(), msg;
                res.insert({a, int(i)});
            } else {
                res.insert({a, int(i)});
            }
        }
    }
    return res;
}

void Parser::_followInsertEof(std::map<Kind, std::set<FoKind>> &res, Kind k) const {
    if (_isTerm(k) || res[k].contains({Kind::Eof})) return;
    res[k].insert({Kind::Eof});
    for (auto const &option : _cfg.at(k)) {
        _followInsertEof(res, option.back());
    }
}

std::map<Kind, int> Parser::_makeFirstSet(CFG::const_reference pair) const {
    return _makeFirstSet(pair.first);
}

std::map<Kind, std::map<Kind, int>> Parser::_makeFirstSets() const {
    std::map<Kind, std::map<Kind, int>> res;
    for (auto const &pair : _cfg) {
        res[pair.first] = _makeFirstSet(pair);
    }
    return res;
}

std::map<Kind, std::set<FoKind>> Parser::_makeFollowSet(CFG::const_reference b) const {
    std::map<Kind, std::set<FoKind>> res;
    for (auto const &option : b.second) {
        for (auto target = option.begin(); target != option.end(); ++target) {
            if (_isTerm(*target)) continue;

            auto const beta = target + 1;

            if (beta == option.end()) {
                res[*target].insert({b.first});
                continue;
            }

            if (_isTerm(*beta)) {
                if (*beta != _epsilon) {
                    res[*target].insert({*beta});
                } else {
                    res[*target].insert({b.first});
                }
            } else {
                auto const &betaFirst = _first.at(*beta);
                for (auto const &pair : betaFirst) {
                    if (pair.first != _epsilon) {
                        res[*target].insert({pair.first});
                    }
                }
                if (std::ranges::any_of(betaFirst, [this](auto k) { return k.first == _epsilon; })) {
                    res[*target].insert({b.first});
                }
            }
        }
    }

    return res;
}

std::map<Kind, std::set<FoKind>> Parser::_makeFollowSets() const {
    std::map<Kind, std::set<FoKind>> res;
    _followInsertEof(res, _root);

    for (auto const &pair : _cfg) {
        auto fo = _makeFollowSet(pair);
        for (const auto &addition : fo) {
            auto &ref = res[addition.first];
            for (FoKind k : addition.second) {
                if (ref.contains(k)) {
                    // auto msg = std::format("{} exists", k.kind.show());
                    // Diagn(), "==", msg, "==\n";
                    // _dump(res);
                    // Diagn(), std::string(4 + msg.size(), '='), "\n";
                } else {
                    ref.insert(k);
                }
            }
        }
    }
    return res;
}

void Parser::_solveReference(Kind k) {
    auto find = _follow.find(k);
    if (find == _follow.end()) {
        _follow[k];
        find = _follow.find(k);
        Quiet<style::yellow>(), "warn: no ", k.show(), '\n';
    }
    auto &ref = find->second;

    for (auto it = ref.begin(); it != ref.end();) {
        if (_isReference(*it)) {
            if (it->kind == k) {
                Diagn(), std::format("!remove: Follow({}) = Follow({})\n", it->kind.show(), it->kind.show());
                it = ref.erase(it);
                continue;
            }
            _solveReference(it->kind);
            auto find = _follow.find(it->kind);
            if (find == _follow.end()) {
                Diagn(), std::format("cannot solve Follow({}) = Follow({})\n", k.show(), it->kind.show());
                _dump(_follow);
                exit(1);
            }
            ref.insert(find->second.begin(), find->second.end());
            it = ref.erase(it);
        } else {
            ++it;
        }
    }
}

bool Parser::_canfollow(std::set<FoKind> const &foEle, Kind input) {
    auto find = std::find_if(foEle.begin(), foEle.end(), [input](FoKind k) { return k.kind == input; });
    return find != foEle.end();
}

void Parser::_dump(std::map<Kind, std::map<Kind, int>> const &fi) {
    for (auto const &pair : fi) {
        Diagn(), "First(", pair.first.show(), ") = ", "{ ";
        for (auto k : pair.second) {
            Diagn(), std::format("{}[{}], ", k.first.show(), k.second);
        }
        Diagn(), "\b\b }\n";
    }
}

void Parser::_dump(std::map<Kind, std::set<FoKind>> const &fo) const {
    for (auto const &pair : fo) {
        Diagn(), "Follow(", pair.first.show(), ") = { ";
        for (auto k : pair.second) {
            if (_isReference(k)) {
                Diagn(), "Follow(", k.kind.show(), "), ";
            } else {
                Diagn(), k.kind.show(), ", ";
            }
        }
        Diagn(), (pair.second.empty() ? "\b}\n" : "\b\b }\n");
    }
}

void Parser::_dump(std::stack<Process> const &stack) {
    Diagn(), " ";
    for (auto const &[k, s] : stack._Get_container()) {
        Diagn(), k.show(), (s.empty() ? "" : ('(' + s.top()->show() + ')')), " ";
    }
    Diagn(), "\n";
}
