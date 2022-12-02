//
// Created by Shae Bolt on 9/28/2022.
//

#ifndef VULKANWORKSPACE_CODE_GENERATION_H
#define VULKANWORKSPACE_CODE_GENERATION_H



#include <string_view>
#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <span>
#include <concepts>

namespace hlspv::codegen {
    class ClassBuilder;

    struct HeaderImplPair {
        std::string header;
        std::string impl;
    };

    [[nodiscard]]
    std::string
    prepend_lines(std::string_view string, std::string prepend_string);

    [[nodiscard]]
    std::string repeat(std::string_view string, std::size_t n);

    class TypeNameParameter {
    public:
        TypeNameParameter() = default;

        TypeNameParameter(std::string_view type, std::string_view name = {},
                          std::string_view default_t = {},
                          bool member_scoped = false);

        static TypeNameParameter
        class_scoped(std::string_view type, std::string_view name = {},
                     std::string_view default_t = {});

        [[nodiscard]]
        std::string build_header() const;

        [[nodiscard]]
        std::string build_impl(const ClassBuilder &class_builder) const;

        [[nodiscard]]
        std::string build_impl() const;

    private:
        std::string m_type;
        std::string m_name;
        std::string m_default;
        bool m_member_scoped;
    };


    class TemplateParameter {
    public:
        TemplateParameter() = default;

        TemplateParameter(std::string_view non_type_type, std::string_view name,
                          std::string_view default_t = {});

        [[nodiscard]]
        static TemplateParameter
        from_typed(std::string_view name, std::string_view default_t = {});

        [[nodiscard]]
        std::string build() const;

    private:
        std::string m_non_type_type;
        std::string m_name;
        std::string m_default;
    };

    [[nodiscard]]
    std::string build_template_definition(
            std::span<const TemplateParameter> template_parameters);

    [[nodiscard]]
    std::string build_parameter_list_header(
            std::span<const TypeNameParameter> parameters);

    [[nodiscard]]
    std::string build_parameter_list_impl(const ClassBuilder &class_builder,
                                          std::span<const TypeNameParameter> parameters);

    [[nodiscard]]
    std::string
    build_parameter_list_impl(std::span<const TypeNameParameter> parameters);

    class FunctionPrototypeBuilder {
    public:
        FunctionPrototypeBuilder &set_name(std::string_view value);

        FunctionPrototypeBuilder &set_return_type(std::string_view value);

        FunctionPrototypeBuilder &
        set_parameters(std::vector<TypeNameParameter> values);

        FunctionPrototypeBuilder &
        set_template_types(std::vector<TemplateParameter> types);

        FunctionPrototypeBuilder &set_static(bool value = true);

        FunctionPrototypeBuilder &set_inline(bool value = true);

        FunctionPrototypeBuilder &set_constexpr(bool value = true);

        FunctionPrototypeBuilder &set_override(bool value = true);

        FunctionPrototypeBuilder &set_noexcept(bool value = true);

        FunctionPrototypeBuilder &set_nodiscard(bool value = true);

        FunctionPrototypeBuilder &set_const(bool value = true);

        FunctionPrototypeBuilder &set_default(bool value = true);

        FunctionPrototypeBuilder &set_delete(bool value = true);

        [[nodiscard]]
        bool must_inline() const;

        [[nodiscard]]
        bool is_header_only() const;

        [[nodiscard]]
        std::string
        build_header(std::string_view indent_spacing, std::size_t indent) const;

        [[nodiscard]]
        std::string
        build_impl(std::string_view indent_spacing, std::size_t indent,
                   const ClassBuilder &class_builder) const;

        [[nodiscard]]
        std::string
        build_impl(std::string_view indent_spacing, std::size_t indent) const;

        [[nodiscard]]
        const std::string &name() const;

        [[nodiscard]]
        FunctionPrototypeBuilder copy() const;

