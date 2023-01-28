#include "cfg.h"
#include "lexer.h"
#include "outs.h"

using namespace token;
using namespace node;

int genCFG(Nonterm &self, std::map<Kind, std::vector<std::vector<Kind>>> &ctx, Kind lhs = Kind::Invalid) {
    auto get = [&self](auto idx) { return self.args[self.size - 1 - idx].get(); };
    auto nonterm = [&get](auto idx) -> Nonterm & { return *static_cast<Nonterm *>(get(idx)); };
    switch (self.kind.value()) {
    case Kind::GRoot:
        if (self.size == 1) break;
        genCFG(nonterm(0), ctx);
        genCFG(nonterm(2), ctx);
        break;
    case Kind::GFact:
        lhs = nonterm(0).args.front()->kind;
        ctx.insert({lhs, {std::vector<Kind>{}}});
        genCFG(nonterm(2), ctx, lhs);
        genCFG(nonterm(3), ctx, lhs);
        break;
    case Kind::GTupl:
        if (self.size == 1) break;
        ctx.at(lhs).back().push_back(nonterm(0).args.front()->kind);
        genCFG(nonterm(1), ctx, lhs);
        break;
    case Kind::GTup_:
        if (self.size == 1) break;
        ctx.at(lhs).push_back({});
        genCFG(nonterm(1), ctx, lhs);
        genCFG(nonterm(2), ctx, lhs);
        break;
    default: return 1;
    }
    return 0;
}

std::vector<std::shared_ptr<Base>> const bnfSymbols{
    std::make_shared<CharRegion>('`', '`', Kind::GEpsilon),
    std::make_shared<StrFixed>("::=", Kind::BnfEqual),
    std::make_shared<StrFixed>("<+>", Kind::ChevronsOr),
    std::make_shared<StrFixed>("<$>", Kind::ChevronsEnd),
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
    std::make_shared<StrFixed>("module", Kind::Module),
    std::make_shared<StrFixed>("Root", Kind::Root),
    std::make_shared<StrFixed>("epsilon", Kind::Epsilon),
    std::make_shared<StrFixed>("Stmt", Kind::Stmt),
    std::make_shared<StrFixed>("Stmt'", Kind::Stm_),
    std::make_shared<StrFixed>("Expr", Kind::Expr),
    std::make_shared<StrFixed>("Expr'", Kind::Exp_),
    std::make_shared<StrFixed>("Fact", Kind::Fact),
    std::make_shared<StrFixed>("Fact'", Kind::Fac_),
    std::make_shared<StrFixed>("Unary", Kind::Unary),
    std::make_shared<StrFixed>("Binary", Kind::Binary),
    std::make_shared<StrFixed>("LtoR", Kind::LtoR),
    std::make_shared<StrFixed>("RtoL", Kind::RtoL),
    std::make_shared<StrFixed>("Number", Kind::Number),
    std::make_shared<StrFixed>("Id", Kind::Id),
    std::make_shared<StrFixed>("Atom", Kind::Atom),
    std::make_shared<StrFixed>("IModule", Kind::IModule),
    std::make_shared<StrFixed>("Set", Kind::Set),
    std::make_shared<StrFixed>("Extract", Kind::Extract),
    std::make_shared<StrFixed>("Annot", Kind::Annot),
};

