#ifndef SIMPLESS_PARSER_HPP
#define SIMPLESS_PARSER_HPP

#include <SimpleSS/Result.hpp>
#include <SimpleSS/StyleSheet.hpp>
#include <SimpleSS/priv/Lexer.hpp>

#include <any>
#include <memory>
#include <vector>


namespace simss{
    Result<StyleSheet> parse(const std::vector<Token>& tokens);
}

#endif