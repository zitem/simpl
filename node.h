#pragma once
#include "set.h"

struct Kind {
    Kind(uint16_t kind) : _val(Value(kind)) {}

    auto operator<=>(Kind const &rhs) const = default;

    enum Value : uint16_t {
        Id,
        Number,
        Comment,
        Apostrophe,
        Quotation,
        SingleEqual,
        DoubleEqual,
        ExclamationEqual,
        SingleColon,
        DoubleColon,
        OpenParenthesis,
        CloseParenthesis,
        OpenSquareBracket,
        CloseSquareBracket,
        OpenCurlyBracket,
        CloseCurlyBracket,
        LessThan,
        GreatThan,
        LessThanOrEqual,
        GreatThanOrEqual,
        SingleAnd,
        DoubleAnd,
        SingleOr,
        DoubleOr,
        SinglePlus,
        DoublePlus,
        SingleMinus,
        DoubleMinus,
        SingleAsterisk,
        DoubleAsterisk,
        SingleSlash,
        DoubleSlash,
        Percent,
        Dot,
        Comma,
        Semicolon,
        Exclamation,
        Question,
        Hash,
        Spaceship,
        BnfEqual,
        ChevronsOr,
        ChevronsEnd,
        Module,
        Set,
        Extract,

        epsilon,
        Eof,

        Invalid,
        Ast,

        Root,
        Para,
        Par_,
        Stmt,
        Stm_,
        Expr,
        Exp_,
        Term,
        Ter_,
        Fact,
        Fac_,
        Tupl,
        Tup_,
        Oprt,
        Unary,
        Binary,
        LtoR,
        RtoL,
        Atom,
        IModule,
        Annot,
        Bool,
        Op1,
        Op2,
        Op3,
        Op4,
        Op5,
        Op6,
        Op7,
        Op8,
        Op9,
        Exp1,
        Exp2,
        Exp3,
        Exp4,
        Exp5,
        Exp6,
        Exp7,
        Exp8,
        Exp9,
        Exp1_,
        Exp2_,
        Exp3_,
        Exp4_,
        Exp5_,
        Exp6_,
        Exp7_,
        Exp8_,
        Exp9_,
        Super,
        Params,

        GRoot,
        GFact,
        GTupl,
        GTup_,
        GId,
        GEpsilon,

        Size
    };

    std::string show() const;
    std::string name() const;
    uint16_t value() const { return _val; }

    struct Info {
        std::string show, name;
    };

private:
    Value _val;
    static std::array<Info, Value::Size> const infos;
};

struct Node {
    struct Pos {
        uint32_t line, column;
    };
    std::string_view view;
    Node(std::string_view view) : view(view) {}
    std::string str() const { return std::string(view); }
    std::pair<Pos, Pos> getRange(std::string const &str) const;
    Node combine(Node const &tail) const;
    void printCode(std::string const &file) const;
};

class Context;

namespace node {

class Fact;
class Module;
class Statements;

struct Token : Node {
    Token(Kind kind, Node const &node);
    Token(Kind kind, std::string_view view);
    virtual ~Token() = default;
    virtual Module const *digest(Context &ctx) { return (void)ctx, nullptr; }
    virtual set::Set solve(Context &ctx) const { return (void)ctx, set::create(); }
    set::Set solveWithParams(set::Set params, Context &ctx) const;
    virtual void dump(size_t indent = 0) const;
    template <typename T> T &cast() { return *static_cast<T *>(this); }
    template <typename T> T const &cast() const { return *static_cast<T const *>(this); }

    Kind kind;
};

struct Nonterm : Token {
    Nonterm(Kind kind);
    void pushArgs(std::vector<std::unique_ptr<Token>> tokens);
    std::vector<std::unique_ptr<Token>> args;
    void dump(size_t indent = 0) const override;
    int size{};
};

template <typename Derived> struct BaseSet : Token {
    BaseSet(Node node) : Token(Kind::Number, node) {}
    BaseSet(std::string_view view) : Token(Kind::Number, view) {}
    set::Set solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override { std::cout << std::string(indent * 2, ' ') << view << "\n"; }
};

struct Bool : BaseSet<Bool> {
    using BaseSet::BaseSet;
    using Set = set::Bool;
    constexpr static auto name = "bool";
    bool value() const { return view.compare("true") == 0; }
};

struct Int : BaseSet<Int> {
    using BaseSet::BaseSet;
    using Set = set::Int;
    constexpr static auto name = "int";
    int value() const { return std::stoi(std::string(view)); }
};

struct Void : BaseSet<Void> {
    using BaseSet::BaseSet;
    using Set = set::Void;
    constexpr static auto name = "void";
};

class Module : public Token {
public:
    struct Facts {
        Facts(std::string name) : name(std::move(name)) {}
        std::string name;
        std::multimap<std::string_view, node::Fact *> facts;
    };
    using Modules = std::map<std::string_view, std::unique_ptr<Module>>;

