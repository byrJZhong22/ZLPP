//
// Created by jun on 18-2-20.
//

#ifndef ZLPP_LOGGING_H
#define ZLPP_LOGGING_H

#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace zh {
    void InitializeLogger(const std::string& info_log_filename,
                          const std::string& warn_log_filename,
                          const std::string& erro_log_filename);

    enum LogSeverity {INFO, WARNING, ERR, FATAL};

    class Logger {
        friend void InitializeLogger(const std::string& info_log_filename,
                                     const std::string& warn_log_filename,
                                     const std::string& erro_log_filename);

    public:
        Logger(LogSeverity s) : severity_(s){}
        ~Logger();

        static std::ostream& GetStream(LogSeverity severity);
        static std::ostream& Start(LogSeverity severity,
                                   const std::string& file,
                                   int line,
                                   const std::string& function);

    private:
        static std::ofstream info_log_file_;
        static std::ofstream warn_log_file_;
        static std::ofstream erro_log_file_;
        LogSeverity severity_;
    };

#define LOG(severity) Logger(severity).Start(severity, __FILE__, __LINE__, __FUNCTION__)
}


#endif //ZLPP_LOGGING_H
