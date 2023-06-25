//
// Created by Shae Bolt on 9/27/2022.
//

#include "ebnf_parse.h"
#include "parse_state.h"
#include "memoizationstate.h"
#include <range/v3/algorithm/find.hpp>
#include <tl/expected.hpp>
#include <fmt/core.h>
#include <optional>
#include <unordered_map>

//namespace hlspv::ebnf {
//    struct ExprStateKey {
//        ExprType expr_type;
//        std::uint64_t token_point;
//    };
//
//    bool operator==(const ExprStateKey &lhs, const ExprStateKey &rhs) {
//        return lhs.expr_type == rhs.expr_type && lhs.token_point == rhs.token_point;
//    }
//
//    bool operator!=(const ExprStateKey &lhs, const ExprStateKey &rhs) {
//        return !(lhs == rhs);
//    }
//}
//namespace std {
//
//    template<>
//    struct hash<hlspv::ebnf::ExprStateKey> {
//        std::size_t operator()(const hlspv::ebnf::ExprStateKey &k) const {
//            using std::size_t;
//            using std::hash;
//            using std::string;
//
//            // Compute individual hash values for first,
//            // second and third and combine them using XOR
//            // and bit shifting:
//
//            return (hash<std::uint64_t>()(hash<std::uint64_t>()(static_cast<std::uint64_t>(k.expr_type)))
//                    ^ hash<std::uint64_t>()(static_cast<std::uint64_t>(k.token_point)));
//        }
//    };
//
//}
namespace hlspv::ebnf {
    using EbnfMemoizationState = hlspv::MemoizationState<ExprType, AnyExpressionPtr, AnyExpressionUniquePtr>;
//    template<typename T>
//    struct ParseStatePair {
//        T value;
//        ParseState<EbnfToken> parse_state;
//    };
//
//    template<typename T>
//    struct MemoizationValue{
//        std::uint64_t consume_size = 0;
//        T* expr_ptr;
//    };
//
//    struct MemoizationAnyValue{
//        std::uint64_t consume_size = 0;
//        AnyExpressionPtr expr_ptr;
//
//        template<typename T>
//        [[nodiscard]]
//        explicit operator MemoizationValue<T>(){
//            return MemoizationValue<T>{consume_size, std::get<T*>(expr_ptr)};
//        }
//    };
//
//
//
//    struct MemoizationState {
//        std::unordered_map<ExprStateKey, MemoizationAnyValue> cache;
//
//        std::vector<AnyExpressionUniquePtr> expressions;
//
//
//        template<typename T>
//        std::optional<MemoizationValue<T>> get_cached(std::uint64_t token_point) {
//            auto key = ExprStateKey{get_expression_type<T>(),
//                                    token_point};
//            if (auto value = cache.find(key);  value != cache.end()) {
//                return static_cast<MemoizationValue<T>>(value->second);
//            }
//            return std::nullopt;
//        }
//
//        template<typename T>
//        T* cache_value(std::unique_ptr<T>&& value, std::uint64_t token_point, std::uint64_t consume_size) {
//            auto ret_ptr = value.get();
//            expressions.push_back(std::move(value));
//            auto key = ExprStateKey{get_expression_type<T>(),
//                                    token_point};
//            cache[key] = {consume_size,ret_ptr};
//            return ret_ptr;
//        }
//    };

    template<typename T>
    auto get_cached(EbnfMemoizationState& memoization_state, std::uint64_t token_point) {
        return memoization_state.get_cached<T>(token_point, get_expression_type<T>());
    }

    template<typename T>
    auto cache_value(EbnfMemoizationState& memoization_state, std::unique_ptr<T>&& value, std::uint64_t token_point, std::uint64_t consume_size) {
        return memoization_state.cache_value<T>(std::move(value), token_point, consume_size, get_expression_type<T>());
    }

