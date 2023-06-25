//
// Created by Bolt on 5/7/2023.
//

#ifndef VULKANWORKSPACE_EBNF_CODE_GEN_H
#define VULKANWORKSPACE_EBNF_CODE_GEN_H

#include "ebnf_expr.h"
#include "ebnf_token.h"
#include <uul/string.h>
#include <fmt/core.h>
#include <string>
#include <variant>
#include <unordered_set>
#include <unordered_map>
#include <optional>

#include <tl/expected.hpp>
#include <parse_state.h>

namespace hlspv::ebnf {

////tl::expected<ExpressionStruct*, ParseError<{2}> >
////consume<{1}>(ParseState<{2}> &parse_state, MemoizationState &memoization_state)
//
//
//    //toplel
//    expression_struct<expression_type>{
//        expression_type expression_type;
//        std::vector<std::variant<ExpressionStruct*,ExprToken>> expressions;
//    }
//    template<expression_type_enum, token_types_enum>
//    some_function that returns expression_struct<expression_type> toplevel(parse_state, memoizeation_state){
//        fill global_expression_parse_function_dict
//        entry point expression
//        return global_expression_parse_function_dict[entry_point];
//    }
//
//    //Turn into struct of expressions?
//    template<typename ExpressionStruct_T, typename TokenType_T>
//    tl::expected<ExpressionStruct_T*, ParseError<TokenType_T> > help_non_symbol_parse(
//            ParseState<TokenType_T> &parse_state,
//            MemoizationState &memoization_state,
//            const NonSymbol& value){
//
//        if(nonSymbol.symbol == terminal){
//            match against symbol
//        }else{ //if non symbol is expression
//            if not found in global expression thing, return error.
//            return global_expression_parse_function_dict[nonSymbol.lexeme()](parse_state, memoization_state);
//        }
//        //memooize
//        //return memoize;
//
//        auto expr = std::visit([=, this](auto &field) -> CodeGenerated {
//            return this->visit(*field);
//        }, expr.expr);
//        if(expr.has_value()){
//            return expr
//        }
//        //expression could be NonTerminal or another expression
//        //if Expression_T is function?
//        //if Expression_T is terminal?
//        //if Expression_T is token?
//    }
//
//    template<typename ExpressionStruct_T, typename TokenType_T>
//    tl::expected<ExpressionStruct_T*, ParseError<TokenType_T> > help_parse_alternation(
//            ParseState<TokenType_T> &parse_state,
//            MemoizationState &memoization_state,
//            const std::vector<Expressions>& values){
//
//        //memooize
//        //return memoize;
//
//        auto expr = std::visit([=, this](auto &field) -> CodeGenerated {
//            return this->visit(*field);
//        }, expr.expr);
//        if(expr.has_value()){
//            return expr
//        }
//        //expression could be NonTerminal or another expression
//        //if Expression_T is function?
//        //if Expression_T is terminal?
//        //if Expression_T is token?
//    }
    struct TerminalOverwriteError {
        EbnfToken rule_name;

        TerminalOverwriteError(EbnfToken rule_name) : rule_name(std::move(rule_name)) {};

        [[nodiscard]]
        std::string error_string() const {
            return fmt::format("Expression {} at line {} overwrites provided user code token {}\n",
                               rule_name.lexeme(), rule_name.line_number(), rule_name.lexeme());
        }
    };

    struct RuleOverwriteError {
        EbnfToken rule_name;
        EbnfToken overwritten_rule_name;

        RuleOverwriteError(EbnfToken rule_name,
                           EbnfToken overwritten_rule_name) : rule_name(std::move(rule_name)),
                                                              overwritten_rule_name(overwritten_rule_name) {};

        [[nodiscard]]
        std::string error_string() const {
            return fmt::format("Rule with name {} at line {} overwrites previously defined rule {} at line {}\n",
                               rule_name.lexeme(), rule_name.line_number(), overwritten_rule_name.lexeme(),
                               overwritten_rule_name.line_number());
        }
    };

    using CodeErrors = std::variant<TerminalOverwriteError, RuleOverwriteError>;

