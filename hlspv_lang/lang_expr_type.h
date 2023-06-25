//
// Created by Bolt on 6/10/2023.
//

#ifndef VULKANWORKSPACE_LANG_EXPR_TYPE_H
#define VULKANWORKSPACE_LANG_EXPR_TYPE_H
#include <uul/enumtraits.h>
#include <uul/string.h>
namespace hlspv::lang{
    enum class ExpressionType{
        ModuleExpression,
        EqualsExpression,
        MemberDeclaration,
        StructDefinition,
        ParameterDeclaration,
        TemplateDefinition,
        FunctionParameterList,
        FunctionDefinition,
        Literal,
        GroupExpression,
        RangeExpression,
        RhsExpression,
        NonArithmeticRhsExpression,
        FunctionCallArgumentList,
        TemplateInstantiation,
        FunctionCall,
        StatementExpression,
        LoopStatementExpression,
        ScopeStatement,
        ConditionalStatement,
        LoopScopeStatement,
        LoopConditionalStatement,
        WhileLoopStatement,
        ForInStatement,
        NormExpression,
        UnaryMinusExpression,
        UnaryPlusExpression,
        UnaryNotExpression,
        UnaryOperatorExpression,
        PowExpression,
        MulExpression,
        DivExpression,
        ModExpression,
        MatMulExpression,
        MulPrecedenceExpression,
        AddExpression,
        SubExpression,
        AddPrecedenceExpression,
        EqualExpression,
        NotEqualExpression,
        GreaterThanExpression,
        GreaterThanEqualsExpression,
        LessThanExpression,
        LessThanEqualsExpression,
        ComparisonPrecedenceExpression,
        AndExpression,
        OrExpression,
        XorExpression,
        ArithmeticExpression,
        InlineAssignmentExpression,
        RequiredAssignmentExpression,
        OptionalAssignmentExpression,
        DeclarationAssignmentExpression,
        AttributeExpression,
        VarDefinitionExpression,
        LetDefinitionExpression,
        VariableDefinitionExpression

    };




    [[nodiscard]]
    std::string to_string(ExpressionType value);
}
namespace uul{
    template<>
    [[nodiscard]]
    hlspv::lang::ExpressionType from_string(std::string_view value);
    template<>
    [[nodiscard]]
    constexpr hlspv::lang::ExpressionType enum_max<hlspv::lang::ExpressionType>() noexcept{
        return hlspv::lang::ExpressionType::ModuleExpression;
    }
    template<>
    [[nodiscard]]
    constexpr hlspv::lang::ExpressionType enum_min<hlspv::lang::ExpressionType>() noexcept{
        return hlspv::lang::ExpressionType::VariableDefinitionExpression;
    }
}
#endif //VULKANWORKSPACE_LANG_EXPR_TYPE_H
