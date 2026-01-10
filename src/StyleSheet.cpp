#include <SimpleSS/StyleSheet.hpp>
#include <SimpleSS/priv/Lexer.hpp>
#include <SimpleSS/priv/Parser.hpp>

#include <fstream>

namespace simss {







    
    Result<StyleSheet> loadFromString(const std::string& source) {
        auto tokens = lex(source);
        return parse(tokens);
    }
    Result<StyleSheet> loadFromFile(const std::string& path) {
        std::ifstream stream(path);
        if (!stream) {
            return { {}, {"ERR: Could not open file: " + path} };
        }
        std::string str;
        std::getline(stream, str, '\0');
        return loadFromString(str);
    }
}