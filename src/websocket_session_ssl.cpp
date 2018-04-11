/********************************************************
 * Description : ssl websocket session
 * Data        : 2018-04-03 10:18:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include "websocket_session_ssl.h"

namespace BoostWeb { // namespace BoostWeb begin

WebsocketsSession::WebsocketsSession(ssl_stream<boost::asio::ip::tcp::socket> stream, std::chrono::seconds timeout, WebServiceBase * service)
    : WebsocketSessionBase<WebsocketsSession>(stream.get_executor().context(), std::move(timeout), service)
    , m_websockets(std::move(stream))
    , m_eof(false)
{

}

boost::beast::websocket::stream<ssl_stream<boost::asio::ip::tcp::socket>> & WebsocketsSession::websocket()
{
    return (m_websockets);
}

boost::asio::ip::tcp::socket & WebsocketsSession::socket()
{
    return (m_websockets.next_layer().next_layer());
}

const char * WebsocketsSession::protocol() const
{
    return ("wss");
}

void WebsocketsSession::timeout()
{
    if (m_eof)
    {
        return;
    }

    m_eof = true;

    start_timer();

    m_timer.expires_after(m_timeout);

    m_websockets.next_layer().async_shutdown(boost::asio::bind_executor(m_strand, std::bind(&WebsocketsSession::on_shutdown, shared_from_this(), std::placeholders::_1)));
}

void WebsocketsSession::on_shutdown(boost::system::error_code ec)
{
    if (ec && boost::asio::error::operation_aborted != ec)
    {
        m_service->on_error(shared_from_this(), protocol(), "shutdown", ec.value(), ec.message().c_str());
    }
    else
    {
        m_service->on_close(shared_from_this());
    }
}

} // namespace BoostWeb end
