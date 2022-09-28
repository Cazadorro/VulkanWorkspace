//
// Created by Shae Bolt on 9/27/2022.
//

#include "lexeme_view.h"

namespace hlspv {
    std::string LexemeView::to_string() const {
        return std::string(to_sv());
    }

    std::string_view LexemeView::to_sv() const {
        return m_source_code.substr(m_start_idx, size());
    }

    std::size_t LexemeView::size() const {
        return (m_end_idx + 1) - m_start_idx;;
    }

    bool LexemeView::is_empty() const {
        return size() == 0;
    }

    LexemeView::LexemeView(std::string_view source_code, std::size_t start_idx,
                           std::size_t end_idx) : m_source_code(source_code),
                                                  m_start_idx(start_idx),
                                                  m_end_idx(end_idx) {

    }

    LexemeView merge(const LexemeView &lhs, const LexemeView &rhs) {
        //TODO assert lhs sourcecode is the same as rhs.
        return LexemeView{lhs.m_source_code, std::min(lhs.m_start_idx, rhs.m_start_idx), std::max(lhs.m_end_idx, rhs.m_end_idx)};
    }
}