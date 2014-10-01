#include <unordered_map>
#include "logging.h"
#include "exceptions.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace logging {

void Handler::write_message(Logger* logger,
                   const datetime::DateTime& time,
                   const std::string& level,
                   const std::string& message) {

    if(!logger) {
        throw ValueError("Tried to write to a NULL logger instance");
    }
    do_write_message(logger, time, level, message);
}

FileHandler::FileHandler(const std::string& filename, bool move_aside):
    filename_(filename) {

    if(move_aside && os::path::exists(filename_)) {
        if(os::path::exists(filename_ + ".old")) {
            os::remove(filename_ + ".old");
        }
        os::rename(filename_, filename_ + ".old");
    }

    stream_.open(filename_.c_str());

    if(!stream_.good()) {
        throw IOError(_u("Unable to open log file at {0}").format(filename_));
    }
}

void FileHandler::do_write_message(Logger* logger,
                   const datetime::DateTime& time,
                   const std::string& level,
                   const std::string& message) {

    if(!stream_.good()) {
        throw IOError("Error writing to log file");
    }
    stream_ << datetime::strftime(time, "%Y-%m-%d %H:%M:%S") << " " << level << " " << message << std::endl;
    stream_.flush();
}

void StdIOHandler::do_write_message(Logger* logger,
                       const datetime::DateTime& time,
                       const std::string& level,
                       const std::string& message) {

        if(level == "ERROR") {
#ifndef __ANDROID__
            std::cerr << datetime::strftime(time, "%Y-%m-%d %H:%M:%S") << " ERROR " << message << std::endl;
#else
            __android_log_print(ANDROID_LOG_ERROR, "kglt", "ERROR %s", message.c_str());
#endif
        } else {
#ifndef __ANDROID__
            std::cout << datetime::strftime(time, "%Y-%m-%d %H:%M:%S") << " " << level << " " << message << std::endl;
#else
            __android_log_print(ANDROID_LOG_ERROR, "kglt", "ERROR %s", message.c_str());
#endif
        }
    }

void debug(const unicode& text, const std::string& file, int32_t line) {
    get_logger("/")->debug(text, file, line);
}

void info(const unicode& text, const std::string& file, int32_t line) {
    get_logger("/")->info(text, file, line);
}

void warn(const unicode& text, const std::string& file, int32_t line) {
    get_logger("/")->warn(text, file, line);
}

void error(const unicode& text, const std::string& file, int32_t line) {
    get_logger("/")->error(text, file, line);
}

Logger* get_logger(const std::string& name) {
    static Logger root("/");
    static std::unordered_map<std::string, Logger::ptr> loggers_;

    if(name.empty() || name == "/") {
        return &root;
    } else {
        if(loggers_.find(name) == loggers_.end()) {
            loggers_[name].reset(new Logger(name));
        }
        return loggers_[name].get();
    }
}

}
