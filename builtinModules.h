#pragma once
#include "node.h"

namespace builtinModules {

std::vector<std::unique_ptr<node::Module>> all();

std::map<std::string, node::Module *> genMap(std::vector<std::unique_ptr<node::Module>> const &all);

struct BuiltinFact : node::Fact {
    BuiltinFact(std::string name);
    std::string name;
};

struct Contains : BuiltinFact {
    Contains();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct If : BuiltinFact {
    If();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Else : BuiltinFact {
    Else();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Not : BuiltinFact {
    Not();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Neg : BuiltinFact {
    Neg();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Eq : BuiltinFact {
    Eq();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Noteq : BuiltinFact {
    Noteq();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Add : BuiltinFact {
    Add();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Sub : BuiltinFact {
    Sub();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Mul : BuiltinFact {
    Mul();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Div : BuiltinFact {
    Div();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Lt : BuiltinFact {
    Lt();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Gt : BuiltinFact {
    Gt();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Lteq : BuiltinFact {
    Lteq();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Gteq : BuiltinFact {
    Gteq();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct And : BuiltinFact {
    And();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Or : BuiltinFact {
    Or();
    std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

} // namespace builtinModules