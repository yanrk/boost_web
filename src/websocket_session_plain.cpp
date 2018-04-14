/********************************************************
 * Description : plain websocket session
 * Data        : 2018-04-03 09:24:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include "websocket_session_plain.h"

namespace BoostWeb { // namespace BoostWeb begin

WebsocketSession::WebsocketSession(boost::asio::ip::tcp::socket socket, Address address, std::chrono::seconds timeout, WebServiceBase * service)
    : WebsocketSessionBase<WebsocketSession>(socket.get_executor().context(), std::move(address), std::move(timeout), service)
    , m_websocket(std::move(socket))
    , m_close(false)
{

}

boost::beast::websocket::stream<boost::asio::ip::tcp::socket> & WebsocketSession::websocket()
{
    return (m_websocket);
}

boost::asio::ip::tcp::socket & WebsocketSession::socket()
{
    return (m_websocket.next_layer());
}

const char * WebsocketSession::protocol() const
{
    return ("ws");
}

void WebsocketSession::timeout()
{
    if (m_close)
    {
        return;
    }

    m_close = true;

    start_timer();

    m_timer.expires_after(m_timeout);

    m_websocket.async_close(boost::beast::websocket::close_code::normal, boost::asio::bind_executor(m_strand, std::bind(&WebsocketSession::on_close, shared_from_this(), std::placeholders::_1)));
}

void WebsocketSession::on_close(boost::system::error_code ec)
{
    if (ec && boost::asio::error::operation_aborted != ec)
    {
        m_service->on_error(shared_from_this(), protocol(), "close", ec.value(), ec.message().c_str());
    }
    else
    {
        m_service->on_close(shared_from_this());
    }
}

} // namespace BoostWeb end
