//
// Created by Bolt on 5/30/2023.
//

#ifndef VULKANWORKSPACE_EBNF_PARSER_GEN_H
#define VULKANWORKSPACE_EBNF_PARSER_GEN_H

#include "ebnf_expr.h"
#include "ebnf_token.h"
#include "parse_state.h"
#include "memoizationstate.h"
#include <uul/string.h>
#include <uul/concepts.h>
#include <uul/assert.h>
#include <uul/enumtraits.h>
#include <fmt/core.h>
#include <string>
#include <variant>
#include <unordered_set>
#include <unordered_map>
#include <optional>

#include <tl/expected.hpp>
#include <parse_state.h>

namespace hlspv::ebnf {
//todo fix things about memoization parse state start

//expression could be NonTerminal or another expression
//if Expression_T is function?
//if Expression_T is terminal?
//if Expression_T is token?

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

    struct UnknownRuleError {
        EbnfToken rule_name;
        explicit UnknownRuleError(EbnfToken rule_name) : rule_name(std::move(rule_name)){};

        [[nodiscard]]
        std::string error_string() const {
            return fmt::format("Undefined rule {} at line {}\n",
                               rule_name.lexeme(), rule_name.line_number());
        }
    };

    struct UnknownTokenError {
        EbnfToken rule_name;
        explicit UnknownTokenError(EbnfToken rule_name) : rule_name(std::move(rule_name)){};

        [[nodiscard]]
        std::string error_string() const {
            return fmt::format("Undefined token {} at line {}\n",
                               rule_name.lexeme(), rule_name.line_number());
        }
    };



    using CodeErrors = std::variant<TerminalOverwriteError, RuleOverwriteError, UnknownRuleError, UnknownTokenError>;

    struct ParserGeneratorErrors {
        std::vector<CodeErrors> errors;

        [[nodiscard]]
        bool has_error() const {
            return !errors.empty();
        }
    };

    template<uul::Enumeration ExpressionType_T, uul::Enumeration TokenType_T>
    struct ExpressionStruct {
        using ExpressionOrToken = std::variant<ExpressionStruct *, TokenType_T>;
        ExpressionType_T type;
        std::vector<ExpressionOrToken> exprs;
        ~ExpressionStruct(){};
    };

    template<uul::Enumeration ExpressionType_T, typename Token_T>
    class ParserGenerator {
    public:
        using TokenType_T = decltype(((Token_T *) nullptr)->type());
        //TODO need to add memoization state?  ParserState?
        //TODO our memoization state doesn't work the same as before, it's it's own thing which doesn't need variants?
        using ExpressionOrToken = std::variant<ExpressionStruct<ExpressionType_T, TokenType_T> *, TokenType_T>;

        using ParsingMemoizationState = MemoizationState<ExpressionType_T, ExpressionStruct<ExpressionType_T, TokenType_T>*>;
        using ExpressionReturn = tl::expected<ExpressionStruct<ExpressionType_T, TokenType_T> *, ParseError<Token_T>>;
        using ExpressionFunction = std::function<ExpressionReturn(ParseState<Token_T> &parse_state, ParsingMemoizationState &memoization_state)>;
        using SubExpressionReturn = tl::expected<std::vector<ExpressionOrToken>, ParseError<Token_T>>;
        using SubExpressionFunction = std::function<SubExpressionReturn(ParseState<Token_T> &parse_state, ParsingMemoizationState &memoization_state)>;
        std::unordered_map<std::string, ExpressionType_T> m_symbol_table;
        std::unordered_map<std::string, TokenType_T> m_token_table;
        std::unordered_map<std::string, ExpressionFunction> m_expression_parser_table;
        std::unique_ptr<std::vector<ExpressionFunction>> m_finished_functions;

