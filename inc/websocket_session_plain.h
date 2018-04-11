/********************************************************
 * Description : plain websocket session
 * Data        : 2018-04-03 09:24:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#ifndef BOOST_WEB_WEBSOCKET_SESSION_PLAIN_H
#define BOOST_WEB_WEBSOCKET_SESSION_PLAIN_H


#include <memory>
#include <type_traits>
#include "websocket_session_base.hpp"

namespace BoostWeb { // namespace BoostWeb begin

class WebsocketSession : public WebsocketSessionBase<WebsocketSession>, public std::enable_shared_from_this<WebsocketSession>
{
public:
    explicit WebsocketSession(boost::asio::ip::tcp::socket socket, std::chrono::seconds timeout, WebServiceBase * service);

public:
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> & websocket();
    boost::asio::ip::tcp::socket & socket();
    const char * protocol() const;

public:
    template <class Body, class Allocator> void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req);
    void timeout();

private:
    void on_close(boost::system::error_code ec);

private:
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket>               m_websocket;
    bool                                                                        m_close;
};

template <class Body, class Allocator>
void WebsocketSession::run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    start_timer();

    accept(std::move(req));
}

template <class Body, class Allocator>
void make_websocket_session(boost::asio::ip::tcp::socket socket, std::chrono::seconds timeout, WebServiceBase * service, boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    std::make_shared<WebsocketSession>(std::move(socket), std::move(timeout), service)->run(std::move(req));
}

} // namespace BoostWeb end


#endif // BOOST_WEB_WEBSOCKET_SESSION_PLAIN_H
