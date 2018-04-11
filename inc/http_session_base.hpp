/********************************************************
* Description : http session base
* Data        : 2018-04-03 14:46:00
* Author      : yanrk
* Email       : yanrkchina@163.com
* Blog        : blog.csdn.net/cxxmaker
* Version     : 1.0
* Copyright(C): 2018
********************************************************/

#ifndef BOOST_WEB_HTTP_SESSION_BASE_HPP
#define BOOST_WEB_HTTP_SESSION_BASE_HPP


#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/make_unique.hpp>
#include "websocket_session_base.hpp"
#include "websocket_session_plain.h"
#include "websocket_session_ssl.h"
#include "boost_web.h"
#include "http_utility.h"

namespace BoostWeb { // namespace BoostWeb begin

template <class Derived>
class HttpSessionBase
{
public:
    HttpSessionBase(boost::asio::io_context & ioc, boost::beast::flat_buffer buffer, const std::string & doc_root, std::chrono::seconds timeout, unsigned char protocol, WebServiceBase * service);

public:
    void recv();

protected:
    void start_timer();

protected:
    void on_timer(boost::system::error_code ec);
    void on_recv(boost::system::error_code ec);
    void on_send(boost::system::error_code ec, bool close);

private:
    Derived & derived();

protected:
    boost::asio::strand<boost::asio::io_context::executor_type>                 m_strand;
    boost::asio::steady_timer                                                   m_timer;
    std::chrono::seconds                                                        m_timeout;
    unsigned char                                                               m_protocol;
    WebServiceBase                                                            * m_service;
    boost::beast::flat_buffer                                                   m_recv_buffer;

private:
    class WorkQueue
    {
    public:
        explicit WorkQueue(HttpSessionBase & session);

    public:
        bool is_full() const;

    public:
        void on_send();

    public:
        template <bool IsRequest, class Body, class Fields>
        void operator () (boost::beast::http::message<IsRequest, Body, Fields> && message);

    private:
        enum { queue_limit = 8 };

        struct Work
        {
            virtual ~Work() = default;
            virtual void operator () () = 0;
        };

        HttpSessionBase                       & m_session;
        std::vector<std::unique_ptr<Work>>      m_work_items;
    };

