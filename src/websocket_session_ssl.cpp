/********************************************************
 * Description : ssl websocket session
 * Data        : 2018-04-03 10:18:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include "websocket_session_ssl.h"

namespace BoostWeb { // namespace BoostWeb begin

WebsocketsSession::WebsocketsSession(boost::beast::ssl_stream<boost::beast::tcp_stream> && stream, Address address, WebServiceBase * service)
    : WebsocketSessionBase<WebsocketsSession>(std::move(address), service)
    , m_websocket(std::move(stream))
{

}

WebsocketsSession::WebsocketsSession(boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> && stream, Address address, WebServiceBase * service)
    : WebsocketSessionBase<WebsocketsSession>(std::move(address), service)
    , m_websocket(std::move(stream))
{

}

boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> & WebsocketsSession::websocket()
{
    return (m_websocket);
}

const char * WebsocketsSession::protocol() const
{
    return ("wss");
}

void make_websocket_session(boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> stream, Address address, WebServiceBase * service, std::size_t identity)
{
    std::make_shared<WebsocketsSession>(std::move(stream), std::move(address), service)->run(identity);
}

} // namespace BoostWeb end
