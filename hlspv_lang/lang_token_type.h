//
// Created by Shae Bolt on 9/18/2022.
//

#ifndef VULKANWORKSPACE_LANG_TOKEN_TYPE_H
#define VULKANWORKSPACE_LANG_TOKEN_TYPE_H

#include <uul/enumtraits.h>
#include <uul/string.h>
#include <string_view>

namespace hlspv::lang{
    enum class TokenType{
        LeftParenthesis,             // (
        RightParenthesis,            // )
        LeftCurlyBrace,              // {
        RightCurlyBrace,             // }
        LeftSquareBracket,           // [
        RightSquareBracket,          // ]
        Comma,                       // ,
        Apostrophe,                  // '
        Dot,                         // .
        Ellipses,                    // ...
        ShortEllipses,               // ..

        Minus,                       // -
        Plus,                        // +
        Colon,                       // :
        Semicolon,                   // ;
        DoubleColon,                 // ::
        ForwardSlash,                // /
        BackSlash,                   // "\"
        Star,                        // *
        DoubleStar,                  // **
        ExclamationPoint,            // !
        QuestionMark,                // ?
        LeftArrow,                   // ->
        LeftMatchArrow,              // =>
        Equals,                      // =
        DoubleEquals,                // ==
        NotEquals,                   // !=
        GreaterThan,                 // >
        GreaterThanEquals,           // >=
        LessThan,                    // <
        LessThanEquals,              // <=
        Pipe,                        // |
        DoublePipe,                  // ||
        Caret,                       // ^
        Ampersand,                   // &
        At,                          // @
        Percent,                     // %
        UnderScore,                  // _
        

        //Literals
        String,                      // "asdf"
        DecimalUnsignedLiteral,      //  123
        DecimalIntegerLiteral,       // -123
        BinaryLiteral,               // 0b010101
        HexidecimalLiteral,          // 0xFFFFF
        OctalLiteral,                // 0oDFDF
        DecimalFloatingPointLiteral, // 1.0e-10
        BooleanLiteral,              //true or false?

        //Keywords
        Null,                        // null
        Let,                         // let
        Var,                         // var
        Const,                       // const
        Function,                    // fn
        LiteralFunction,             // lit
        OperatorOverload,            // op
        Struct,                      // struct
        Enum,                        // enum
        Trait,                       // trait
        Impl,                        // impl
        Type,                        // type
        Loop,                        // loop
        For,                         // for
        While,                       // while
        Continue,                    // continue
        Break,                       // break
        Match,                       // match
        As,                          // as
        If,                          // if
        Else,                        // else
        Elif,                        // elif
        And,                         // and
        Or,                          // or
        Xor,                         // xor
        Not,                         // not
        Pub,                         // pub
        Mod,                         // mod
        Where,                       // where
        Use,                         // use
        In,                          // in
        Static,                      // static
        Mut,                         // mut
        Extern,                      // extern
        Return,                      // return

        //Special
        Identifier,

        EndOfFile
    };



    [[nodiscard]]
    std::string to_string(TokenType token);

}
namespace uul{
    template<>
    [[nodiscard]]
    hlspv::lang::TokenType from_string(std::string_view value);
    template<>
    [[nodiscard]]
    constexpr hlspv::lang::TokenType enum_max<hlspv::lang::TokenType>() noexcept{
        return hlspv::lang::TokenType::EndOfFile;
    }
    template<>
    [[nodiscard]]
    constexpr hlspv::lang::TokenType enum_min<hlspv::lang::TokenType>() noexcept{
        return hlspv::lang::TokenType::LeftParenthesis;
    }
}
#endif //VULKANWORKSPACE_LANG_TOKEN_TYPE_H
