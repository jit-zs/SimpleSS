#include <SimpleSS/priv/Lexer.hpp>

#include <algorithm>
#include <functional>
#include <map>



namespace simss {

    std::vector<Token> lex(const std::string& source) {
        std::vector<Token> tokens;
        std::string::const_iterator it = source.begin();
        size_t line = 1;
        size_t column = 1;
        static const std::function<void()> increment = [&]() {
            it++;
            if (*it == '\n') {
                line++;
                column = 1;
            }
            else {
                column++;
            }
            };
        while (it != source.end()) {
            if (std::isalpha(*it)) {
                std::string value;
                while (it != source.end() && (std::isalnum(*it) || *it == '_')) {
                    value += *it;
                    increment();
                }
                tokens.push_back({ Token::Type::Identifier, value , line, column });
            }
            else if (std::isdigit(*it)) {
                std::string value;
                while (it != source.end() && std::isdigit(*it)) {
                    value += *it;
                    increment();
                }
                tokens.push_back({ Token::Type::Number, value , line, column });
            }
            else if (*it == '"') {
                std::string value;
                increment();
                while (it != source.end() && *it != '"') {
                    value += *it;
                    increment();
                }
                increment();
                tokens.push_back({ Token::Type::String, value , line, column });
            }
            else if (!std::isspace(*it)) {
                tokens.push_back({ Token::Type::Symbol, std::string(1, *it) , line, column });
                increment();
            }
            else {
                increment();
            }

        }


        // Remove comments and combine tokens
        for (size_t i = 0; i < tokens.size() - 1; i++) {
            Token& currentToken = tokens[i];
            Token& nextToken = tokens[i + 1];

            if (currentToken.value == "/" && nextToken.value == "/") {
                tokens.erase(tokens.begin() + i, std::find_if(tokens.begin() + i, tokens.end(), [&](const Token& tok) {
                    return tok.line != currentToken.line;
                    }));
                i--;
            }
            else if (currentToken.value == "/" && nextToken.value == "*") {
                tokens.erase(tokens.begin() + i, std::find_if(tokens.begin() + i, tokens.end() - 1, [&](const Token& tok) {
                    return tok.value == "*" && (&tok + 1)->value == "/";
                    }) + 2);
                i--;
            }
            else if (currentToken.value == "true" || currentToken.value == "false") {
                currentToken.type = Token::Type::Bool;
            }
            else if (currentToken.value == "@" && nextToken.type == Token::Type::Identifier) {
                currentToken.type = Token::Type::Usage;
                currentToken.value = nextToken.value;
                tokens.erase(tokens.begin() + i + 1);
            }
            else if (currentToken.value == "." && nextToken.type == Token::Type::Identifier) {
                currentToken.type = Token::Type::SubName;
                currentToken.value += nextToken.value;
                tokens.erase(tokens.begin() + i + 1);
            }
            else if (currentToken.type == Token::Type::Number && nextToken.value == ".") {
                currentToken.value += ".";
                tokens.erase(tokens.begin() + i + 1);
                if (i < tokens.size() - 1 && tokens[i + 1].type == Token::Type::Number) {
                    currentToken.value += tokens[i + 1].value;
                    tokens.erase(tokens.begin() + i + 1);
                }
            }
            else if (currentToken.value == "." && nextToken.type == Token::Type::Number) {
                currentToken.type = Token::Type::Number;
                currentToken.value += nextToken.value;
                tokens.erase(tokens.begin() + i + 1);
            }
            else if (currentToken.value == "|" && nextToken.type == Token::Type::Identifier) {
                currentToken.type = Token::Type::State;
                currentToken.value = tokens[i + 1].value;
                tokens.erase(tokens.begin() + i + 1, tokens.begin() + i + 2);
            }
            else if (currentToken.type == Token::Type::Identifier && nextToken.value == ":") {
                currentToken.type = Token::Type::Property;
                tokens.erase(tokens.begin() + i + 1);
            }
            else if (currentToken.type == Token::Type::Identifier && nextToken.value == "(") {
                currentToken.type = Token::Type::FuncCall;
                tokens.erase(tokens.begin() + i + 1);
            }

        }
        for (size_t i = 0; i < tokens.size(); i++) {
            if (tokens[i].type == Token::Type::Number && i < tokens.size() - 1 && tokens[i + 1].type == Token::Type::Identifier) {
                tokens[i].subvalues = { tokens[i + 1].value };
                tokens[i].type = Token::Type::LiteralNumber;
                tokens.erase(tokens.begin() + i + 1);
            }
        }


        for (size_t i = 0; i < tokens.size(); i++) {
            if (tokens[i].type == Token::Type::Identifier && i < tokens.size() - 1 && tokens[i + 1].type == Token::Type::SubName) {
                tokens[i].subvalues.clear();
                auto begin = tokens.begin() + i + 1;
                auto end = std::find_if(begin, tokens.end(), [&](const Token& tok) {
                    return tok.type != Token::Type::SubName;
                    });
                tokens[i].type = Token::Type::Path;
                auto tokIt = begin;
                while (tokIt != end) {
                    tokens[i].value += tokIt->value;
                    tokIt++;
                }
                tokens.erase(tokens.begin() + i + 1, end);
            }

            else if (tokens[i].type == Token::Type::FuncCall) {
                const auto begin = tokens.begin() + i + 1;
                const auto end = std::find_if(begin, tokens.end(), [&](const Token& tok) {
                    return tok.value == ")";
                    });
                tokens[i].children.insert(tokens[i].children.begin(), begin, end);
                tokens.erase(begin, end + (end != tokens.end() ? 1 : 0));
            }

        }
        for (size_t i = 0; i < tokens.size(); i++) {
            if (tokens[i].type == Token::Type::Property) {
                const auto begin = tokens.begin() + i + 1;
                const auto end = std::find_if(begin, tokens.end(), [&](const Token& tok) {
                    return tok.value == ";";
                    });
                tokens[i].children.insert(tokens[i].children.begin(), begin, end);
                tokens.erase(begin, end + (end != tokens.end() ? 1 : 0));
            }
        }
        for (size_t i = 0; i < tokens.size(); i++) {
            if (tokens[i].value == "{") {
                const auto begin = tokens.begin() + i + 1;
                const auto end = std::find_if(begin, tokens.end(), [&](const Token& tok) {
                    return tok.value == "}";
                    });
                tokens[i].type = Token::Type::Object;
                tokens[i].value.clear();
                tokens[i].children.insert(tokens[i].children.begin(), begin, end);
                tokens.erase(begin, end + (end != tokens.end() ? 1 : 0));
            }

        }
        return tokens;
    }
}