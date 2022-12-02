//
// Created by Shae Bolt on 9/28/2022.
//

#include "code_generation.h"
#include <fmt/core.h>
#include <range/v3/action/push_back.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/repeat.hpp>
#include <range/v3/range/conversion.hpp>
#include <sstream>
#include <cassert>

std::string hlspv::codegen::repeat(std::string_view string, std::size_t n) {
    std::string repeated;
    repeated.reserve(string.size() * n);
    for (const auto &value: ranges::views::repeat(string) |
                            ranges::views::take(n)) {
        repeated += value;
    }
    return repeated;
}

hlspv::codegen::TypeNameParameter::TypeNameParameter(std::string_view type,
                                                     std::string_view name,
                                                     std::string_view default_t,
                                                     bool member_scoped)
        : m_type(type), m_name(name), m_default(default_t), m_member_scoped(member_scoped) {

}

std::string hlspv::codegen::TypeNameParameter::build_header() const {
    std::string string;
    string = fmt::format("{} {}", m_type, m_name);
    if (m_default.empty()) {
        return string;
    } else {
        return fmt::format("{} = {}", string, m_default);
    }
}

hlspv::codegen::TypeNameParameter
hlspv::codegen::TypeNameParameter::class_scoped(std::string_view type,
                                                std::string_view name,
                                                std::string_view default_t) {
    return hlspv::codegen::TypeNameParameter(type,name, default_t, true);
}

std::string hlspv::codegen::TypeNameParameter::build_impl(
        const hlspv::codegen::ClassBuilder &class_builder) const {
    if(m_member_scoped){
        return fmt::format("{} {}", m_type, m_name);
    }else{
        return fmt::format("{0}::{1} {0}::{2}", class_builder.name(), m_type, m_name);
    }
}

std::string hlspv::codegen::TypeNameParameter::build_impl() const {
    return fmt::format("{} {}", m_type, m_name);
}

hlspv::codegen::TemplateParameter::TemplateParameter(
        std::string_view non_type_type,
        std::string_view name,
        std::string_view default_t) : m_non_type_type(non_type_type),
                                      m_name(name), m_default(default_t) {

}

hlspv::codegen::TemplateParameter
hlspv::codegen::TemplateParameter::from_typed(std::string_view name,
                                              std::string_view default_t) {
    return hlspv::codegen::TemplateParameter({}, name, default_t);
}


std::string hlspv::codegen::TemplateParameter::build() const {
    std::string string;
    if (m_non_type_type.empty()) {
        string = fmt::format("typename {}", m_name);
    } else {
        string = fmt::format("{} {}", m_non_type_type, m_name);
    }
    if (m_default.empty()) {
        return string;
    } else {
        return fmt::format("{} = {}", string, m_default);
    }
}

std::string hlspv::codegen::build_template_definition(
        std::span<const TemplateParameter> template_parameters) {
    std::string template_type_string;
    template_type_string += template_parameters.front().build();
    for (const auto &type: template_parameters | ranges::views::drop(1)) {
        template_type_string += ", " + type.build();
    }
    std::string template_definition = fmt::format("template<{}>",
                                                  template_type_string);
    return template_definition;
}

std::string hlspv::codegen::build_parameter_list_header(
        std::span<const TypeNameParameter> parameters) {
    std::string parameters_string;
    parameters_string += parameters.front().build_header();
    for (const auto &type: parameters | ranges::views::drop(1)) {
        parameters_string += ", " + type.build_header();
    }
    std::string template_definition = fmt::format("({})", parameters_string);
    return template_definition;
}

std::string hlspv::codegen::build_parameter_list_impl(
        const hlspv::codegen::ClassBuilder &class_builder,
        std::span<const TypeNameParameter> parameters) {
    std::string parameters_string;
    parameters_string += parameters.front().build_impl(class_builder);
    for (const auto &type: parameters | ranges::views::drop(1)) {
        parameters_string += ", " + type.build_impl(class_builder);
    }
    std::string template_definition = fmt::format("({})", parameters_string);
    return template_definition;
}
std::string hlspv::codegen::build_parameter_list_impl(
        std::span<const TypeNameParameter> parameters) {
    std::string parameters_string;
    parameters_string += parameters.front().build_impl();
    for (const auto &type: parameters | ranges::views::drop(1)) {
        parameters_string += ", " + type.build_impl();
    }
    std::string template_definition = fmt::format("({})", parameters_string);
    return template_definition;
}

