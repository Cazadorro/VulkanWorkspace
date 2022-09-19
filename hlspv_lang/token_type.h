//
// Created by Shae Bolt on 9/18/2022.
//

#ifndef VULKANWORKSPACE_TOKEN_TYPE_H
#define VULKANWORKSPACE_TOKEN_TYPE_H
#include <map>
namespace hlspv{
    enum class TokenType{
        LeftParenthesis,
        RightParenthesis,
        LeftCurlyBrace,
        RightCurlyBrace,
        LeftSquareBracket,
        RightSquareBracket,
        Comma,
        Dot,
        Elipses,
        Minus,
        Plus,
        Colon,
        Semicolon,
        DoubleColon,
        ForwardSlash,
        BackSlash,
        Star,
        ExclamationPoint,
        QuestionMark,
        LeftArrow,
        LeftMatchArrow,
        Equals,
        DoubleEquals,
        NotEquals,
        GreaterThan,
        GreaterThanEquals,
        LessThan,
        LessThanEquals,

        //Literals

        String,
        DecimalUnsignedLiteral,
        DecimalIntegerLiteral,
        BinaryLiteral,
        HexidecimalLiteral,
        OctalLiteral,
        DecimalFloatingPointLiteral,
        BooleanLiteral, //true or false?
        Identifier,

        //Keywords
        Let,
        Const,
        Function,
        Struct,
        Enum,
        Trait,
        Impl,
        Type,
        Loop,
        For,
        While,
        Continue,
        Break,
        Match,
        As,
        If,
        Else,
        Elif,
        And,
        Or,
        Xor,
        Not,
        Pub,
        Mod,
        Return,
        EndOfFile
    };
}
#endif //VULKANWORKSPACE_TOKEN_TYPE_H
