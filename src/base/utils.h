//
// Created by jun on 18-2-28.
//

#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <string>
#include <cstdarg>

namespace utils {
    /*! \brief error message buffer length */
    const int kPrintBuffer = 1 << 12;

    inline void HandleCheckError(const char *msg) {
        fprintf(stderr, "%s\n", msg);
        exit(-1);
    }

    inline void HandleAssertError(const char *msg) {
        fprintf(stderr, "AssertError:%s\n", msg);
        exit(-1);
    }

    inline void Check(bool exp, const char *fmt, ...) {
        if (!exp) {
            std::string msg(kPrintBuffer, '\0');
            va_list args;
            va_start(args, fmt);
            vsnprintf(&msg[0], kPrintBuffer, fmt, args);
            va_end(args);
            HandleCheckError(msg.c_str());
        }
    }

/*! \brief assert a condition is true, use this to handle debug information */
    inline void Assert(bool exp, const char *fmt, ...) {
        if (!exp) {
            std::string msg(kPrintBuffer, '\0');
            va_list args;
            va_start(args, fmt);
            vsnprintf(&msg[0], kPrintBuffer, fmt, args);
            va_end(args);
            HandleAssertError(msg.c_str());
        }
    }

/*! \brief report error message, same as check */
    inline void Error(const char *fmt, ...) {
        {
            std::string msg(kPrintBuffer, '\0');
            va_list args;
            va_start(args, fmt);
            vsnprintf(&msg[0], kPrintBuffer, fmt, args);
            va_end(args);
            HandleCheckError(msg.c_str());
        }
    }
}


#endif //NETWORK_UTILS_H
