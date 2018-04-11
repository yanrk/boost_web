/********************************************************
 * Description : ssl websocket session
 * Data        : 2018-04-03 10:18:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#ifndef BOOST_WEB_WEBSOCKET_SESSION_SSL_H
#define BOOST_WEB_WEBSOCKET_SESSION_SSL_H


#include <memory>
#include <type_traits>
#include "common/ssl_stream.hpp"
#include "websocket_session_base.hpp"

namespace BoostWeb { // namespace BoostWeb begin

class WebsocketsSession : public WebsocketSessionBase<WebsocketsSession>, public std::enable_shared_from_this<WebsocketsSession>
{
public:
    explicit WebsocketsSession(ssl_stream<boost::asio::ip::tcp::socket> stream, std::chrono::seconds timeout, WebServiceBase * service);

public:
    boost::beast::websocket::stream<ssl_stream<boost::asio::ip::tcp::socket>> & websocket();
    boost::asio::ip::tcp::socket & socket();
    const char * protocol() const;

public:
    template <class Body, class Allocator> void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req);
    void timeout();

private:
    void on_shutdown(boost::system::error_code ec);

private:
    boost::beast::websocket::stream<ssl_stream<boost::asio::ip::tcp::socket>>   m_websockets;
    bool                                                                        m_eof;
};

template <class Body, class Allocator>
void WebsocketsSession::run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    start_timer();

    accept(std::move(req));
}

template <class Body, class Allocator>
void make_websocket_session(ssl_stream<boost::asio::ip::tcp::socket> stream, std::chrono::seconds timeout, WebServiceBase * service, boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    std::make_shared<WebsocketsSession>(std::move(stream), std::move(timeout), service)->run(std::move(req));
}

} // namespace BoostWeb end


#endif // BOOST_WEB_WEBSOCKET_SESSION_SSL_H