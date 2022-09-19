//
// Created by Shae Bolt on 9/17/2022.
//

#include "scan.h"
#include <iostream>
#include <fstream>
#include <string>

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
    return 0;
}
