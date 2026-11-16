#pragma once

#include <stdexcept>
#include <string>

namespace domain {

class AppError : public std::runtime_error {
public:
    explicit AppError(const std::string& message)
        : std::runtime_error(message)
    {
    }
};

class NotFoundError : public AppError {
public:
    explicit NotFoundError(const std::string& message)
        : AppError(message)
    {
    }
};

class ValidationError : public AppError {
public:
    explicit ValidationError(const std::string& message)
        : AppError(message)
    {
    }
};

} // namespace domain