    struct CodeGenerated {
        std::vector<CodeErrors> errors;
        std::string generated_code;
        explicit CodeGenerated(std::vector<CodeErrors> errors) : errors(std::move(errors)){};
        explicit CodeGenerated( std::string_view generated_code) : generated_code(generated_code){};
        void append(const CodeGenerated &code_generated) {
            errors.insert(errors.end(), code_generated.errors.begin(), code_generated.errors.end());
            generated_code += code_generated.generated_code;
        }
        [[nodiscard]]
        bool has_error() const{
            return !errors.empty();
        }
    };

    class AstParseCodeGenerator {
    public:
        std::string m_indent = "    ";
        std::string m_token_name = "NOTSETYET";
        std::string m_namespace = "NOTSETYET";
        std::unordered_map<std::string, std::optional<EbnfToken>> m_symbol_table;

        //userCode = tokens,
        CodeGenerated generate(const Expression &expr, const std::vector<std::string> &user_code_token_type_strings) {
            for (const auto &token_type_string: user_code_token_type_strings) {
                m_symbol_table.insert({token_type_string, {}});
            } //fill with terminals we know exist at first.
            return expr.accept(*this);
        }

        CodeGenerated visit(const Expression &expr) {
            //expr is only a list of rules, so we know top level is just rules.
            //find the non-terminal for each rule, add so that we know what each actually is
            //check if overrides the name of a user terminal
            //TODO finish generated code, preamble, includes and the like.
            std::string includes = "#include <vector>\n\n";
            std::string expression_struct_def = fmt::format("{0}struct ExpressionStruct{{\n"
                                                            "{0}{0}ExpressionType type;\n"
                                                            "{0}{0}std::vector<ExpressionStruct> expressions;\n"
                                                            "{0}}};\n\n", m_indent);
            std::string forward_parser_function_defs;
            std::string expression_type_enum_class_code;
            expression_type_enum_class_code += fmt::format("{0}enum class ExpressionType{{\n", m_indent);
            for (const auto &rule: expr.exprs) {
                if (m_symbol_table.contains(rule->non_terminal.lexeme())) {
                    auto token = m_symbol_table[rule->non_terminal.lexeme()];
                    if (token.has_value()) {
                        return CodeGenerated{{RuleOverwriteError(rule->non_terminal, token.value())}};
                    } else {
                        return CodeGenerated{{TerminalOverwriteError(rule->non_terminal)}};
                    }
                } else {
                    expression_type_enum_class_code += fmt::format("{0}{0}{1},\n", m_indent,rule->non_terminal.lexeme());
                    m_symbol_table.insert({rule->non_terminal.lexeme(), rule->non_terminal});
                }

                forward_parser_function_defs +=
                        fmt::format("{0}template<>\n"
                                    "{0}tl::expected<ExpressionStruct*, ParseError<{1}> >\n"
                                    "{0}consume<{2}>(ParseState<{1}> &parse_state, MemoizationState &memoization_state);\n", m_indent, m_token_name, rule->non_terminal.lexeme());
            }
//
//            struct Expression{
//                ExpressionType type;
//                std::vector<Expressions> expressions;
//            };

            // template<typename T>
            //    tl::expected<T*, ParseError<EbnfToken> >
            //    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            //            const std::vector<ExprType> &exclude_list);

            expression_type_enum_class_code += fmt::format("{0}}};\n\n", m_indent);
            auto generated_code = includes + expression_type_enum_class_code + expression_struct_def + forward_parser_function_defs;
            auto code_generated = CodeGenerated(generated_code);
            code_generated.append(parenthesize_list(expr.exprs));
            return code_generated;
        }

