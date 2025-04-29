#include <iostream>
#include "MySqlClient.h"

MySqlClient::~MySqlClient()
{
    close();
}

void MySqlClient::init_pool(
    boost::asio::io_context& io_context,
    const std::string& database,
    const std::string& username,
    const std::string& password,
    const std::string& host,
    std::size_t pool_size,
	unsigned short port
) {
    //pool_io_ctx_ = &io_context;
    //boost::mysql::pool_params params;
    //params.server_address.emplace_host_and_port(host, port);
    //params.username = username;
    //params.password = password;
    //params.database = database;
    //pool_ = std::make_unique<boost::mysql::connection_pool>(
    //    io_context.get_executor(), std::move(params), pool_size
    //);
    //pool_->async_run(boost::asio::detached);
}

bool MySqlClient::query_sync(const std::string& sql, boost::mysql::results& results, int timeout_ms)
{
    //boost::mysql::pooled_connection conn = co_await pool_.async_get_connection();
    //auto conn = pool_->async_get_connection(boost::asio::use_awaitable);
    //conn_.execute(sql, results);
	return true;
}

boost::asio::awaitable<boost::mysql::results> MySqlClient::query_async(const std::string& sql, int timeout_ms)
{
    boost::mysql::results results;
    //auto conn = co_await pool_->async_get_connection(boost::asio::use_awaitable);
    //co_await conn_.execute(sql, results);
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
