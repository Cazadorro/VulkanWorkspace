//
// Created by Shae Bolt on 9/27/2022.
//

#ifndef VULKANWORKSPACE_LEXEME_VIEW_H
#define VULKANWORKSPACE_LEXEME_VIEW_H

#include <string_view>
#include <string>
#include <cstdint>

namespace hlspv {

    class LexemeView {
    public:
        LexemeView() = default;
        LexemeView(std::string_view source_code, std::size_t start_idx, std::size_t end_idx);
        [[nodiscard]]
        std::string to_string() const;
        [[nodiscard]]
        std::string_view to_sv() const;
        [[nodiscard]]
        std::size_t size() const;
        [[nodiscard]]
        bool is_empty() const;
    private:
        friend LexemeView merge(const LexemeView& lhs, const LexemeView& rhs);
        std::string_view m_source_code;
        std::size_t m_start_idx = 0;
        std::size_t m_end_idx = 0;
    };
    [[nodiscard]]
    LexemeView merge(const LexemeView& lhs, const LexemeView& rhs);
}

#endif //VULKANWORKSPACE_LEXEME_VIEW_H
