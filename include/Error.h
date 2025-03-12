#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <iostream>


enum class ErrorCode {
    NONE,
    KEY_NOT_FOUND,
    INVALID_ARGUMENT,
    OPERATION_FAILED,
    UNKOWN_ERROR,
    RIRI_INTERNAL_ERROR
};

inline std::string
errorMessage(ErrorCode Code) {
    switch (Code) {
        case ErrorCode::NONE:
            return "No error";
        case ErrorCode::KEY_NOT_FOUND:
            return "Key not found";
        case ErrorCode::INVALID_ARGUMENT:
            return "Invalid argument";
        case ErrorCode::OPERATION_FAILED:
            return "Operation failed";
        case ErrorCode::UNKOWN_ERROR:
            return "Unknown error";
        case ErrorCode::RIRI_INTERNAL_ERROR:
            return "RiRi internal error";
        default:
            return "Unknown error";
    };
}

template <typename T>
struct ErrorOr {
    bool success;
    T value;
    ErrorCode errorCode;
 
    static ErrorOr<T> Ok(T val) {
        return {true, val, ErrorCode::NONE};
    }


    static ErrorOr<T> Err(ErrorCode code, std::string custom_msg ="") {
        return {false, T(), code, custom_msg.empty()? ::errorMessage(code): custom_msg};
    }

    std::string errorMessage() const {
        return ::errorMessage(errorCode);
    }

};

inline void handleError(const ErrorOr<std::string>& result){
    if (!result.success) {
        std::cerr << "[ERROR]: "<< result.errorMessage() << std::endl;
    }
}


#endif // ERROR_H