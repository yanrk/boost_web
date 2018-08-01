/********************************************************
* Description : websocket session base
* Data        : 2018-04-02 22:25:00
* Author      : yanrk
* Email       : yanrkchina@163.com
* Blog        : blog.csdn.net/cxxmaker
* Version     : 1.0
* Copyright(C): 2018
********************************************************/

#ifndef BOOST_WEB_WEBSOCKET_SESSION_BASE_HPP
#define BOOST_WEB_WEBSOCKET_SESSION_BASE_HPP


#include <deque>
#include <string>
#include <memory>
#include <utility>
#include <functional>
#include <boost/beast/core.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/post.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include "address.h"
#include "boost_web.h"

namespace BoostWeb { // namespace BoostWeb begin

template <class Derived>
class WebsocketSessionBase : public WebsocketConnectionBase
{
private:
    struct BufferNode
    {
        BufferNode(bool text, boost::beast::flat_buffer && buffer);
        BufferNode(BufferNode && other);

        bool                            m_text;
        boost::beast::flat_buffer       m_buffer;
    };

public:
    explicit WebsocketSessionBase(boost::asio::io_context & ioc, Address address, std::chrono::seconds timeout, WebServiceBase * service);

public:
    virtual void set_user_data(void * user_data) override;
    virtual void * get_user_data() const override;

public:
    virtual void get_host_address(std::string & ip, unsigned short & port) const override;
    virtual void get_peer_address(std::string & ip, unsigned short & port) const override;

public:
    virtual bool recv_buffer_has_data() override;
    virtual bool recv_buffer_data_is_text() override;
    virtual const void * recv_buffer_data() override;
    virtual std::size_t recv_buffer_size() override;
    virtual bool recv_buffer_drop_len(std::size_t len) override;
    virtual bool send_buffer_fill_len(bool text, const void * data, std::size_t len) override;

public:
    template <class Body, class Allocator> void accept(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req);
    void recv();
    void send();
    void activity();

protected:
    void start_timer();

protected:
    void on_timer(boost::system::error_code ec);
    void on_accept(boost::system::error_code ec);
    void on_control(boost::beast::websocket::frame_type kind, boost::beast::string_view payload);
    void on_ping(boost::system::error_code ec);
    void on_recv(boost::system::error_code ec, std::size_t bytes_transferred);
    void on_send(boost::system::error_code ec, std::size_t bytes_transferred);
    void on_post(std::shared_ptr<BufferNode> buffer_node);

private:
    Derived & derived();

protected:
    boost::asio::strand<boost::asio::io_context::executor_type>                 m_strand;
    boost::asio::steady_timer                                                   m_timer;
    Address                                                                     m_address;
    std::chrono::seconds                                                        m_timeout;
    WebServiceBase                                                            * m_service;

private:
    std::deque<BufferNode>                                                      m_recv_queue;
    std::deque<BufferNode>                                                      m_send_queue;

private:
    char                                                                        m_ping_state;

private:
    void                                                                      * m_user_data;
};

template <class Derived>
WebsocketSessionBase<Derived>::BufferNode::BufferNode(bool text, boost::beast::flat_buffer && buffer)
    : m_text(text)
    , m_buffer(std::move(buffer))
{

}

template <class Derived>
WebsocketSessionBase<Derived>::BufferNode::BufferNode(BufferNode && other)
    : m_text(other.m_text)
    , m_buffer(std::move(other.m_buffer))
{

}

template <class Derived>
WebsocketSessionBase<Derived>::WebsocketSessionBase(boost::asio::io_context & ioc, Address address, std::chrono::seconds timeout, WebServiceBase * service)
    : m_strand(ioc.get_executor())
    , m_timer(ioc, (std::chrono::steady_clock::time_point::max)())
    , m_address(std::move(address))
    , m_timeout(std::move(timeout))
    , m_service(service)
    , m_recv_queue()
    , m_send_queue()
    , m_ping_state(0x0)
    , m_user_data(nullptr)
{
    BOOST_ASSERT(nullptr != service);
}

template <class Derived>
Derived & WebsocketSessionBase<Derived>::derived()
{
    return (static_cast<Derived &>(*this));
}

template <class Derived>
void WebsocketSessionBase<Derived>::set_user_data(void * user_data)
{
    m_user_data = user_data;
}

template <class Derived>
void * WebsocketSessionBase<Derived>::get_user_data() const
{
    return (m_user_data);
}

template <class Derived>
void WebsocketSessionBase<Derived>::get_host_address(std::string & ip, unsigned short & port) const
{
    ip = m_address.m_host_ip;
    port = m_address.m_host_port;
}

template <class Derived>
void WebsocketSessionBase<Derived>::get_peer_address(std::string & ip, unsigned short & port) const
{
    ip = m_address.m_peer_ip;
    port = m_address.m_peer_port;
}

template <class Derived>
bool WebsocketSessionBase<Derived>::recv_buffer_has_data()
{
    return (!m_recv_queue.empty());
}

template <class Derived>
bool WebsocketSessionBase<Derived>::recv_buffer_data_is_text()
{
    if (m_recv_queue.empty())
    {
        return (false);
    }
    return (m_recv_queue.front().m_text);
}

template <class Derived>
const void * WebsocketSessionBase<Derived>::recv_buffer_data()
{
    if (m_recv_queue.empty())
    {
        return (nullptr);
    }
    return (boost::asio::buffer_cast<const char *>(m_recv_queue.front().m_buffer.data()));
}

template <class Derived>
std::size_t WebsocketSessionBase<Derived>::recv_buffer_size()
{
    if (m_recv_queue.empty())
    {
        return (0);
    }
    return (m_recv_queue.front().m_buffer.size());
}

template <class Derived>
bool WebsocketSessionBase<Derived>::recv_buffer_drop_len(std::size_t len)
{
    if (m_recv_queue.empty())
    {
        return (false);
    }

    if (m_recv_queue.front().m_buffer.size() > len)
    {
        m_recv_queue.front().m_buffer.consume(len);
    }
    else
    {
        m_recv_queue.pop_front();
    }

    return (true);
}

template <class Derived>
bool WebsocketSessionBase<Derived>::send_buffer_fill_len(bool text, const void * data, std::size_t len)
{
    boost::beast::flat_buffer buffer;
    std::copy_n(reinterpret_cast<const char *>(0 != len ? data : ""), len, boost::asio::buffers_begin(buffer.prepare(len)));
    buffer.commit(len);

    boost::asio::post(derived().websocket().get_executor(), boost::asio::bind_executor(m_strand, std::bind(&WebsocketSessionBase::on_post, derived().shared_from_this(), std::make_shared<BufferNode>(text, std::move(buffer)))));

    return (true);
}

template <class Derived>
void WebsocketSessionBase<Derived>::start_timer()
{
    m_timer.expires_at((std::chrono::steady_clock::time_point::max)());

    m_timer.async_wait(boost::asio::bind_executor(m_strand, std::bind(&WebsocketSessionBase::on_timer, derived().shared_from_this(), std::placeholders::_1)));
}

template <class Derived>
template <class Body, class Allocator>
void WebsocketSessionBase<Derived>::accept(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    /*
     * need not use "derived().shared_from_this()" replace "this"
     * refer to the instructions for websocket::stream::control_callback()
     */
    derived().websocket().control_callback(std::bind(&WebsocketSessionBase::on_control, this, std::placeholders::_1, std::placeholders::_2));

    m_timer.expires_after(m_timeout);

    derived().websocket().async_accept(req, boost::asio::bind_executor(m_strand, std::bind(&WebsocketSessionBase::on_accept, derived().shared_from_this(), std::placeholders::_1)));
}

template <class Derived>
void WebsocketSessionBase<Derived>::recv()
{
    m_recv_queue.emplace_back(BufferNode(true, boost::beast::flat_buffer()));
    BOOST_ASSERT(!m_recv_queue.empty());
    derived().websocket().async_read(m_recv_queue.back().m_buffer, boost::asio::bind_executor(m_strand, std::bind(&WebsocketSessionBase::on_recv, derived().shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
}

template <class Derived>
void WebsocketSessionBase<Derived>::send()
{
    BOOST_ASSERT(!m_send_queue.empty());
    derived().websocket().text(m_send_queue.front().m_text);
    derived().websocket().async_write(m_send_queue.front().m_buffer.data(), boost::asio::bind_executor(m_strand, std::bind(&WebsocketSessionBase::on_send, derived().shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
}

template <class Derived>
void WebsocketSessionBase<Derived>::activity()
{
    m_ping_state = 0x0;

    m_timer.expires_after(m_timeout);
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_timer(boost::system::error_code ec)
{
    if (ec && boost::asio::error::operation_aborted != ec)
    {
        m_service->on_error(derived().shared_from_this(), derived().protocol(), "timer", ec.value(), ec.message().c_str());
        return;
    }

    if (m_timer.expiry() <= std::chrono::steady_clock::now())
    {
        if (derived().websocket().is_open() && 0x0 == m_ping_state)
        {
            m_ping_state = 0x1;

            m_timer.expires_after(m_timeout);

            derived().websocket().async_ping({}, boost::asio::bind_executor(m_strand, std::bind(&WebsocketSessionBase::on_ping, derived().shared_from_this(), std::placeholders::_1)));
        }
        else
        {
            derived().timeout();
            return;
        }
    }

    m_timer.async_wait(boost::asio::bind_executor(m_strand, std::bind(&WebsocketSessionBase::on_timer, derived().shared_from_this(), std::placeholders::_1)));
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_accept(boost::system::error_code ec)
{
    if (ec)
    {
        if (boost::asio::error::operation_aborted != ec)
        {
            m_service->on_error(derived().shared_from_this(), derived().protocol(), "accept", ec.value(), ec.message().c_str());
        }
        return;
    }

    m_service->on_accept(derived().shared_from_this(), m_address.m_host_port);

    recv();
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_control(boost::beast::websocket::frame_type kind, boost::beast::string_view payload)
{
    boost::ignore_unused(kind, payload);

    activity();
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_ping(boost::system::error_code ec)
{
    if (ec)
    {
        if (boost::asio::error::operation_aborted != ec)
        {
            m_service->on_error(derived().shared_from_this(), derived().protocol(), "ping", ec.value(), ec.message().c_str());
        }
        return;
    }

    if (0x1 == m_ping_state)
    {
        m_ping_state = 0x2;
    }
    else
    {
        BOOST_ASSERT(0x0 == m_ping_state);
    }
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_recv(boost::system::error_code ec, std::size_t bytes_transferred)
{
    if (boost::beast::websocket::error::closed == ec || boost::asio::error::operation_aborted == ec)
    {
        return;
    }
    else if (ec)
    {
        m_service->on_error(derived().shared_from_this(), derived().protocol(), "recv", ec.value(), ec.message().c_str());
        return;
    }

    activity();

    BOOST_ASSERT(!m_recv_queue.empty());

    m_recv_queue.back().m_text = derived().websocket().got_text();
//  m_recv_queue.back().m_buffer.commit(bytes_transferred);

    m_service->on_recv(derived().shared_from_this());

    recv();
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_send(boost::system::error_code ec, std::size_t bytes_transferred)
{
    if (ec)
    {
        if (boost::asio::error::operation_aborted != ec)
        {
            m_service->on_error(derived().shared_from_this(), derived().protocol(), "send", ec.value(), ec.message().c_str());
        }
        return;
    }

    BOOST_ASSERT(!m_send_queue.empty());

    if (m_send_queue.front().m_buffer.size() > bytes_transferred)
    {
        m_send_queue.front().m_buffer.consume(bytes_transferred);
        return;
    }

    m_send_queue.pop_front();

    m_service->on_send(derived().shared_from_this());

    if (!m_send_queue.empty())
    {
        send();
    }
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_post(std::shared_ptr<BufferNode> buffer_node)
{
    m_send_queue.emplace_back(std::move(*buffer_node));

    if (1 == m_send_queue.size())
    {
        send();
    }
}

} // namespace BoostWeb end


#endif // BOOST_WEB_WEBSOCKET_SESSION_BASE_HPP
