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
    {     "Op1",                "Op1"},
    {     "Op2",                "Op2"},
    {     "Op3",                "Op3"},
    {     "Op4",                "Op4"},
    {     "Op5",                "Op5"},
    {     "Op6",                "Op6"},
    {     "Op7",                "Op7"},
    {     "Op8",                "Op8"},
    {     "Op9",                "Op9"},
    {    "Exp1",               "Exp1"},
    {    "Exp2",               "Exp2"},
    {    "Exp3",               "Exp3"},
    {    "Exp4",               "Exp4"},
    {    "Exp5",               "Exp5"},
    {    "Exp6",               "Exp6"},
    {    "Exp7",               "Exp7"},
    {    "Exp8",               "Exp8"},
    {    "Exp9",               "Exp9"},
    {   "Exp1_",              "Exp1_"},
    {   "Exp2_",              "Exp2_"},
    {   "Exp3_",              "Exp3_"},
    {   "Exp4_",              "Exp4_"},
    {   "Exp5_",              "Exp5_"},
    {   "Exp6_",              "Exp6_"},
    {   "Exp7_",              "Exp7_"},
    {   "Exp8_",              "Exp8_"},
    {   "Exp9_",              "Exp9_"},

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
    if (std::any_of(params.begin(), params.end(), [](auto const &pair) { return !pair.second; })) {
        return nullptr;
    }
    if (auto setsfind = params.find(str()); setsfind != params.end()) {
        if (!setsfind->second) {
            return nullptr;
        }
        return setsfind->second->clone();
    }
    if (auto setsfind = ctx.sets.find(str()); setsfind != ctx.sets.end()) {
        if (!setsfind->second) {
            return nullptr;
        }
        return setsfind->second->clone();
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
    Quiet<style::yellow>(), "undefined extract '", view, "'\n";
    printCode(ctx.file);
    std::cout << std::flush;
    return nullptr;
}

std::unique_ptr<set::ISet> solveExpr(Module const *module, Set const &extract, Statements const &params, Context &ctx) {
    auto p = params.solve(ctx);
    auto *sets = &p.release()->cast<set::Sets>();
    sets->module = module;
    ctx.params.push(std::unique_ptr<set::Sets>(sets));
    auto solve = extract.solve(ctx);
    ctx.params.pop();
    return solve;
}

std::unique_ptr<set::ISet> Expression::solve(Context &ctx) const {
    auto name = std::string(_module);
    auto findBuiltin = ctx.modules.find(name);
    Module const *module{};
    if (findBuiltin != ctx.modules.end()) {
        module = findBuiltin->second;
    }
    if (const auto *find = ctx.params.top()->module->find(name)) {
        if (module) {
            Quiet<style::yellow>(), "overwrite built-in module '", name, "'\n";
        }
        module = find;
    }
    if (!module) {
        Quiet<style::red>(), "undeclared module '", name, "'\n";
        _stmts->printCode(ctx.file);
        return nullptr;
    }
    auto facts = module->getFacts().facts;
    auto factsFind = facts.find(_extract->str());
    if (factsFind == facts.end()) {
        Quiet<style::red>(), "undeclared extract '", _extract->view, "'\n";
        _extract->printCode(ctx.file);
        return nullptr;
    }
    return ::solveExpr(module, *_extract, *_stmts, ctx);
}

std::unique_ptr<set::ISet> Unary::solve(Context &ctx) const {
    auto module = ctx.modules.find(_op == Kind::Exclamation ? "Not" : "Neg");
    return ::solveExpr(module->second, Set("extract"), *_params, ctx);
}

std::unique_ptr<set::ISet> Binary::solve(Context &ctx) const {
    static std::map<Kind, std::string> const TABLE{
        {      Kind::SinglePlus,   "Add"},
        {     Kind::SingleMinus,   "Sub"},
        {  Kind::SingleAsterisk,   "Mul"},
        {     Kind::SingleSlash,   "Div"},
        {        Kind::LessThan,    "Lt"},
        {       Kind::GreatThan,    "Gt"},
        { Kind::LessThanOrEqual,  "Lteq"},
        {Kind::GreatThanOrEqual,  "Gteq"},
        {     Kind::DoubleEqual,    "Eq"},
        {Kind::ExclamationEqual, "Noteq"},
        {     Kind::Exclamation,   "Not"},
    };
    auto module = ctx.modules.find(TABLE.at(_op));
    return ::solveExpr(module->second, Set("extract"), *_params, ctx);
}

std::unique_ptr<set::ISet> Fact::solve(Context &ctx) const {
    if (!_rvalue) return nullptr;
    auto rsolve = _rvalue->solve(ctx);
    if (!rsolve) {
        return nullptr;
    }
    auto const &lannot = _lvalue->cast<Set>().getSuperset();
    // default lvalue superset is universe
    auto lsuperset = lannot ? lannot->solve(ctx) : std::make_unique<set::Universe>();
    auto sameSuper = lsuperset->contains(*rsolve, ctx.sets.at("bool").get());
    if (!sameSuper->cast<set::Bool>().value()) {
        return nullptr;
    }
    return rsolve;
}

