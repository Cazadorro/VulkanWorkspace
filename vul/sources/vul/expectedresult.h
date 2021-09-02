//
// Created by Shae Bolt on 6/2/2021.
//

#ifndef VULKANWORKSPACE_EXPECTEDRESULT_H
#define VULKANWORKSPACE_EXPECTEDRESULT_H
#include "vul/enums.h"
#include <fmt/format.h>
#include <stdexcept>
#include <gsl/span>
#include <tuple>
namespace vul {
    template<typename T>
    class ExpectedResult {
    public:
        Result result;
        T value;
        ExpectedResult(Result result, T&& value);
        std::tuple<Result, T> asTuple() {
            return std::make_tuple(result, std::move(value));
        }
        T exceptValue(const gsl::span<Result>& other_success_results = {}, const std::string& message = ""){
            if(result == Result::Success){
               return value;
            }else{
                for(const auto& success_result: other_success_results){
                    if(result == success_result){
                        return value;
                    }
                }
                throw std::runtime_error(fmt::format("Error: {}\n\t{}\n", vul::to_string(result), message));
            }
        }
        T assertValue(const gsl::span<Result>& other_success_results = {}, const std::string& message = ""){
            if(result == Result::Success){
                return std::move(value);
            }else{
                for(const auto& success_result: other_success_results){
                    if(result == success_result){
                        return std::move(value);
                    }
                }
                fmt::print(stderr, "Error: {}\n\t{}\n", vul::to_string(result), message);
                std::terminate();
            }
        }
        bool hasValue(const gsl::span<Result>& other_success_results = {}){
            if(result == Result::Success){
                return true;
            }else{
                for(const auto& success_result: other_success_results){
                    if(result == success_result){
                        return true;
                    }
                }
            }
            return false;
        }

    private:

    };
    template<typename T>
    ExpectedResult<T>::ExpectedResult(Result result_t, T &&value_t) : result(result_t), value(std::move(value_t)){
    }
}


#endif //VULKANWORKSPACE_EXPECTEDRESULT_H
