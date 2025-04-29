#pragma once
#include <boost/mysql.hpp>
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <string>

class MySqlClient
{
public:
	MySqlClient(const MySqlClient& other) = delete;
	MySqlClient(MySqlClient&& other) noexcept = delete;
	MySqlClient& operator=(const MySqlClient& other) = delete;
	MySqlClient& operator=(MySqlClient&& other) noexcept = delete;

    MySqlClient() = default;
    ~MySqlClient();
    static void init_pool(
        boost::asio::io_context& io_context,
        const std::string& database,
        const std::string& username,
        const std::string& password,
        const std::string& host,
		std::size_t pool_size = 1,
		unsigned short port = boost::mysql::default_port
    );
    bool query_sync(const std::string& sql, boost::mysql::results& results, int timeout_ms = 3000);
    boost::asio::awaitable<boost::mysql::results> query_async(const std::string& sql, int timeout_ms = 3000);

    bool begin_transaction_sync();
    bool commit_transaction_sync();
    bool rollback_transaction_sync();
    void close();
    bool is_connected() const;
private:
    boost::mysql::tcp_connection conn_;
    bool m_connected = false;
    std::string m_host;
    std::string m_port;
    std::string m_user;
    std::string m_password;
    std::string m_database;
};
