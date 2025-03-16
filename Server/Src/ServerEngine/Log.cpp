#include "stdafx.h"
#include "Log.h"
#include <iostream>


using namespace std::chrono_literals;

Logger::~Logger() {
    shutdown();
}

bool Logger::init(std::string_view prefix, const std::filesystem::path& dir) {
    prefix_ = prefix;
    dir_ = dir;

    try {
        std::filesystem::create_directories(dir_);
        rotate_file_();

        running_ = true;
        worker_ = std::jthread([this](std::stop_token st) {
            while (!st.stop_requested()) {
                std::this_thread::sleep_for(250ms);
                flush();
            }
            });
        return true;
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::println(std::cerr, "[FATAL] Failed to initialize logger: {}", e.what());
        return false;
    }
}

void Logger::flush() {
    std::lock_guard lock(mutex_);
    file_.flush();
}

void Logger::shutdown() noexcept {
    if (running_.exchange(false)) {
        worker_.request_stop();
        flush();
        file_.close();
    }
}

void Logger::rotate_file_() {
    const auto now = std::chrono::system_clock::now();
    const auto filename = std::format("{}-{:%Y%m%d-%H%M%S}.log",
        prefix_, now);

    file_.close();
    file_.open(dir_ / filename, std::ios::app | std::ios::ate);

    if (!file_) {
        throw std::runtime_error(std::format(
            "Cannot open log file: {}", (dir_ / filename).string()));
    }
}

std::string_view Logger::to_string(LogLevel lv) noexcept {
    switch (lv) {
    case LogLevel::Trace:    return "TRACE";
    case LogLevel::Debug:    return "DEBUG";
    case LogLevel::Info:     return "INFO";
    case LogLevel::Warning:  return "WARNING";
    case LogLevel::Error:    return "ERROR";
    case LogLevel::Critical: return "CRITICAL";
    default:              return "UNKNOWN";
    }
}
#ifdef AAAAAAAA
CLog::CLog(void)
{
    m_LogLevel = 4;
}

CLog::~CLog(void)
{
    Close();
}

CLog* CLog::GetInstancePtr()
{
    static CLog Log;

    return &Log;
}

BOOL CLog::Start(std::string strPrefix, std::string strLogDir)
{
    if(!CommonFunc::CreateDir(strLogDir))
    {
        return FALSE;
    }
    m_strPrefix = strPrefix;
    m_strLogDir = strLogDir;
#ifdef WIN32
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS);
#endif
    m_pLogFile = NULL;

    tm CurTime = CommonFunc::GetCurrTmTime();

    CHAR szFileName[512];

    snprintf(szFileName, 512, "{}/{}-%02d%02d%02d-%02d%02d%02d.log",  strLogDir.c_str(), strPrefix.c_str(), CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec);

    m_pLogFile = fopen(szFileName, "w+");

    if(m_pLogFile == NULL)
    {
        return FALSE;
    }

    m_LogCount = 0;

    return TRUE;
}

BOOL CLog::Close()
{
    if(m_pLogFile == NULL)
    {
        return FALSE;
    }

    fflush(m_pLogFile);

    fclose(m_pLogFile);

    m_pLogFile = NULL;

    return TRUE;
}

void CLog::LogHiInfo(char* lpszFormat, ...)
{
    if (m_LogLevel < Log_HiInfo)
    {
        return;
    }

    if (m_pLogFile == NULL)
    {
        return;
    }

    CHAR szLog[2048] = { 0 };

    tm CurTime = CommonFunc::GetCurrTmTime();
    snprintf(szLog, 2048, "[%02d-%02d-%02d %02d:%02d:%02d][%04x][I] ", CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 0xffff & CommonFunc::GetCurThreadID());

    va_list argList;
    va_start(argList, lpszFormat);
    vsnprintf(szLog + 28, 1024 - 28, lpszFormat, argList);
    va_end(argList);

    strncat(szLog, "\n", 3);

    m_WriteMutex.lock();
    fputs(szLog, m_pLogFile);
    fflush(m_pLogFile);
    m_LogCount++;
    CommonFunc::PrintColorText(szLog, Log_Info);
    CheckAndCreate();
    m_WriteMutex.unlock();

    return;
}

