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
        UnsignedInteger,
        Comma,
        VerticalLine,
        QuestionMark,
        Ampersand,
        ExclamationPoint,
        Plus,
        Star,
        Equals,
        Colon,
        SemiColon,
        LeftParenthesis,
        RightParenthesis,
        LeftSquareBracket,
        RightSquareBracket,
        EndOfFile
    };
}
#endif //VULKANWORKSPACE_EBNFTOKENTYPE_H