    private:
        std::string m_name;
        std::string m_return_type;
        std::vector<TypeNameParameter> m_parameters;
        std::vector<TemplateParameter> m_template_types;
        bool m_static = false;
        bool m_inline = false;
        bool m_constexpr = false;
        bool m_override = false;
        bool m_noexcept = false;
        bool m_nodiscard = false;
        bool m_const = false;
        bool m_default = false;
        bool m_delete = false;
    };

    using MemberFunctionDefinitionFormatter = std::function<std::string(
            const FunctionPrototypeBuilder &, const ClassBuilder &)>;

    using FunctionDefinitionFormatter = std::function<std::string(
            const FunctionPrototypeBuilder &)>;

    template<class T>
    concept MemberDefinitionFormatInvocable = std::invocable<T, const FunctionPrototypeBuilder &, const ClassBuilder &>
    && requires(T t, const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder){
        {t(prototype_builder, class_builder)} -> std::convertible_to<std::string>;
    };

    template<class T>
    concept DefinitionFormatInvocable = std::invocable<T, const FunctionPrototypeBuilder &>
                                        && requires(T t, const FunctionPrototypeBuilder & prototype_builder){
        {t(prototype_builder)} -> std::convertible_to<std::string>;
    };
    template<class T>
    concept DefinitionFormatComposable = MemberDefinitionFormatInvocable<T> || DefinitionFormatInvocable<T>;
    template<class T>
    concept DefinitionFormatComposableOrString =
    DefinitionFormatComposable<T>
    || std::is_same_v<T, std::string>
    || std::is_same_v<T, std::string_view>;

    MemberFunctionDefinitionFormatter lift_def_format(MemberFunctionDefinitionFormatter definition_formatter);
    MemberFunctionDefinitionFormatter lift_def_format(FunctionDefinitionFormatter definition_formatter);
    MemberFunctionDefinitionFormatter lift_def_format(std::string_view string);

    MemberFunctionDefinitionFormatter
    operator | (DefinitionFormatComposable auto &&lhs, DefinitionFormatComposableOrString auto&& rhs){
        return [lhs = lift_def_format(lhs), rhs = lift_def_format(rhs)](const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder){
            return lhs(prototype_builder, class_builder) + rhs(prototype_builder, class_builder);
        };
    }

    MemberFunctionDefinitionFormatter
    operator | (DefinitionFormatComposableOrString auto&& lhs, DefinitionFormatComposable auto &&rhs){
        return [lhs = lift_def_format(lhs), rhs = lift_def_format(rhs)](const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder){
            return lhs(prototype_builder, class_builder) + rhs(prototype_builder, class_builder);
        };
    }

    using MemberFunctionPrototypeFormatter = std::function<FunctionPrototypeBuilder(
    const FunctionPrototypeBuilder &,
            const ClassBuilder &)>;
    using FunctionPrototypeFormatter = std::function<FunctionPrototypeBuilder(const FunctionPrototypeBuilder &)>;

    template<class T>
    concept MemberPrototypeFormatInvocable = std::invocable<T, const FunctionPrototypeBuilder &, const ClassBuilder &>
                                              && requires(T t, const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder){
        {t(prototype_builder, class_builder)} -> std::convertible_to<FunctionPrototypeBuilder>;
    };

    template<class T>
    concept PrototypeFormatInvocable = std::invocable<T, const FunctionPrototypeBuilder &>
                                        && requires(T t, const FunctionPrototypeBuilder & prototype_builder){
        {t(prototype_builder)} -> std::convertible_to<FunctionPrototypeBuilder>;
    };
    template<class T>
    concept PrototypeFormatComposable = MemberPrototypeFormatInvocable<T> || PrototypeFormatInvocable<T>;

    MemberFunctionPrototypeFormatter lift_proto_format(MemberFunctionPrototypeFormatter prototype_formatter);
    MemberFunctionPrototypeFormatter lift_proto_format(FunctionPrototypeFormatter prototype_formatter);
    MemberFunctionPrototypeFormatter lift_proto_format(FunctionPrototypeBuilder prototype_builder);


//    MemberFunctionPrototypeFormatter compose(PrototypeFormatComposable auto &&lhs, PrototypeFormatComposable auto&& rhs){
//        return [lhs = lift_proto_format(lhs), rhs = lift_proto_format(rhs)](const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder){
//            return rhs(lhs(prototype_builder, class_builder), class_builder);
//        };
//    }
    MemberFunctionPrototypeFormatter
    operator | (PrototypeFormatComposable auto &&lhs, PrototypeFormatComposable auto &&rhs){
        return [lhs = lift_proto_format(lhs), rhs = lift_proto_format(rhs)](const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder){
            return rhs(lhs(prototype_builder, class_builder), class_builder);
        };
    }


