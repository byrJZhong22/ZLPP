//
// Created by jun on 18-2-20.
//

#include "logging.h"

#include <stdlib.h>
#include <time.h>

//------------------------------------------------------------------------------
// Logger
//------------------------------------------------------------------------------

namespace zh {
    std::ofstream Logger::info_log_file_;
    std::ofstream Logger::warn_log_file_;
    std::ofstream Logger::erro_log_file_;

    void InitializeLogger(const std::string& info_log_filename,
                          const std::string& warn_log_filename,
                          const std::string& erro_log_filename) {
        Logger::info_log_file_.open(info_log_filename.c_str());
        Logger::warn_log_file_.open(warn_log_filename.c_str());
        Logger::erro_log_file_.open(erro_log_filename.c_str());
        //check if the file has been open
        bool bo = false;
        if (!Logger::info_log_file_.is_open()) {
            std::cout << "Cannot create file: " << info_log_filename << ". "
                      << "Please check that wether you need to "
                      << "create a new directory. \n";
            bo = true;
        }

        if (!Logger::warn_log_file_.is_open()) {
            std::cout << "Cannot create file: " << warn_log_filename << ". "
                      << "Please check that wether you need to "
                      << "create a new directory. \n";
            bo = true;
        }
        if (!Logger::erro_log_file_.is_open()) {
            std::cout << "Cannot create file:  " << erro_log_filename << ". "
                      << "Please check that wether you need to "
                      << "create a new directory. \n";
            bo = true;
        }

        if (bo) {exit(0);}
    }

    std::ostream& Logger::GetStream(LogSeverity severity) {
        if (severity == INFO) {
            if (info_log_file_.is_open())
                return info_log_file_;
            else
                return std::cout;
        } else if (severity == WARNING) {
            if (warn_log_file_.is_open())
                return warn_log_file_;
            else
                return std::cout;
        } else if (severity == ERR || severity == FATAL) {
            if (erro_log_file_.is_open())
                return erro_log_file_;
            else
                return std::cerr;
        }
        return std::cout;
    }

    std::ostream& Logger::Start(LogSeverity severity,
                                const std::string& file,
                                int line,
                                const std::string& function) {
        time_t tm;
        time(&tm);
        char time_string[128];
        ctime_r(&tm, time_string);

        return GetStream(severity) << time_string
                                   << " " << file << ":" << line
                                   << " (" << function << ") "
                                   << std::flush;
    }

    Logger::~Logger() {
        GetStream(severity_) << "\n" << std::flush;
        if (severity_ == FATAL) {
            info_log_file_.close();
            warn_log_file_.close();
            erro_log_file_.close();
            abort();
        }
    }
}


