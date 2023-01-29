#include "node.h"
#include "builtinModules.h"
#include "outs.h"

using namespace node;

std::string Kind::show() const {
    return infos[_val].show;
}

std::string Kind::name() const {
    return infos[_val].name;
}

std::array<Kind::Info, Kind::Value::Size> const Kind::infos{
  /* clang-format off */ Kind::Info  /* clang-format on */
    {      "id",                 "Id"},
    {     "num",             "Number"},
    { "comment",            "Comment"},
    {   "'...'",         "Apostrophe"},
    { "\"...\"",          "Quotation"},
    {       "=",        "SingleEqual"},
    {      "==",        "DoubleEqual"},
    {      "!=",   "ExclamationEqual"},
    {       ":",        "SingleColon"},
    {      "::",        "DoubleColon"},
    {       "(",    "OpenParenthesis"},
    {       ")",   "CloseParenthesis"},
    {       "[",  "OpenSquareBracket"},
    {       "]", "CloseSquareBracket"},
    {       "{",   "OpenCurlyBracket"},
    {       "}",  "CloseCurlyBracket"},
    {       "<",           "LessThan"},
    {       ">",          "GreatThan"},
    {      "<=",    "LessThanOrEqual"},
    {      ">=",   "GreatThanOrEqual"},
    {       "&",          "SingleAnd"},
    {      "&&",          "DoubleAnd"},
    {       "|",           "SingleOr"},
    {      "||",           "DoubleOr"},
    {       "+",         "SinglePlus"},
    {      "++",         "DoublePlus"},
    {       "-",        "SingleMinus"},
    {      "--",        "DoubleMinus"},
    {       "*",     "SingleAsterisk"},
    {      "**",     "DoubleAsterisk"},
    {       "/",        "SingleSlash"},
    {      "//",        "DoubleSlash"},
    {       "%",            "Percent"},
    {       ".",                "Dot"},
    {       ",",              "Comma"},
    {       ";",          "Semicolon"},
    {       "!",        "Exclamation"},
    {       "?",           "Question"},
    {       "#",               "Hash"},
    {     "<=>",          "Spaceship"},
    {     "::=",           "BnfEqual"},
    {     "<+>",         "ChevronsOr"},
    {     "<$>",        "ChevronsEnd"},
    {  "module",             "module"},
    {     "set",                "set"},
    { "extract",            "extract"},

    {      "ε",            "Epsilon"},
    {       "$",                "Eof"},

    {    "😨",            "Invalid"},
    {     "Ast",                "Ast"},

    {    "Root",               "Root"},
    {    "Para",               "Para"},
    {   "Para'",               "Par_"},
    {    "Stmt",               "Stmt"},
    {   "Stmt'",               "Stm_"},
    {    "Expr",               "Expr"},
    {   "Expr'",               "Exp_"},
    {    "Term",               "Term"},
    {   "Term'",               "Ter_"},
    {    "Fact",               "Fact"},
    {   "Fact'",               "Fac_"},
    {   "Tuple",               "Tupl"},
    {  "Tuple'",               "Tup_"},
    {    "Oprt",               "Oprt"},
    {   "Unary",              "Unary"},
    {  "Binary",             "Binary"},
    {    "LtoR",               "LtoR"},
    {    "RtoL",               "RtoL"},
    {    "Atom",               "Atom"},
    { "IModule",            "IModule"},
    {   "Annot",              "Annot"},
    {    "Bool",               "Bool"},

    {   "GRoot",              "GRoot"},
    {   "GFact",              "GFact"},
    {   "GTupl",              "GTupl"},
    {   "GTup_",              "GTup_"},
    {     "GId",                "GId"},
    {"GEpsilon",           "GEpsilon"},
};

Context::Context(std::string const &file) : _modules(builtinModules::all()), file(file) {
    modules = builtinModules::genMap(_modules);
    sets["bool"] = std::make_unique<set::Identity>();
    sets["int"] = std::make_unique<set::Identity>();
}

