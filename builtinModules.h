#pragma once
#include "node.h"

namespace builtinModules {

node::Module::Modules all();

struct BuiltinFact : node::Fact {
    BuiltinFact(std::string name);
    std::string name;
};

struct Contains : BuiltinFact {
    Contains();
    set::Set solve(Context &ctx) const override;
};

struct If : BuiltinFact {
    If();
    set::Set solve(Context &ctx) const override;
};

struct Else : BuiltinFact {
    Else();
    set::Set solve(Context &ctx) const override;
};

struct Not : BuiltinFact {
    Not();
    set::Set solve(Context &ctx) const override;
};

struct Neg : BuiltinFact {
    Neg();
    set::Set solve(Context &ctx) const override;
};

struct Eq : BuiltinFact {
    Eq();
    set::Set solve(Context &ctx) const override;
};

struct Noteq : BuiltinFact {
    Noteq();
    set::Set solve(Context &ctx) const override;
};

struct Add : BuiltinFact {
    Add();
    set::Set solve(Context &ctx) const override;
};

struct Sub : BuiltinFact {
    Sub();
    set::Set solve(Context &ctx) const override;
};

struct Mul : BuiltinFact {
    Mul();
    set::Set solve(Context &ctx) const override;
};

struct Div : BuiltinFact {
    Div();
    set::Set solve(Context &ctx) const override;
};

struct Lt : BuiltinFact {
    Lt();
    set::Set solve(Context &ctx) const override;
};

struct Gt : BuiltinFact {
    Gt();
    set::Set solve(Context &ctx) const override;
};

struct Lteq : BuiltinFact {
    Lteq();
    set::Set solve(Context &ctx) const override;
};

struct Gteq : BuiltinFact {
    Gteq();
    set::Set solve(Context &ctx) const override;
};

struct And : BuiltinFact {
    And();
    set::Set solve(Context &ctx) const override;
};

struct Or : BuiltinFact {
    Or();
    set::Set solve(Context &ctx) const override;
};

struct MakeArray : BuiltinFact {
    MakeArray();
    set::Set solve(Context &ctx) const override;
};

} // namespace builtinModules