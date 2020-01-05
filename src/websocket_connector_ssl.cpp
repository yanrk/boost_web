/********************************************************
 * Description : ssl websocket connector
 * Data        : 2020-01-05 04:55:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include "address.h"
#include "boost_web.h"
#include "websocket_session_ssl.h"
#include "websocket_connector_ssl.h"

namespace BoostWeb { // namespace BoostWeb begin

WebsocketsConnector::WebsocketsConnector(boost::asio::io_context & ioc, boost::asio::ssl::context & ctx, WebServiceBase * service, std::size_t identity, std::size_t timeout, const char * host, const char * port, const char * target)
    : m_resolver(boost::asio::make_strand(ioc))
    , m_stream(boost::asio::make_strand(ioc), ctx)
    , m_service(service)
    , m_identity(identity)
    , m_timeout(timeout)
    , m_host(host)
    , m_port(port)
    , m_target(target)
{

}

void WebsocketsConnector::error(const char * what, boost::beast::error_code ec)
{
    m_service->on_error("wss", what, ec.value(), ec.message().c_str());
    m_service->on_connect(nullptr, m_identity);
}

void WebsocketsConnector::run()
{
    m_resolver.async_resolve(m_host, m_port, boost::beast::bind_front_handler(&WebsocketsConnector::on_resolve, shared_from_this()));
}

void WebsocketsConnector::on_resolve(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results)
{
    if (ec)
    {
        error("resolve", ec);
        return;
    }

    boost::beast::get_lowest_layer(m_stream).expires_after(std::chrono::seconds(m_timeout));
    boost::beast::get_lowest_layer(m_stream).async_connect(results, boost::beast::bind_front_handler(&WebsocketsConnector::on_connect, shared_from_this()));
}

void WebsocketsConnector::on_connect(boost::beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint)
{
    boost::ignore_unused(endpoint);

    if (ec)
    {
        error("connect", ec);
        return;
    }

    boost::beast::get_lowest_layer(m_stream).expires_after(std::chrono::seconds(m_timeout));
    m_stream.next_layer().async_handshake(boost::asio::ssl::stream_base::client, boost::beast::bind_front_handler(&WebsocketsConnector::on_ssl_handshake, shared_from_this()));
}

void WebsocketsConnector::on_ssl_handshake(boost::beast::error_code ec)
{
    if (ec)
    {
        error("ssl handshake", ec);
        return;
    }

    boost::beast::get_lowest_layer(m_stream).expires_never();
    m_stream.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));
    m_stream.set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::request_type & req){req.set(boost::beast::http::field::user_agent, "boost web client on boost beast");}));
    m_stream.async_handshake(m_host, m_target, boost::beast::bind_front_handler(&WebsocketsConnector::on_handshake, shared_from_this()));
}

void WebsocketsConnector::on_handshake(boost::beast::error_code ec)
{
    if (ec)
    {
        error("handshake", ec);
        return;
    }

    Address address;
    address.m_host_ip = boost::beast::get_lowest_layer(m_stream).socket().local_endpoint(ec).address().to_string(ec);
    address.m_host_port = boost::beast::get_lowest_layer(m_stream).socket().local_endpoint(ec).port();
    address.m_peer_ip = boost::beast::get_lowest_layer(m_stream).socket().remote_endpoint(ec).address().to_string(ec);
    address.m_peer_port = boost::beast::get_lowest_layer(m_stream).socket().remote_endpoint(ec).port();

    make_websocket_session(std::move(m_stream), std::move(address), m_service, m_identity);
}

} // namespace BoostWeb end
