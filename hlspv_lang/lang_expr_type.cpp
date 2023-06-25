//
// Created by Bolt on 6/10/2023.
//

#include "lang_expr_type.h"

#include <uul/assert.h>

namespace hlspv::lang {
#define UUL_CREATE_ENUM_CASE_TO_STRING(X) \
        case X:{                          \
           return #X;                     \
        }                                 \
                                          \


#define UUL_CREATE_ENUM_CASE_FROM_STRING(X) \
        if(value == #X){                   \
           return X;                        \
        }                                   \
                                            \





    std::string to_string(ExpressionType value) {
        using enum ExpressionType;
        switch (value) {
            UUL_CREATE_ENUM_CASE_TO_STRING(ModuleExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(EqualsExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(MemberDeclaration)
            UUL_CREATE_ENUM_CASE_TO_STRING(StructDefinition)
            UUL_CREATE_ENUM_CASE_TO_STRING(ParameterDeclaration)
            UUL_CREATE_ENUM_CASE_TO_STRING(TemplateDefinition)
            UUL_CREATE_ENUM_CASE_TO_STRING(FunctionParameterList)
            UUL_CREATE_ENUM_CASE_TO_STRING(FunctionDefinition)
            UUL_CREATE_ENUM_CASE_TO_STRING(Literal)
            UUL_CREATE_ENUM_CASE_TO_STRING(GroupExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(RangeExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(RhsExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(NonArithmeticRhsExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(FunctionCallArgumentList)
            UUL_CREATE_ENUM_CASE_TO_STRING(TemplateInstantiation)
            UUL_CREATE_ENUM_CASE_TO_STRING(FunctionCall)
            UUL_CREATE_ENUM_CASE_TO_STRING(StatementExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(LoopStatementExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(ScopeStatement)
            UUL_CREATE_ENUM_CASE_TO_STRING(ConditionalStatement)
            UUL_CREATE_ENUM_CASE_TO_STRING(LoopScopeStatement)
            UUL_CREATE_ENUM_CASE_TO_STRING(LoopConditionalStatement)
            UUL_CREATE_ENUM_CASE_TO_STRING(WhileLoopStatement)
            UUL_CREATE_ENUM_CASE_TO_STRING(ForInStatement)
            UUL_CREATE_ENUM_CASE_TO_STRING(NormExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(UnaryMinusExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(UnaryPlusExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(UnaryNotExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(UnaryOperatorExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(PowExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(MulExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(DivExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(ModExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(MatMulExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(MulPrecedenceExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(AddExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(SubExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(AddPrecedenceExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(EqualExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(NotEqualExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(GreaterThanExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(GreaterThanEqualsExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(LessThanExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(LessThanEqualsExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(ComparisonPrecedenceExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(AndExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(OrExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(XorExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(ArithmeticExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(InlineAssignmentExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(RequiredAssignmentExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(OptionalAssignmentExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(DeclarationAssignmentExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(AttributeExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(VarDefinitionExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(LetDefinitionExpression)
            UUL_CREATE_ENUM_CASE_TO_STRING(VariableDefinitionExpression)
            default: {
                return "UNKNOWN";
            }
        }
    }
}

namespace uul {
    hlspv::lang::ExpressionType from_string(std::string_view value) {
        using enum hlspv::lang::ExpressionType;
        UUL_CREATE_ENUM_CASE_FROM_STRING(ModuleExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(EqualsExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(MemberDeclaration)
        UUL_CREATE_ENUM_CASE_FROM_STRING(StructDefinition)
        UUL_CREATE_ENUM_CASE_FROM_STRING(ParameterDeclaration)
        UUL_CREATE_ENUM_CASE_FROM_STRING(TemplateDefinition)
        UUL_CREATE_ENUM_CASE_FROM_STRING(FunctionParameterList)
        UUL_CREATE_ENUM_CASE_FROM_STRING(FunctionDefinition)
        UUL_CREATE_ENUM_CASE_FROM_STRING(Literal)
        UUL_CREATE_ENUM_CASE_FROM_STRING(GroupExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(RangeExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(RhsExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(NonArithmeticRhsExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(FunctionCallArgumentList)
        UUL_CREATE_ENUM_CASE_FROM_STRING(TemplateInstantiation)
        UUL_CREATE_ENUM_CASE_FROM_STRING(FunctionCall)
        UUL_CREATE_ENUM_CASE_FROM_STRING(StatementExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(LoopStatementExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(ScopeStatement)
        UUL_CREATE_ENUM_CASE_FROM_STRING(ConditionalStatement)
        UUL_CREATE_ENUM_CASE_FROM_STRING(LoopScopeStatement)
        UUL_CREATE_ENUM_CASE_FROM_STRING(LoopConditionalStatement)
        UUL_CREATE_ENUM_CASE_FROM_STRING(WhileLoopStatement)
        UUL_CREATE_ENUM_CASE_FROM_STRING(ForInStatement)
        UUL_CREATE_ENUM_CASE_FROM_STRING(NormExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(UnaryMinusExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(UnaryPlusExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(UnaryNotExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(UnaryOperatorExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(PowExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(MulExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(DivExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(ModExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(MatMulExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(MulPrecedenceExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(AddExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(SubExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(AddPrecedenceExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(EqualExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(NotEqualExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(GreaterThanExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(GreaterThanEqualsExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(LessThanExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(LessThanEqualsExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(ComparisonPrecedenceExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(AndExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(OrExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(XorExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(ArithmeticExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(InlineAssignmentExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(RequiredAssignmentExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(OptionalAssignmentExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(DeclarationAssignmentExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(AttributeExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(VarDefinitionExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(LetDefinitionExpression)
        UUL_CREATE_ENUM_CASE_FROM_STRING(VariableDefinitionExpression)
        UUL_ASSERT(false, "Shouldn't get here!");
    }
}