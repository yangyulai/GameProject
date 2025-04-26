#pragma once 
#include <format>
#include <filesystem>
#include "spdlog/async.h"           // 异步日志支持
#include "spdlog/async_logger.h"    // 异步 logger
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <memory>
#include <vector>
#include <string>

class Logger2 {
public:
    // 获取单例对象
    static Logger2& instance() {
        static Logger2 instance;
        return instance;
    }

    // 初始化 logger，配置控制台和文件两个 sink，使用异步模式
    void init(const std::string& logFile = "logs.txt") {
        // 初始化异步日志线程池：队列大小为8192，后台线程1个
        spdlog::init_thread_pool(8192, 1);

        // 创建两个 sink
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
        std::vector<spdlog::sink_ptr> sinks{ consoleSink, fileSink };

        // 创建异步 logger，使用 spdlog 内部的线程池和阻塞策略
        logger_ = std::make_shared<spdlog::async_logger>("async_logger",
            sinks.begin(),
            sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);
        spdlog::register_logger(logger_);

        // 设置日志格式和日志级别
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
        logger_->set_level(spdlog::level::debug);
    }

    void trace(const std::string& message) {
        logger_->trace(message);
    }
    void debug(const std::string& message) {
        logger_->debug(message);
    }

    void info(const std::string& message) {
        logger_->info(message);
    }

    void warn(const std::string& message) {
        logger_->warn(message);
    }

    void error(const std::string& message) {
        logger_->error(message);
    }
    void critical(const std::string& message) {
        logger_->error(message);
    }

    template<typename... Args>
    void trace(const char* fmt, const Args&... args) {
        logger_->trace(fmt, args...);
    }
    template<typename... Args>
    void debug(const char* fmt, const Args&... args) {
        logger_->debug(fmt, args...);
    }
    template<typename... Args>
    void info(const char* fmt, const Args&... args) {
        logger_->info(fmt, args...);
    }
    template<typename... Args>
    void warn(const char* fmt, const Args&... args) {
        logger_->warn(fmt, args...);
    }
    template<typename... Args>
    void error(const char* fmt, const Args&... args) {
        logger_->error(fmt, args...);
    }

    template<typename... Args>
    void critical(const char* fmt, const Args&... args) {
        logger_->critical(fmt, args...);
    }
    // 禁止拷贝和赋值
    Logger2(const Logger2&) = delete;
    Logger2& operator=(const Logger2&) = delete;

private:
    Logger2() {
        init();
    }
    ~Logger2() {
        spdlog::drop_all();
    }

    std::shared_ptr<spdlog::logger> logger_;
};

#define ERROR_RETURN_TRUE(P) \
    if((P) == FALSE)\
    {\
        sLogger.error("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        return TRUE;    \
    }


#define ERROR_RETURN_FALSE(P) \
    if((P) == FALSE)\
    {\
        spdlog::error("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        return FALSE;   \
    }

#define ERROR_RETURN_NULL(P) \
    if((P) == FALSE)\
{\
    spdlog::error("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return NULL;    \
}

#define ERROR_RETURN_NONE(P) \
    if((P) == FALSE)\
{\
    spdlog::error("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return ;    \
}

#define ERROR_RETURN_VALUE(P, V) \
    if((P) == FALSE)\
{\
    spdlog::error("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return V;   \
}

#define ERROR_CONTINUE_EX(P) \
    if((P) == FALSE)\
{\
    spdlog::error("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    continue; \
}

#define ERROR_TO_CONTINUE(P) \
    if((P) == FALSE)\
{\
    spdlog::error("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    continue; \
}
