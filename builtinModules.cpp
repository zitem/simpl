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

Add::Add() : BuiltinFact("Add") {}
std::unique_ptr<set::ISet> Add::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return std::make_unique<set::Number>(x->cast<set::Number>() + y->cast<set::Number>());
}

Sub::Sub() : BuiltinFact("Sub") {}
std::unique_ptr<set::ISet> Sub::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return std::make_unique<set::Number>(x->cast<set::Number>() - y->cast<set::Number>());
}

Mul::Mul() : BuiltinFact("Mul") {}
std::unique_ptr<set::ISet> Mul::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    auto lhs = x->cast<set::Number>().value();
    auto rhs = y->cast<set::Number>().value();
    return std::make_unique<set::Number>(x->cast<set::Number>() * y->cast<set::Number>());
}

Div::Div() : BuiltinFact("Div") {}
std::unique_ptr<set::ISet> Div::solve(Context &ctx) const {
    auto const &facts = ctx.params.top()->cast<set::Sets>();
    auto const &x = facts.at("x");
    auto const &y = facts.at("y");
    if (!x || !y) return nullptr;
    return std::make_unique<set::Number>(x->cast<set::Number>() / y->cast<set::Number>());
}

} // namespace builtinModules