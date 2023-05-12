//
// Created by Bolt on 5/7/2023.
//

#ifndef VULKANWORKSPACE_EBNF_CODE_GEN_H
#define VULKANWORKSPACE_EBNF_CODE_GEN_H

#include "ebnf_expr.h"
#include "ebnf_token.h"
#include "fmt/core.h"
#include <string>
#include <variant>
#include <unordered_set>
#include <unordered_map>
#include <optional>

namespace hlspv::ebnf {


//    struct TerminalOverwriteError {
//        EbnfToken rule_name;
//
//        TerminalOverwriteError(EbnfToken rule_name) : rule_name(std::move(rule_name)) {};
//
//        [[nodiscard]]
//        std::string error_string() const {
//            return fmt::format("Exprssion {} at line {} overwrites provided user code token {}\n",
//                               rule_name.lexeme(), rule_name.line_number(), rule_name.lexeme());
//        }
//    };
//
//    struct RuleOverwriteError {
//        EbnfToken rule_name;
//        EbnfToken overwritten_rule_name;
//
//        RuleOverwriteError(EbnfToken rule_name,
//                           EbnfToken overwritten_rule_name) : rule_name(std::move(rule_name)),
//        overwritten_rule_name(overwritten_rule_name) {};
//
//        [[nodiscard]]
//        std::string error_string() const {
//            return fmt::format("Rule with name {} at line {} overwrites previously defined rule {} at line {}\n",
//                               rule_name.lexeme(), rule_name.line_number(), overwritten_rule_name.lexeme(), overwritten_rule_name.line_number());
//        }
//    };
//    using CodeErrors = std::variant<TerminalOverwriteError, RuleOverwriteError>;
//    struct CodeGenerated {
//        std::vector<CodeErrors> errors;
//        void append(const CodeGenerated& code_generated){
//            errors.insert(errors.end(), code_generated.errors.begin(), code_generated.errors.end());
//        }
//    };
//
//    class AstParseCodeGenerator {
//    public:
//        std::unordered_map<std::string, std::optional<EbnfToken>> m_symbol_table;
//
//        //userCode = tokens,
//        CodeGenerated generate(const Expression &expr, const std::vector<std::string> &user_code_token_type_strings) {
//            for (const auto &token_type_string: user_code_token_type_strings) {
//                m_symbol_table.insert({token_type_string, {}});
//            } //fill with terminals we know exist at first.
//            return expr.accept(*this);
//        }
//
//        CodeGenerated visit(const Expression &expr) {
//            //expr is only a list of rules, so we know top level is just rules.
//            //find the non-terminal for each rule, add so that we know what each actually is
//            //check if overrides the name of a user terminal
//            for (const auto &rule: expr.exprs) {
//                if (m_symbol_table.contains(rule.non_terminal.lexeme())) {
//                    auto token = m_symbol_table[rule.non_terminal.lexeme()];
//                    if(token.has_value()){
//                        return CodeGenerated{{RuleOverwriteError(rule.non_terminal, token.value())}};
//                    }else{
//                        return CodeGenerated{{TerminalOverwriteError(rule.non_terminal)}};
//                    }
//                }else{
//                    m_symbol_table.insert({rule.non_terminal.lexeme(), rule.non_terminal});
//                }
//            }
//            return parenthesize_list(expr.exprs);
//        }
//
//        CodeGenerated visit(const RuleDefinitionExpr &expr) {
//            return parenthesize(expr.right);
//        }
//
//        CodeGenerated visit(const RhsExpr &expr) {
//            //need to use variant TODO~!!!
//            return std::visit([=, this](auto &field) -> CodeGenerated {
//                return this->visit(*field);
//            }, expr.expr);
//        }
//
//        CodeGenerated visit(const AlternationExpr &expr) {
//            return  parenthesize_list(expr.exprs);
//        }
//
//        CodeGenerated visit(const ConcatenationExpr &expr) {
//            return parenthesize_list(expr.exprs);
//        }
//
//        CodeGenerated visit(const PreAndPostFixableExpr &expr) {
//            return std::visit([=, this](auto &field) -> CodeGenerated {
//                return this->visit(*field);
//            }, expr.expr);
//        }
//
//        CodeGenerated visit(const OptionalExpr &expr) {
//            return parenthesize("Optional", expr) + "?";
//        }
//
//        CodeGenerated visit(const AtLeastOneExpr &expr) {
//            return parenthesize("Optional", expr) + "+";
//        }
//
//        CodeGenerated visit(const RepeatExpr &expr) {
//            return parenthesize("Optional", expr) + "*";
//        }
//
//        CodeGenerated visit(const GroupingExpr &expr) {
//            return parenthesize("GroupingExpr", expr.expr);
//        }
//
//        CodeGenerated visit(const NonSymbolExpr &expr) {
//            return expr.token.lexeme();
//        }
//
//    private:
//        template<typename ...Args>
//        CodeGenerated parenthesize( Args &&... exprs) {
//            CodeGenerated builder;
//            ([&] {
//                builder.append(exprs.accept(*this));
//            }(), ...);
//            return builder;
//        }
//
//        template<typename T>
//        CodeGenerated parenthesize_list(const std::vector<T> &exprs) {
//            CodeGenerated builder;
//            for (const auto &expr: exprs) {
//                builder.append(expr.accept(*this));
//            }
//            return builder;
//        }
//    };
}
#endif //VULKANWORKSPACE_EBNF_CODE_GEN_H
