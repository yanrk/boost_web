/********************************************************
 * Description : ssl websocket connector
 * Data        : 2020-01-05 04:55:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_WEBSOCKET_CONNECTOR_SSL_H
#define BOOST_WEB_WEBSOCKET_CONNECTOR_SSL_H


#include <memory>
#include <chrono>
#include <type_traits>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>

namespace BoostWeb { // namespace BoostWeb begin

class WebsocketsConnector : public std::enable_shared_from_this<WebsocketsConnector>
{
public:
    explicit WebsocketsConnector(boost::asio::io_context & ioc, boost::asio::ssl::context & ctx, WebServiceBase * service, std::size_t identity, std::size_t timeout, const char * host, const char * port, const char * target);

public:
    void run();

private:
    void error(const char * what, boost::beast::error_code ec);

private:
    void on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint);
    void on_ssl_handshake(boost::beast::error_code ec);
    void on_handshake(boost::beast::error_code ec);

private:
    boost::asio::ip::tcp::resolver                                                          m_resolver;
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>>     m_stream;
    WebServiceBase                                                                        * m_service;
    std::size_t                                                                             m_identity;
    std::size_t                                                                             m_timeout;
    std::string                                                                             m_host;
    std::string                                                                             m_port;
    std::string                                                                             m_target;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_WEBSOCKET_CONNECTOR_SSL_H
