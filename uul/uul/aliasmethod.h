//
// Created by Bolt on 2/8/2023.
//

#ifndef VULKANWORKSPACE_ALIASMETHOD_H
#define VULKANWORKSPACE_ALIASMETHOD_H
#include "uul/assert.h"
#include <span>
#include <valarray>
#include <vector>
#include <functional>
#include <cstdint>
#include <numeric>
namespace uul{
    //O(1) probability selector, idea is to take probabilities and spread them out across values, such that random
    // selection selects a position, and two values are chosen on coin flip, which ends up being uniform
    template<typename ProbabilityType_T = double, typename IndexType_T = std::uint64_t>
    class AliasTable{
    public:
        explicit AliasTable(const std::span<const ProbabilityType_T>& probabilities){
            auto probability_sum = std::accumulate(probabilities.begin(), probabilities.end(), static_cast<ProbabilityType_T>(0.0));
            UUL_ASSERT(probability_sum == static_cast<ProbabilityType_T>(1.0), "Probabilities must add to 1.0");
            auto weighted_probability_array = std::valarray<ProbabilityType_T>(probabilities.data(), probabilities.size()) * probabilities.size();
            auto u_weighted_probability_array = std::vector<ProbabilityType_T>(weighted_probability_array.size());
            auto alias_index_list = std::vector<IndexType_T>(weighted_probability_array.size());
            std::vector<IndexType_T> lt_one_indexes; //less than one
            std::vector<IndexType_T> gte_one_indexes; //greater than equal to one
            for(std::size_t index = 0; index < weighted_probability_array.size(); ++index){
                auto & probability = weighted_probability_array[index];
                if(probability <  static_cast<ProbabilityType_T>(1.0)){
                    lt_one_indexes.push_back(index);
                }else{
                    gte_one_indexes.push_back(index);
                }
            }

            //spreads gte indexes out
            while(!(lt_one_indexes.empty() || gte_one_indexes.empty())){
                auto lt_index = lt_one_indexes.back();
                lt_one_indexes.pop_back();
                auto gte_index = gte_one_indexes.back();
                gte_one_indexes.pop_back();

                auto probability_lt = weighted_probability_array[lt_index];
                u_weighted_probability_array[lt_index] = probability_lt;
                alias_index_list[lt_index] = gte_index;
                weighted_probability_array[gte_index] = weighted_probability_array[gte_index] + probability_lt -  static_cast<ProbabilityType_T>(1.0);

                if(weighted_probability_array[gte_index] <  static_cast<ProbabilityType_T>(1.0)){
                    lt_one_indexes.push_back(gte_index);
                }else{
                    gte_one_indexes.push_back(gte_index);
                }
            }

            while(!gte_one_indexes.empty()){
                auto gte_index = gte_one_indexes.back();
                gte_one_indexes.pop_back();
                u_weighted_probability_array[gte_index] = static_cast<ProbabilityType_T>(1.0);
            }
            while(!gte_one_indexes.empty()){
                auto lt_index = lt_one_indexes.back();
                lt_one_indexes.pop_back();
                u_weighted_probability_array[lt_index] =  static_cast<ProbabilityType_T>(1.0);
            }
            m_u_choice_weighted_array = u_weighted_probability_array;
            m_alias_index_list = alias_index_list;
        }

        IndexType_T index_select(const std::function<IndexType_T(std::size_t)>& uniform_range_gen,
                                 const std::function<ProbabilityType_T(void)>& uniform_real_gen) const{
            auto idx = uniform_range_gen(m_u_choice_weighted_array.size());
            auto r = uniform_real_gen();
            //perform weighted coin flip for probability bin.
            if( r < m_u_choice_weighted_array[idx]){
                return idx;
            }else{
                return m_alias_index_list[idx];
            }
        }
    private:
        std::vector<ProbabilityType_T> m_u_choice_weighted_array;
        std::vector<IndexType_T> m_alias_index_list;
    };
}
#endif //VULKANWORKSPACE_ALIASMETHOD_H