std::string hlspv::codegen::prepend_lines(std::string_view string,
                                          std::string prepend_string) {
    std::stringstream ss((std::string(string)));
    std::string line;
    std::string prepended_string;
    while(std::getline(ss, line)){
        prepend_string += fmt::format("{}{}\n", prepend_string, line);
    }
    return prepend_string;
}

hlspv::codegen::MemberFunctionDefinitionFormatter
hlspv::codegen::lift_def_format(
        hlspv::codegen::MemberFunctionDefinitionFormatter definition_formatter) {
    return definition_formatter;
}

hlspv::codegen::MemberFunctionDefinitionFormatter
hlspv::codegen::lift_def_format(
        hlspv::codegen::FunctionDefinitionFormatter definition_formatter){
    return [definition_formatter = std::move(definition_formatter)](const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder &){
        return definition_formatter(prototype_builder);
    };
}

hlspv::codegen::MemberFunctionDefinitionFormatter
hlspv::codegen::lift_def_format(std::string_view string){
    return lift_def_format([string = std::string(string)](const FunctionPrototypeBuilder &){
        return string;
    });
}

hlspv::codegen::MemberFunctionPrototypeFormatter
hlspv::codegen::lift_proto_format(
        hlspv::codegen::MemberFunctionPrototypeFormatter prototype_formatter) {
    return prototype_formatter;
}

hlspv::codegen::MemberFunctionPrototypeFormatter
hlspv::codegen::lift_proto_format(
        hlspv::codegen::FunctionPrototypeFormatter prototype_formatter) {
    return [prototype_formatter = std::move(prototype_formatter)](const FunctionPrototypeBuilder & prototype_builder, const ClassBuilder &){
        return prototype_formatter(prototype_builder);
    };
}

hlspv::codegen::MemberFunctionPrototypeFormatter
hlspv::codegen::lift_proto_format(
        hlspv::codegen::FunctionPrototypeBuilder prototype_builder) {
    return lift_proto_format([prototype_builder = std::move(prototype_builder)](const FunctionPrototypeBuilder &){
        return prototype_builder;
    });
}

hlspv::codegen::FunctionPrototypeBuilder hlspv::codegen::as_const_formatter(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder){
    return prototype_builder.copy().set_const();
}

