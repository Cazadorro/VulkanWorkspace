//
// Created by Shae Bolt on 9/26/2022.
//

#ifndef VULKANWORKSPACE_EBNFTOKENTYPE_H
#define VULKANWORKSPACE_EBNFTOKENTYPE_H
namespace hlspv{
    enum class EbnfTokenType{
        NonTerminalIdentifier,
        TerminalString,
        TerminalUserCode,
        Comma,
        VerticalLine,
        QuestionMark,
        Plus,
        Star,
        Equals,
        SemiColon,
        LeftParenthesis,
        RightParenthesis,
        EndOfFile
    };
}
#endif //VULKANWORKSPACE_EBNFTOKENTYPE_H
