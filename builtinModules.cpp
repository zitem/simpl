#include "builtinModules.h"

using namespace node;

namespace builtinModules {

std::vector<std::unique_ptr<Module>> all() {
    std::vector<std::unique_ptr<Module>> res;
    struct BuiltinModule : Module {
        using Module::Module;
        [[nodiscard]] Node combine(Node const &tail) const override { return tail; }
    };
    auto push = [&res](auto ptr) {
        auto stmt = std::make_unique<Statements>();
        auto const &name = ptr->name;
        stmt->pushBack(std::move(ptr));
        auto module = std::make_unique<BuiltinModule>(std::move(stmt));
        module->setName(name);
        res.push_back(std::move(module));
    };
    push(std::make_unique<Contains>());
    push(std::make_unique<If>());
    push(std::make_unique<Eq>());
    push(std::make_unique<Not>());
    push(std::make_unique<Add>());
    push(std::make_unique<Sub>());
    push(std::make_unique<Mul>());
    push(std::make_unique<Div>());
    return res;
};

std::map<std::string, Module *> genMap(std::vector<std::unique_ptr<Module>> const &all) {
    std::map<std::string, Module *> res;
    for (auto const &m : all) {
        res[m->stmts->get().front()->cast<BuiltinFact>().name] = &*m;
    }
    return res;
}

BuiltinFact::BuiltinFact(std::string name) : Fact(std::make_unique<Set>("extract"), nullptr), name(std::move(name)) {}

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
    return nullptr;
}

Not::Not() : BuiltinFact("Not") {}
std::unique_ptr<set::ISet> Not::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &v = facts.at("v");
    if (!v) return nullptr;
    return std::make_unique<set::Bool>(!v->cast<set::Bool>().value(), v->superset());
}

Eq::Eq() : BuiltinFact("Eq") {}
std::unique_ptr<set::ISet> Eq::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return x->equal(*y, ctx.sets.at("bool").get());
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

} // namespace builtinModules