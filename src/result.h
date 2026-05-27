#pragma once

#include <optional>
#include <QString>

template<typename T>
struct Result {
    std::optional<T> value;
    QString error;
    bool success() const { return value.has_value(); }
    explicit operator bool() const { return success(); }

    static Result ok(T val) { return Result{std::move(val), {}}; }
    static Result fail(QString err) { return Result{std::nullopt, std::move(err)}; }
};

template<typename T>
Result(std::optional<T>, QString) -> Result<T>;
