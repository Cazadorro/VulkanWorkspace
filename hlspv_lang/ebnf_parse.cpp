//
// Created by Shae Bolt on 9/27/2022.
//

#include "ebnf_parse.h"
#include "parse_state.h" 
#include <range/v3/algorithm/find.hpp>
#include <tl/expected.hpp>
#include <fmt/core.h>
#include <optional>

namespace hlspv::ebnf {
    template<typename T>
    struct ParseStatePair {
        T value;
        ParseState<EbnfToken> parse_state;
    };

    template<typename T>
    tl::expected<T, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<Expression, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RuleDefinitionExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RhsExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<AlternationExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<ConcatenationExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<PostFixableExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<OptionalExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<AtLeastOneExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RepeatExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<GroupingExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<NonSymbolExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state, const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<Expression, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                 const std::vector<ExprType> &exclude_list) {
        std::vector<RuleDefinitionExpr> rules;
        while (!parse_state.at_end()) {
            if (auto result = consume<RuleDefinitionExpr>(parse_state,
                                                          exclude_list); result.has_value()) {
                rules.push_back(std::move(result.value()));
            } else {
                return tl::make_unexpected(result.error());
            }
        }
        return Expression{std::move(rules)};
    }

    template<>
    tl::expected<RuleDefinitionExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken>  &parse_state,
            const std::vector<ExprType> &exclude_list) {

        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if (current_parse_state.current_token().type() !=
            EbnfTokenType::NonTerminalIdentifier) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "RuleDefinitionExpr must start with NonTerminalIdentifier"));
        }
        auto non_terminal = current_parse_state.current_token();
        current_parse_state.consume_current_token();
        if (current_parse_state.current_token().type() !=
            EbnfTokenType::Equals) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "RuleDefinitionExpr must start with NonTerminalIdentifier followed by Equals"));
        }
        current_parse_state.consume_current_token();
        if (auto result = consume<RhsExpr>(current_parse_state,
                                           exclude_list); result.has_value()) {
            if (current_parse_state.current_token().type() !=
                EbnfTokenType::SemiColon) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "RuleDefinitionExpr must end with SemiColon"));
            }
            current_parse_state.consume_current_token();
            parse_state = current_parse_state;
            return RuleDefinitionExpr{non_terminal, std::move(result.value())};
        } else {
            return tl::make_unexpected(result.error());
        }

    }

    template<>
    tl::expected<RhsExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                              const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        ParseError<EbnfToken>  longest_error{{}, "All excluded"};

        if (ranges::find(exclude_list, ConcatenationExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<ConcatenationExpr>(current_parse_state,
                                                         exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, AlternationExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<AlternationExpr>(current_parse_state,
                                                       exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, OptionalExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<OptionalExpr>(current_parse_state,
                                                    exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, AtLeastOneExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<AtLeastOneExpr>(current_parse_state,
                                                      exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, RepeatExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<RepeatExpr>(current_parse_state,
                                                  exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }

        if (ranges::find(exclude_list, GroupingExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<GroupingExpr>(current_parse_state,
                                                    exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, NonSymbolExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<NonSymbolExpr>(current_parse_state,
                                                     exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }

        return tl::make_unexpected(longest_error);
    }

    template<>
    tl::expected<AlternationExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                      const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;

        auto new_exclude_list = exclude_list;
        new_exclude_list.push_back(ExprType::AlternationExpr);
        std::vector<RhsExpr> exprs;
        while (true) {
            auto rhs_expr = consume<RhsExpr>(current_parse_state,
                                             new_exclude_list);
            if (rhs_expr.has_value()) {
                exprs.push_back(std::move(rhs_expr.value()));
                if (current_parse_state.current_token().type() != EbnfTokenType::VerticalLine) {
                    break;
                }
                current_parse_state.consume_current_token();
            } else {
                return tl::make_unexpected(rhs_expr.error());
            }
        }
        if (exprs.size() < 2) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(
                    current_parse_state.create_error(
                            "Expected VerticalLine symbol"));
        }
        parse_state = current_parse_state;
        return AlternationExpr{std::move(exprs)};
    }

    template<>
    tl::expected<ConcatenationExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                        const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;

        auto new_exclude_list = exclude_list;
        new_exclude_list.push_back(ExprType::ConcatenationExpr);
        std::vector<RhsExpr> exprs;
        while (true) {
            auto rhs_expr = consume<RhsExpr>(current_parse_state,
                                             new_exclude_list);
            if (rhs_expr.has_value()) {
                exprs.push_back(std::move(rhs_expr.value()));
                if (current_parse_state.current_token().type() != EbnfTokenType::Comma) {
                    break;
                }
                current_parse_state.consume_current_token();
            } else {
                return tl::make_unexpected(rhs_expr.error());
            }
        }
        if (exprs.size() < 2) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "Expected Comma symbol"));
        }
        parse_state = current_parse_state;
        return ConcatenationExpr{std::move(exprs)};
    }

    template<>
    tl::expected<PostFixableExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                      const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        ParseError<EbnfToken>  longest_error{{}, "All excluded"};
        if (ranges::find(exclude_list, GroupingExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<GroupingExpr>(current_parse_state,
                                                    exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return PostFixableExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, NonSymbolExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<NonSymbolExpr>(current_parse_state,
                                                     exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return PostFixableExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        return tl::make_unexpected(longest_error);
    }

    template<>
    tl::expected<OptionalExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                   const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (auto result = consume<PostFixableExpr>(current_parse_state,
                                                   exclude_list); result.has_value()) {
            if (current_parse_state.current_token().type() !=
                EbnfTokenType::QuestionMark) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "OptionalExpr must end with QuestionMark"));
            }
            current_parse_state.consume_current_token();
            parse_state = current_parse_state;
            return OptionalExpr{std::move(result.value())};
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<AtLeastOneExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                     const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (auto result = consume<PostFixableExpr>(current_parse_state,
                                                   exclude_list); result.has_value()) {
            if (current_parse_state.current_token().type() !=
                EbnfTokenType::Plus) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "AtLeastOneExpr must end with Plus"));
            }
            current_parse_state.consume_current_token();
            parse_state = current_parse_state;
            return AtLeastOneExpr{std::move(result.value())};
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<RepeatExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                 const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (auto result = consume<PostFixableExpr>(current_parse_state,
                                                   exclude_list); result.has_value()) {
            if (current_parse_state.current_token().type() !=
                EbnfTokenType::Star) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "RepeatExpr must end with Star"));
            }
            parse_state = current_parse_state;
            return RepeatExpr{std::move(result.value())};
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<GroupingExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                   const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (current_parse_state.current_token().type() !=
            EbnfTokenType::LeftParenthesis) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "GroupingExpr must start with LeftParenthesis"));
        }
        if (auto result = consume<RhsExpr>(current_parse_state,
                                           {}); result.has_value()) {
            if (current_parse_state.current_token().type() !=
                EbnfTokenType::RightParenthesis) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "GroupingExpr must end with RightParenthesis"));
            }
            current_parse_state.consume_current_token();
            parse_state = current_parse_state;
            return GroupingExpr{std::move(result.value())};
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<NonSymbolExpr, ParseError<EbnfToken> > consume(ParseState<EbnfToken>  &parse_state,
                                                    const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (current_parse_state.current_token().type() !=
            EbnfTokenType::NonTerminalIdentifier &&
            current_parse_state.current_token().type() !=
            EbnfTokenType::TerminalString &&
            current_parse_state.current_token().type() !=
            EbnfTokenType::TerminalUserCode) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "NonSymbolExpr must be a non symbol terminal"));
        }
        auto non_symbol = current_parse_state.current_token();
        current_parse_state.consume_current_token();
        parse_state = current_parse_state;
        return NonSymbolExpr{non_symbol};
    }


    tl::expected<Expression, ParseError<EbnfToken> >
    parse(std::span<const EbnfToken> tokens) {
        ParseState<EbnfToken>  parse_state(tokens);
        return consume<Expression>(parse_state, std::vector<ExprType>{});
    }




}