    const std::string                                                         & m_doc_root;
    boost::beast::http::request<boost::beast::http::string_body>                m_request;
    WorkQueue                                                                   m_work_queue;
};

template <class Derived>
HttpSessionBase<Derived>::WorkQueue::WorkQueue(HttpSessionBase<Derived> & session)
    : m_session(session)
    , m_work_items()
{
    static_assert(queue_limit > 0, "queue limit must be positive");
    m_work_items.reserve(queue_limit);
}

template <class Derived>
bool HttpSessionBase<Derived>::WorkQueue::is_full() const
{
    return (m_work_items.size() >= queue_limit);
}

template <class Derived>
void HttpSessionBase<Derived>::WorkQueue::on_send()
{
    BOOST_ASSERT(!m_work_items.empty());

    m_work_items.erase(m_work_items.begin());

    if (!m_work_items.empty())
    {
        (*m_work_items.front())();
    }
}

template <class Derived>
template <bool IsRequest, class Body, class Fields>
void HttpSessionBase<Derived>::WorkQueue::operator () (boost::beast::http::message<IsRequest, Body, Fields> && message)
{
    struct WorkImpl : Work
    {
        WorkImpl(HttpSessionBase & http, boost::beast::http::message<IsRequest, Body, Fields> && request)
            : m_http(http)
            , m_request(std::move(request))
        {

        }

        virtual void operator () ()
        {
            boost::beast::http::async_write(m_http.derived().stream(), m_request, boost::asio::bind_executor(m_http.m_strand, std::bind(&HttpSessionBase::on_send, m_http.derived().shared_from_this(), std::placeholders::_1, m_request.need_eof())));
        }

        HttpSessionBase                                       & m_http;
        boost::beast::http::message<IsRequest, Body, Fields>    m_request;
    };

    m_work_items.push_back(boost::make_unique<WorkImpl>(m_session, std::move(message)));

    if (1 == m_work_items.size())
    {
        (*m_work_items.front())();
    }
}

template <class Derived>
HttpSessionBase<Derived>::HttpSessionBase(boost::asio::io_context & ioc, boost::beast::flat_buffer buffer, const std::string & doc_root, std::chrono::seconds timeout, unsigned char protocol, WebServiceBase * service)
    : m_strand(ioc.get_executor())
    , m_timer(ioc, (std::chrono::steady_clock::time_point::max)())
    , m_timeout(std::move(timeout))
    , m_protocol(protocol)
    , m_service(service)
    , m_recv_buffer(std::move(buffer))
    , m_doc_root(doc_root)
    , m_request()
    , m_work_queue(*this)
{
    BOOST_ASSERT(nullptr != service);
}

template <class Derived>
Derived & HttpSessionBase<Derived>::derived()
{
    return (static_cast<Derived &>(*this));
}

template <class Derived>
void HttpSessionBase<Derived>::start_timer()
{
    m_timer.expires_at((std::chrono::steady_clock::time_point::max)());

    m_timer.async_wait(boost::asio::bind_executor(m_strand, std::bind(&HttpSessionBase::on_timer, derived().shared_from_this(), std::placeholders::_1)));
}

template <class Derived>
void HttpSessionBase<Derived>::recv()
{
    m_timer.expires_after(m_timeout);

    m_request = {};

    boost::beast::http::async_read(derived().stream(), m_recv_buffer, m_request, boost::asio::bind_executor(m_strand, std::bind(&HttpSessionBase::on_recv, derived().shared_from_this(), std::placeholders::_1)));
}

template <class Derived>
void HttpSessionBase<Derived>::on_timer(boost::system::error_code ec)
{
    if (ec && boost::asio::error::operation_aborted != ec)
    {
        m_service->on_error(derived().protocol(), "timer", ec.value(), ec.message().c_str());
        return;
    }

    if (m_timer.expiry() <= std::chrono::steady_clock::now())
    {
        derived().timeout();
        return;
    }

    m_timer.async_wait(boost::asio::bind_executor(m_strand, std::bind(&HttpSessionBase::on_timer, derived().shared_from_this(), std::placeholders::_1)));
}

template <class Derived>
void HttpSessionBase<Derived>::on_recv(boost::system::error_code ec)
{
    if (boost::beast::http::error::end_of_stream == ec)
    {
        derived().eof();
        return;
    }

    if (ec)
    {
        if (boost::asio::error::operation_aborted != ec)
        {
            m_service->on_error(derived().protocol(), "recv", ec.value(), ec.message().c_str());
        }
        return;
    }

    if (boost::beast::websocket::is_upgrade(m_request))
    {
        if (0x0 == ((support_protocol_t::protocol_ws | support_protocol_t::protocol_wss) & (derived().max_support_protocol() & m_protocol)))
        {
            m_service->on_error(derived().protocol(), "recv", 1, (0x0 != (support_protocol_t::protocol_wss & derived().max_support_protocol()) ? "not support wss protocol" : "not support ws protocol"));
        }
        else
        {
            make_websocket_session(derived().release_stream(), std::move(m_timeout), m_service, std::move(m_request));
        }
        return;
    }

    if (0x0 == ((support_protocol_t::protocol_http | support_protocol_t::protocol_https) & (derived().max_support_protocol() & m_protocol)))
    {
        m_service->on_error(derived().protocol(), "recv", 1, (0x0 != (support_protocol_t::protocol_https & derived().max_support_protocol()) ? "not support https protocol" : "not support http protocol"));
        return;
    }

    handle_request(m_service, m_doc_root, std::move(m_request), m_work_queue);

    if (!m_work_queue.is_full())
    {
        recv();
    }
}

template <class Derived>
void HttpSessionBase<Derived>::on_send(boost::system::error_code ec, bool close)
{
    if (ec)
    {
        if (boost::asio::error::operation_aborted != ec)
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

    bool was_full = m_work_queue.is_full();

    m_work_queue.on_send();

    if (was_full)
    {
        recv();
    }
}

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_SESSION_BASE_HPP
