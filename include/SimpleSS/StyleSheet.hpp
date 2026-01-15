#ifndef SIMPLESS_STYLESHEET_HPP
#define SIMPLESS_STYLESHEET_HPP

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <SimpleSS/Result.hpp>
#include <SimpleSS/Value.hpp>

namespace simss {
    typedef std::map<std::string, Value> Style;
    class StyleSheet {
        std::map<std::string, Style> mStyles;
    public:
        StyleSheet() = default;
        ~StyleSheet() = default;
        void setValue(const std::string& style, const std::string& name, const Value& value) {
            mStyles[style][name] = value;
        }
        void addStyle(const std::string& name, const Style& style) {
            mStyles[name] = style;
        }
        bool hasStyle(const std::string& style) const {
            return mStyles.count(style);
        }
        const Style& getStyle(const std::string& style) const {
            return mStyles.at(style);
        }
        Value getValue(const std::string& object, const std::string& value, const Value::ValueVariant& _default = std::monostate()) const {
            return getNearestValue(object, value, _default);
        }
    private:
        /// @brief 
        /// @param name 
        /// @param value 
        /// @returns A const reference to the value or a null varian 
        Value getNearestValue(std::string name, const std::string& value, const Value::ValueVariant& _default = std::monostate()) const {
            std::vector<std::string> layers;

            if (std::count(name.begin(), name.end(), '|')) {
                if (mStyles.count(name) && mStyles.at(name).count(value))
                    return mStyles.at(name).at(value);
                else
                    name.erase(name.begin() + name.find('|'), name.end());
            }

            while (std::count(name.begin(), name.end(), '.')) {
                layers.push_back(name);
                name.erase(name.begin() + name.find_last_of('.'), name.end());
            }
            layers.push_back(name);
            layers.push_back("");

            for (const auto& layer : layers) {
                if (mStyles.count(layer) && mStyles.at(layer).count(value))
                    return mStyles.at(layer).at(value);
            }
            return _default;
        }
    };

    Result<StyleSheet> loadFromString(const std::string& source);
    Result<StyleSheet> loadFromFile(const std::string& path);
}

#endif