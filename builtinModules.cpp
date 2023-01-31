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
std::unique_ptr<set::ISet> Contains::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return x->contains(*y, ctx.sets.at("bool").get());
}

If::If() : BuiltinFact("If") {}
std::unique_ptr<set::ISet> If::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &v = facts.at("v");
    if (!v) return nullptr;
    if (v->cast<set::Bool>().value()) {
        return std::make_unique<set::Universe>();
    }
    return std::make_unique<set::Void>();
}

Else::Else() : BuiltinFact("Else") {}
std::unique_ptr<set::ISet> Else::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &v = facts.at("v");
    if (!v) return nullptr;
    if (!v->cast<set::Bool>().value()) {
        return std::make_unique<set::Universe>();
    }
    return std::make_unique<set::Void>();
}

Not::Not() : BuiltinFact("Not") {}
std::unique_ptr<set::ISet> Not::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &v = facts.at("v");
    if (!v) return nullptr;
    return !*v;
}

Neg::Neg() : BuiltinFact("Neg") {}
std::unique_ptr<set::ISet> Neg::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &v = facts.at("v");
    if (!v) return nullptr;
    return -*v;
}

Eq::Eq() : BuiltinFact("Eq") {}
std::unique_ptr<set::ISet> Eq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return x->equals(*y, ctx.sets.at("bool").get());
}

Noteq::Noteq() : BuiltinFact("Noteq") {}
std::unique_ptr<set::ISet> Noteq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return x->noteq(*y, ctx.sets.at("bool").get());
}

Add::Add() : BuiltinFact("Add") {}
std::unique_ptr<set::ISet> Add::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return *x + *y;
}

Sub::Sub() : BuiltinFact("Sub") {}
std::unique_ptr<set::ISet> Sub::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return *x - *y;
}

Mul::Mul() : BuiltinFact("Mul") {}
std::unique_ptr<set::ISet> Mul::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return *x * *y;
}

Div::Div() : BuiltinFact("Div") {}
std::unique_ptr<set::ISet> Div::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return *x / *y;
}

Lt::Lt() : BuiltinFact("Lt") {}
std::unique_ptr<set::ISet> Lt::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return x->lt(*y, ctx.sets.at("bool").get());
}

Gt::Gt() : BuiltinFact("Gt") {}
std::unique_ptr<set::ISet> Gt::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return x->gt(*y, ctx.sets.at("bool").get());
}

Lteq::Lteq() : BuiltinFact("Lteq") {}
std::unique_ptr<set::ISet> Lteq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return x->lteq(*y, ctx.sets.at("bool").get());
}

Gteq::Gteq() : BuiltinFact("Gteq") {}
std::unique_ptr<set::ISet> Gteq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return x->gteq(*y, ctx.sets.at("bool").get());
}

} // namespace builtinModules