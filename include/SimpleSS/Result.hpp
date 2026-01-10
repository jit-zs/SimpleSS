#ifndef SIMPLESS_RESULT_HPP
#define SIMPLESS_RESULT_HPP

#include <string>
#include <vector>

namespace simss {
    template <typename T, std::enable_if_t<std::is_copy_constructible_v<T>&& std::is_destructible_v<T>, bool> = true>
    class Result {
        T mValue;
        std::vector<std::string> mErrors;
    public:
        Result(const T& value, const std::vector<std::string>& errors = {})
            : mValue(value), mErrors(errors) {
        }
        Result() = delete;
        const T& getValue() const { return mValue; }
        const std::vector<std::string>& getErrors() const { return mErrors; }
        operator bool() const { return mErrors.empty(); }
    };
}

#endif