std::pair<Node::Pos, Node::Pos> Node::getRange(std::string const &str) const {
    if (view._Unchecked_begin() < str._Unchecked_begin() || view._Unchecked_end() > str._Unchecked_end()) return {};

    char const *begin = str._Unchecked_begin();
    std::string_view const offset(begin, view._Unchecked_begin() - begin);
    uint32_t const lfNum = std::count(offset.begin(), offset.end(), '\n');
    uint32_t headColumn, tailColumn;
    if (lfNum == 0) {
        headColumn = offset.size();
    } else {
        headColumn = offset.size() - offset.find_last_of('\n') - 1;
    }

    uint32_t restLfNum = std::count(view.begin(), view.end(), '\n');
    if (restLfNum == 0) {
        tailColumn = view.size() + headColumn;
    } else {
        std::string_view const v(begin, view._Unchecked_end() - begin);
        tailColumn = v.size() - v.find_last_of('\n') - 1;
    }
    return {
        {            lfNum + 1, headColumn + 1},
        {lfNum + restLfNum + 1,     tailColumn}
    };
}

Node Node::combine(Node const &tail) const {
    auto const *head = view._Unchecked_begin();
    if (!head) return tail;
    auto size = tail.view._Unchecked_end() - head;
    return std::string_view{head, size_t(size)};
}

void Node::printCode(std::string const &file) const {
    auto const [head, tail] = getRange(file);
    if (head.line + head.column + tail.line + tail.column == 0) {
        Quiet(), "code out of file '", view, "'\n\n";
        std::cout << std::flush;
        return;
    }
    auto const lineNum = "line " + std::to_string(head.line) + ": ";
    Quiet<style::black>(), lineNum;
    if (head.line != tail.line) {
        Quiet(), "printing code... but has not support multiline yet🥺\n\n";
        std::cout << std::flush;
        return;
    }
    char const *begin = view._Unchecked_begin() + 1 - head.column;
    std::istringstream iss(begin);
    std::string line;
    std::getline(iss, line);
    Quiet(), line, '\n';
    Quiet(), std::string(head.column - 1 + lineNum.size(), ' '), std::string(tail.column + 1 - head.column, '^'), '\n';
    std::cout << std::flush;
}

std::unique_ptr<set::ISet> Set::solve(Context &ctx) const {
    auto &params = ctx.params.top()->cast<set::Sets>();
    {
        auto setsfind = params.find(str());
        if (setsfind != params.end()) {
            if (!setsfind->second) {
                return nullptr;
            }
            return setsfind->second->clone();
            // return std::make_unique<set::Ref>(setsfind->second.get());
        }
    }
    {
        auto setsfind = ctx.sets.find(str());
        if (setsfind != ctx.sets.end()) {
            if (!setsfind->second) {
                return nullptr;
            }
            return std::make_unique<set::Ref>(setsfind->second.get());
        }
    }
    auto facts = params.module->getFacts();
    auto factFind = facts.facts.equal_range(str());
    std::unique_ptr<set::ISet> solved;
    Fact *solvedFact;
    for (auto it = factFind.first; it != factFind.second; ++it) {
        if (auto solving = it->second->solve(ctx)) {
            if (solved) {
                Quiet<style::red>(), "'", view, "' ambiguous\n";
                solvedFact->printCode(ctx.file);
                it->second->printCode(ctx.file);
                return nullptr;
            }
            solved = std::move(solving);
            solvedFact = it->second;
        }
    }
    if (solved) {
        return solved;
    }
    Quiet<style::red>(), "undefined extract '", view, "'\n";
    printCode(ctx.file);
    std::cout << std::flush;
    return nullptr;
}

std::unique_ptr<set::ISet> Expression::solve(Context &ctx) const {
    auto moduleName = module->cast<Module>().getName();
    auto moduleFind = ctx.modules.find(moduleName);
    if (moduleFind == ctx.modules.end()) {
        Quiet<style::red>(), "undeclared module '", moduleName, "'\n";
        module->printCode(ctx.file);
        return nullptr;
    }
    auto facts = moduleFind->second->getFacts().facts;
    auto factsFind = facts.find(extract->str());
    if (factsFind == facts.end()) {
        Quiet<style::red>(), "undeclared extract '", extract->view, "'\n";
        extract->printCode(ctx.file);
        return nullptr;
    }
    auto params = module->solve(ctx);
    params->cast<set::Sets>().module = moduleFind->second;
    auto ptr = std::make_unique<set::Sets>();
    ptr.reset(&params.release()->cast<set::Sets>());
    ctx.params.push(std::move(ptr));
    auto solve = extract->solve(ctx);
    ctx.params.pop();
    return solve;
}