    FunctionPrototypeBuilder as_const_formatter(const FunctionPrototypeBuilder & prototype_builder);

    FunctionPrototypeBuilder copy_constructor_formatter(const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder);
    FunctionPrototypeBuilder move_constructor_formatter(const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder);
    FunctionPrototypeBuilder copy_assignment_formatter(const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder);
    FunctionPrototypeBuilder move_assignment_formatter(const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder);
    FunctionPrototypeBuilder default_constructor_formatter(const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder);
    FunctionPrototypeBuilder destructor_formatter(const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder & class_builder);
    class FunctionBuilder {
    public:
        FunctionBuilder &
        set_prototype(const FunctionPrototypeBuilder &prototype_builder);

        FunctionBuilder &
        set_prototype(FunctionPrototypeFormatter prototype_formatter);

        FunctionBuilder &
        set_prototype(MemberFunctionPrototypeFormatter prototype_formatter);

        FunctionBuilder &
        set_definition(std::string_view definition);

        FunctionBuilder &
        set_definition(FunctionDefinitionFormatter definition_formatter);

        FunctionBuilder &
        set_definition(MemberFunctionDefinitionFormatter definition_formatter);

        [[nodiscard]]
        FunctionPrototypeBuilder create_prototype_builder() const;

        [[nodiscard]]
        FunctionPrototypeBuilder
        create_prototype_builder(const ClassBuilder &class_builder) const;

        [[nodiscard]]
        std::string create_definition(
                const FunctionPrototypeBuilder &prototype_builder) const;

        [[nodiscard]]
        std::string
        create_definition(const FunctionPrototypeBuilder &prototype_builder,
                          const ClassBuilder &class_builder) const;

        [[nodiscard]]
        std::string
        build_header(std::string_view indent_spacing, std::size_t indent) const;

        [[nodiscard]]
        std::string
        build_header(std::string_view indent_spacing, std::size_t indent,
                     const ClassBuilder &class_builder) const;

        [[nodiscard]]
        std::string
        build_impl(std::string_view indent_spacing, std::size_t indent) const;

        [[nodiscard]]
        std::string
        build_impl(std::string_view indent_spacing, std::size_t indent,
                   const ClassBuilder &class_builder) const;


        [[nodiscard]]
        FunctionBuilder copy() const;

        [[nodiscard]]
        FunctionBuilder const_copy(bool value = true) const;

    protected:
        MemberFunctionPrototypeFormatter m_prototype_formatter;
        MemberFunctionDefinitionFormatter m_definition_formatter;
    };

    using MemberFormatFunction = std::function<std::string(
            const ClassBuilder &)>;


    class VariableBuilder {
    public:
        VariableBuilder &set_name(std::string_view name);

        VariableBuilder &set_type(std::string_view type);

        VariableBuilder &set_const(bool value = true);

        VariableBuilder &set_static(bool value = true);

        VariableBuilder &set_constexpr(bool value = true);

        VariableBuilder &set_assignment(std::string_view assignment);

        [[nodiscard]]
        std::string build() const;

        [[nodiscard]]
        VariableBuilder copy() const;

    private:
        std::string m_name;
        std::string m_type;
        std::string m_assignment;
        bool m_static = false;
        bool m_constexpr = false;
        bool m_const = false;
    };

    class MemberVariableBuilder {
    public:
        MemberVariableBuilder &set_name(std::string_view name);

        MemberVariableBuilder &set_type(std::string_view type);

        MemberVariableBuilder &set_const(bool value = true);

        MemberVariableBuilder &set_static(bool value = true);