        struct ExpressionParser{
            ExpressionFunction function;
            std::unique_ptr<std::vector<ExpressionFunction>> function_list;
        };
        //userCode = tokens,
        tl::expected<ExpressionParser, std::vector<CodeErrors>>
        generate(const Expression &expr, ExpressionType_T start_expression) {
            using std::to_string;
            using uul::from_string;
            m_finished_functions = std::make_unique<std::vector<ExpressionFunction>>(std::vector<ExpressionFunction>{});
            for (const auto value: uul::enum_range(uul::enum_min<TokenType_T>(), uul::enum_max<TokenType_T>())) {
                m_token_table.insert(std::pair{to_string(value), value});
            }
            auto errors = expr.accept(*this);
            if (!errors.has_value()) {
                tl::make_unexpected(errors);
            }
            m_finished_functions->resize(uul::underlying_enum_max<ExpressionType_T>());
            for(const auto [key, value] : m_expression_parser_table){
                auto index = uul::to_underlying(from_string<ExpressionType_T>(key));
                (*m_finished_functions)[index] = value;
            }
            return ExpressionParser{m_expression_parser_table[to_string(start_expression)], std::move(m_finished_functions)};
        }

        tl::expected<tl::monostate, std::vector<CodeErrors>> visit(const Expression &expr) {
            using std::to_string;
            using uul::from_string;
            //expr is only a list of rules, so we know top level is just rules.
            //find the non-terminal for each rule, add so that we know what each actually is
            //check if overrides the name of a user terminal
            for (const auto &rule: expr.exprs) {
                if (m_symbol_table.contains(rule->non_terminal.lexeme())) {
                    auto token = m_symbol_table[rule->non_terminal.lexeme()];
                    return tl::make_unexpected(std::vector<CodeErrors>{RuleOverwriteError(rule->non_terminal, token)});
                } else if (m_token_table.contains(rule->non_terminal.lexeme())) {
                    auto token = m_token_table[rule->non_terminal.lexeme()];
                    return tl::make_unexpected(std::vector<CodeErrors>{TerminalOverwriteError(rule->non_terminal)});
                } else {
                    m_symbol_table.insert({rule->non_terminal.lexeme(),rule->non_terminal});
                }
            }

            std::vector<CodeErrors> errors;
            for (const auto &rule: expr.exprs) {
                if (auto result = rule->accept(*this); !result.has_value()) {
                    errors.push_back(result.error());
                }
            }
            if (errors.empty()) {
                return tl::monostate();
            } else {
                return tl::make_unexpected(errors);
            }
        }

