#include "cfg.h"
#include "lexer.h"
#include "outs.h"
#include "parser.h"

std::unique_ptr<node::Token> genAst(node::Nonterm &self, Context &ctx) {
    auto get = [&self](auto idx) { return self.args[self.size - 1 - idx].get(); };
    auto nonterm = [&get](auto idx) -> node::Nonterm & { return get(idx)->template cast<node::Nonterm>(); };

    switch (self.kind.value()) {

    case Kind::Root: return genAst(nonterm(0), ctx);

    case Kind::Stmt: {
        if (auto stmt = genAst(nonterm(1), ctx)) {
            stmt->cast<node::Statements>().pushFront(genAst(nonterm(0), ctx));
            return stmt;
        }
        auto stmt = std::make_unique<node::Statements>(genAst(nonterm(0), ctx));
        return stmt;
    }

    case Kind::Stm_: {
        if (get(0)->kind == Kind::Epsilon) return nullptr;
        if (auto stmt = genAst(nonterm(2), ctx)) {
            stmt->cast<node::Statements>().pushFront(genAst(nonterm(1), ctx));
            return stmt;
        }
        return std::make_unique<node::Statements>(genAst(nonterm(1), ctx));
    }

    case Kind::Fact:
        switch (get(0)->kind.value()) {
        case Kind::Id: {
            auto lvalue = genAst(nonterm(0), ctx);
            if (auto annot = genAst(nonterm(1), ctx)) {
                lvalue->cast<node::Set>().setSuperset(std::move(annot));
            }
            return std::make_unique<node::Fact>(std::move(lvalue), genAst(nonterm(2), ctx));
        }
        case Kind::Module: {
            auto module = std::make_unique<node::Module>(
                genAst(nonterm(3), ctx), get(0)->combine(*get(4)), std::string(nonterm(1).view)
            );
            return module;
        }
        default: return nullptr;
        }

    case Kind::Fac_: return get(0)->kind == Kind::Epsilon ? nullptr : genAst(nonterm(1), ctx);

    case Kind::Exp1:
        switch (get(0)->kind.value()) {
        case Kind::RtoL: {
            auto lexp = genAst(nonterm(0), ctx);
            lexp->cast<node::Unary>().setParam(genAst(nonterm(1), ctx));
            return lexp;
        }
        case Kind::Id:
            if (auto expr = genAst(nonterm(1), ctx)) {
                expr->cast<node::Expression>().setExtract(genAst(nonterm(0), ctx));
                return expr;
            }
            return genAst(nonterm(0), ctx);
        case Kind::OpenParenthesis: return genAst(nonterm(1), ctx);
        case Kind::OpenCurlyBracket: return std::make_unique<node::Void>(Node(get(0)->view).combine(*get(1)));
        default: return genAst(nonterm(0), ctx);
        }

    case Kind::Exp2:
    case Kind::Exp3:
    case Kind::Exp4:
    case Kind::Exp5:
    case Kind::Exp6: {
        auto l = genAst(nonterm(0), ctx);
        auto r = genAst(nonterm(1), ctx);
        if (r) {
            auto &binary = r->cast<node::Binary>();
            binary.setLhs(std::move(l));
            return r;
        }
        return l;
    }

    case Kind::Exp2_:
    case Kind::Exp3_:
    case Kind::Exp4_:
    case Kind::Exp5_:
    case Kind::Exp6_: {
        if (get(0)->kind == Kind::Epsilon) return nullptr;
        auto left = genAst(nonterm(0), ctx);
        auto expr = genAst(nonterm(1), ctx);
        auto right = genAst(nonterm(2), ctx);
        if (!right) {
            left->cast<node::Binary>().setRhs(std::move(expr));
            return left;
        }
        left->cast<node::Binary>().setRhs(std::move(expr));
        right->cast<node::Binary>().setLhs(std::move(left));
        return right;
    }

    case Kind::RtoL: return std::make_unique<node::Unary>(*get(0));

    case Kind::Op2:
    case Kind::Op3:
    case Kind::Op4:
    case Kind::Op5:
    case Kind::Op6: return std::make_unique<node::Binary>(*get(0));

    case Kind::Extract:
        switch (get(0)->kind.value()) {
        case Kind::Epsilon: return nullptr;
        case Kind::SingleColon:
            return std::make_unique<node::Expression>(genAst(nonterm(5), ctx), get(1)->view, genAst(nonterm(3), ctx));
        }

    case Kind::Annot: return get(0)->kind == Kind::Epsilon ? nullptr : genAst(nonterm(1), ctx);

    case Kind::Id: return std::make_unique<node::Set>(self.view);

    case Kind::Bool: return std::make_unique<node::Bool>(self);

    case Kind::Number: return std::make_unique<node::Int>(self);

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
            // std::make_shared<StrFixed>("++", Kind::DoublePlus),
            std::make_shared<CharFixed>('-', Kind::SingleMinus),
            // std::make_shared<StrFixed>("--", Kind::DoubleMinus),
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
    auto str = buffer.str() + ' ';
    parser.setRawText(str);

    Lexer lexer(str);
    auto next = lexer.next(bnf);
    int syntaxErr{};
    for (; !lexer.empty(); next = lexer.next(bnf)) {
        Diagn(), "== ", next.kind.name(), " ", next.view, " ==\n";
        syntaxErr += parser.parse(next);
    }

    if (next.kind != Kind::Eof && next.view.length() > 1) {
        Quiet<style::red>(), "lex error: rest is '", next.view;
        return;
    }

    if (syntaxErr >= 1) {
        Quiet<style::red>(), syntaxErr, " syntax error", syntaxErr == 1 ? "\n" : "s\n";
        return;
    }

    Context ctx(str);
    auto ast = genAst(parser.getCst()._Get_container().front()->cast<node::Nonterm>(), ctx);
    // ast->dump();
    // std::cout << std::flush;

    auto modulename = filename2module(filename);
    auto root = node::Module(std::move(ast), {str}, modulename);
    ctx.params.push(root);
    auto expr = node::Expression(std::make_unique<node::Set>("main"), modulename);
    auto solved = expr.solve(ctx);

    // compile to llvm ir here

    if (solved.ok()) {
        Quiet<style::blue>(), "> ", solved.show(), "\n";
    } else {
        Quiet<style::blue>(), "> unsolved module '", root.getName(), "'\n";
    }
}

#include <chrono>

int main(int argc, char *argv[]) {
    run(((void)argc, argv[1])); // NOLINT
    std::vector<std::string> emoji = {"🥳", "😘", "😗", "😙", "😚"};
    auto idx = std::chrono::system_clock::now().time_since_epoch().count() % emoji.size();
    std::cout << emoji[idx] << '\n';
}