#ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

#include <string>
#include <thread>
#include <fstream>
#include <iostream>
#include <set>

#include "os.h"
#include "unicode.h"
#include "datetime.h"

namespace logging {

enum LOG_LEVEL {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4
};

class Logger;

class Handler {
public:
    typedef std::shared_ptr<Handler> ptr;

    virtual ~Handler() {}
    void write_message(Logger* logger,
                       const datetime::DateTime& time,
                       const std::string& level,
                       const std::string& message);

private:
    virtual void do_write_message(Logger* logger,
                       const datetime::DateTime& time,
                       const std::string& level,
                       const std::string& message) = 0;
};

class StdIOHandler : public Handler {
private:
    void do_write_message(Logger* logger,
                       const datetime::DateTime& time,
                       const std::string& level,
                       const std::string& message) override;
};

class FileHandler : public Handler {
public:
    FileHandler(const std::string& filename, bool move_aside=true);

private:
    void do_write_message(Logger* logger,
                       const datetime::DateTime& time,
                       const std::string& level,
                       const std::string& message);
    std::string filename_;
    std::ofstream stream_;
};

class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name):
        name_(name),
        level_(LOG_LEVEL_DEBUG) {

    }

    void add_handler(Handler::ptr handler) {
        //FIXME: check it doesn't exist already
        handlers_.push_back(handler);
    }

    void debug(const std::string& text, const std::string& file="None", int32_t line=-1) {
        if(level_ < LOG_LEVEL_DEBUG) return;

        write_message("DEBUG", text, file, line);
    }

    void debug(const unicode& text, const std::string& file="None", int32_t line=-1) {
        debug(text.encode(), file, line);
    }

    void info(const std::string& text, const std::string& file="None", int32_t line=-1) {
        if(level_ < LOG_LEVEL_INFO) return;

        write_message("INFO", text, file, line);
    }

    void info(const unicode& text, const std::string& file="None", int32_t line=-1) {
        info(text.encode(), file, line);
    }

    void warn(const std::string& text, const std::string& file="None", int32_t line=-1) {
        if(level_ < LOG_LEVEL_WARN) return;

        write_message("WARN", text, file, line);
    }

    void warn(const unicode& text, const std::string& file="None", int32_t line=-1) {
        warn(text.encode(), file, line);
    }

    void warn_once(const unicode& text, const std::string& file="None", int32_t line=-1) {
        if(line == -1) {
            warn(text, file, line); //Can't warn once if no line is specified
        }

        static std::set<unicode> warned;

        unicode key = _u("{0}:{1}:{2}").format(file, line, text);

        if(warned.find(key) != warned.end()) {
            return;
        }
        warned.insert(key);

        warn(text, file, line);
    }

    void error(const std::string& text, const std::string& file="None", int32_t line=-1) {
        if(level_ < LOG_LEVEL_ERROR) return;

        write_message("ERROR", text, file, line);
    }

    void error(const unicode& text, const std::string& file="None", int32_t line=-1) {
        error(text.encode(), file, line);
    }

    void set_level(LOG_LEVEL level) {
        level_ = level;
    }

private:
    void write_message(const std::string& level, const std::string& text,
                       const std::string& file, int32_t line) {

        unicode file_out = file;
        if(file != "None") {
            file_out = os::path::abs_path(file);
        }

        std::stringstream s;
        s << std::this_thread::get_id() << ": ";
        s << text << " (" << file_out << ":" << _u("{0}").format(line) << ")";
        for(uint32_t i = 0; i < handlers_.size(); ++i) {
            handlers_[i]->write_message(this, datetime::now(), level, s.str());
        }
    }

    std::string name_;
    std::vector<Handler::ptr> handlers_;

    LOG_LEVEL level_;
};

Logger* get_logger(const std::string& name);

void debug(const unicode& text, const std::string& file="None", int32_t line=-1);
void info(const unicode& text, const std::string& file="None", int32_t line=-1);
void warn(const unicode& text, const std::string& file="None", int32_t line=-1);
void warn_once(const unicode& text, const std::string& file="None", int32_t line=-1);
void error(const unicode& text, const std::string& file="None", int32_t line=-1);

}

#define L_DEBUG(txt) \
    logging::debug((txt), __FILE__, __LINE__)

#define L_INFO(txt) \
    logging::info((txt), __FILE__, __LINE__)

#define L_WARN(txt) \
    logging::warn((txt), __FILE__, __LINE__)

#define L_WARN_ONCE(txt) \
    logging::warn_once((txt), __FILE__, __LINE__)

#define L_ERROR(txt) \
    logging::error((txt), __FILE__, __LINE__)

#define L_DEBUG_N(name, txt) \
    logging::get_logger((name))->debug((txt), __FILE__, __LINE__)

#define L_INFO_N(name, txt) \
    logging::get_logger((name))->info((txt), __FILE__, __LINE__)

#define L_WARN_N(name, txt) \
    logging::get_logger((name))->warn((txt), __FILE__, __LINE__)

#define L_ERROR_N(name, txt) \
    logging::get_logger((name))->error((txt), __FILE__, __LINE__)

#endif // LOGGING_H_INCLUDED
