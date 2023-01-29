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
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct If : BuiltinFact {
    If();
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Not : BuiltinFact {
    Not();
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Eq: BuiltinFact {
    Eq();
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Add : BuiltinFact {
    Add();
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Sub : BuiltinFact {
    Sub();
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Mul : BuiltinFact {
    Mul();
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

struct Div : BuiltinFact {
    Div();
    [[nodiscard]] std::unique_ptr<set::ISet> solve(Context &ctx) const override;
};

} // namespace builtinModules