        MemberVariableBuilder &set_constexpr(bool value = true);

        MemberVariableBuilder &set_assignment(std::string_view assignment);

        MemberVariableBuilder &
        set_assignment(MemberFormatFunction format_function);

        [[nodiscard]]
        std::string build(const ClassBuilder &class_builder) const;

        [[nodiscard]]
        MemberVariableBuilder copy() const;

    private:
        VariableBuilder m_variable_builder;
        MemberFormatFunction m_assignment_format;
    };

    class UsingDeclarationBuilder {
    public:
        UsingDeclarationBuilder &set_name(std::string_view name);

        UsingDeclarationBuilder &set_definition(std::string_view definition);

        [[nodiscard]]
        UsingDeclarationBuilder copy() const;

    private:
        std::string m_name;
        std::string m_definition;
    };

    enum class EnumClassNumberingSystem {
        Automatic,
        Incremental,
        PowerOfTwo
    };

    //TODO use function to build enum index, not enum!
    class EnumClassBuilder {
    public:
        EnumClassBuilder &set_name(std::string_view name);

        EnumClassBuilder &set_underlying_type(std::string_view underlying_type);

        EnumClassBuilder &add_enumerations(std::vector<std::string> names);

        EnumClassBuilder &set_enumerations(std::vector<std::string> names);

        EnumClassBuilder &
        set_numbering_system(EnumClassNumberingSystem numbering_system);

        [[nodiscard]]
        EnumClassBuilder copy() const;

    private:
        std::string m_name;
        std::string m_underlying_type;
        std::vector<std::string> m_enumerations;
        EnumClassNumberingSystem m_numbering_system = EnumClassNumberingSystem::Automatic;
    };



    class ClassBuilder {
    public:

        ClassBuilder &set_struct(bool value = true);

        ClassBuilder &set_name(std::string_view value);

        ClassBuilder &set_namespace(std::string_view value);

        ClassBuilder &
        set_private_variables(
                std::vector<MemberVariableBuilder> variables);

        ClassBuilder &
        set_public_variables(
                std::vector<MemberVariableBuilder> variables);

        ClassBuilder &
        set_protected_variables(
                std::vector<MemberVariableBuilder> variables);

        ClassBuilder &
        add_private_variables(
                std::vector<MemberVariableBuilder> variables);

        ClassBuilder &
        add_public_variables(
                std::vector<MemberVariableBuilder> variables);

        ClassBuilder &
        add_protected_variables(
                std::vector<MemberVariableBuilder> variables);

        ClassBuilder &
        set_private_functions(std::vector<FunctionBuilder> functions);

        ClassBuilder &
        set_public_functions(std::vector<FunctionBuilder> functions);

        ClassBuilder &
        set_protected_functions(std::vector<FunctionBuilder> functions);

        ClassBuilder &
        add_private_functions(std::vector<FunctionBuilder> functions);

        ClassBuilder &
        add_public_functions(std::vector<FunctionBuilder> functions);

        ClassBuilder &
        add_protected_functions(std::vector<FunctionBuilder> functions);


        ClassBuilder &set_template_types(std::vector<TemplateParameter> types);


        [[nodiscard]]
        std::string build_header() const;

        [[nodiscard]]
        std::string build_impl() const;

        [[nodiscard]]
        HeaderImplPair build() const;

        [[nodiscard]]
        bool is_header_only() const;

        const std::string &name() const;

        [[nodiscard]]
        ClassBuilder copy() const;

    private:
        bool m_struct = false;
        std::string m_name;
        std::string m_namespace;

        std::vector<TemplateParameter> m_template_types;

        std::vector<MemberVariableBuilder> m_private_variables;
        std::vector<MemberVariableBuilder> m_protected_variables;
        std::vector<MemberVariableBuilder> m_public_variables;

        std::vector<FunctionBuilder> m_private_functions;
        std::vector<FunctionBuilder> m_protected_functions;
        std::vector<FunctionBuilder> m_public_functions;


    };
}
#endif //VULKANWORKSPACE_CODE_GENERATION_H
