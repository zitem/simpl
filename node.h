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

        GRoot,
        GFact,
        GTupl,
        GTup_,
        GId,
        GEpsilon,

        Size
    };

    [[nodiscard]] std::string show() const;
    [[nodiscard]] std::string name() const;
    [[nodiscard]] uint16_t value() const { return _val; }

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
    virtual ~Node() = default;
    [[nodiscard]] std::string str() const { return std::string(view); }
    [[nodiscard]] std::pair<Pos, Pos> getRange(std::string const &str) const;
    [[nodiscard]] virtual Node combine(Node const &tail) const;
    void printCode(std::string const &file) const;
};

class Context;

namespace node {

struct Token : Node {
    Token(Kind kind, Node const &node);
    Token(Kind kind, std::string_view view);
    Kind kind;
    int size{};
    [[nodiscard]] std::string show() const { return kind.show(); }
    template <typename T> [[nodiscard]] T &cast() { return *static_cast<T *>(this); }
    template <typename T> [[nodiscard]] T const &cast() const { return *static_cast<T const *>(this); }
    virtual void print(size_t indent = 0) const;
    [[nodiscard]] virtual std::unique_ptr<set::ISet> solve(Context &ctx) const { return (void)ctx, nullptr; }
};
struct Nonterm : Token {
    Nonterm(Kind kind);
    void pushArgs(std::vector<std::unique_ptr<Token>> tokens);
    std::vector<std::unique_ptr<Token>> args;
    void print(size_t indent = 0) const override;
};

struct Set : Token {
    Module *parent{};
    std::unique_ptr<Set> annotation;
    Set(std::string_view view, Module *parent = nullptr);
    void setSuperset(std::unique_ptr<Token> &&set);
    [[nodiscard]] std::string value() const { return std::string(view); }
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    void print(size_t indent = 0) const override;
};

template <typename Derived> struct BaseSet : Token {
    BaseSet(std::string_view view) : Token(Kind::Number, view) {}
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    void print(size_t indent = 0) const override { std::cout << std::string(indent * 2, ' ') << view << "\n"; }
};

struct Bool : BaseSet<Bool> {
    using BaseSet::BaseSet;
    using Set = set::Bool;
    constexpr static auto name = "bool";
    [[nodiscard]] bool value() const { return view.compare("true") == 0; }
};

struct Int : BaseSet<Int> {
    using BaseSet::BaseSet;
    using Set = set::Int;
    constexpr static auto name = "int";
    [[nodiscard]] int value() const { return std::stoi(std::string(view)); }
};

struct Expression : Token {
    Expression(std::unique_ptr<Token> &&extract, std::unique_ptr<Token> &&module = {});
    void print(size_t indent = 0) const override;
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    std::unique_ptr<Token> extract;
    std::unique_ptr<Token> module;
};

struct Fact : Token {
    Fact(std::unique_ptr<Token> &&lhs, std::unique_ptr<Token> &&rhs);
    void print(size_t indent = 0) const override;
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    std::unique_ptr<Token> lhs;
    std::unique_ptr<Token> rhs;
};

class Statements : public Token {
public:
    Statements();
    void pushFront(std::unique_ptr<Token> &&stmt);
    void pushBack(std::unique_ptr<Token> &&stmt);
    void print(size_t indent = 0) const override;
    [[nodiscard]] std::deque<std::unique_ptr<Token>> const &get() const { return _statements; }
private:
    std::deque<std::unique_ptr<Token>> _statements;
};

class Module : public Token {
public:
    Module(std::unique_ptr<Statements> &&statements = nullptr);
    Module(std::unique_ptr<Token> &&statements, Node const &node);
    void print(size_t indent = 0) const override;
    void setName(std::string const &name);
    void setName(std::string_view const &name);
    [[nodiscard]] std::string const &getName() const;
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
    std::unique_ptr<Statements> stmts;
    [[nodiscard]] set::Module getFacts() const;
    [[nodiscard]] auto find(std::string const &lhs) const -> std::deque<std::unique_ptr<Token>>::const_iterator;
private:
    std::string _name;
};

} // namespace node

struct Context {
    Context(std::string const &file);
    std::map<std::string, node::Module *> modules;
    std::map<std::string, std::unique_ptr<set::ISet>> sets;
    std::stack<node::Module *> currentModule;
    std::stack<std::unique_ptr<set::Sets>> params;
    std::string const &file;
private:
    std::vector<std::unique_ptr<node::Module>> _modules;
};

template <typename Derived> std::unique_ptr<set::ISet> node::BaseSet<Derived>::solve(Context &ctx) const {
    return std::make_unique<typename Derived::Set>(cast<Derived>().value(), ctx.sets.at(Derived::name).get());
}