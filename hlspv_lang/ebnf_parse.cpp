//
// Created by Shae Bolt on 9/27/2022.
//

#include "ebnf_parse.h"
#include "parse_state.h"
#include <range/v3/algorithm/find.hpp>
#include <tl/expected.hpp>
#include <fmt/core.h>
#include <optional>
#include <unordered_map>

namespace hlspv::ebnf {
    struct ExprStateKey {
        ExprType expr_type;
        std::uint64_t token_point;
    };

    bool operator==(const ExprStateKey &lhs, const ExprStateKey &rhs) {
        return lhs.expr_type == rhs.expr_type && lhs.token_point == rhs.token_point;
    }

    bool operator!=(const ExprStateKey &lhs, const ExprStateKey &rhs) {
        return !(lhs == rhs);
    }
}
namespace std {

    template<>
    struct hash<hlspv::ebnf::ExprStateKey> {
        std::size_t operator()(const hlspv::ebnf::ExprStateKey &k) const {
            using std::size_t;
            using std::hash;
            using std::string;

            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return (hash<std::uint64_t>()(hash<std::uint64_t>()(static_cast<std::uint64_t>(k.expr_type)))
                    ^ hash<std::uint64_t>()(static_cast<std::uint64_t>(k.token_point)));
        }
    };

}
namespace hlspv::ebnf {
    template<typename T>
    struct ParseStatePair {
        T value;
        ParseState<EbnfToken> parse_state;
    };


    struct MemoizationState {
        std::unordered_map<ExprStateKey, AnyExpression> cache;

        template<typename T>
        std::optional<T> get_cached(std::uint64_t token_point) {
            auto key = ExprStateKey{get_expression_type<T>(),
                                    token_point};
            if (auto value = cache.find(key);  value != cache.end()) {
                return value;
            }
            return std::nullopt;
        }

        template<typename T>
        void cache_value(T value, std::uint64_t token_point) {
            auto key = ExprStateKey{get_expression_type<T>(),
                                    token_point};
            cache[key] = value;
        }
    };

