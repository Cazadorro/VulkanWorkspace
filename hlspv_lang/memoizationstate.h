//
// Created by Bolt on 5/30/2023.
//

#ifndef VULKANWORKSPACE_MEMOIZATIONSTATE_H
#define VULKANWORKSPACE_MEMOIZATIONSTATE_H
#include "parse_state.h"
#include <uul/concepts.h>
#include <cstdint>
#include <unordered_map>
#include <memory>


namespace hlspv {
    template<uul::Enumeration T>
    struct ExprStateKey {
        T expr_type;
        std::uint64_t token_point;
    };
    template<uul::Enumeration T>
    bool operator==(const ExprStateKey<T> &lhs, const ExprStateKey<T> &rhs) {
        return lhs.expr_type == rhs.expr_type && lhs.token_point == rhs.token_point;
    }
    template<uul::Enumeration T>
    bool operator!=(const ExprStateKey<T> &lhs, const ExprStateKey<T> &rhs) {
        return !(lhs == rhs);
    }
}
namespace std {

    template<uul::Enumeration T>
    struct hash<hlspv::ExprStateKey<T>> {
        std::size_t operator()(const hlspv::ExprStateKey<T> &k) const {
            using std::size_t;
            using std::hash;

            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return (hash<std::uint64_t>()(hash<std::uint64_t>()(static_cast<std::uint64_t>(k.expr_type)))
                    ^ hash<std::uint64_t>()(static_cast<std::uint64_t>(k.token_point)));
        }
    };
}

namespace hlspv{
    template<uul::Enumeration TokenType_T, typename T>
    struct ParseStatePair {
        T value;
        ParseState<TokenType_T> parse_state;
    };

    template<typename T>
    struct MemoizationValue{
        std::uint64_t consume_size = 0;
        T* expr_ptr;
    };


    template<typename ExpressionPtr_T>
    struct MemoizationAnyValue{
        std::uint64_t consume_size = 0;
        ExpressionPtr_T expr_ptr;

        template<typename T = decltype(*std::declval<ExpressionPtr_T>())>
        [[nodiscard]]
        explicit operator MemoizationValue<T>(){
            if constexpr(uul::is_variant_v<ExpressionPtr_T>){
                return MemoizationValue<T>{consume_size, std::get<T*>(expr_ptr)};
            }else{
                return MemoizationValue<T>{consume_size, expr_ptr};
            }

        }
    };



    template<uul::Enumeration ExpressionTokenType_T, typename ExpressionPtr_T, typename ExpressionUniquePtr_T = typename std::unique_ptr<std::remove_reference_t<decltype(
            *std::declval<ExpressionPtr_T>()
            )>>>
    struct MemoizationState {
        std::unordered_map<ExprStateKey<ExpressionTokenType_T>, MemoizationAnyValue<ExpressionPtr_T> > cache;
        std::vector<ExpressionUniquePtr_T> expressions;

        template<typename T>
        std::optional<MemoizationValue<T>> get_cached(std::uint64_t token_point,  ExpressionTokenType_T expr_type_enum) {
            auto key = ExprStateKey<ExpressionTokenType_T>{expr_type_enum,
                                    token_point};
            if (auto value = cache.find(key);  value != cache.end()) {
                return static_cast<MemoizationValue<T>>(value->second);
            }
            return std::nullopt;
        }

        template<typename T>
        T* cache_value(std::unique_ptr<T>&& value, std::uint64_t token_point, std::uint64_t consume_size, ExpressionTokenType_T expr_type_enum) {
            auto ret_ptr = value.get();
            expressions.push_back(std::move(value));
            auto key = ExprStateKey<ExpressionTokenType_T>{expr_type_enum,
                                    token_point};
            cache[key] = {consume_size,ret_ptr};
            return ret_ptr;
        }
    };

}
#endif //VULKANWORKSPACE_MEMOIZATIONSTATE_H