std::unique_ptr<set::ISet> Statements::solve(Context &ctx) const {
    auto sets = std::make_unique<set::Sets>();
    for (auto const &s : get()) {
        if (s->kind != Kind::Fact) continue;
        auto &fact = s->cast<Fact>();
        auto name = std::string(fact.lvalue().view);
        sets->emplace(name, s->solve(ctx));
    }
    return sets;
}

std::unique_ptr<set::ISet> Module::solve(Context & /*ctx*/) const {
    return nullptr;
}

Token::Token(Kind kind, std::string_view view) : Node(view), kind(kind) {}

Token::Token(Kind kind, Node const &node) : Node(node), kind(kind) {}

Nonterm::Nonterm(Kind kind) : Token(kind, {}) {}

void Nonterm::pushArgs(std::vector<std::unique_ptr<Token>> tokens) {
    view = tokens.front()->combine(*tokens.back()).view;
    args = std::move(tokens);
};

Set::Set(std::string_view view, std::unique_ptr<Token> &&annotation)
    : Token(Kind::Set, view), _annotation(std::move(annotation)) {}

void Set::setSuperset(std::unique_ptr<Token> &&set) {
    _annotation = std::move(set);
}

Token *Set::getSuperset() {
    return _annotation.get();
}

Expression::Expression(std::unique_ptr<Token> &&extract, std::string_view module, std::unique_ptr<Token> &&stmts)
    : Token(Kind::Expr, stmts->combine(*extract)),
      _extract(&extract.release()->cast<Set>()),
      _stmts(&stmts.release()->cast<Statements>()),
      _module(module) {}

void Expression::setModuleName(std::string_view name) {
    _module = name;
    view = Node(name).combine(view).view;
}

std::string_view Expression::getModuleName() const {
    return _module;
}

void Unary::setParam(std::unique_ptr<Token> &&param) {
    view = combine(*param).view;
    auto factView = param->view;
    auto fact = std::make_unique<Fact>(std::make_unique<Set>("v"), std::move(param));
    fact->view = factView;
    _params = std::make_unique<Statements>(std::move(fact));
}

void Binary::setLhs(std::unique_ptr<Token> &&param) {
    if (_binaryLhs) {
        _binaryLhs->setLhs(std::move(param));
        return;
    }
    if (param->kind == Kind::Binary) {
        _binaryLhs = &param->cast<Binary>();
    }
    view = param->combine(*this).view;
    auto factView = param->view;
    auto fact = std::make_unique<Fact>(std::make_unique<Set>("x"), std::move(param));
    fact->view = factView;
    _params->pushFront(std::move(fact));
}

void Binary::setRhs(std::unique_ptr<Token> &&param) {
    view = combine(*param).view;
    auto factView = param->view;
    auto fact = std::make_unique<Fact>(std::make_unique<Set>("y"), std::move(param));
    fact->view = factView;
    _params->pushBack(std::move(fact));
}

Fact::Fact(std::unique_ptr<Token> &&lvalue, std::unique_ptr<Token> &&rvalue)
    : Token(Kind::Fact, rvalue ? lvalue->combine(*rvalue) : lvalue->view),
      _lvalue(&lvalue.release()->cast<Set>()),
      _rvalue(std::move(rvalue)) {}

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

Module::Module(std::unique_ptr<Token> &&statements, Node const &node, std::string name)
    : Token(Kind::Module, node),
      _stmts(std::make_unique<Statements>()),
      _name(std::move(name)) // \n
{
    _stmts.reset(&statements.release()->cast<Statements>());
    view = _stmts->view;
    auto &stmts = _stmts->_statements;
    for (auto it = stmts.begin(); it != stmts.end();) {
        auto &stmt = *it;
        if (stmt->kind == Kind::Module) {
            auto module = std::unique_ptr<Module>(&stmt.release()->cast<Module>());
            auto const &name = module->getName();
            if (name == _name) {
                Quiet<style::red>(), "module '", name, "' has the same name as parent\n";
            }
            _modules.insert({name, std::move(module)});
            it = stmts.erase(it);
        } else {
            ++it;
        }
    }
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

set::Module Module::getFacts() const {
    auto m = set::Module(getName());
    for (auto const &s : _stmts->cast<Statements>().get()) {
        auto &fact = s->cast<Fact>();
        auto name = std::string(fact.lvalue().view);
        m.facts.emplace(name, &fact);
    }
    return m;
}

Module const *Module::find(std::string const &name) const {
    if (name == _name) {
        return this;
    }
    auto f = _modules.find(name);
    if (f != _modules.end()) {
        return f->second.get();
    }
    return nullptr;
}

void Token::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "token\n";
}

void Nonterm::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "nonterm\n";
}

void Set::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << view << "\n";
}

void Expression::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "expr\n";
    if (_extract) _extract->dump(indent + 1);
    if (_stmts) _stmts->dump(indent + 1);
}

void Unary::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << _op.show() << "\n";
    _params->dump(indent + 1);
}

void Binary::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << _op.show() << "\n";
    _params->dump(indent + 1);
}

void Fact::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "fact\n";
    if (_lvalue) _lvalue->dump(indent + 1);
    if (_rvalue) _rvalue->dump(indent + 1);
}

void Statements::dump(size_t indent) const {
    for (auto const &s : _statements) {
        s->dump(indent);
    }
}

void Module::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "module " << getName() << "\n";
    _stmts->dump(indent + 1);
}