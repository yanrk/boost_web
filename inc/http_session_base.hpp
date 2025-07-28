/********************************************************
* Description : http session base
* Data        : 2018-04-03 14:46:00
* Author      : yanrk
* Email       : yanrkchina@163.com
* Blog        : blog.csdn.net/cxxmaker
* Version     : 2.0
* Copyright(C): 2019 - 2020
********************************************************/

#ifndef BOOST_WEB_HTTP_SESSION_BASE_HPP
#define BOOST_WEB_HTTP_SESSION_BASE_HPP


#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include <functional>
#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/make_unique.hpp>
#include <boost/optional/optional.hpp>
#include <boost/core/ignore_unused.hpp>
#include "websocket_session_base.hpp"
#include "websocket_session_plain.h"
#include "websocket_session_ssl.h"
#include "http_utility.h"
#include "address.h"
#include "boost_web.h"

namespace BoostWeb { // namespace BoostWeb begin

template <class Derived>
class HttpSessionBase : public HttpConnectionBase
{
public:
    explicit HttpSessionBase(boost::beast::flat_buffer buffer, const std::shared_ptr<const std::string> & doc_root, Address address, std::chrono::seconds timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service);

public:
    virtual void get_host_address(std::string & ip, unsigned short & port) const override;
    virtual void get_peer_address(std::string & ip, unsigned short & port) const override;

public:
    void recv();

protected:
    void on_recv(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_send(bool close, boost::beast::error_code ec, std::size_t bytes_transferred);

private:
    Derived & derived();

private:
    void send();

protected:
    Address                                                                     m_address;
    std::chrono::seconds                                                        m_timeout;
    uint64_t                                                                    m_body_limit;
    unsigned char                                                               m_protocol;
    WebServiceBase                                                            * m_service;
    boost::beast::flat_buffer                                                   m_recv_buffer;

private:
    enum { queue_limit = 8 };

private:
    std::shared_ptr<const std::string>                                                      m_doc_root;
    boost::optional<boost::beast::http::request_parser<boost::beast::http::string_body>>    m_parser;
    std::queue<boost::beast::http::message_generator>                                       m_response_queue;
};

template <class Derived>
HttpSessionBase<Derived>::HttpSessionBase(boost::beast::flat_buffer buffer, const std::shared_ptr<const std::string> & doc_root, Address address, std::chrono::seconds timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service)
    : m_address(std::move(address))
    , m_timeout(std::move(timeout))
    , m_body_limit(body_limit)
    , m_protocol(protocol)
    , m_service(service)
    , m_recv_buffer(std::move(buffer))
    , m_doc_root(doc_root)
    , m_parser()
    , m_response_queue()
{
    BOOST_ASSERT(nullptr != service);
}

template <class Derived>
Derived & HttpSessionBase<Derived>::derived()
{
    return static_cast<Derived &>(*this);
}

template <class Derived>
void HttpSessionBase<Derived>::get_host_address(std::string & ip, unsigned short & port) const
{
    ip = m_address.m_host_ip;
    port = m_address.m_host_port;
}

template <class Derived>
void HttpSessionBase<Derived>::get_peer_address(std::string & ip, unsigned short & port) const
{
    ip = m_address.m_peer_ip;
    port = m_address.m_peer_port;
}

template <class Derived>
void HttpSessionBase<Derived>::recv()
{
    m_parser.emplace();

    if (0 != m_body_limit)
    {
        m_parser->body_limit(m_body_limit);
    }

    boost::beast::get_lowest_layer(derived().stream()).expires_after(m_timeout);

    boost::beast::http::async_read(
        derived().stream(),
        m_recv_buffer,
        *m_parser,
        [self = derived().shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred) {
            self->on_recv(ec, bytes_transferred);
        }
    );
}

template <class Derived>
void HttpSessionBase<Derived>::on_recv(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (boost::beast::http::error::end_of_stream == ec)
    {
        derived().eof();
        return;
    }

    if (ec)
    {
        if (boost::asio::ssl::error::stream_truncated != ec)
        {
            m_service->on_error(derived().protocol(), "recv", ec.value(), ec.message().c_str());
        }
        return;
    }

    if (boost::beast::websocket::is_upgrade(m_parser->get()))
    {
        if (0x0 == ((support_protocol_t::protocol_ws | support_protocol_t::protocol_wss) & (derived().max_support_protocol() & m_protocol)))
        {
            m_service->on_error(derived().protocol(), "recv", 1, (0x0 != (support_protocol_t::protocol_wss & derived().max_support_protocol()) ? "not support wss protocol" : "not support ws protocol"));
        }
        else
        {
            boost::beast::get_lowest_layer(derived().stream()).expires_never();
            make_websocket_session(derived().release_stream(), std::move(m_address), m_service, m_parser->release());
        }
        return;
    }

    if (0x0 == ((support_protocol_t::protocol_http | support_protocol_t::protocol_https) & (derived().max_support_protocol() & m_protocol)))
    {
        m_service->on_error(derived().protocol(), "recv", 1, (0x0 != (support_protocol_t::protocol_https & derived().max_support_protocol()) ? "not support https protocol" : "not support http protocol"));
        return;
    }

    boost::beast::http::message_generator message = handle_request(m_service, *m_doc_root, *this, m_parser->release());

    m_response_queue.push(std::move(message));

    if (m_response_queue.size() == 1)
    {
        send();
    }

    if (m_response_queue.size() < queue_limit)
    {
        recv();
    }
}

template <class Derived>
void HttpSessionBase<Derived>::send()
{
    if (m_response_queue.empty())
    {
        return;
    }

    bool close = !m_response_queue.front().keep_alive();

    boost::beast::async_write(
        derived().stream(),
        std::move(m_response_queue.front()),
        [self = derived().shared_from_this(), close](boost::beast::error_code ec, std::size_t bytes_transferred) {
            self->on_send(close, ec, bytes_transferred);
        }
    );
}

template <class Derived>
void HttpSessionBase<Derived>::on_send(bool close, boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        if (boost::asio::ssl::error::stream_truncated != ec)
        {
            m_service->on_error(derived().protocol(), "send", ec.value(), ec.message().c_str());
        }
        return;
    }

    if (close)
    {
        derived().eof();
        return;
    }

    if (m_response_queue.size() == queue_limit)
    {
        recv();
    }

    m_response_queue.pop();

    send();
}

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_SESSION_BASE_HPP
