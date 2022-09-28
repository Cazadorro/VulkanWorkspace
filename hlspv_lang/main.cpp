//
// Created by Shae Bolt on 9/17/2022.
//

#include "scan.h"
//#include "ebnfgenerator.h"
#include "ebnf_scan.h"
#include "ebnf_parse.h"
#include "ebnf_semantics.h"
#include "ebnf_ast_printer.h"
#include "astprinter.h"
#include <iostream>
#include <fstream>
#include <string>


//TODO lexeme could be it's own class? ie like a string view, so you could concat all lexemes together in parsing ?
int main(){

    std::ifstream in("C:/Users/Shae Bolt/Documents/GitRepositories/VulkanWorkspace/hlspv_lang/test_parse.txt");
    std::string contents((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
    auto scan_result = hlspv::scan(contents);
    for(const auto& error: scan_result.errors){
        std::cout << error.create_info_string() << std::endl;
    }
    for(const auto& token: scan_result.tokens){
        std::cout << token.lexeme() << std::endl;
    }
    auto minus_token =hlspv::Token{hlspv::TokenType::Minus,{"-",0,0},{}, 1};
    auto num_0_token = hlspv::Token{hlspv::TokenType::DecimalFloatingPointLiteral, {"123.0",0,0}, 123.0, 1};
    auto star_token = hlspv::Token{hlspv::TokenType::Star,{"*",0,0},{}, 1};
    auto num_1_token = hlspv::Token{hlspv::TokenType::DecimalFloatingPointLiteral, {"45.67",0,0}, 45.67, 1};
//    hlspv::Expression expression2 =  hlspv::Expression{hlspv::NumericLiteral{num_0_token}};
    hlspv::Expression expression{hlspv::BinaryExpression{
        hlspv::Expression{hlspv::UnaryExpression{hlspv::UnaryOperator{
                hlspv::Token{hlspv::TokenType::Minus,{"-",0,0}, {}, 1}},
                               hlspv::PrimaryExpression{hlspv::NumericLiteral{num_0_token}}
        }},
        hlspv::BinaryOperator{star_token},
        hlspv::Expression{
                hlspv::GroupingExpression{hlspv::NumericLiteral{num_1_token}}
        }
    }};

    std::cout << hlspv::AstPrinter().print(expression) << std::endl;

    auto ebnf_scan_result = hlspv::ebnf::scan("hello_world = 'abc' | 'abcd' | nothing, turtle;\n"
                                              "turtle = hello_world;\n"
                                              "nothing = turtle;\n"
                                              "temp = hello;\n"
                                              "temp2 = hello2;");
    for(const auto& error: ebnf_scan_result.errors){
        std::cout << error.create_info_string() << std::endl;
    }
    for(const auto& token: ebnf_scan_result.tokens){
        std::cout << token.lexeme() << std::endl;
    }
    auto ebnf_parse_result = hlspv::ebnf::parse(ebnf_scan_result.tokens);
    if(ebnf_parse_result.has_value()){
        std::cout << hlspv::ebnf::AstPrinter().print(ebnf_parse_result.value()) << std::endl;
        auto symbol_table = hlspv::ebnf::calc_symbol_table(ebnf_parse_result.value());
        auto symbol_errors = hlspv::ebnf::analyze_symbols(ebnf_scan_result.tokens, symbol_table);
        for(const auto& errors : symbol_errors){
            std::cout << errors.message << std::endl;
        }
    }else{
        std::cout << ebnf_parse_result.error().create_info_string() << std::endl;
    }

//    hlspv::ebnf::parse();
    return 0;
}
