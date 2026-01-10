#ifndef SIMPLESS_LEXER_HPP
#define SIMPLESS_LEXER_HPP

#include <string>
#include <vector>


namespace simss{
    struct Token{
        enum class Type{
            None,
            Identifier,
            Number,
            String,
            Symbol,



            Usage,
            SubName,
            FuncCall,
            State,
            Property,   
            Bool,
               
            
            

            Object,
            LiteralNumber, // Number with user literal attached (e.g., 10px)
            Path,
        } type;
      
        std::string value;
        size_t line = 0;
        size_t column = 0;

        std::vector<std::string> subvalues;
        std::vector<Token> children;
    };
    std::vector<Token> lex(const std::string& source);
}

#endif