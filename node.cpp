#include "node.h"
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
    {   "Super",              "Super"},
    {  "Params",             "Params"},

    {   "GRoot",              "GRoot"},
    {   "GFact",              "GFact"},
    {   "GTupl",              "GTupl"},
    {   "GTup_",              "GTup_"},
    {     "GId",                "GId"},
    {"GEpsilon",           "GEpsilon"},
};

Context::Context(std::string const &file) : file(file), global(set::std()) {}

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

Module const *Set::digest(Context &ctx) {
    if (_params) {
        _params->digest(ctx);
    }
    if (auto const *modFind = ctx.scope.top()->find(view)) {
        ref = modFind;
    }
    return ref;
}

Module const *Fact::digest(Context &ctx) {
    if (auto *super = _lvalue->getSuperset()) {
        super->digest(ctx);
    }
    return _rvalue ? _rvalue->digest(ctx) : nullptr;
}

Module const *Module::digest(Context &ctx) {
    ctx.scope.push(this);
    _stmts->digest(ctx);
    for (auto const &module : _modules) {
        module.second->digest(ctx);
    }
    ctx.scope.pop();
    return this;
}

Module const *Statements::digest(Context &ctx) {
    for (auto const &fact : get()) {
        fact->digest(ctx);
    }
    return nullptr;
}

Module const *Expression::digest(Context &ctx) {
    auto const *module = _super ? _super->digest(ctx) : _extract->digest(ctx);
    if (!module) {
        module = ctx.scope.top();
    }
    auto const *ref = module->find(getExtractName());
    if (ref) {
        _extract->ref = ref;
    } else {
        auto f = module->getFacts().facts;
        auto find = f.equal_range(getExtractName());
        for (auto it = find.first; it != find.second; ++it) {
            _extract->facts.push_back(it->second);
        }
    }
    return ref;
}

std::map<Kind, std::string_view> const Unary::table{
    {Kind::Exclamation, "Not"},
    {Kind::SingleMinus, "Neg"},
};

Module const *Unary::digest(Context &ctx) {
    _params->digest(ctx);
    auto id = table.at(_op);
    if (auto const *module = ctx.scope.top()->find(id)) {
        ref = module;
    }
    return ref;
}

std::map<Kind, std::string_view> const Binary::table{
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
    {       Kind::DoubleAnd,   "And"},
    {        Kind::DoubleOr,    "Or"},
};

Module const *Binary::digest(Context &ctx) {
    _params->digest(ctx);
    if (auto const *find = ctx.scope.top()->find(table.at(_op))) {
        ref = find;
    }
    return ref;
}

set::Set Set::solve(Context &ctx) const {
    auto params = _params ? _params->solve(ctx) : set::create<set::Sets>();
    if (!params.ok()) {
        return set::create();
    }
    if (ref) {
        auto set = ref->solveWithParams(std::move(params), ctx);
        return set;
    }
    if (auto local = ctx.params.top().extract(view); local.ok()) { // module a { b = c + 1 }
        auto resolve = local.resolve(params);                      //                ^
        return resolve;
    }
    if (auto global = ctx.global.extract(view); global.ok()) { // main = int
        auto resolve = global.resolve(params);                 //        ^^^
        return resolve;
    }
    auto solved = set::create();
    Fact const *solvedFact;
    for (auto const *f : facts) {
        if (auto solving = f->solve(ctx); solving.ok()) {
            if (solved.ok()) {
                Quiet<style::red>(), "'", f->lvalue().view, "' ambiguous\n";
                solvedFact->printCode(ctx.file);
                f->printCode(ctx.file);
                return set::create();
            }
            solved = std::move(solving);
            solvedFact = f;
        }
    }
    if (solved.ok()) {
        return solved;
    }

    Quiet<style::yellow>(), "undefined extract '", view, "'\n";
    printCode(ctx.file);
    std::cout << std::flush;
    return set::create();
}

set::Set Expression::solve(Context &ctx) const {
    if (_super) {
        auto super = _super->solve(ctx);
        if (!super.ok()) {
            return set::create();
        }
        if (auto ex = super.extract(_extract->view); ex.ok()) {
            return ex;
        }
    }
    if (auto solve = _extract->solve(ctx); solve.ok()) {
        return solve;
    }
    Quiet<style::yellow>(), "undeclared set '", _extract->view, "'\n";
    _extract->printCode(ctx.file);
    return set::create();
}

set::Set Unary::solve(Context &ctx) const {
    auto const params = _params->solve(ctx);
    if (!params.ok()) {
        return set::create();
    }
    if (auto ex = ctx.global.extract(table.at(_op)); ex.ok()) {
        return ex.resolve(std::move(params)).extract("extract");
    }
    return set::create();
}

set::Set Binary::solve(Context &ctx) const {
    auto const params = _params->solve(ctx);
    if (!params.ok()) {
        return set::create();
    }
    if (ref) {
        if (auto local = ref->solveWithParams(params.clone(), ctx); local.ok()) {
            return local.extract("extract");
        }
    }
    if (auto ex = ctx.global.extract(table.at(_op)); ex.ok()) {
        return ex.resolve(params).extract("extract");
    }
    return set::create();
}

