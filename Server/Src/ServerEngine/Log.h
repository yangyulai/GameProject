#pragma once 
#include <format>
#include <filesystem>
#include <semaphore> // Add this include for std::counting_semaphore
#include <fstream>
#include <ostream>
#include <source_location>

enum class LogLevel : uint8_t {
    Trace,      // 追踪级（新增）
    Debug,
    Info,
    Warning,
    Error,
    Critical    // 致命错误级（新增）
};


class Logger {
    // 单例控制 
    Logger() = default;
    ~Logger();

    // 异步写入器 
    std::jthread worker_;
    std::atomic_bool running_{ false };

    // 文件管理 
    std::ofstream file_;
    std::filesystem::path dir_;
    std::string prefix_;

    // 线程安全 
    std::mutex mutex_;
    std::atomic<LogLevel> level_{ LogLevel::Info };

public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    // 核心接口 
    bool init(std::string_view prefix,
        const std::filesystem::path& dir = "logs");
    void flush();
    void shutdown() noexcept;

    // 日志级别控制 
    void set_level(LogLevel lv) noexcept { level_.store(lv); }
    [[nodiscard]] LogLevel level() const noexcept { return level_.load(); }

    // 格式化日志接口（模板实现需在头文件）
    template <typename... Args>
    void log_impl(LogLevel lv, std::format_string<Args...> fmt, Args&&... args,
        std::source_location loc) {
        if (lv < level_.load(std::memory_order_acquire)) return;

        const auto now = std::chrono::system_clock::now();
        const auto msg = std::format("[{:%F %T.%3ms}][{:<7}][{}:{}] {}\n",
            now,
            to_string(lv),
            loc.file_name(),
            loc.line(),
            std::vformat(fmt.get(), std::make_format_args(args...)));

        {
            std::lock_guard lock(mutex_);
            file_ << msg;
            std::println("{}", msg.substr(0, msg.find_last_of('\n')));
        }
    }
    template <typename... Args>
    void log(LogLevel lv, auto&&... args) {
        log_impl(lv, std::forward<decltype(args)>(args)...,
            std::source_location::current());
    }
private:
    void rotate_file_();
    static std::string_view to_string(LogLevel lv) noexcept;
};

#define LOG_TRACE(...)    Logger::instance().log(LogLevel::Trace, __VA_ARGS__)
#define LOG_DEBUG(...)    Logger::instance().log(LogLevel::Debug, __VA_ARGS__)
#define LOG_INFO(...)     Logger::instance().log(LogLevel::Info, __VA_ARGS__)
#define LOG_WARN(...)     Logger::instance().log(LogLevel::Warning, __VA_ARGS__)
#define LOG_ERROR(...)    Logger::instance().log(LogLevel::Error, __VA_ARGS__)
#define LOG_CRITICAL(...) Logger::instance().log(LogLevel::Critical, __VA_ARGS__)

//#define AAAAAAAAAAAAA
#ifdef AAAAAAAAAAAAA
class CLog
{
private:
    CLog(void);
    ~CLog(void);

public:
    static CLog* GetInstancePtr();

    BOOL Start(std::string strPrefix, std::string strLogDir = "log");

    BOOL Close();

    void LogHiInfo(char* lpszFormat, ...);

    void LogWarn(char* lpszFormat, ...);

    void LogError(char* lpszFormat, ...);

    void LogInfo(char* lpszFormat, ...);

    void SetLogLevel(int Level);

    void SetTitle(char* lpszFormat, ...);

    void CheckAndCreate();

protected:
    std::mutex          m_WriteMutex;

    INT32               m_LogCount;

    FILE*               m_pLogFile;

    INT32               m_LogLevel;

    std::string         m_strPrefix;
    std::string         m_strLogDir;
};

#define ERROR_RETURN_TRUE(P) \
    if((P) == FALSE)\
    {\
        LOG_ERROR("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        return TRUE;    \
    }


#define ERROR_RETURN_FALSE(P) \
    if((P) == FALSE)\
    {\
        LOG_ERROR("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        return FALSE;   \
    }

#define ERROR_RETURN_NULL(P) \
    if((P) == FALSE)\
{\
    LOG_ERROR("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return NULL;    \
}

#define ERROR_RETURN_NONE(P) \
    if((P) == FALSE)\
{\
    LOG_ERROR("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return ;    \
}

#define ERROR_RETURN_VALUE(P, V) \
    if((P) == FALSE)\
{\
    LOG_ERROR("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    return V;   \
}

#define ERROR_CONTINUE_EX(P) \
    if((P) == FALSE)\
{\
    LOG_ERROR("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    continue; \
}

#define ERROR_TO_CONTINUE(P) \
    if((P) == FALSE)\
{\
    LOG_ERROR("Error : File:{}, Func: {} Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
    continue; \
}

#else

#define ERROR_RETURN_TRUE(P)
#define ERROR_RETURN_FALSE(P) 
#define ERROR_RETURN_NULL(P) 
#define ERROR_RETURN_NONE(P) 
#define ERROR_RETURN_VALUE(P, V)
#define ERROR_CONTINUE_EX(P)
#define ERROR_TO_CONTINUE(P)
#endif