        CodeGenerated visit(const RuleDefinitionExpr &expr) {
            std::string indent = "    ";
            std::string top_code =
                            fmt::format("{0}template<>\n"
                                        "{0}tl::expected<ExpressionStruct*, ParseError<{2}> >\n"
                                        "{0}consume<{1}>(ParseState<{2}> &parse_state, MemoizationState &memoization_state){{\n", indent, expr.non_terminal.lexeme(), m_token_name);
            std::string end_code = fmt::format("{0}}}\n", indent);

            std::string body_code = fmt::format("{0}{0}auto current_parse_state = parse_state;\n"
                                    "{0}{0}current_parse_state.reset_start_index();\n"
                                    "{0}{0}if(auto cached_expr = memoization_state.get_cached<{1}>(parse_state.current_token_index); cached_expr.has_value()) {{\n"
                                    "{0}{0}{0}current_parse_state.consume_count_token(cached_expr.value().consume_size);\n"
                                    "{0}{0}{0}parse_state = current_parse_state;\n"
                                    "{0}{0}{0}return cached_expr.value().expr_ptr;\n"
                                    "{0}{0}}}\n", indent, expr.non_terminal.lexeme());


            return parenthesize(expr.right);
        }
        CodeGenerated visit(const RhsExpr &expr) {
            //need to use variant TODO~!!!
            return std::visit([=, this](auto &field) -> CodeGenerated {
                return this->visit(*field);
            }, expr.expr);
        }

        CodeGenerated visit(const AlternationExpr &expr) {

            std::string body_code;

            for(expr)
                //{0}{0}if (auto result = consume<ConcatenationExpr>(current_parse_state, memoization_state); result.has_value()) {{
                for(const auto& sub_expr : expr.exprs){
                    body_code += fmt::format("{0}{0}{{\n", m_indent);
                    //TODO need to add indents to sub code
                    auto sub_expr_generated = sub_expr->accept(*this);
                    if(sub_expr_generated.has_error()){
                        return sub_expr_generated;
                    }
                    body_code += uul::add_to_each_line(sub_expr_generated.generated_code, m_indent);
                    std::string body_code = fmt::format( "{0}{0}if (result_fn(current_parse_state, memoization_state).has_value()) {{\n"
                                                         "{0}{0}{0}parse_state = current_parse_state;\n"
                                                         "{0}{0}{0}return memoization_state.cache_value(std::make_unique<ExpressionStruct>(result.value()), parse_state.current_token_index, parse_state.size());\n"
                                                         "{0}{0}}} else {{\n"
                                                         "{0}{0}{0}if (longest_error.size() < result.error().size()) {{\n"
                                                         "{0}{0}{0}{0}longest_error = result.error();\n"
                                                         "{0}{0}{0}}}\n"
                                                         "{0}{0}}}\n\n", m_indent, expr.non_terminal.lexeme());
                    body_code += fmt::format("{0}{0}}}\n", m_indent);
                }

            return parenthesize_list(expr.exprs);
        }

        CodeGenerated visit(const ConcatenationExpr &expr) {
            return parenthesize_list(expr.exprs);
        }

        CodeGenerated visit(const PreAndPostFixableExpr &expr) {
            return std::visit([=, this](auto &field) -> CodeGenerated {
                return this->visit(*field);
            }, expr.expr);
        }

        CodeGenerated visit(const OptionalExpr &expr) {
            return parenthesize("Optional", *expr.expr) + "?";
        }

        CodeGenerated visit(const AtLeastOneExpr &expr) {
            return parenthesize("Optional", *expr.expr) + "+";
        }

        CodeGenerated visit(const RepeatExpr &expr) {
            return parenthesize("Optional", *expr.expr) + "*";
        }

        CodeGenerated visit(const GroupingExpr &expr) {
            return parenthesize("GroupingExpr", *expr.expr);
        }

        CodeGenerated visit(const NonSymbolExpr &expr) {
            return expr.token.lexeme();
        }

    private:
        template<typename ...Args>
        CodeGenerated parenthesize(Args &&... exprs) {
            CodeGenerated builder;
            ([&] {
                builder.append(exprs.accept(*this));
            }(), ...);
            return builder;
        }

        template<typename T>
        CodeGenerated parenthesize_list(const std::vector<T> &exprs) {
            CodeGenerated builder;
            for (const auto &expr: exprs) {
                builder.append(expr.accept(*this));
            }
            return builder;
        }
    };
}
#endif //VULKANWORKSPACE_EBNF_CODE_GEN_H