void CLog::LogWarn( char* lpszFormat, ... )
{
    if(m_LogLevel < Log_Warn)
    {
        return ;
    }

    if(m_pLogFile == NULL)
    {
        return ;
    }

    tm CurTime = CommonFunc::GetCurrTmTime();

    CHAR szLog[2048] = {0};
    snprintf(szLog, 2048, "[%02d-%02d-%02d %02d:%02d:%02d][%04x][W] ", CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 0xffff & CommonFunc::GetCurThreadID());

    va_list argList;
    va_start( argList, lpszFormat );
    vsnprintf(szLog + 28, 1024 - 28,  lpszFormat, argList);
    va_end( argList );

    strncat(szLog, "\n", 3);

    m_WriteMutex.lock();
    fputs(szLog, m_pLogFile);
    fflush(m_pLogFile);
    m_LogCount++;
    CommonFunc::PrintColorText(szLog, Log_Warn);
    CheckAndCreate();
    m_WriteMutex.unlock();

    return ;
}

void CLog::LogError( char* lpszFormat, ... )
{
    if(m_LogLevel < Log_Error)
    {
        return ;
    }

    if(m_pLogFile == NULL)
    {
        return ;
    }

    CHAR szLog[2048] = { 0 };
    tm CurTime = CommonFunc::GetCurrTmTime();
    snprintf(szLog, 2048, "[%02d-%02d-%02d %02d:%02d:%02d][%04x][E] ", CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 0xffff & CommonFunc::GetCurThreadID());

    va_list argList;
    va_start( argList, lpszFormat );
    vsnprintf(szLog + 28, 1024 - 28, lpszFormat, argList);
    va_end( argList );

    strncat(szLog, "\n", 3);

    m_WriteMutex.lock();
    fputs(szLog, m_pLogFile);
    fflush(m_pLogFile);
    m_LogCount++;
    CommonFunc::PrintColorText(szLog, Log_Error);
    CheckAndCreate();
    m_WriteMutex.unlock();

    return ;
}

void CLog::LogInfo( char* lpszFormat, ... )
{
    if(m_LogLevel < Log_Info)
    {
        return ;
    }

    if(m_pLogFile == NULL)
    {
        return ;
    }

    CHAR szLog[2048] = { 0 };

    tm CurTime = CommonFunc::GetCurrTmTime();
    snprintf(szLog, 2048, "[%02d-%02d-%02d %02d:%02d:%02d][%04x][I] ", CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 0xffff & CommonFunc::GetCurThreadID());

    va_list argList;
    va_start( argList, lpszFormat );
    vsnprintf(szLog + 28, 1024 - 28,  lpszFormat, argList);
    va_end( argList );

    strncat(szLog, "\n", 3);

    m_WriteMutex.lock();
    fputs(szLog, m_pLogFile);
    fflush(m_pLogFile);
    m_LogCount++;
    CommonFunc::PrintColorText(szLog, Log_Info);
    CheckAndCreate();
    m_WriteMutex.unlock();

    return ;
}

void CLog::SetLogLevel( int Level )
{
    m_LogLevel = Level;

    return ;
}

void CLog::SetTitle(char* lpszFormat, ...)
{
#ifdef WIN32
    CHAR szLog[512] = {0};
    snprintf(szLog, 512, "{}: ", m_strPrefix.c_str());

    INT32 nSize = (INT32)strlen(szLog);

    va_list argList;
    va_start(argList, lpszFormat);
    vsnprintf(szLog + nSize, 512 - nSize, lpszFormat, argList);
    va_end(argList);

    SetConsoleTitle(szLog);
#endif
    return;
}

void CLog::CheckAndCreate()
{
    //超过10M就新建一个文件
    if (ftell(m_pLogFile) < 1024 * 1024 * 10)
    {
        return;
    }

    fclose(m_pLogFile);

    m_pLogFile = NULL;

    tm CurTime = CommonFunc::GetCurrTmTime();

    CHAR szFileName[512];

    snprintf(szFileName, 512, "{}/{}-%02d%02d%02d-%02d%02d%02d.log", m_strLogDir.c_str(), m_strPrefix.c_str(), CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec);

    m_pLogFile = fopen(szFileName, "w+");

    if (m_pLogFile == NULL)
    {
        return ;
    }

    m_LogCount = 0;

    return;
}

#endif