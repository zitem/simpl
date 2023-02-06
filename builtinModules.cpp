#include "builtinModules.h"

namespace builtinModules {

std::vector<std::unique_ptr<node::Module>> all() {
    static std::string_view const VIEW{"built-in module"};
    std::vector<std::unique_ptr<node::Module>> res;
    auto push = [&res](auto ptr) {
        auto stmt = std::make_unique<node::Statements>();
        auto const &name = ptr->name;
        stmt->pushBack(std::move(ptr));
        auto module = std::make_unique<node::Module>(std::move(stmt), VIEW, name);
        res.push_back(std::move(module));
    };
    push(std::make_unique<Contains>());
    push(std::make_unique<If>());
    push(std::make_unique<Else>());
    push(std::make_unique<Not>());
    push(std::make_unique<Neg>());
    push(std::make_unique<Eq>());
    push(std::make_unique<Noteq>());
    push(std::make_unique<Add>());
    push(std::make_unique<Sub>());
    push(std::make_unique<Mul>());
    push(std::make_unique<Div>());
    push(std::make_unique<Lt>());
    push(std::make_unique<Gt>());
    push(std::make_unique<Lteq>());
    push(std::make_unique<Gteq>());
    push(std::make_unique<And>());
    push(std::make_unique<Or>());
    return res;
};

std::map<std::string, node::Module *> genMap(std::vector<std::unique_ptr<node::Module>> const &all) {
    std::map<std::string, node::Module *> res;
    for (auto const &m : all) {
        res[m->getFacts().facts.begin()->second->cast<BuiltinFact>().name] = &*m;
    }
    return res;
}

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

} // namespace builtinModules