#include <SimpleSS/priv/Parser.hpp>

#include <iostream>
#include <ranges>
#include <stack>



namespace simss {


    static std::map<std::string, Value(*)(double*)> functions = {
    {
        "frgb", [](double* vars)->Value {
            return {Value::Color{uint8_t(vars[0] * 255),uint8_t(vars[1] * 255),uint8_t(vars[2] * 255), uint8_t(255)}};
        }

    },
    {
        "frgba", [](double* vars)->Value {
            return {Value::Color{uint8_t(vars[0] * 255), uint8_t(vars[1] * 255), uint8_t(vars[2] * 255), uint8_t(vars[3] * 255)}};
        }
    },
    {
        "rgb", [](double* vars)->Value{
            return {Value::Color{(uint8_t)vars[0], (uint8_t)vars[1], (uint8_t)vars[2], (uint8_t)255}};
        }
    },
    {
        "rgba", [](double* vars)->Value{
            return {Value::Color{(uint8_t)vars[0], (uint8_t)vars[1], (uint8_t)vars[2], (uint8_t)vars[3]}};
        }
    }
    };
    inline std::string lineAndColumn(const Token& token) {
        return "[" + std::to_string(token.line) + ", " + std::to_string(token.column) + "]";
    }
    std::string parseExpression(Token* tokens, size_t length) {
        std::string result;
        for (size_t i = 0; i < length; i++) {

        }
        return result;
    }
    Result<Style> parseObject(const Token& object) {
        Style style;
        std::vector<std::string> errors;
        for (const auto& prop : object.children) {
            if (prop.children.size() != 1) {
                errors.push_back("ERR: Improperly formatted property" + lineAndColumn(prop));
                continue;

            }

            const Token& expr = prop.children[0];
            if (expr.type == Token::Type::Number) {
                style[prop.value] = std::stod(expr.value);
            }
            else if (expr.type == Token::Type::String) {
                style[prop.value] = expr.value;
            }
            else if (expr.type == Token::Type::LiteralNumber) {
                style[prop.value] = Value::NumberLiteral(std::stod(expr.value), expr.subvalues[0]);
            }
            else if (expr.type == Token::Type::Bool) {
                style[prop.value] = bool(expr.value == "true");
            }
            else if (expr.type == Token::Type::FuncCall) {
                std::string funcName = expr.value;
                if (!functions.count(funcName)) {
                    errors.push_back(std::string("ERR: The function ") + funcName + " does not exist" + lineAndColumn(prop));
                }
                std::vector<double> args;
                for (const auto& arg : expr.children){
                    if (arg.type == Token::Type::Number){
                        args.push_back(std::stod(arg.value));
                    }
                }
                style[prop.value] = functions[funcName](args.data());
            }
            else {
                errors.push_back("ERR: Invalid value for property" + lineAndColumn(expr));
            }
        }
        return { style, errors };
    }
    Result<StyleSheet> parse(const std::vector<Token>& tokens) {
        StyleSheet sheet;
        std::vector<std::string> errors;


        std::vector<Token>::const_iterator it = tokens.begin();
        bool globalDeclared = false;


        const auto currentToken = [&]() -> const Token& {
            return *it;
            };
        const auto valid = [&]()->bool {
            return it < tokens.end();
            };
        std::string usage;
        while (it < tokens.end()) {
            if (currentToken().type == Token::Type::Object) {
                if (globalDeclared) {
                    errors.push_back("ERR: Global state already declared" + lineAndColumn(currentToken()));
                }
                globalDeclared = true;
                auto style = parseObject(currentToken());
                errors.insert(errors.end(), style.getErrors().begin(), style.getErrors().end());
                if (style)
                    sheet.addStyle("", style.getValue());

            }
            else if (currentToken().type == Token::Type::Path || currentToken().type == Token::Type::Identifier) {
                std::string name = currentToken().value;
                it++;
                if (valid() && currentToken().type == Token::Type::State) {
                    name.append("|");
                    name.append(currentToken().value);
                    it++;
                }
                if (!valid() || currentToken().type != Token::Type::Object) {
                    errors.push_back("ERR: Expected object" + lineAndColumn(it >= tokens.end() ? *(--it) : currentToken()));
                }
                else {
                    auto style = parseObject(currentToken());
                    errors.insert(errors.end(), style.getErrors().begin(), style.getErrors().end());
                    if (style) {
                        sheet.addStyle(name, style.getValue());
                    }
                }
            }
            else {
                errors.push_back("ERR: Invalid token" + lineAndColumn(currentToken()));
            }
            it++;
        }
        return { sheet, errors };
    }

}