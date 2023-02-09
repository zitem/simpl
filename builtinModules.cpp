#include "builtinModules.h"

namespace builtinModules {

node::Module::Modules all() {
    constexpr static std::string_view const VIEW{"built-in module"};
    node::Module::Modules res;
    [insert =
         [&res]<typename T> {
             auto fact = std::make_unique<T>();
             auto const &name = fact->name;
             auto stmt = std::make_unique<node::Statements>(std::move(fact));
             auto module = std::make_unique<node::Module>(std::move(stmt), VIEW, name);
             res.insert({name, std::move(module)});
         }]<typename... Ts> {
        (..., insert.operator()<Ts>());
    }.
    operator()<Contains, If, Else, Not, Neg, Eq, Noteq, Add, Sub, Mul, Div, Lt, Gt, Lteq, Gteq, And, Or>();
    return res;
};

BuiltinFact::BuiltinFact(std::string name)
    : Fact(std::make_unique<node::Set>("extract"), nullptr), name(std::move(name)) {}

Contains::Contains() : BuiltinFact("Contains") {}
set::Set Contains::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x.contains(y);
}

If::If() : BuiltinFact("If") {}
set::Set If::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &v = facts.set.extract("v");
    if (!v.ok()) return set::create();
    if (v.cast<set::Bool>().value()) {
        return set::create<set::Universe>();
    }
    return set::create<set::Void>();
}

Else::Else() : BuiltinFact("Else") {}
set::Set Else::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &v = facts.set.extract("v");
    if (!v.ok()) return set::create();
    if (!v.cast<set::Bool>().value()) {
        return set::create<set::Universe>();
    }
    return set::create<set::Void>();
}

Not::Not() : BuiltinFact("Not") {}
set::Set Not::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &v = facts.set.extract("v");
    if (!v.ok()) return set::create();
    return !v;
}

Neg::Neg() : BuiltinFact("Neg") {}
set::Set Neg::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &v = facts.set.extract("v");
    if (!v.ok()) return set::create();
    return -v;
}

Eq::Eq() : BuiltinFact("Eq") {}
set::Set Eq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x == y;
}

Noteq::Noteq() : BuiltinFact("Noteq") {}
set::Set Noteq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x != y;
}

Add::Add() : BuiltinFact("Add") {}
set::Set Add::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x + y;
}

Sub::Sub() : BuiltinFact("Sub") {}
set::Set Sub::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x - y;
}

Mul::Mul() : BuiltinFact("Mul") {}
set::Set Mul::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x * y;
}

Div::Div() : BuiltinFact("Div") {}
set::Set Div::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x / y;
}

Lt::Lt() : BuiltinFact("Lt") {}
set::Set Lt::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x < y;
}

Gt::Gt() : BuiltinFact("Gt") {}
set::Set Gt::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x > y;
}

Lteq::Lteq() : BuiltinFact("Lteq") {}
set::Set Lteq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x <= y;
}

Gteq::Gteq() : BuiltinFact("Gteq") {}
set::Set Gteq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x >= y;
}

And::And() : BuiltinFact("And") {}
set::Set And::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x && y;
}

Or::Or() : BuiltinFact("Or") {}
set::Set Or::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &x = facts.set.extract("x");
    auto const &y = facts.set.extract("y");
    if (!x.ok() || !y.ok()) return set::create();
    return x || y;
}

MakeArray::MakeArray() : BuiltinFact("MakeArray") {}
set::Set MakeArray::solve(Context &ctx) const {
    auto const &facts = ctx.params.top();
    auto const &super = facts.set.extract("x");
    auto const &size = facts.set.extract("y");
    if (!super.ok()) return set::create();
    auto arr = set::create<set::Array>();
    if (size.ok()) {
        arr.cast<set::Array>().resize(size.cast<set::Int>().value());
    }
    return arr;
}

} // namespace builtinModules