    //Exclude List is what you *can't* match against later on, so for example, if you were previously trying to match against a value
    //and recursevily you run into something that would violate trying to match *that* value, you would no longer be able to match against that value.
    template<typename T>
    tl::expected<T, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<Expression, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RuleDefinitionExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RhsExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<AlternationExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<ConcatenationExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<PreAndPostFixableExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<OptionalExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<AtLeastOneExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RepeatExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<BracketExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<AndPredicateExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<NotPredicateExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<GroupingExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<NonSymbolExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<Expression, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        std::vector<RuleDefinitionExpr> rules;
        while (!parse_state.at_end()) {
            if (auto result = consume<RuleDefinitionExpr>(parse_state,
                                                          memoization_state,
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
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
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
                                           memoization_state,
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
    tl::expected<RhsExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        ParseError<EbnfToken> longest_error{{}, "All excluded"};

        if(auto cached_expr = memoization_state.get_cached<RhsExpr>(parse_state.current_token_index); cached_expr.has_value()) {
            return  RhsExpr{cached_expr.value()};
        }
        if (ranges::find(exclude_list, ConcatenationExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<ConcatenationExpr>(current_parse_state,
                                                         memoization_state,
                                                         exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
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
                                                       memoization_state,
                                                       exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, AndPredicateExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<AndPredicateExpr>(current_parse_state,
                                                        memoization_state,
                                                        exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, NotPredicateExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<NotPredicateExpr>(current_parse_state,
                                                        memoization_state,
                                                        exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
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
                                                    memoization_state,
                                                    exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
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
                                                      memoization_state,
                                                      exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
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
                                                  memoization_state,
                                                  exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
                parse_state = current_parse_state;
                return RhsExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, BracketExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<BracketExpr>(current_parse_state,
                                                   memoization_state,
                                                   exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
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
                                                    memoization_state,
                                                    exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
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
                                                     memoization_state,
                                                     exclude_list); result.has_value()) {
                memoization_state.cache_value(RhsExpr{std::move(result.value())}, parse_state.current_token_index);
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

    //Exclude list used because if we didn't, it would attempt to keep matching list all the way down. eg a | b | c | d would match:
    // a | (b |(c |(d |))) instead of a | b | c | d and wouldn't be able to deal with a | b, c | d.
    template<>
    tl::expected<AlternationExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;

        auto new_exclude_list = exclude_list;
        new_exclude_list.push_back(ExprType::AlternationExpr);
        std::vector<RhsExpr> exprs;
        while (true) {
            auto rhs_expr = consume<RhsExpr>(current_parse_state,
                                             memoization_state,
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

    //Exclude list used because if we didn't, it would attempt to keep matching list all the way down.
    template<>
    tl::expected<ConcatenationExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;

        auto new_exclude_list = exclude_list;
        new_exclude_list.push_back(ExprType::ConcatenationExpr);
        std::vector<RhsExpr> exprs;
        while (true) {
            auto rhs_expr = consume<RhsExpr>(current_parse_state,
                                             memoization_state,
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
    tl::expected<PreAndPostFixableExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        ParseError<EbnfToken> longest_error{{}, "All excluded"};
        if (ranges::find(exclude_list, GroupingExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<GroupingExpr>(current_parse_state,
                                                    memoization_state,
                                                    exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return PreAndPostFixableExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, NonSymbolExpr::type) ==
            exclude_list.end()) {
            if (auto result = consume<NonSymbolExpr>(current_parse_state,
                                                     memoization_state,
                                                     exclude_list); result.has_value()) {
                parse_state = current_parse_state;
                return PreAndPostFixableExpr{std::move(result.value())};
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        return tl::make_unexpected(longest_error);
    }

    template<>
    tl::expected<OptionalExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (auto result = consume<PreAndPostFixableExpr>(current_parse_state,
                                                         memoization_state,
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
    tl::expected<AtLeastOneExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (auto result = consume<PreAndPostFixableExpr>(current_parse_state,
                                                         memoization_state,
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
    tl::expected<RepeatExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (auto result = consume<PreAndPostFixableExpr>(current_parse_state,
                                                         memoization_state,
                                                         exclude_list); result.has_value()) {
            if (current_parse_state.current_token().type() !=
                EbnfTokenType::Star) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "RepeatExpr must end with Star"));
            }
            current_parse_state.consume_current_token();
            parse_state = current_parse_state;
            return RepeatExpr{std::move(result.value())};
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<BracketExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (auto result = consume<PreAndPostFixableExpr>(current_parse_state,
                                                         memoization_state, exclude_list); result.has_value()) {
            if (current_parse_state.current_token().type() !=
                EbnfTokenType::LeftSquareBracket) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "BracketExpr must start with LeftSquareBracket"));
            }
            current_parse_state.consume_current_token();
            std::uint64_t min = 0; // if nothing is set, range starts at 0.
            std::uint64_t max = 0; // if nothing is set after :, 0 signifies infiniity, as it doesn't make sense to end at 0 otherwise.
            bool first_integer_set = false;
            bool colon_set = false;
            //[10] valid
            //[:] valid
            //[10:] valid
            //[] invalid
            if (current_parse_state.current_token().type() == EbnfTokenType::UnsignedInteger) {
                min = current_parse_state.current_token().value<std::uint64_t>();
                current_parse_state.consume_current_token();
                first_integer_set = true;
            }

            if (current_parse_state.current_token().type() == EbnfTokenType::Colon) {
                current_parse_state.consume_current_token();
                colon_set = true;
            } else if (!first_integer_set) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "BracketExpr must atleast have : or have a number ([:] vs [10])"));
            }

            if (colon_set && current_parse_state.current_token().type() == EbnfTokenType::UnsignedInteger) {
                max = current_parse_state.current_token().value<std::uint64_t>();
                current_parse_state.consume_current_token();
                if (max <= min) {
                    current_parse_state.reset_start_index(parse_state.get_current_token_index());
                    return tl::make_unexpected(current_parse_state.create_error(
                            "BracketExpr max value in brackets [min:max] [:max] must be greater than zero and greater than min"));
                }
            } else if (!colon_set) {
                max = min;  // [10] so min=10, and max=10.
            }
            if (current_parse_state.current_token().type() !=
                EbnfTokenType::RightSquareBracket) {
                current_parse_state.reset_start_index(parse_state.get_current_token_index());
                return tl::make_unexpected(current_parse_state.create_error(
                        "BracketExpr must end with RightSquareBracket"));
            }
            current_parse_state.consume_current_token();
            parse_state = current_parse_state;
            return BracketExpr{std::move(result.value()), min, max};
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<AndPredicateExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (current_parse_state.current_token().type() != EbnfTokenType::Ampersand) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "AndPredicateExpr must start with Ampersand"));
        }
        if (auto result = consume<PreAndPostFixableExpr>(current_parse_state,
                                                         memoization_state, exclude_list); result.has_value()) {
            parse_state = current_parse_state;
            return AndPredicateExpr{std::move(result.value())};
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<NotPredicateExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (current_parse_state.current_token().type() != EbnfTokenType::ExclamationPoint) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "NotPredicateExpr must start with ExclamationPoint"));
        }
        if (auto result = consume<PreAndPostFixableExpr>(current_parse_state,
                                                         memoization_state, exclude_list); result.has_value()) {
            parse_state = current_parse_state;
            return NotPredicateExpr{std::move(result.value())};
        } else {
            return tl::make_unexpected(result.error());
        }
    }


    //We don't pass on exclude list in grouping expression because we know things "reset".
    template<>
    tl::expected<GroupingExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        if (current_parse_state.current_token().type() !=
            EbnfTokenType::LeftParenthesis) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "GroupingExpr must start with LeftParenthesis"));
        }
        current_parse_state.consume_current_token();
        if (auto result = consume<RhsExpr>(current_parse_state,
                                           memoization_state,
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
    tl::expected<NonSymbolExpr, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, MemoizationState &memoization_state,
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
        ParseState<EbnfToken> parse_state(tokens);
        MemoizationState memoization_state;
        return consume<Expression>(parse_state, memoization_state, std::vector<ExprType>{});
    }


}