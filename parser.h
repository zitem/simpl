#pragma once
#include "node.h"

struct FoKind {
    Kind kind;
    auto operator<=>(FoKind const &rhs) const { return kind <=> rhs.kind; }
};

struct Process {
    Process(Kind kind, std::stack<std::unique_ptr<node::Token>> stack = {}) : kind(kind), stack(std::move(stack)) {}
    Kind kind;
    std::stack<std::unique_ptr<node::Token>> stack;
};

class Parser {
public:
    using CFG = std::map<Kind, std::vector<std::vector<Kind>>>;
    void setupGramma(CFG cfg, Kind root = Kind::Root, Kind epsilon = Kind::epsilon);
    void setRawText(std::string const &str);
    std::stack<std::unique_ptr<node::Token>> const &getCst() const;
    int parse(node::Token const &input);
    void reset();

private:
    void _pop();
    bool _isTerm(Kind k) const;
    bool _isReference(FoKind k) const;
    std::vector<Kind> _getTerm(Kind k) const;

    std::map<Kind, int> _makeFirstSet(Kind kind) const;
    std::map<Kind, int> _makeFirstSet(CFG::const_reference pair) const;
    std::map<Kind, std::map<Kind, int>> _makeFirstSets() const;

    void _followInsertEof(std::map<Kind, std::set<FoKind>> &res, Kind k) const;
    std::map<Kind, std::set<FoKind>> _makeFollowSet(CFG::const_reference b) const;
    std::map<Kind, std::set<FoKind>> _makeFollowSets() const;
    void _solveReference(Kind k);
    static bool _canfollow(std::set<FoKind> const &foEle, Kind input);

    static void _dump(std::map<Kind, std::map<Kind, int>> const &fi);
    void _dump(std::map<Kind, std::set<FoKind>> const &fo) const;
    static void _dump(std::stack<Process> const &stack);

private:
    Kind _root{Kind::Root};
    Kind _epsilon{Kind::epsilon};
    std::array<bool, Kind::Size> _nonterms;
    std::map<Kind, std::map<Kind, int>> _first;
    std::map<Kind, std::set<FoKind>> _follow;
    CFG _cfg;
    std::stack<std::unique_ptr<node::Token>> _cst;
    std::stack<Process> _stack;
    std::string const *_text;
};
