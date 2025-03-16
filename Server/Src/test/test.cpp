#include<string>
#include<Windows.h>
#include <boost/dll/runtime_symbol_info.hpp>
#include <filesystem>
std::wstring GetCurrentExeDir()
{
    wchar_t szPath[1024] = { 0 };
#ifdef WIN32
    GetModuleFileName(NULL, szPath, 1024);
#else
    readlink("/proc/self/exe", szPath, sizeof(szPath));
    char* p = strrchr(szPath, '/');
#endif
    return std::wstring(szPath);
}
#include <string>
#include <iostream>
#include <format>
#include <expected>
#include <system_error>

std::string GetCurrentExeDir2()
{
    try {
        // 获取当前可执行文件的路径
        std::filesystem::path exePath;
#ifdef _WIN32
        const char* buffer = std::getenv("");
        if (buffer != nullptr) {
            exePath = std::filesystem::canonical(std::filesystem::path(buffer));
        }
        else {
            throw std::runtime_error("Failed to get executable path from environment.");
        }
#else
        exePath = std::filesystem::canonical("/proc/self/exe"); // Linux
#endif

        // 获取可执行文件所在目录
        std::filesystem::path exeDir = exePath.parent_path();

        // 返回目录的字符串表示
        return exeDir.string();
    }
    catch (const std::filesystem::filesystem_error& e) {
        // 处理异常（例如，路径不存在）
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}

std::string GetCurrentExeDir3() {
    auto exePath = boost::dll::program_location().string();
    return std::filesystem::path(exePath).parent_path().string();
}
int main()
{
    std::wstring exeDir = GetCurrentExeDir();

    std::string exeDir2 = GetCurrentExeDir2();
    std::string exeDir3 = GetCurrentExeDir3();

    return 0;
}