        tl::expected<tl::monostate, std::vector<CodeErrors>> visit(const RuleDefinitionExpr &expr) {
            using std::to_string;
            using uul::from_string;
            auto result = expr.right->accept(*this);
            if (result.has_value()) {
                m_expression_parser_table[expr.non_terminal.lexeme()] =
                        [sub_expression_function = result.value(),
                                expression_enum_value = from_string<ExpressionType_T>(expr.non_terminal.lexeme())](
                                ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> ExpressionReturn{
                            auto current_parse_state = parse_state;
                            current_parse_state.reset_start_index();
                            if (auto cached_expr = memoization_state.get_cached(
                                        parse_state.current_token_index); cached_expr.has_value()) {
                                current_parse_state.consume_count_token(cached_expr.value().consume_size);
                                parse_state = current_parse_state;
                                return cached_expr.value().expr_ptr;
                            }
                            auto result = sub_expression_function(current_parse_state, memoization_state);
                            if (result.has_value()) {
//                                parse_state.start_token_index

                                parse_state.set_current_token_index(current_parse_state);
                                return cache_value<ExpressionStruct<ExpressionType_T, TokenType_T>>(memoization_state,
                                                   std::make_unique<ExpressionStruct<ExpressionType_T, TokenType_T>>(
                                                           expression_enum_value, sub_expression_function(parse_state, memoization_state)),
                                                           parse_state.start_token_index,
                                                           parse_state.consume_size()
                                                   );
                            } else {
                                return tl::make_unexpected(result.error());
                            }
                        };
                return tl::monostate();
            } else {
                return result;
            }
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const RhsExpr &expr) {
            //need to use variant TODO~!!!
            return std::visit([=, this](auto &field) -> tl::expected<ExpressionFunction, std::vector<CodeErrors>> {
                return this->visit(*field);
            }, expr.expr);
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const AlternationExpr &expr) {
            std::vector<ExpressionFunction> functions;
            for (const auto &sub_expr: expr.exprs) {
                auto result = sub_expr->accept(*this);
                if (result.has_value()) {
                    functions.push_back(result.value());
                } else {
                    return result;
                }
            }
            return [functions](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state)-> SubExpressionReturn {
                ParseError<TokenType_T> longest_error{{}, ""};
                auto current_parse_state = parse_state;
                for (const auto &function: functions) {
                    auto temp_parse_state = current_parse_state;
                    if (auto result = function(temp_parse_state, memoization_state); result.has_value()) {
                        parse_state = temp_parse_state;
                        return result;
                    } else {
                        if (longest_error.size() < result.error().size()) {
                            longest_error = result.error();
                        }
                    }
                }
                return tl::make_unexpected(longest_error);
            };
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const ConcatenationExpr &expr) {
            std::vector<ExpressionFunction> functions;
            for (const auto &sub_expr: expr.exprs) {
                auto result = sub_expr->accept(*this);
                if (result.has_value()) {
                    functions.push_back(result.value());
                } else {
                    return result;
                }
            }
            return [functions](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state)-> SubExpressionReturn {
                std::vector<ExpressionOrToken> expressions;
                auto current_parse_state = parse_state;
                for (const auto &function: functions) {
                    auto result = function(current_parse_state, memoization_state);
                    if (!result.has_value()) {
                        return result;
                    } else {
                        expressions.insert(expressions.end(), result.value().begin(), result.value.end());
                    }
                }
                parse_state = current_parse_state;
                return expressions;
            };
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const PreAndPostFixableExpr &expr) {
            return std::visit([=, this](auto &field) -> tl::expected<ExpressionFunction, std::vector<CodeErrors>> {
                return this->visit(*field);
            }, expr.expr);
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const OptionalExpr &expr) {
            ExpressionFunction function;
            if (auto result = expr.expr->accept(*this); result.has_value()) {
                function = result.value();
            } else {
                return result;
            }
            return [function](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> SubExpressionReturn{
                auto current_parse_state = parse_state;
                if(auto result = function(current_parse_state, memoization_state); result.has_value()){
                    parse_state = current_parse_state;
                    return result;
                }else{
                    return std::vector<ExpressionOrToken>{};
                }
            };
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const AtLeastOneExpr &expr) {
            ExpressionFunction function;
            if (auto result = expr.expr->accept(*this); result.has_value()) {
                function = result.value();
            } else {
                return result;
            }
            return [function](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> SubExpressionReturn{
                std::vector<ExpressionOrToken> expressions;
                auto current_parse_state = parse_state;
                auto result = function(current_parse_state, memoization_state);
                if( result.has_value()){
                    expressions.push_back(result);
                }else{
                    return result;
                }
                while(result.has_value()){
                    result = function(current_parse_state, memoization_state);
                    if(result.has_value()){
                        expressions.push_back(result);
                    }
                }
                parse_state = current_parse_state;
                return expressions;
            };
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const RepeatExpr &expr) {
            ExpressionFunction function;
            if (auto result = expr.expr->accept(*this); result.has_value()) {
                function = result.value();
            } else {
                return result;
            }
            return [function](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> SubExpressionReturn{
                std::vector<ExpressionOrToken> expressions;
                auto current_parse_state = parse_state;
                auto result = function(current_parse_state, memoization_state);
                if( result.has_value()){
                    expressions.push_back(result);
                }else{
                    return std::vector<ExpressionOrToken>{};
                }
                while(result.has_value()){
                    result = function(current_parse_state, memoization_state);
                    if(result.has_value()){
                        expressions.push_back(result);
                    }
                }
                parse_state = current_parse_state;
                return expressions;
            };
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const BracketExpr &expr) {
            ExpressionFunction function;
            if (auto result = expr.expr->accept(*this); result.has_value()) {
                function = result.value();
            } else {
                return result;
            }
            return [function, min=expr.min, max=expr.max](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> SubExpressionReturn{
                std::vector<ExpressionOrToken> expressions;
                auto current_parse_state = parse_state;
                std::size_t expr_count = 0;
                ParseError<TokenType_T> last_error;
                bool max_is_infinite = max == 0;
                while(true){
                    if(!(max_is_infinite) && expr_count >= max){
                        break;
                    }
                    auto result = function(current_parse_state, memoization_state);
                    if(result.has_value()){
                        expressions.push_back(result);
                    }else{
                        last_error = result.error();
                        break;
                    }
                    expr_count += 1;
                }

                if(expr_count < min){
                    return tl::make_unexpected(parse_state.create_error(
                            fmt::format("Expected atleast {} tokens, found {}, last error {}", min, expr_count, last_error.create_info_string())));
                }

                parse_state = current_parse_state;
                return expressions;
            };
        }
        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const AndPredicateExpr &expr) {
            ExpressionFunction function;
            if (auto result = expr.expr->accept(*this); result.has_value()) {
                function = result.value();
            } else {
                return result;
            }
            return [function](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> SubExpressionReturn{
                auto current_parse_state = parse_state;
                auto result = function(current_parse_state, memoization_state);
                if(result.has_value()){
                    return std::vector<ExpressionOrToken>{};
                }else{
                    return tl::make_unexpected(parse_state.create_error(
                            fmt::format("Expected Values Ahead, error {}", result.error().create_info_string())));
                }
            };
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const NotPredicateExpr &expr) {
            ExpressionFunction function;
            if (auto result = expr.expr->accept(*this); result.has_value()) {
                function = result.value();
            } else {
                return result;
            }
            return [function](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> SubExpressionReturn{
                auto current_parse_state = parse_state;
                auto result = function(current_parse_state, memoization_state);
                if(result.has_value()){
                    return tl::make_unexpected(parse_state.create_error(
                            fmt::format("Did not expect certain values ahead")));
                }else{
                    return std::vector<ExpressionOrToken>{};
                }
            };
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const GroupingExpr &expr) {
            return  expr.expr->accept(*this);
        }

        tl::expected<SubExpressionFunction, std::vector<CodeErrors>> visit(const NonSymbolExpr &expr) {
            using std::to_string;
            using uul::from_string;
            if(expr.token.type() == EbnfTokenType::NonTerminalIdentifier){
                auto lexeme = expr.token.lexeme();
                if(!m_symbol_table.contains(lexeme)){
                    return tl::make_unexpected(UnknownRuleError{expr.token});
                }
                auto non_terminal_value = from_string<ExpressionType_T>(lexeme);
                auto non_terminal_index = static_cast<std::underlying_type_t<ExpressionType_T>>(non_terminal_value);
                return [function_list_ptr = m_finished_functions.get(), non_terminal_index](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> SubExpressionReturn{
                    auto result = (*function_list_ptr)[non_terminal_index](parse_state, memoization_state);
                    if(result.has_value){
                        parse_state.reset_start_index();
                        return std::vector<ExpressionOrToken>{result};
                    }else{
                        return tl::make_unexpected(result.error());
                    }
                };
            }else if(expr.token.type() == EbnfTokenType::NonTerminalIdentifier){
                auto lexeme = expr.token.lexeme();
                if(!m_token_table.contains(lexeme)){
                    return tl::make_unexpected(UnknownTokenError{expr.token});
                }
                auto expected_token = from_string<TokenType_T>(lexeme);
                return [function_list_ptr = m_finished_functions.get(), expected_token](ParseState<TokenType_T> &parse_state, ParsingMemoizationState &memoization_state) -> SubExpressionReturn{
                    auto current_token = parse_state.current_token();
                    if(current_token == expected_token){
                        parse_state.consume_current_token();
                        parse_state.reset_start_index();
                        return std::vector<ExpressionOrToken>{current_token};
                    }else{
                        using std::to_string;
                        return tl::make_unexpected(parse_state.create_error(
                                fmt::format("Expected token {}, found {}", to_string(expected_token), to_string(current_token))));
                    }
                };
            }else{
                UUL_ASSERT(false);
            }
        }
    private:
    };
}
#endif //VULKANWORKSPACE_EBNF_PARSER_GEN_H
