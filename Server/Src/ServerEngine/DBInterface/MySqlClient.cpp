#include <iostream>
#include "MySqlClient.h"

MySqlClient::MySqlClient(boost::asio::io_context& io_context)
    : ctx_(io_context), conn_(ctx_)
{
}

MySqlClient::~MySqlClient()
{
    close();
}

bool MySqlClient::connect_sync(const std::string& host, const std::string& port,
    const std::string& user, const std::string& password,
    const std::string& database)
{
    try
    {
        m_host = host;
        m_port = port;
        m_user = user;
        m_password = password;
        m_database = database;

        boost::asio::ip::tcp::resolver resolver(ctx_);
        auto endpoints = resolver.resolve(host, port);

        boost::mysql::handshake_params params(user, password, database);
        conn_.connect(*endpoints.begin(), params);

        m_connected = true;
        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "connect_sync error: " << ex.what() << std::endl;
        m_connected = false;
        return false;
    }
}

boost::asio::awaitable<bool> MySqlClient::connect_async(const std::string& host, const std::string& port,
    const std::string& user, const std::string& password,
    const std::string& database)
{
    try
    {
        m_host = host;
        m_port = port;
        m_user = user;
        m_password = password;
        m_database = database;

        boost::asio::ip::tcp::resolver resolver(co_await boost::asio::this_coro::executor);
        auto endpoints = co_await resolver.async_resolve(host, port, boost::asio::use_awaitable);

        boost::mysql::handshake_params params(user, password, database);
        co_await conn_.async_connect(*endpoints.begin(), params, boost::asio::use_awaitable);

        m_connected = true;
        co_return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "connect_async error: " << ex.what() << std::endl;
        m_connected = false;
        co_return false;
    }
}

bool MySqlClient::reconnect_sync()
{
    close();
    return connect_sync(m_host, m_port, m_user, m_password, m_database);
}

boost::asio::awaitable<bool> MySqlClient::reconnect_async()
{
    close();
    co_return co_await connect_async(m_host, m_port, m_user, m_password, m_database);
}

bool MySqlClient::query_sync(const std::string& sql, boost::mysql::results& results, int timeout_ms)
{
    if (!m_connected)
    {
        if (!reconnect_sync())
            return false;
    }

    try
    {
        boost::asio::steady_timer timer(ctx_);
        bool timed_out = false;

        timer.expires_after(std::chrono::milliseconds(timeout_ms));
        timer.async_wait([&](const boost::system::error_code& ec)
            {
                if (!ec)
                {
                    timed_out = true;
                    conn_.stream().lowest_layer().cancel();
                }
            });

        conn_.execute(sql, results);
        timer.cancel();
        return !timed_out;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "query_sync error: " << ex.what() << std::endl;
        m_connected = false;
        return false;
    }
}

boost::asio::awaitable<boost::mysql::results> MySqlClient::query_async(const std::string& sql, int timeout_ms)
{
    boost::mysql::results results;

    if (!m_connected)
    {
        bool reconnected = co_await reconnect_async();
        if (!reconnected)
            co_return results;
    }

    try
    {
        boost::asio::steady_timer timer(co_await boost::asio::this_coro::executor);
        bool timed_out = false;

        timer.expires_after(std::chrono::milliseconds(timeout_ms));
        timer.async_wait([&](const boost::system::error_code& ec)
            {
                if (!ec)
                {
                    timed_out = true;
                    conn_.stream().lowest_layer().cancel();
                }
            });

        co_await conn_.async_execute(sql, results, boost::asio::use_awaitable);
        timer.cancel();

        if (!timed_out)
            co_return results;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "query_async error: " << ex.what() << std::endl;
        m_connected = false;
    }

    co_return results;
}

bool MySqlClient::begin_transaction_sync()
{
    boost::mysql::results res;
    return query_sync("START TRANSACTION", res);
}

bool MySqlClient::commit_transaction_sync()
{
    boost::mysql::results res;
    return query_sync("COMMIT", res);
}

bool MySqlClient::rollback_transaction_sync()
{
    boost::mysql::results res;
    return query_sync("ROLLBACK", res);
}

void MySqlClient::close()
{
    if (m_connected)
    {
        boost::system::error_code ec;
        conn_.close();
        m_connected = false;
    }
}

bool MySqlClient::is_connected() const
{
    return m_connected;
}