Parser::CFG const cfgcfg{
    {Kind::GRoot, {{Kind::GFact, Kind::ChevronsEnd, Kind::GRoot}, {Kind::GEpsilon}}},
    {Kind::GFact, {{Kind::GId, Kind::BnfEqual, Kind::GTupl, Kind::GTup_}}},
    {Kind::GTupl, {{Kind::GId, Kind::GTupl}, {Kind::GEpsilon}}},
    {Kind::GTup_, {{Kind::ChevronsOr, Kind::GTupl, Kind::GTup_}, {Kind::GEpsilon}}},
 // clang-format off
    {Kind::GId, {
  // clang-format on
         {Kind::SingleEqual},
         {Kind::DoubleEqual},
         {Kind::ExclamationEqual},
         {Kind::SingleColon},
         {Kind::DoubleColon},
         {Kind::OpenParenthesis},
         {Kind::CloseParenthesis},
         {Kind::OpenSquareBracket},
         {Kind::CloseSquareBracket},
         {Kind::OpenCurlyBracket},
         {Kind::CloseCurlyBracket},
         {Kind::LessThan},
         {Kind::GreatThan},
         {Kind::LessThanOrEqual},
         {Kind::GreatThanOrEqual},
         {Kind::SingleAnd},
         {Kind::DoubleAnd},
         {Kind::SingleOr},
         {Kind::DoubleOr},
         {Kind::SinglePlus},
         {Kind::DoublePlus},
         {Kind::SingleMinus},
         {Kind::DoubleMinus},
         {Kind::SingleAsterisk},
         {Kind::DoubleAsterisk},
         {Kind::SingleSlash},
         {Kind::DoubleSlash},
         {Kind::Percent},
         {Kind::Dot},
         {Kind::Comma},
         {Kind::Semicolon},
         {Kind::Exclamation},
         {Kind::Question},
         {Kind::Hash},
         {Kind::Spaceship},
         {Kind::Module},
         {Kind::Root},
         {Kind::Epsilon},
         {Kind::Stmt},
         {Kind::Stm_},
         {Kind::Expr},
         {Kind::Exp_},
         {Kind::Fact},
         {Kind::Fac_},
         {Kind::Unary},
         {Kind::Binary},
         {Kind::LtoR},
         {Kind::RtoL},
         {Kind::Number},
         {Kind::Id},
         {Kind::Atom},
         {Kind::IModule},
         {Kind::Set},
         {Kind::Extract},
         {Kind::Annot},
}}};

void LangCFG::test() {
    static std::string question = "Root  ::= epsilon <+> Stmt <$>\n"
                                  "Stmt  ::= Expr ; Stmt' <$>\n"
                                  "Stmt' ::= epsilon <+> Expr ; Stmt' <+> ; Stmt' <$>\n"
                                  "Expr  ::= Id Expr' <$>\n"
                                  "Expr' ::= epsilon <+> + Id Expr' <$>\n";

    static Parser::CFG sampleAnswer{
        {Kind::Root,                                                             {{Kind::Epsilon}, {Kind::Stmt}}},
        {Kind::Stmt,                                                 {{Kind::Expr, Kind::Semicolon, Kind::Stm_}}},
        {Kind::Stm_, {{Kind::Epsilon}, {Kind::Expr, Kind::Semicolon, Kind::Stm_}, {Kind::Semicolon, Kind::Stm_}}},
        {Kind::Expr,                                                                    {{Kind::Id, Kind::Exp_}}},
        {Kind::Exp_,                                 {{Kind::Epsilon}, {Kind::SinglePlus, Kind::Id, Kind::Exp_}}},
    };

    Parser parser;
    parser.setupGramma(cfgcfg, Kind::GRoot, Kind::GEpsilon);

    Lexer lexer(question);
    for (auto next = lexer.next(bnfSymbols); !lexer.empty(); next = lexer.next(bnfSymbols)) {
        Diagn(), "== ", next.kind.name(), " ", next.view, " ==\n";
        parser.parse(next);
    }

    Parser::CFG answer;
    if (parser.getCST().empty()) {
        Quiet(), "cst empty\n", exit(1);
    }
    genCFG(*static_cast<Nonterm *>(parser.getCST()._Get_container().front().get()), answer);

    Quiet(), "test ", answer == sampleAnswer ? "ok" : "failed", '\n';
}

Parser::CFG LangCFG::getCFG() {
    Parser parser;
    parser.setupGramma(cfgcfg, Kind::GRoot, Kind::GEpsilon);

    std::ifstream file("simpl.bnf");
    std::stringstream buffer;
    buffer << file.rdbuf();
    auto str = buffer.str();

    Lexer lexer(str);
    auto next = lexer.next(bnfSymbols);
    int err{};
    for (; !lexer.empty(); next = lexer.next(bnfSymbols)) {
        Diagn(), "== ", next.kind.name(), " ", next.view, " ==\n";
        err += parser.parse(next);
    }

    Parser::CFG langCFG;

    if (parser.getCST().empty()) {
        Quiet<style::red>(), "cst empty\n";
        exit(1);
    }

    genCFG(*static_cast<Nonterm *>(parser.getCST()._Get_container().front().get()), langCFG);

    Quiet(), style::red;
    if (next.view.length() > 1) {
        Quiet<"lex error: rest is\n">();
        exit(1);
    }
    if (err == 1) {
        Quiet(), "1 error\n", exit(1);
    } else if (err > 1) {
        Quiet(), err, " errors\n", exit(1);
    }
    Quiet(), style::reset;
    return langCFG;
}