std::unique_ptr<set::ISet> Fact::solve(Context &ctx) const {
    if (!rhs) return nullptr;
    auto rsolve = rhs->solve(ctx);
    auto const &lannot = lhs->cast<Set>().annotation;
    if (lannot && rsolve) {
        auto ssolve = lannot->solve(ctx);
        auto sameSuper = ssolve->equal(*rsolve->superset(), ctx.sets.at("bool").get());
        if (!sameSuper->cast<set::Bool>().value()) {
            return nullptr;
        }
    }
    return rsolve;
}

set::Module Module::getFacts() const {
    auto m = set::Module(getName());
    for (auto const &s : stmts->cast<Statements>().get()) {
        if (s->kind != Kind::Fact) continue;
        auto &fact = s->cast<Fact>();
        auto name = std::string(fact.lhs->view);
        m.facts.emplace(name, &fact);
    }
    return m;
}

std::unique_ptr<set::ISet> Module::solve(Context &ctx) const {
    auto sets = std::make_unique<set::Sets>();
    for (auto const &s : stmts->cast<Statements>().get()) {
        if (s->kind != Kind::Fact) continue;
        auto &fact = s->cast<Fact>();
        auto name = std::string(fact.lhs->view);
        sets->emplace(name, s->solve(ctx));
    }
    return sets;
}

Token::Token(Kind kind, std::string_view view) : Node(view), kind(kind) {}

Token::Token(Kind kind, Node const &node) : Node(node), kind(kind) {}

Nonterm::Nonterm(Kind kind) : Token(kind, {}) {}

void Nonterm::pushArgs(std::vector<std::unique_ptr<Token>> tokens) {
    view = tokens.front()->combine(*tokens.back()).view;
    args = std::move(tokens);
};

Set::Set(std::string_view view, Module *parent) : Token(Kind::Atom, view), parent(parent) {}

void Set::setSuperset(std::unique_ptr<Token> &&set) {
    annotation.reset(&set.release()->cast<Set>());
}

Expression::Expression(std::unique_ptr<Token> &&extract, std::unique_ptr<Token> &&module)
    : Token(Kind::Expr, module->combine(*extract)), extract(std::move(extract)), module(std::move(module)) {}

Fact::Fact(std::unique_ptr<Token> &&lhs, std::unique_ptr<Token> &&rhs)
    : Token(Kind::Fact, rhs ? lhs->combine(*rhs) : lhs->view), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

Statements::Statements() : Token(Kind::Stmt, {}) {}

void Statements::pushFront(std::unique_ptr<Token> &&stmt) {
    if (!stmt) return;
    _statements.push_front(std::move(stmt));
    view = _statements.front()->combine(*_statements.back()).view;
}

void Statements::pushBack(std::unique_ptr<Token> &&stmt) {
    if (!stmt) return;
    _statements.push_back(std::move(stmt));
    view = _statements.front()->combine(*_statements.back()).view;
}

Module::Module(std::unique_ptr<Statements> &&statements) : Module(std::move(statements), Node{{}}) {}

Module::Module(std::unique_ptr<Token> &&statements, Node const &node)
    : Token(Kind::Module, node), stmts(std::make_unique<Statements>()) {
    if (statements) {
        stmts.reset(&statements.release()->cast<Statements>());
    }
    view = stmts->view;
}

void Module::setName(std::string const &name) {
    _name = name;
}

void Module::setName(std::string_view const &name) {
    _name = name;
    view = Node(name).combine(*this).view;
}

std::string const &Module::getName() const {
    return _name;
}

auto Module::find(std::string const &lhs) const -> std::deque<std::unique_ptr<Token>>::const_iterator {
    return std::find_if(stmts->get().begin(), stmts->get().end(), [&lhs](std::unique_ptr<Token> const &token) {
        return token->kind == Kind::Fact && token->cast<Fact>().lhs->view == lhs;
    });
}

void Token::print(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "token\n";
}

void Nonterm::print(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "nonterm\n";
}

void Set::print(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << view << "\n";
}

void Expression::print(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "expr\n";
    if (extract) extract->print(indent + 1);
    if (module) module->print(indent + 1);
}

void Fact::print(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "fact\n";
    if (lhs) lhs->print(indent + 1);
    if (rhs) rhs->print(indent + 1);
}

void Statements::print(size_t indent) const {
    for (auto const &s : _statements) {
        s->print(indent);
    }
}

void Module::print(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "module " << getName() << "\n";
    stmts->print(indent + 1);
}