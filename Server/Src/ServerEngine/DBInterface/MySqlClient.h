#pragma once
#include <boost/mysql.hpp>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <string>

class MySqlClient
{
public:
    MySqlClient(boost::asio::io_context& io_context);
    ~MySqlClient();
    bool connect_sync(const std::string& host, const std::string& port,
        const std::string& user, const std::string& password,
        const std::string& database);

    boost::asio::awaitable<bool> connect_async(const std::string& host, const std::string& port,
        const std::string& user, const std::string& password,
        const std::string& database);

    bool query_sync(const std::string& sql, boost::mysql::results& results, int timeout_ms = 3000);
    boost::asio::awaitable<boost::mysql::results> query_async(const std::string& sql, int timeout_ms = 3000);

    bool begin_transaction_sync();
    bool commit_transaction_sync();
    bool rollback_transaction_sync();
    void close();
    bool is_connected() const;

private:
    bool reconnect_sync();
    boost::asio::awaitable<bool> reconnect_async();
    boost::asio::io_context& ctx_;
    boost::mysql::tcp_connection conn_;
    bool m_connected = false;
    std::string m_host;
    std::string m_port;
    std::string m_user;
    std::string m_password;
    std::string m_database;
};