set::Set Fact::solve(Context &ctx) const {
    if (!_rvalue) return set::create();
    auto rsolve = _rvalue->solve(ctx);
    if (!rsolve.ok()) {
        return set::create();
    }
    auto const &lannot = _lvalue->cast<Set>().getSuperset();
    // default lvalue superset is universe
    auto lsuperset = lannot ? lannot->solve(ctx) : set::create<set::Universe>();
    auto sameSuper = lsuperset.contains(rsolve);
    if (!sameSuper.ok()) {
        return set::create();
    }
    if (!sameSuper.cast<set::Bool>().value()) {
        return set::create();
    }
    return rsolve;
}

set::Set Statements::solve(Context &ctx) const {
    auto sets = std::make_unique<set::Sets>();
    for (auto const &s : get()) {
        auto &fact = s->cast<Fact>();
        auto name = fact.lvalue().view;
        auto solve = s->solve(ctx);
        if (!solve.ok()) {
            return set::create();
        }
        bool ambiguous = !sets->add(name, solve.move());
        if (ambiguous) {
            Quiet<style::red>(), "'", name, "' ambiguous\n";
            fact.printCode(ctx.file);
            return set::create();
        }
    }
    return {std::move(sets)};
}

set::Set Module::solve(Context &ctx) const {
    auto res = set::create<set::Sets>();
    auto &set = res.cast<set::Sets>();
    for (auto const &stmt : _stmts->get()) {
        auto const &fact = stmt->cast<Fact>();
        auto name = fact.lvalue().view;
        if (auto ex = ctx.params.top().extract(name); ex.ok()) {
            set.add(name, ex.move());
        } else {
            auto solve = fact.solve(ctx);
            auto debug = solve.clone();
            bool ambiguous = !set.add(name, solve.move());
            if (ambiguous) {
                Quiet<style::red>(), "'", name, "' ambiguous\n";
                fact.printCode(ctx.file);
                return set::create();
            }
        }
    }
    return res;
}

Token::Token(Kind kind, std::string_view view) : Node(view), kind(kind) {}

Token::Token(Kind kind, Node const &node) : Node(node), kind(kind) {}

set::Set Token::solveWithParams(set::Set params, Context &ctx) const {
    ctx.params.push(std::move(params));
    auto slv = solve(ctx);
    ctx.params.pop();
    return slv;
}

Nonterm::Nonterm(Kind kind) : Token(kind, {}) {}

void Nonterm::pushArgs(std::vector<std::unique_ptr<Token>> tokens) {
    view = tokens.front()->combine(*tokens.back()).view;
    args = std::move(tokens);
};

Set::Set(std::string_view view, std::unique_ptr<Token> &&annotation, std::unique_ptr<Token> &&params)
    : Token(Kind::Set, view), _annotation(std::move(annotation)), _params(&params.release()->cast<Statements>()) {}

void Set::setSuperset(std::unique_ptr<Token> &&set) {
    _annotation = std::move(set);
}

Token *Set::getSuperset() {
    return _annotation.get();
}

void Set::setParams(std::unique_ptr<Token> &&params) {
    _params.reset(&params.release()->cast<Statements>());
}

Expression::Expression(std::unique_ptr<Token> &&extract, std::unique_ptr<Token> &&super)
    : Token(Kind::Expr, extract->view), _extract(&extract.release()->cast<Set>()) {
    if (super) {
        _super.reset(&super.release()->cast<Expression>());
        view = combine(*_super).view;
    } else {
        _super = nullptr;
    }
}

void Expression::setExtract(std::unique_ptr<Token> &&extract) {
    _extract.reset(&extract.release()->cast<Set>());
}

void Expression::setSuperset(std::unique_ptr<Token> &&super) {
    _super.reset(&super.release()->cast<Expression>());
    view = combine(*super).view;
}

std::string_view Expression::getExtractName() const {
    return _extract->view;
}

Module const *Expression::getModule() const {
    return _extract->ref;
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

node::Module::Module(Modules &&modules)
    : Token(Kind::Module, {}), _stmts(std::make_unique<Statements>()), _name("std"), _modules(std::move(modules)) {}

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

Module::Facts Module::getFacts() const {
    auto m = Module::Facts(getName());
    for (auto const &s : _stmts->get()) {
        auto &fact = s->cast<Fact>();
        auto name = fact.lvalue().view;
        m.facts.emplace(name, &fact);
    }
    return m;
}

Module const *Module::find(std::string_view name) const {
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
    if (_params) _params->dump(indent + 1);
}

void Expression::dump(size_t indent) const {
    std::cout << std::string(indent * 2, ' ') << "extract " << _extract->view << ": "
              << (_super ? _super->getExtractName() : "?") << "\n";
    if (_extract) _extract->dump(indent + 1);
    if (_super) _super->dump(indent + 1);
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
    for (auto const &m : _modules) {
        m.second->dump(indent + 1);
    }
}