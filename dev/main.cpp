#include <fstream>
#include <iostream>
#include <SimpleSS/priv/Lexer.hpp>
#include <SimpleSS/SimpleSS.hpp>
#include <magic_enum/magic_enum.hpp>
std::ostream& operator<<(std::ostream& stream, const simss::Token& token) {
    stream << magic_enum::enum_name(token.type) << "[" << token.line << ", " << token.column << "] = " << "\"" << token.value << "\"";
    return stream;
}
int main() {

    auto result = simss::loadFromFile("example.sss");
    if (!result) {
        for (const auto& err : result.getErrors()) std::cout << err << std::endl;
        return 1;
    }
    simss::StyleSheet sheet = result.getValue();
    std::cout << "name>FontSize: " << sheet.getValue("name", "FontSize").asNumber() << std::endl;
    std::cout << "name.subname>FontSize: " << sheet.getValue("name.subname", "FontSize").asNumber() << std::endl;
    std::cout << "name.subname.subbername>FontSize: " << sheet.getValue("name.subname.subbername", "FontSize").asNumber() << std::endl;
    std::cout << "name.subname.subbername|state>FontSize: " << sheet.getValue("name.subname.subbername|state", "FontSize").asNumber() << std::endl;
    simss::Value::Color color = sheet.getValue("name", "Color").asColor();

    std::cout << "name>Color: " << (int)color[0] << ", " << (int)color[1] << ", " << (int)color[2] << ", " << (int)color[3] << std::endl;
    return 0;
}