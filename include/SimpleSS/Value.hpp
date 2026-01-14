#ifndef SIMPLESS_VALUE_HPP
#define SIMPLESS_VALUE_HPP

#include <array>
#include <cmath>
#include <limits>
#include <math.h>
#include <stdint.h>
#include <string>
#include <variant>
#include <vector>



namespace simss {
    class Value {
    public:
        typedef std::pair<double, std::string> NumberLiteral;
        typedef std::array<uint8_t, 4> Color;
        typedef std::variant<std::monostate, NumberLiteral, double, bool, std::string, Color, std::vector<Value>> ValueVariant;

    private:
        ValueVariant mValue;

    public:
        Value() = default;
        Value(ValueVariant value) : mValue(value) {}
        Value& operator=(const ValueVariant& value) {
            mValue = value;
            return *this;
        }
        bool isNumber() const {
            return std::holds_alternative<double>(mValue);
        }
        bool isNumberLiteral() const {
            return std::holds_alternative<NumberLiteral>(mValue);
        }
        bool isString() const {
            return std::holds_alternative<std::string>(mValue);
        }
        bool isBool() const {
            return std::holds_alternative<bool>(mValue);
        }
        bool isColor() const {
            return std::holds_alternative<Color>(mValue);
        }
        bool isObject() const {
            return std::holds_alternative<std::vector<Value>>(mValue);
        }


        double asNumber(double fallback = -1) const {
            return std::holds_alternative<double>(mValue) ? std::get<double>(mValue) : fallback;
        }
        NumberLiteral asNumberLiteral(const NumberLiteral& fallback = { 0, "" }) {
            return std::holds_alternative<NumberLiteral>(mValue) ? std::get<NumberLiteral>(mValue) : fallback;
        }
        std::string asString(const std::string& fallback = "") const {
            return std::holds_alternative<std::string>(mValue) ? std::get<std::string>(mValue) : fallback;
        }
        bool asBool(bool fallback = false) const {
            return std::holds_alternative<bool>(mValue) ? std::get<bool>(mValue) : fallback;
        }
        Color asColor(const Color& fallback = {}) const {
            return std::holds_alternative<Color>(mValue) ? std::get<Color>(mValue) : fallback;
        }

        // Planned and not used
        const std::vector<Value>& asObject() const {
            return std::get<std::vector<Value>>(mValue);
        }

    };
}

#endif