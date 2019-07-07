/********************************************************
 * Description : plain websocket session
 * Data        : 2018-04-03 09:24:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include "websocket_session_plain.h"

namespace BoostWeb { // namespace BoostWeb begin

WebsocketSession::WebsocketSession(boost::beast::tcp_stream && stream, Address address, WebServiceBase * service)
    : WebsocketSessionBase<WebsocketSession>(std::move(address), service)
    , m_websocket(std::move(stream))
{

}

boost::beast::websocket::stream<boost::beast::tcp_stream> & WebsocketSession::websocket()
{
    return (m_websocket);
}

const char * WebsocketSession::protocol() const
{
    return ("ws");
}

} // namespace BoostWeb end