hlspv::codegen::FunctionPrototypeBuilder
hlspv::codegen::copy_constructor_formatter(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder,
        const hlspv::codegen::ClassBuilder &class_builder) {
    return prototype_builder.copy()
    .set_name(fmt::format("{0}", class_builder.name()))
    .set_parameters({TypeNameParameter::class_scoped(
            {fmt::format("const {}&", class_builder.name())}})
            //TODO need to deal with type parameter, currently... doesn't really work?
            //TODO use function for each parameter!
            //ie need const CLASS::NAME& to work in impl
            //impl vs non impl name?
            //auto search for the name backwards if given class name or nested names?
}

hlspv::codegen::FunctionPrototypeBuilder
hlspv::codegen::move_constructor_formatter(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder,
        const hlspv::codegen::ClassBuilder &class_builder) {
    return hlspv::codegen::FunctionPrototypeBuilder();
}

hlspv::codegen::FunctionPrototypeBuilder
hlspv::codegen::copy_assignment_formatter(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder,
        const hlspv::codegen::ClassBuilder &class_builder) {
    return hlspv::codegen::FunctionPrototypeBuilder();
}

hlspv::codegen::FunctionPrototypeBuilder
hlspv::codegen::move_assignment_formatter(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder,
        const hlspv::codegen::ClassBuilder &class_builder) {
    return hlspv::codegen::FunctionPrototypeBuilder();
}

hlspv::codegen::FunctionPrototypeBuilder
hlspv::codegen::default_constructor_formatter(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder,
        const hlspv::codegen::ClassBuilder &class_builder) {
    return hlspv::codegen::FunctionPrototypeBuilder();
}

hlspv::codegen::FunctionPrototypeBuilder hlspv::codegen::destructor_formatter(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder,
        const hlspv::codegen::ClassBuilder &class_builder) {
    return hlspv::codegen::FunctionPrototypeBuilder();
}


hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_name(std::string_view value) {
    m_name = std::string(value);
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_return_type(
        std::string_view value) {
    m_return_type = std::string(value);
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_parameters(
        std::vector<TypeNameParameter> values) {
    m_parameters = std::move(values);
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_template_types(
        std::vector<TemplateParameter> types) {
    m_template_types = std::move(types);
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_static(bool value) {
    m_static = value;
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_inline(bool value) {
    m_inline = value;
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_constexpr(bool value) {
    m_constexpr = value;
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_override(bool value) {
    m_override = value;
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_noexcept(bool value) {
    m_noexcept = value;
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_nodiscard(bool value) {
    m_nodiscard = value;
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_const(bool value) {
    m_const = value;
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_default(bool value) {
    m_default = value;
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder &
hlspv::codegen::FunctionPrototypeBuilder::set_delete(bool value) {
    m_delete = value;
    return *this;
}


bool hlspv::codegen::FunctionPrototypeBuilder::must_inline() const {
    return m_inline || m_constexpr || !m_template_types.empty();
}


std::string hlspv::codegen::FunctionPrototypeBuilder::build_header(
        std::string_view indent_spacing, std::size_t indent) const {
    std::string header;
    auto def_indent = repeat(indent_spacing, indent);
    if (!m_template_types.empty()) {
        auto template_definition = build_template_definition(m_template_types);
        header += fmt::format("{}{}\n", def_indent, template_definition);
    }
    if (m_nodiscard) {
        header += fmt::format("{}{}\n", def_indent, "[[nodiscard]]");
    }
    std::string def_line = def_indent;
    if (m_static) {
        def_line += "static ";
    }
    if (m_inline) {
        def_line += "inline ";
    }
    if (m_constexpr) {
        def_line += "constexpr ";
    }
    if(!m_return_type.empty()){
        def_line += fmt::format("{} ", m_return_type);
    }
    if(!m_name.empty()){
        def_line += fmt::format("{} ", m_name);
    }
    def_line += fmt::format("{}", build_parameter_list_header(m_parameters));
    if (m_const) {
        def_line += " const";
    }
    if (m_override) {
        def_line += " override";
    }
    if (m_noexcept) {
        def_line += " noexcept";
    }
    if (m_default) {
        def_line += " = default";
    }
    if (m_default) {
        def_line += " = delete";
    }
    header += def_line;
    return header;
}

std::string
hlspv::codegen::FunctionPrototypeBuilder::build_impl(
                                                     std::string_view indent_spacing,
                                                     std::size_t indent,
                                                     const ClassBuilder& class_builder) const {
    if (is_header_only()) {
        return "";
    }
    std::string impl;
    auto def_indent = repeat(indent_spacing, indent);
    std::string def_line = def_indent;
    if (m_static) {
        def_line += "static ";
    }

    if(!m_return_type.empty()){
        def_line += fmt::format("{} ", m_return_type);
    }
    if(!m_name.empty()){
        def_line += fmt::format("{}::{} ", class_builder.name(), m_name);
    }
    def_line += fmt::format("{}", build_parameter_list_impl(class_builder,m_parameters));
    if (m_const) {
        def_line += " const";
    }
    if (m_override) {
        def_line += " override";
    }
    if (m_noexcept) {
        def_line += " noexcept";
    }
    impl += def_line;
    return impl;
}

std::string hlspv::codegen::FunctionPrototypeBuilder::build_impl(
        std::string_view indent_spacing, std::size_t indent) const {
    if (is_header_only()) {
        return "";
    }
    std::string impl;
    auto def_indent = repeat(indent_spacing, indent);
    std::string def_line = def_indent;
    if (m_static) {
        def_line += "static ";
    }
    assert(!m_return_type.empty());
    def_line += fmt::format("{} ", m_return_type);
    if(!m_name.empty()){
        def_line += fmt::format("{} ", m_name);
    }
    def_line += fmt::format("{}", build_parameter_list_impl(m_parameters));
    if (m_const) {
        def_line += " const";
    }
    if (m_override) {
        def_line += " override";
    }
    if (m_noexcept) {
        def_line += " noexcept";
    }
    impl += def_line;
    return impl;
}

bool hlspv::codegen::FunctionPrototypeBuilder::is_header_only() const {
    return must_inline() || m_default || m_default;
}


const std::string &hlspv::codegen::FunctionPrototypeBuilder::name() const {
    return m_name;
}

hlspv::codegen::FunctionPrototypeBuilder
hlspv::codegen::FunctionPrototypeBuilder::copy() const {
    return *this;
}

hlspv::codegen::FunctionBuilder &hlspv::codegen::FunctionBuilder::set_prototype(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder) {
    return set_prototype([prototype_builder](const FunctionPrototypeBuilder&){
        return prototype_builder;
    });
}

hlspv::codegen::FunctionBuilder &hlspv::codegen::FunctionBuilder::set_prototype(
        hlspv::codegen::FunctionPrototypeFormatter prototype_formatter) {
   return set_prototype([prototype_formatter=std::move(prototype_formatter)](const FunctionPrototypeBuilder& prototype_builder, const ClassBuilder &){
       return prototype_formatter(prototype_builder);
   });
}

hlspv::codegen::FunctionBuilder &hlspv::codegen::FunctionBuilder::set_prototype(
        hlspv::codegen::MemberFunctionPrototypeFormatter prototype_formatter) {
    m_prototype_formatter = std::move(prototype_formatter);
    return *this;
}

hlspv::codegen::FunctionBuilder &
hlspv::codegen::FunctionBuilder::set_definition(std::string_view definition) {
    return set_definition([definition = std::string(definition)](const FunctionPrototypeBuilder&){
        return definition;
    });
}

hlspv::codegen::FunctionBuilder &
hlspv::codegen::FunctionBuilder::set_definition(
        hlspv::codegen::FunctionDefinitionFormatter definition_formatter) {
    return set_definition([definition_formatter = std::move(definition_formatter)](const FunctionPrototypeBuilder& prototype_builder, const ClassBuilder &){
        return definition_formatter(prototype_builder);
    });
}

hlspv::codegen::FunctionBuilder &
hlspv::codegen::FunctionBuilder::set_definition(
        hlspv::codegen::MemberFunctionDefinitionFormatter definition_formatter) {
    m_definition_formatter = std::move(definition_formatter);
    return *this;
}

hlspv::codegen::FunctionPrototypeBuilder
hlspv::codegen::FunctionBuilder::create_prototype_builder() const {
    return m_prototype_formatter(FunctionPrototypeBuilder{}, ClassBuilder{});
}

hlspv::codegen::FunctionPrototypeBuilder
hlspv::codegen::FunctionBuilder::create_prototype_builder(
        const hlspv::codegen::ClassBuilder &class_builder) const {
    return m_prototype_formatter(FunctionPrototypeBuilder{}, class_builder);
}

std::string hlspv::codegen::FunctionBuilder::create_definition(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder) const {
    return m_definition_formatter(prototype_builder, ClassBuilder{});
}

std::string hlspv::codegen::FunctionBuilder::create_definition(
        const hlspv::codegen::FunctionPrototypeBuilder &prototype_builder,
        const hlspv::codegen::ClassBuilder &class_builder) const {
    return m_definition_formatter(prototype_builder, class_builder);
}

std::string
hlspv::codegen::FunctionBuilder::build_header(std::string_view indent_spacing,
                                              std::size_t indent) const {
    return build_header(indent_spacing, indent, ClassBuilder{});
}

std::string
hlspv::codegen::FunctionBuilder::build_header(std::string_view indent_spacing,
                                              std::size_t indent,
                                              const hlspv::codegen::ClassBuilder &class_builder) const {
    auto prototype_builder = create_prototype_builder(class_builder);
    auto definition = create_definition(prototype_builder, class_builder);
    auto header = prototype_builder.build_header(indent_spacing, indent);
    if(prototype_builder.must_inline() || class_builder.is_header_only()){
        auto def_indent = repeat(indent_spacing, indent);
        auto body_indent = repeat(indent_spacing, indent+1);
        header += fmt::format("{{\n"
                              "{}"
                              "{}}}\n", prepend_lines(definition, body_indent), def_indent);
    }else{
        header += ";\n";
    }
    return header;
}

std::string
hlspv::codegen::FunctionBuilder::build_impl(std::string_view indent_spacing,
                                            std::size_t indent) const {
    return build_impl(indent_spacing, indent, ClassBuilder{});
}

std::string
hlspv::codegen::FunctionBuilder::build_impl(std::string_view indent_spacing,
                                            std::size_t indent,
                                            const ClassBuilder& class_builder) const {
    auto prototype_builder = create_prototype_builder(class_builder);
    auto definition = create_definition(prototype_builder, class_builder);
    if(prototype_builder.must_inline() || class_builder.is_header_only()){
        return "";
    }else{
        auto impl = prototype_builder.build_impl(indent_spacing, indent, class_builder);
        auto def_indent = repeat(indent_spacing, indent);
        auto body_indent = repeat(indent_spacing, indent+1);
        impl += fmt::format("{{\n"
                              "{}"
                              "{}}}\n", prepend_lines(definition, body_indent), def_indent);
        return impl;
    }
}



hlspv::codegen::FunctionBuilder hlspv::codegen::FunctionBuilder::copy() const {
    return *this;
}

hlspv::codegen::FunctionBuilder
hlspv::codegen::FunctionBuilder::const_copy(bool value) const {
    return copy().set_prototype(m_prototype_formatter | as_const_formatter);
}




hlspv::codegen::VariableBuilder &
hlspv::codegen::VariableBuilder::set_name(std::string_view name) {
    m_name = std::string(name);
    return *this;
}

hlspv::codegen::VariableBuilder &
hlspv::codegen::VariableBuilder::set_type(std::string_view type) {
    m_type = std::string(type);
    return *this;
}

hlspv::codegen::VariableBuilder &
hlspv::codegen::VariableBuilder::set_static(bool value) {
    m_static = value;
    return *this;
}

hlspv::codegen::VariableBuilder &
hlspv::codegen::VariableBuilder::set_constexpr(bool value) {
    m_constexpr = value;
    return *this;
}

hlspv::codegen::VariableBuilder &
hlspv::codegen::VariableBuilder::set_assignment(
        std::string_view assignment) {
    m_assignment = std::string(assignment);
    return *this;
}

hlspv::codegen::VariableBuilder &
hlspv::codegen::VariableBuilder::set_const(bool value) {
    m_const = value;
    return *this;
}

std::string hlspv::codegen::VariableBuilder::build() const {
    std::string declaration;
    if (m_static) {
        declaration += "static ";
    }
    if (m_constexpr) {
        declaration += "constexpr ";
    }
    if (m_const) {
        declaration += "const ";
    }
    declaration += fmt::format("{} {}", m_type, m_name);
    if (!m_assignment.empty()) {
        return declaration + fmt::format(" = {};", m_assignment);
    }else{
        return declaration + ";";
    }
}

hlspv::codegen::VariableBuilder hlspv::codegen::VariableBuilder::copy() const {
    return *this;
}

hlspv::codegen::MemberVariableBuilder &
hlspv::codegen::MemberVariableBuilder::set_assignment(
        hlspv::codegen::MemberFormatFunction format_function) {
    m_assignment_format = std::move(format_function);
    return *this;
}

std::string hlspv::codegen::MemberVariableBuilder::build(
        const hlspv::codegen::ClassBuilder &class_builder) const {
    auto temp = m_variable_builder;
    if (m_assignment_format) {
        temp.set_assignment(m_assignment_format(class_builder));
    }
    return temp.build();
}

hlspv::codegen::MemberVariableBuilder &
hlspv::codegen::MemberVariableBuilder::set_name(std::string_view name) {
    m_variable_builder.set_name(name);
    return *this;
}

hlspv::codegen::MemberVariableBuilder &
hlspv::codegen::MemberVariableBuilder::set_type(std::string_view type) {
    m_variable_builder.set_type(type);
    return *this;
}

hlspv::codegen::MemberVariableBuilder &
hlspv::codegen::MemberVariableBuilder::set_const(bool value) {
    m_variable_builder.set_const(value);
    return *this;
}

hlspv::codegen::MemberVariableBuilder &
hlspv::codegen::MemberVariableBuilder::set_static(bool value) {
    m_variable_builder.set_static(value);
    return *this;
}

hlspv::codegen::MemberVariableBuilder &
hlspv::codegen::MemberVariableBuilder::set_constexpr(bool value) {
    m_variable_builder.set_constexpr(value);
    return *this;
}

hlspv::codegen::MemberVariableBuilder &
hlspv::codegen::MemberVariableBuilder::set_assignment(
        std::string_view assignment) {
    m_assignment_format = [assignment_ret = std::string(assignment)](const ClassBuilder &){
        return assignment_ret;
    };
    return *this;
}

hlspv::codegen::MemberVariableBuilder
hlspv::codegen::MemberVariableBuilder::copy() const {
    return *this;
}

hlspv::codegen::EnumClassBuilder &
hlspv::codegen::EnumClassBuilder::set_name(std::string_view name) {
    m_name = std::string(name);
    return *this;
}

hlspv::codegen::EnumClassBuilder &
hlspv::codegen::EnumClassBuilder::set_underlying_type(
        std::string_view underlying_type) {
    m_underlying_type = std::string(underlying_type);
    return *this;
}

hlspv::codegen::EnumClassBuilder &
hlspv::codegen::EnumClassBuilder::add_enumerations(
        std::vector<std::string> names) {
    ranges::push_back(m_enumerations, std::move(names));
    return *this;
}

hlspv::codegen::EnumClassBuilder &
hlspv::codegen::EnumClassBuilder::set_enumerations(
        std::vector<std::string> names) {
    m_enumerations = std::move(names);
    return *this;
}

hlspv::codegen::EnumClassBuilder &
hlspv::codegen::EnumClassBuilder::set_numbering_system(
        hlspv::codegen::EnumClassNumberingSystem numbering_system) {
    m_numbering_system = numbering_system;
    return *this;
}

hlspv::codegen::EnumClassBuilder
hlspv::codegen::EnumClassBuilder::copy() const {
    return *this;
}

hlspv::codegen::UsingDeclarationBuilder &
hlspv::codegen::UsingDeclarationBuilder::set_name(std::string_view name) {
    m_name = std::string(name);
    return *this;
}

hlspv::codegen::UsingDeclarationBuilder &
hlspv::codegen::UsingDeclarationBuilder::set_definition(
        std::string_view definition) {
    m_definition = std::string(definition);
    return *this;
}

hlspv::codegen::UsingDeclarationBuilder
hlspv::codegen::UsingDeclarationBuilder::copy() const {
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_struct(bool value) {
    m_struct = value;
    return *this;
}


hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_name(std::string_view value) {
    m_name = std::string(value);
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_namespace(std::string_view value) {
    m_namespace = std::string(value);
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_private_functions(
        std::vector<FunctionBuilder> functions) {
    m_private_functions = std::move(functions);
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_public_functions(
        std::vector<FunctionBuilder> functions) {
    m_public_functions = std::move(functions);
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_protected_functions(
        std::vector<FunctionBuilder> functions) {
    m_protected_functions = std::move(functions);
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::add_private_functions(
        std::vector<FunctionBuilder> functions) {
    ranges::push_back(m_private_functions, std::move(functions));
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::add_public_functions(
        std::vector<FunctionBuilder> functions) {
    ranges::push_back(m_public_functions, std::move(functions));
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::add_protected_functions(
        std::vector<FunctionBuilder> functions) {
    ranges::push_back(m_protected_functions, std::move(functions));
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_private_variables(
        std::vector<MemberVariableBuilder> variables) {
    m_private_variables = std::move(variables);
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_public_variables(
        std::vector<MemberVariableBuilder> variables) {
    m_public_variables = std::move(variables);
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::set_protected_variables(
        std::vector<MemberVariableBuilder> variables) {
    m_protected_variables = std::move(variables);
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::add_private_variables(
        std::vector<MemberVariableBuilder> variables) {
    ranges::push_back(m_private_variables, std::move(variables));
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::add_public_variables(
        std::vector<MemberVariableBuilder> variables) {
    ranges::push_back(m_public_variables, std::move(variables));
    return *this;
}

hlspv::codegen::ClassBuilder &
hlspv::codegen::ClassBuilder::add_protected_variables(
        std::vector<MemberVariableBuilder> variables) {
    ranges::push_back(m_protected_variables, std::move(variables));
    return *this;
}

hlspv::codegen::ClassBuilder &hlspv::codegen::ClassBuilder::set_template_types(
        std::vector<TemplateParameter> types) {
    m_template_types = std::move(types);
    return *this;
}


//TODO support constexpr function and inline.
std::string hlspv::codegen::ClassBuilder::build_header() const {
    std::string header;
    std::uint32_t indent = 0;
    std::string indent_spacing = "    ";
    if (!m_namespace.empty()) {
        indent += 1;
    }

    if (!m_template_types.empty()) {
        auto template_definition = build_template_definition(m_template_types);
        header += fmt::format("{}{}\n", repeat(indent_spacing, indent),
                              template_definition);
    }
    std::string section_indent = repeat(indent_spacing, indent);
    indent += 1;
    header += fmt::format("{}public:\n", section_indent);
    std::string member_indent = repeat(indent_spacing, indent);
    for (const auto &member_variable: m_public_variables) {
        header += fmt::format("{}{}\n", member_indent,
                              member_variable.build(*this));
    }
    for (const auto &member_functions: m_public_functions) {
        header +=  member_functions.build_header(indent_spacing, indent, *this);
    }
    header += fmt::format("{}protected:\n", section_indent);
    for (const auto &member_variable: m_protected_variables) {
        header += fmt::format("{}{}\n", member_indent,
                              member_variable.build(*this));
    }
    for (const auto &member_functions: m_protected_functions) {
        header +=  member_functions.build_header(indent_spacing, indent, *this);
    }
    header += fmt::format("{}private:\n", section_indent);
    for (const auto &member_variable: m_private_variables) {
        header += fmt::format("{}{}\n", member_indent,
                              member_variable.build(*this));
    }
    for (const auto &member_functions: m_private_functions) {
        header +=  member_functions.build_header(indent_spacing, indent, *this);
    }
    indent -= 1;

    std::string type_string = m_struct ? "struct" : "class";
    header = fmt::format("{0}{3} {1}{{\n"
                          "{2}"
                          "{0}}};\n", repeat(indent_spacing, indent), m_name,
                          header, type_string);
    if (!m_namespace.empty()) {
        header = fmt::format("namespace {}{{\n"
                             "{}"
                             "}}\n", m_namespace, header);
    }
    return header;
}

std::string hlspv::codegen::ClassBuilder::build_impl() const {
    if(is_header_only()){
        return "";
    }
    std::string impl;
    std::uint32_t indent = 0;
    std::string indent_spacing = "    ";
    if (!m_namespace.empty()) {
        indent += 1;
    }
    std::string member_indent = repeat(indent_spacing, indent);
    for (const auto &member_functions: m_public_functions) {
        impl +=  member_functions.build_impl(indent_spacing, indent, *this);
    }
    for (const auto &member_functions: m_protected_functions) {
        impl +=  member_functions.build_impl(indent_spacing, indent, *this);
    }
    for (const auto &member_functions: m_private_functions) {
        impl +=  member_functions.build_impl(indent_spacing, indent, *this);
    }
    if (!m_namespace.empty()) {
        impl = fmt::format("namespace {}{{\n"
                             "{}"
                             "}}\n", m_namespace, impl);
    }
    return impl;
}


hlspv::codegen::HeaderImplPair hlspv::codegen::ClassBuilder::build() const {
    return HeaderImplPair{build_header(), build_impl()};
}

bool hlspv::codegen::ClassBuilder::is_header_only() const {
    return !m_template_types.empty();
}

const std::string& hlspv::codegen::ClassBuilder::name() const{
    return m_name;
}

hlspv::codegen::ClassBuilder hlspv::codegen::ClassBuilder::copy() const {
    return *this;
}




