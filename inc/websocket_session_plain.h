/********************************************************
 * Description : plain websocket session
 * Data        : 2018-04-03 09:24:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_WEBSOCKET_SESSION_PLAIN_H
#define BOOST_WEB_WEBSOCKET_SESSION_PLAIN_H


#include <memory>
#include <type_traits>
#include <boost/beast/core.hpp>
#include "websocket_session_base.hpp"

namespace BoostWeb { // namespace BoostWeb begin

class WebsocketSession : public WebsocketSessionBase<WebsocketSession>, public std::enable_shared_from_this<WebsocketSession>
{
public:
    explicit WebsocketSession(boost::beast::tcp_stream && stream, Address address, WebServiceBase * service);

public:
    boost::beast::websocket::stream<boost::beast::tcp_stream> & websocket();
    const char * protocol() const;

private:
    boost::beast::websocket::stream<boost::beast::tcp_stream>                               m_websocket;
};

template <class Body, class Allocator>
void make_websocket_session(boost::beast::tcp_stream stream, Address address, WebServiceBase * service, boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    std::make_shared<WebsocketSession>(std::move(stream), std::move(address), service)->run(std::move(req));
}

} // namespace BoostWeb end


#endif // BOOST_WEB_WEBSOCKET_SESSION_PLAIN_H
