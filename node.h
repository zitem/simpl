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

        Epsilon,
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

struct Token : Node {
    Token(Kind kind, Node const &node);
    Token(Kind kind, std::string_view view);
    virtual ~Token() = default;

    virtual std::unique_ptr<set::ISet> solve(Context &ctx) const { return (void)ctx, nullptr; }
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

struct Set : Token {
    Set(std::string_view view, std::unique_ptr<Token> &&annotation = {});
    void setSuperset(std::unique_ptr<Token> &&set);
    Token *getSuperset();
    std::string value() const { return std::string(view); }
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
private:
    std::unique_ptr<Token> _annotation;
};

template <typename Derived> struct BaseSet : Token {
    BaseSet(std::string_view view) : Token(Kind::Number, view) {}
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
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

class Statements;

class Module : public Token {
public:
    Module(std::unique_ptr<Token> &&statements, Node const &node, std::string name);
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    void setName(std::string const &name);
    void setName(std::string_view const &name);
    std::string const &getName() const;
    set::Module getFacts() const;
    Module const *find(std::string const &name) const;
private:
    std::unique_ptr<Statements> _stmts;
    std::string _name;
    std::map<std::string_view, std::unique_ptr<Module>> _modules;
};

class Statements : public Token {
public:
    template <typename... T> Statements(T &&...init) : Token(Kind::Stmt, {}) { (..., pushBack(std::move(init))); }
    void pushFront(std::unique_ptr<Token> &&stmt);
    void pushBack(std::unique_ptr<Token> &&stmt);
    void dump(size_t indent = 0) const override;
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    std::deque<std::unique_ptr<Token>> const &get() const { return _statements; }
private:
    std::deque<std::unique_ptr<Token>> _statements;
    friend Module::Module(std::unique_ptr<Token> &&, Node const &, std::string);
};

class Expression : public Token {
public:
    Expression(
        std::unique_ptr<Token> &&extract,
        std::string_view module = "",
        std::unique_ptr<Token> &&stmts = std::make_unique<Statements>()
    );
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    void setModuleName(std::string_view name);
    std::string_view getModuleName() const;
private:
    std::unique_ptr<Set> _extract;
    std::unique_ptr<Statements> _stmts;
    std::string_view _module;
};

class Unary : public Token {
public:
    Unary(Token const &op) : Token(Kind::Unary, op), _op(op.kind) {}
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    void setParam(std::unique_ptr<Token> &&param);
private:
    std::unique_ptr<Statements> _params;
    Kind _op;
};

class Binary : public Token {
public:
    Binary(Token const &op) : Token(Kind::Binary, op), _op(op.kind) {}
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    void dump(size_t indent = 0) const override;
    void competedLhs(std::unique_ptr<Token> &&param);
    void setLhs(std::unique_ptr<Token> &&param);
    void setRhs(std::unique_ptr<Token> &&param);
private:
    std::unique_ptr<Statements> _params = std::make_unique<Statements>();
    Binary *_binaryLhs{};
    Kind _op;
};

class Fact : public Token {
public:
    Fact(std::unique_ptr<Token> &&lvalue, std::unique_ptr<Token> &&rvalue);
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
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
    std::map<std::string, node::Module *> modules;
    set::Sets sets;
    std::stack<std::unique_ptr<set::Sets>> params;
    std::string const &file;
private:
    std::vector<std::unique_ptr<node::Module>> _modules;
};

template <typename Derived> std::unique_ptr<set::ISet> node::BaseSet<Derived>::solve(Context &ctx) const {
    return std::make_unique<typename Derived::Set>(cast<Derived>().value(), ctx.sets.at(Derived::name).get());
}