    //Exclude List is what you *can't* match against later on, so for example, if you were previously trying to match against a value
    //and recursevily you run into something that would violate trying to match *that* value, you would no longer be able to match against that value.
    template<typename T>
    tl::expected<T*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<Expression*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RuleDefinitionExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RhsExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<AlternationExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<ConcatenationExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<PreAndPostFixableExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<OptionalExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<AtLeastOneExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<RepeatExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<BracketExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<AndPredicateExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<NotPredicateExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<GroupingExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<NonSymbolExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list);

    template<>
    tl::expected<Expression*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        std::vector<RuleDefinitionExpr*> rules;
        while (!parse_state.at_end()) {
            if (auto result = consume<RuleDefinitionExpr>(parse_state,
                                                          memoization_state,
                                                          exclude_list); result.has_value()) {
                rules.push_back(result.value());
            } else {
                return tl::make_unexpected(result.error());
            }
        }
        return cache_value(memoization_state, std::make_unique<Expression>(rules), parse_state.start_token_index, parse_state.consume_size());
    }

    template<>
    tl::expected<RuleDefinitionExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {

        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<RuleDefinitionExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }

        if (current_parse_state.current_token().type() !=
            EbnfTokenType::NonTerminalIdentifier) {
            auto lexeme = current_parse_state.current_token().lexeme();
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
            parse_state.set_current_token_index(current_parse_state);
            return cache_value(memoization_state, std::make_unique<RuleDefinitionExpr>(non_terminal, result.value()), parse_state.start_token_index, parse_state.consume_size());
        } else {
            return tl::make_unexpected(result.error());
        }

    }

    template<>
    tl::expected<RhsExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<RhsExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }

        ParseError<EbnfToken> longest_error{{}, "All excluded"};


        if (ranges::find(exclude_list, ConcatenationExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<ConcatenationExpr>(temp_parse_state,
                                                         memoization_state,
                                                         exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, AlternationExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<AlternationExpr>(temp_parse_state,
                                                       memoization_state,
                                                       exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());

            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, AndPredicateExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<AndPredicateExpr>(temp_parse_state,
                                                        memoization_state,
                                                        exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, NotPredicateExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<NotPredicateExpr>(temp_parse_state,
                                                        memoization_state,
                                                        exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, OptionalExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<OptionalExpr>(temp_parse_state,
                                                    memoization_state,
                                                    exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.current_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, AtLeastOneExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<AtLeastOneExpr>(temp_parse_state,
                                                      memoization_state,
                                                      exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, RepeatExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<RepeatExpr>(temp_parse_state,
                                                  memoization_state,
                                                  exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, BracketExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<BracketExpr>(temp_parse_state,
                                                   memoization_state,
                                                   exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, GroupingExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<GroupingExpr>(temp_parse_state,
                                                    memoization_state,
                                                    exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, NonSymbolExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<NonSymbolExpr>(temp_parse_state,
                                                     memoization_state,
                                                     exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<RhsExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
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
    tl::expected<AlternationExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {



        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<AlternationExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
        auto new_exclude_list = exclude_list;
        new_exclude_list.push_back(ExprType::AlternationExpr);
        std::vector<RhsExpr*> exprs;
        while (true) {
            auto rhs_expr = consume<RhsExpr>(current_parse_state,
                                             memoization_state,
                                             new_exclude_list);
            if (rhs_expr.has_value()) {
                exprs.push_back(rhs_expr.value());
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
        parse_state.set_current_token_index(current_parse_state);
        return cache_value(memoization_state, std::make_unique<AlternationExpr>(exprs), parse_state.start_token_index, parse_state.consume_size());

    }

    //Exclude list used because if we didn't, it would attempt to keep matching list all the way down.
    template<>
    tl::expected<ConcatenationExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<ConcatenationExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
        auto new_exclude_list = exclude_list;
        new_exclude_list.push_back(ExprType::ConcatenationExpr);
        std::vector<RhsExpr*> exprs;
        while (true) {
            auto rhs_expr = consume<RhsExpr>(current_parse_state,
                                             memoization_state,
                                             new_exclude_list);
            if (rhs_expr.has_value()) {
                exprs.push_back(rhs_expr.value());
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
        parse_state.set_current_token_index(current_parse_state);
        return cache_value(memoization_state, std::make_unique<ConcatenationExpr>(exprs), parse_state.start_token_index, parse_state.consume_size());
    }

    template<>
    tl::expected<PreAndPostFixableExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<PreAndPostFixableExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
        ParseError<EbnfToken> longest_error{{}, "All excluded"};
        if (ranges::find(exclude_list, GroupingExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<GroupingExpr>(temp_parse_state,
                                                    memoization_state,
                                                    exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<PreAndPostFixableExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        if (ranges::find(exclude_list, NonSymbolExpr::type) ==
            exclude_list.end()) {
            auto temp_parse_state = current_parse_state;
            if (auto result = consume<NonSymbolExpr>(temp_parse_state,
                                                     memoization_state,
                                                     exclude_list); result.has_value()) {
                parse_state = temp_parse_state;
                return cache_value(memoization_state, std::make_unique<PreAndPostFixableExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
            } else {
                if (longest_error.size() < result.error().size()) {
                    longest_error = result.error();
                }
            }
        }
        return tl::make_unexpected(longest_error);
    }

    template<>
    tl::expected<OptionalExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<OptionalExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
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
            parse_state.set_current_token_index(current_parse_state);
            return cache_value(memoization_state, std::make_unique<OptionalExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<AtLeastOneExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<AtLeastOneExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
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
            parse_state.set_current_token_index(current_parse_state);
            return cache_value(memoization_state, std::make_unique<AtLeastOneExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<RepeatExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<RepeatExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
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
            parse_state.set_current_token_index(current_parse_state);
            return cache_value(memoization_state, std::make_unique<RepeatExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<BracketExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<BracketExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
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
            parse_state.set_current_token_index(current_parse_state);
            return cache_value(memoization_state, std::make_unique<BracketExpr>(result.value(), min, max), parse_state.start_token_index, parse_state.consume_size());
        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<AndPredicateExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<AndPredicateExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
        if (current_parse_state.current_token().type() != EbnfTokenType::Ampersand) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "AndPredicateExpr must start with Ampersand"));
        }
        if (auto result = consume<PreAndPostFixableExpr>(current_parse_state,
                                                         memoization_state, exclude_list); result.has_value()) {
            parse_state.set_current_token_index(current_parse_state);
            return cache_value(memoization_state, std::make_unique<AndPredicateExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());

        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<NotPredicateExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<NotPredicateExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
        if (current_parse_state.current_token().type() != EbnfTokenType::ExclamationPoint) {
            current_parse_state.reset_start_index(parse_state.get_current_token_index());
            return tl::make_unexpected(current_parse_state.create_error(
                    "NotPredicateExpr must start with ExclamationPoint"));
        }
        if (auto result = consume<PreAndPostFixableExpr>(current_parse_state,
                                                         memoization_state, exclude_list); result.has_value()) {
            parse_state.set_current_token_index(current_parse_state);
            return cache_value(memoization_state, std::make_unique<NotPredicateExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());
        } else {
            return tl::make_unexpected(result.error());
        }
    }


    //We don't pass on exclude list in grouping expression because we know things "reset".
    template<>
    tl::expected<GroupingExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<GroupingExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
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
            parse_state.set_current_token_index(current_parse_state);
            return cache_value(memoization_state, std::make_unique<GroupingExpr>(result.value()), parse_state.start_token_index, parse_state.consume_size());

        } else {
            return tl::make_unexpected(result.error());
        }
    }

    template<>
    tl::expected<NonSymbolExpr*, ParseError<EbnfToken> >
    consume(ParseState<EbnfToken> &parse_state, EbnfMemoizationState &memoization_state,
            const std::vector<ExprType> &exclude_list) {
        auto current_parse_state = parse_state;
        current_parse_state.reset_start_index();
        if(auto cached_expr = get_cached<NonSymbolExpr>(memoization_state, parse_state.current_token_index); cached_expr.has_value()) {
            current_parse_state.consume_count_token(cached_expr.value().consume_size);
            parse_state.set_current_token_index(current_parse_state);
            return cached_expr.value().expr_ptr;
        }
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
        parse_state.set_current_token_index(current_parse_state);
        return cache_value(memoization_state, std::make_unique<NonSymbolExpr>(non_symbol), parse_state.start_token_index, parse_state.consume_size());
    }


    ParseResult
    parse(std::span<const EbnfToken> tokens) {
        ParseState<EbnfToken> parse_state(tokens);
        EbnfMemoizationState memoization_state;
        auto expression_ptr = consume<Expression>(parse_state, memoization_state, std::vector<ExprType>{});
        return {std::move(expression_ptr), std::move(memoization_state.expressions)};
    }


}