    Module(std::unique_ptr<Token> &&statements, Node const &node, std::string name);
    Module(Modules &&modules);
    Module const *digest(Context &ctx) override;
    set::Set solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    void setName(std::string const &name);
    void setName(std::string_view const &name);
    std::string const &getName() const;
    Facts getFacts() const;
    set::Set genSet(set::Set const &param, Context &ctx) const;
    Module const *find(std::string_view name) const;
private:
    std::unique_ptr<Statements> _stmts;
    std::string _name;
    Modules _modules;
};

class Statements : public Token {
public:
    template <typename... T> Statements(T &&...init) : Token(Kind::Stmt, {}) { (..., pushBack(std::move(init))); }
    Module const *digest(Context &ctx) override;
    void pushFront(std::unique_ptr<Token> &&stmt);
    void pushBack(std::unique_ptr<Token> &&stmt);
    void dump(size_t indent = 0) const override;
    set::Set solve(Context &ctx) const override;
    std::deque<std::unique_ptr<Token>> const &get() const { return _statements; }
private:
    std::deque<std::unique_ptr<Token>> _statements;
    friend Module::Module(std::unique_ptr<Token> &&, Node const &, std::string);
};

struct Set : Token {
    Set(std::string_view view, std::unique_ptr<Token> &&annotation = {}, std::unique_ptr<Token> &&params = {});
    void setSuperset(std::unique_ptr<Token> &&set);
    Token *getSuperset();
    void setParams(std::unique_ptr<Token> &&params);
    std::string value() const { return std::string(view); }
    Module const *digest(Context &ctx) override;
    set::Set solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    Module const *ref{};
    std::vector<Fact const *> facts;
private:
    std::unique_ptr<Token> _annotation;
    std::unique_ptr<Statements> _params;
};

class Expression : public Token {
public:
    Expression(std::unique_ptr<Token> &&extract, std::unique_ptr<Token> &&super);
    Module const *digest(Context &ctx) override;
    set::Set solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    void setExtract(std::unique_ptr<Token> &&extract);
    void setSuperset(std::unique_ptr<Token> &&super);
    std::string_view getExtractName() const;
    Module const *getModule() const;
private:
    std::unique_ptr<Set> _extract;
    std::unique_ptr<Expression> _super;
};

class Unary : public Token {
public:
    Unary(Token const &op) : Token(Kind::Unary, op), _op(op.kind) {}
    Module const *digest(Context &ctx) override;
    set::Set solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    void setParam(std::unique_ptr<Token> &&param);
    Module const *ref{};
private:
    static std::map<Kind, std::string_view> const table;
    std::unique_ptr<Statements> _params;
    Kind _op;
};

class Binary : public Token {
public:
    Binary(Token const &op) : Token(Kind::Binary, op), _op(op.kind) {}
    Module const *digest(Context &ctx) override;
    set::Set solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    void competedLhs(std::unique_ptr<Token> &&param);
    void setLhs(std::unique_ptr<Token> &&param);
    void setRhs(std::unique_ptr<Token> &&param);
    Module const *ref{};
private:
    static std::map<Kind, std::string_view> const table;
    std::unique_ptr<Statements> _params = std::make_unique<Statements>();
    Binary *_binaryLhs{};
    Kind _op;
};

class Fact : public Token {
public:
    Fact(std::unique_ptr<Token> &&lvalue, std::unique_ptr<Token> &&rvalue);
    Module const *digest(Context &ctx) override;
    set::Set solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    Set const &lvalue() const { return *_lvalue; }
    Token const &rvalue() const { return *_rvalue; }
private:
    std::unique_ptr<Set> _lvalue;
    std::unique_ptr<Token> _rvalue;
};

} // namespace node

struct Context {
    Context(std::string const &file);
    set::Set global;
    std::stack<set::Set> params;
    std::stack<node::Module const *> scope;
    std::string const &file;
};

template <typename Derived> set::Set node::BaseSet<Derived>::solve(Context & /*ctx*/) const {
    if constexpr (std::is_same_v<typename Derived::Set, set::Void>) {
        return set::create<typename Derived::Set>();
    } else {
        return set::create<typename Derived::Set>(cast<Derived>().value());
    }
}