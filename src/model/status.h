#pragma once

#include <string>
#include <utility>

class Status {
public:
    enum StatusCode : unsigned char {
        Success = 0,
        Error,
        DBOpenError
    };

    Status() : Status(Success) {}
    explicit Status(StatusCode code, std::string errorMessage = {})
        : code_(code), message_(std::move(errorMessage)) {}

    StatusCode GetCode() const {
        return code_;
    }

    std::string Msg() const& {
        if (this) return success_msg_;
        return message_;
    }

    std::string Msg()&& {
        if (this) return success_msg_;
        return std::move(message_);
    }

private:
    StatusCode code_;
    std::string message_;

    static constexpr const char* success_msg_ = "success";
};
