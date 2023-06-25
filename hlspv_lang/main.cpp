//
// Created by Shae Bolt on 9/17/2022.
//

#include "code_generation.h"
#include "lang_scan.h"
//#include "ebnfgenerator.h"
#include "ebnf_scan.h"
#include "ebnf_parse.h"
#include "ebnf_parser_gen.h"
#include "ebnf_semantics.h"
#include "ebnf_ast_printer.h"

#include "lang_expr_type.h"
#include "lang_token_type.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


//struct ConcatenationExpr{
//    static constexpr auto type = ExprType::ConcatenationExpr;
//    std::vector<RhsExpr> exprs;
//
//    template<typename Visitor>
//    auto accept(Visitor &visitor) {
//        return visitor.visit(*this);
//    }
//
//    template<typename Visitor>
//    auto accept(Visitor &visitor) const {
//        return visitor.visit(*this);
//    }
//
//    ConcatenationExpr(std::vector<RhsExpr> expr);
//
//    ConcatenationExpr(ConcatenationExpr &&) = default;
//
//    ConcatenationExpr &operator=(ConcatenationExpr &&) = default;
//
//    ~ConcatenationExpr() = default;
//};

//TODO lexeme could be it's own class? ie like a string view, so you could concat all lexemes together in parsing ?
int main() {

    hlspv::codegen::ClassBuilder class_builder;
    hlspv::codegen::MemberVariableBuilder exprs_builder;
    hlspv::codegen::MemberVariableBuilder type_builder;
    hlspv::codegen::FunctionBuilder accept_builder;
//static constexpr auto type = ExprType::ConcatenationExpr;
    hlspv::codegen::FunctionBuilder constructor_builder;
    if(false){
        using namespace hlspv::codegen;
        auto accept_prototype = FunctionPrototypeBuilder()
                .set_template_types({TemplateParameter::from_typed("Visitor")})
                .set_return_type("auto")
                .set_name("accept")
                .set_parameters({TypeNameParameter{"Visitor&", "visitor"}});
        auto accept_definition = std::string("return visitor.visit(*this);");

        auto accept_function = FunctionBuilder()
                .set_prototype(accept_prototype)
                .set_definition(accept_definition);

        exprs_builder
                .set_type("std::vector<RhsExpr>")
                .set_name("exprs");

        class_builder
                .set_struct()
                .set_name("ConcatenationExpr")
                .add_public_variables(std::vector{
                        MemberVariableBuilder()
                                .set_static()
                                .set_constexpr()
                                .set_type("auto")
                                .set_name("type")
                                .set_assignment(
                                        "ExprType::ConcatenationExpr"),
                        exprs_builder
                })
                .add_public_functions({
                                              accept_function,
                                              accept_function.const_copy(),
                                              FunctionBuilder().set_prototype(
                                                      FunctionPrototypeBuilder()
                                                              .set_name(
                                                                      "ConcatenationExpr")
                                                              .set_parameters(
                                                                      {TypeNameParameter{
                                                                              "std::vector<RhsExpr>",
                                                                              "expr"}})),
                                              FunctionBuilder().set_prototype(
                                                      FunctionPrototypeBuilder()
                                                              .set_name(
                                                                      "ConcatenationExpr")
                                                              .set_parameters(
                                                                      {TypeNameParameter{"ConcatenationExpr&&"}})
                                                              .set_default()),
                                              FunctionBuilder().set_prototype(
                                                      FunctionPrototypeBuilder()
                                                      .set_return_type("ConcatenationExpr& ")
                                                              .set_name(
                                                                      "operator=")
                                                              .set_parameters(
                                                                      {TypeNameParameter{"ConcatenationExpr&&"}})
                                                              .set_default()),
                                              FunctionBuilder().set_prototype(
                                                      FunctionPrototypeBuilder()
                                                              .set_name("~ConcatenationExpr")
                                                              .set_default())



                                      });

        std::cout << class_builder.build_header() << std::endl;
    }


    std::ifstream in(
            "C:/Users/Bolt/Documents/GitRepositories/VulkanWorkspace/hlspv_lang/test_parse.txt");
    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    auto scan_result = hlspv::lang::scan(contents);
    for (const auto &error: scan_result.errors) {
        std::cout << error.create_info_string() << std::endl;
    }
    for (const auto &token: scan_result.tokens) {
        std::cout << token.lexeme() << std::endl;
    }
    auto minus_token = hlspv::lang::LangToken{hlspv::lang::TokenType::Minus, {"-", 0, 0}, {},
                                    1};
    auto num_0_token = hlspv::lang::LangToken
            {hlspv::lang::TokenType::DecimalFloatingPointLiteral, {"123.0", 0, 0},
            123.0, 1};
    auto star_token = hlspv::lang::LangToken{hlspv::lang::TokenType::Star, {"*", 0, 0}, {}, 1};
    auto num_1_token = hlspv::lang::LangToken{
                hlspv::lang::TokenType::DecimalFloatingPointLiteral, {"45.67", 0, 0},
            45.67, 1};

//    auto ebnf_scan_result = hlspv::ebnf::scan(
//            "hello_world = 'abc' | 'abcd' | nothing, turtle;\n"
//            "turtle = ((hello_world)*)+;\n"
//            "turtle2 = !((hello_world));\n"
//            "turtle3 = (&(hello_world))[10:200];\n"
//            "turtle4 = (&(hello_world))[:];\n"
//            "turtle4 = (&(hello_world))[10:];\n"
//            "turtle4 = (&(hello_world))[10];\n"
//            "turtle4 = (&(hello_world))[:10];\n"
//            "nothing = turtle;\n"
//            "temp = hello_shouldnt_find;\n"
//            "temp2 = hello_shouldnt_find2;");
    {
        std::cout << "STUUUUF\n\n\n" << std::endl;
        std::ifstream in(
                "C:/Users/Bolt/Documents/GitRepositories/VulkanWorkspace/hlspv_lang/lang_peg.txt");
        std::string contents((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());
        auto ebnf_scan_result = hlspv::ebnf::scan(contents);
        for (const auto &error: ebnf_scan_result.errors) {
            std::cout << error.create_info_string() << std::endl;
        }
        for (const auto &token: ebnf_scan_result.tokens) {
            std::cout << token.lexeme() << std::endl;
        }
        auto [ebnf_parse_result, parse_memory] = hlspv::ebnf::parse(ebnf_scan_result.tokens);
        if(true) {
            if (ebnf_parse_result.has_value()) {
                auto ebnf_parse_result_ptr = ebnf_parse_result.value();
                std::cout << hlspv::ebnf::AstPrinter().print(*ebnf_parse_result_ptr)
                          << std::endl;
                auto symbol_table = hlspv::ebnf::calc_symbol_table(
                        *ebnf_parse_result.value());
                std::cout << std::endl;
                std::cout << std::endl;
                for (auto &entry: symbol_table) {
                    std::cout << entry << std::endl;
                }
                std::cout << std::endl;
                std::cout << std::endl;
                auto symbol_errors = hlspv::ebnf::analyze_symbols(
                        ebnf_scan_result.tokens, symbol_table);
                for (const auto &errors: symbol_errors) {
                    std::cout << errors.message << std::endl;
                }
            } else {
                std::cout << ebnf_parse_result.error().create_info_string()
                          << std::endl;
            }
        }
        std::cout << "STUUUUF\n\n\n" << std::endl;
        hlspv::ebnf::ParserGenerator<hlspv::lang::ExpressionType, hlspv::lang::LangToken> parser_gen;

        auto parse_gen_result = parser_gen.generate(*(ebnf_parse_result.value()), hlspv::lang::ExpressionType::ModuleExpression );



    }
    auto ebnf_scan_result = hlspv::ebnf::scan(
//            "hello_world = 'abc' | 'abcd' | nothing, turtle;\n"
            "turtle = ((hello_world)*)+;"
   );
    for (const auto &error: ebnf_scan_result.errors) {
        std::cout << error.create_info_string() << std::endl;
    }
    for (const auto &token: ebnf_scan_result.tokens) {
        std::cout << token.lexeme() << std::endl;
    }
    auto [ebnf_parse_result, parse_memory] = hlspv::ebnf::parse(ebnf_scan_result.tokens);

    if (ebnf_parse_result.has_value()) {
        auto ebnf_parse_result_ptr = ebnf_parse_result.value();
        std::cout << hlspv::ebnf::AstPrinter().print(*ebnf_parse_result_ptr)
                  << std::endl;
        auto symbol_table = hlspv::ebnf::calc_symbol_table(
                *ebnf_parse_result.value());
        std::cout << std::endl;
        std::cout << std::endl;
        for(auto& entry : symbol_table){
            std::cout << entry << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
        auto symbol_errors = hlspv::ebnf::analyze_symbols(
                ebnf_scan_result.tokens, symbol_table);
        for (const auto &errors: symbol_errors) {
            std::cout << errors.message << std::endl;
        }
    } else {
        std::cout << ebnf_parse_result.error().create_info_string()
                  << std::endl;
    }

//    hlspv::ebnf::parse();
    return 0;
}
