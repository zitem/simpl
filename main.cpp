#include "cfg.h"
#include "lexer.h"
#include "outs.h"
#include "parser.h"

using namespace node;

std::unique_ptr<Token> genAst(Nonterm &self, Context &ctx) {
    auto get = [&self](auto idx) { return self.args[self.size - 1 - idx].get(); };
    auto nonterm = [&get](auto idx) -> Nonterm & { return get(idx)->template cast<Nonterm>(); };

    switch (self.kind.value()) {

    case Kind::Root: {
        auto module = std::make_unique<Module>();
        ctx.currentModule.push(module.get());
        auto stmt = genAst(nonterm(0), ctx);
        module->stmts.reset(&stmt.release()->cast<Statements>());
        module->view = self.view;
        ctx.currentModule.pop();
        return module;
    }

    case Kind::Stmt: {
        if (auto stmt = genAst(nonterm(1), ctx)) {
            stmt->cast<Statements>().pushFront(genAst(nonterm(0), ctx));
            return stmt;
        }
        auto stmt = std::make_unique<Statements>();
        stmt->pushFront(genAst(nonterm(0), ctx));
        return stmt;
    }

    case Kind::Stm_: {
        if (get(0)->kind == Kind::Epsilon) return nullptr;
        if (auto stmt = genAst(nonterm(2), ctx)) {
            stmt->cast<Statements>().pushFront(genAst(nonterm(1), ctx));
            return stmt;
        }
        auto stmt = std::make_unique<Statements>();
        stmt->pushFront(genAst(nonterm(1), ctx));
        return stmt;
    }

    case Kind::Fact:
        switch (get(0)->kind.value()) {
        case Kind::Id: {
            auto lhs = genAst(nonterm(0), ctx);
            if (auto ann = genAst(nonterm(1), ctx)) {
                lhs->cast<Set>().setSuperset(std::move(ann));
            }
            return std::make_unique<Fact>(std::move(lhs), genAst(nonterm(2), ctx));
        }
        case Kind::Module: {
            auto module = std::make_unique<Module>();
            ctx.currentModule.push(module.get());
            auto stmt = genAst(nonterm(3), ctx);
            ctx.currentModule.pop();
            module->stmts.reset(&stmt.release()->cast<Statements>());
            module->setName(nonterm(1).view);
            ctx.modules[module->getName()] = module.get();
            return module;
        }
        default: return nullptr;
        }

    case Kind::Fac_: return get(0)->kind == Kind::Epsilon ? nullptr : genAst(nonterm(1), ctx);

    case Kind::Expr: {
        auto hasR2L = get(0)->kind == Kind::RtoL;
        auto expr = genAst(nonterm(hasR2L ? 1 : 0), ctx);
        auto ltor = genAst(nonterm(hasR2L ? 2 : 1), ctx);
        auto res = ltor ? std::make_unique<Expression>(std::move(ltor), std::move(expr)) : std::move(expr);
        if (hasR2L) {
            auto exp2 = genAst(nonterm(0), ctx);
            auto stmt = std::make_unique<Statements>();
            auto fact = std::make_unique<Fact>(std::make_unique<Set>("v", ctx.currentModule.top()), std::move(res));
            stmt->pushBack(std::move(fact));
            auto modu = std::make_unique<Module>(std::move(stmt));
            modu->setName(nonterm(0).view == "!" ? std::string("Not") : "Neg");
            res = std::make_unique<Expression>(std::make_unique<Set>("extract"), std::move(modu));
        }
        return res;
    }

    case Kind::Exp_:
        if (get(0)->kind == Kind::Id) {
            if (auto expr = genAst(nonterm(1), ctx)) {
                expr->cast<Expression>().module->cast<Module>().setName(nonterm(0).view);
                return expr;
            }
        }
        return genAst(nonterm(0), ctx);

    case Kind::LtoR: {
        if (get(0)->kind == Kind::Epsilon) return nullptr;
        auto expr = genAst(nonterm(1), ctx);
        auto ltor = genAst(nonterm(2), ctx);
        if (!ltor) return expr;
        // TODO: support operator
        return std::make_unique<Expression>(std::move(ltor), std::move(expr));
    }

    case Kind::Extract:
        switch (get(0)->kind.value()) {
        case Kind::Epsilon: return nullptr;
        case Kind::OpenParenthesis:
            return std::make_unique<Expression>(
                genAst(nonterm(4), ctx), // extract
                std::make_unique<Module>(genAst(nonterm(1), ctx), self.view)
            );
        case Kind::Dot: return std::make_unique<Expression>(genAst(nonterm(1), ctx), std::make_unique<Module>());
        }

    case Kind::Annot: return get(0)->kind == Kind::Epsilon ? nullptr : genAst(nonterm(1), ctx);

    case Kind::Id: return std::make_unique<Set>(self.view, ctx.currentModule.top());

    case Kind::Bool: return std::make_unique<Bool>(self.view);

    case Kind::Number: return std::make_unique<Int>(self.view);

    default: return nullptr;
    }
}

void run(char const *filename) {
    auto const bnf = [] {
        using namespace token;
        return std::vector<std::shared_ptr<Base>>{
            std::make_shared<StrFixed>("module", Kind::Module),
            std::make_shared<StrFixed>("false", Kind::Bool),
            std::make_shared<StrFixed>("true", Kind::Bool),
            std::make_shared<CharRegion>('\'', '\'', Kind::Apostrophe),
            std::make_shared<CharRegion>('"', '"', Kind::Quotation),
            std::make_shared<CharFixed>('=', Kind::SingleEqual),
            std::make_shared<StrFixed>("==", Kind::DoubleEqual),
            std::make_shared<StrFixed>("!=", Kind::ExclamationEqual),
            std::make_shared<CharFixed>(':', Kind::SingleColon),
            std::make_shared<StrFixed>("::", Kind::DoubleColon),
            std::make_shared<CharFixed>('(', Kind::OpenParenthesis),
            std::make_shared<CharFixed>(')', Kind::CloseParenthesis),
            std::make_shared<CharFixed>('[', Kind::OpenSquareBracket),
            std::make_shared<CharFixed>(']', Kind::CloseSquareBracket),
            std::make_shared<CharFixed>('{', Kind::OpenCurlyBracket),
            std::make_shared<CharFixed>('}', Kind::CloseCurlyBracket),
            std::make_shared<CharFixed>('<', Kind::LessThan),
            std::make_shared<CharFixed>('>', Kind::GreatThan),
            std::make_shared<StrFixed>("<=", Kind::LessThanOrEqual),
            std::make_shared<StrFixed>(">=", Kind::GreatThanOrEqual),
            std::make_shared<CharFixed>('&', Kind::SingleAnd),
            std::make_shared<StrFixed>("&&", Kind::DoubleAnd),
            std::make_shared<CharFixed>('|', Kind::SingleOr),
            std::make_shared<StrFixed>("||", Kind::DoubleOr),
            std::make_shared<CharFixed>('+', Kind::SinglePlus),
            std::make_shared<StrFixed>("++", Kind::DoublePlus),
            std::make_shared<CharFixed>('-', Kind::SingleMinus),
            std::make_shared<StrFixed>("--", Kind::DoubleMinus),
            std::make_shared<CharFixed>('*', Kind::SingleAsterisk),
            std::make_shared<StrFixed>("**", Kind::DoubleAsterisk),
            std::make_shared<CharFixed>('/', Kind::SingleSlash),
            std::make_shared<StrFixed>("//", Kind::DoubleSlash),
            std::make_shared<CharFixed>('%', Kind::Percent),
            std::make_shared<CharFixed>('.', Kind::Dot),
            std::make_shared<CharFixed>(',', Kind::Comma),
            std::make_shared<CharFixed>(';', Kind::Semicolon),
            std::make_shared<CharFixed>('!', Kind::Exclamation),
            std::make_shared<CharFixed>('?', Kind::Question),
            std::make_shared<CharFixed>('#', Kind::Hash),
            std::make_shared<StrFixed>("<=>", Kind::Spaceship),
            std::make_shared<Id>(Kind::Id),
            std::make_shared<Number>(Kind::Number),
            std::make_shared<StrRegion>("//", "\n", Kind::Comment),
            std::make_shared<StrRegion>("/*", "*/", Kind::Comment),
        };
    }();

    setVerbosity(Verbosity::Quiet);
    auto const cfg = LangCFG::getCFG();
    // setVerbosity(Verbosity::Diagnostic);

    Parser parser;
    parser.setupGramma(cfg);

    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    auto str = buffer.str();
    parser.setRawText(str);

    Lexer lexer(str);
    auto next = lexer.next(bnf);
    int syntaxErr{};
    for (; !lexer.empty(); next = lexer.next(bnf)) {
        Diagn(), "== ", next.kind.name(), " ", next.view, " ==\n";
        syntaxErr += parser.parse(next);
    }

    Context ctx(str);
    auto ast = genAst(parser.getCST()._Get_container().front()->cast<Nonterm>(), ctx);
    // ast->print();
    // std::cout << std::flush;

    auto &root = ast->cast<Module>();
    root.setName(filename2module(filename));
    ctx.modules[root.getName()] = &root;
    ctx.params.push(std::make_unique<set::Sets>(&root));
    auto expr = Expression(std::make_unique<Set>("main"), std::move(ast));
    auto solved = expr.solve(ctx);

    // compile to llvm ir here

    if (solved) {
        Quiet<style::blue>(), "> ", solved->show(), "\n";
    } else {
        Quiet<style::blue>(), "> unsolved module '", root.getName(), "'\n";
    }

    if (next.view.length() > 1) {
        Quiet<style::red>(), "lex error: rest is '", next.view;
    }

    if (syntaxErr == 1) {
        Quiet<style::red>(), "1 syntax error\n";
    } else if (syntaxErr > 1) {
        Quiet<style::red>(), syntaxErr, " syntax errors\n";
    }
}

#include <chrono>

int main(int argc, char *argv[]) {
    run(((void)argc, argv[1])); // NOLINT
    std::vector<std::string> emoji = {"🥳", "😘", "😗", "😙", "😚"};
    auto idx = std::chrono::system_clock::now().time_since_epoch().count() % 5;
    std::cout << emoji[idx] << '\n';
}

void lex2() {
    std::ifstream file("Test.sip");
    std::stringstream buffer;
    buffer << file.rdbuf();
    auto str = buffer.str();

    uint32_t cnt{};
    enum { COUNT = 1 << 8 };
    using namespace std::chrono;
    auto start = system_clock::now();
    for (int i{}; i < COUNT; ++i) {
        Lexer lexer(str);
    }
    auto end = system_clock::now();
    std::cout << cnt / COUNT << "[" << duration_cast<milliseconds>(end - start) << "]";
}
