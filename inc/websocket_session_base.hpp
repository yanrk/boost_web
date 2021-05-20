/********************************************************
* Description : websocket session base
* Data        : 2018-04-02 22:25:00
* Author      : yanrk
* Email       : yanrkchina@163.com
* Blog        : blog.csdn.net/cxxmaker
* Version     : 2.0
* Copyright(C): 2019 - 2020
********************************************************/

#ifndef BOOST_WEB_WEBSOCKET_SESSION_BASE_HPP
#define BOOST_WEB_WEBSOCKET_SESSION_BASE_HPP


#include <atomic>
#include <deque>
#include <string>
#include <memory>
#include <utility>
#include <functional>
#include <boost/asio/buffer.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <boost/asio/post.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
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
    explicit WebsocketSessionBase(Address address, WebServiceBase * service);

public:
    virtual void set_user_data(void * user_data) override;
    virtual void * get_user_data() const override;

public:
    virtual void get_host_address(std::string & ip, unsigned short & port) const override;
    virtual void get_peer_address(std::string & ip, unsigned short & port) const override;

public:
    virtual std::size_t recv_queue_size() override;
    virtual bool recv_buffer_has_data() override;
    virtual bool recv_buffer_data_is_text() override;
    virtual const void * recv_buffer_data() override;
    virtual std::size_t recv_buffer_size() override;
    virtual bool recv_buffer_drop() override;
    virtual std::size_t send_queue_size() override;
    virtual bool send_buffer_fill(bool text, const void * data, std::size_t len) override;

public:
    virtual void close() override;

public:
    template <class Body, class Allocator> void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req);
    void run(const void * identity);
    void eof();

public:
    template <class Body, class Allocator> void accept(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req);
    void recv();
    void send();

protected:
    void on_accept(boost::beast::error_code ec);
    void on_recv(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_send(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_close(boost::beast::error_code ec);
    void on_post(std::shared_ptr<BufferNode> buffer_node);

private:
    Derived & derived();

protected:
    Address                                                                     m_address;
    WebServiceBase                                                            * m_service;

private:
    std::deque<BufferNode>                                                      m_recv_queue;
    std::deque<BufferNode>                                                      m_send_queue;

private:
    std::atomic<std::size_t>                                                    m_recv_queue_size;
    std::atomic<std::size_t>                                                    m_send_queue_size;

private:
    void                                                                      * m_user_data;
    bool                                                                        m_has_close;
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
WebsocketSessionBase<Derived>::WebsocketSessionBase(Address address, WebServiceBase * service)
    : m_address(std::move(address))
    , m_service(service)
    , m_recv_queue()
    , m_send_queue()
    , m_recv_queue_size(0)
    , m_send_queue_size(0)
    , m_user_data(nullptr)
    , m_has_close(false)
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
std::size_t WebsocketSessionBase<Derived>::recv_queue_size()
{
    return (m_recv_queue_size);
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
bool WebsocketSessionBase<Derived>::recv_buffer_drop()
{
    if (m_recv_queue.empty())
    {
        return (false);
    }
    m_recv_queue.pop_front();
    --m_recv_queue_size;
    return (true);
}

template <class Derived>
std::size_t WebsocketSessionBase<Derived>::send_queue_size()
{
    return (m_send_queue_size);
}

template <class Derived>
bool WebsocketSessionBase<Derived>::send_buffer_fill(bool text, const void * data, std::size_t len)
{
    boost::beast::flat_buffer buffer;
    std::copy_n(reinterpret_cast<const char *>(0 != len ? data : ""), len, boost::asio::buffers_begin(buffer.prepare(len)));
    buffer.commit(len);

    boost::asio::post(derived().websocket().get_executor(), boost::beast::bind_front_handler(&WebsocketSessionBase::on_post, derived().shared_from_this(), std::make_shared<BufferNode>(text, std::move(buffer))));

    return (true);
}

template <class Derived>
void WebsocketSessionBase<Derived>::close()
{
    eof();
}

template <class Derived>
template <class Body, class Allocator>
void WebsocketSessionBase<Derived>::run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    accept(std::move(req));
}

template <class Derived>
void WebsocketSessionBase<Derived>::run(const void * identity)
{
    if (!m_service->on_connect(derived().shared_from_this(), identity))
    {
        close();
        return;
    }

    recv();
}

template <class Derived>
void WebsocketSessionBase<Derived>::eof()
{
    derived().websocket().async_close(boost::beast::websocket::normal, boost::beast::bind_front_handler(&WebsocketSessionBase::on_close, derived().shared_from_this()));
}

template <class Derived>
template <class Body, class Allocator>
void WebsocketSessionBase<Derived>::accept(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    derived().websocket().read_message_max(0);
    derived().websocket().set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
    derived().websocket().set_option(boost::beast::websocket::stream_base::decorator([](boost::beast::websocket::response_type & res){res.set(boost::beast::http::field::server, "boost web server on boost beast");}));
    derived().websocket().async_accept(req, boost::beast::bind_front_handler(&WebsocketSessionBase::on_accept, derived().shared_from_this()));
}

template <class Derived>
void WebsocketSessionBase<Derived>::recv()
{
    m_recv_queue.emplace_back(BufferNode(true, boost::beast::flat_buffer()));
    BOOST_ASSERT(!m_recv_queue.empty());
    derived().websocket().async_read(m_recv_queue.back().m_buffer, boost::beast::bind_front_handler(&WebsocketSessionBase::on_recv, derived().shared_from_this()));
}

template <class Derived>
void WebsocketSessionBase<Derived>::send()
{
    BOOST_ASSERT(!m_send_queue.empty());
    derived().websocket().text(m_send_queue.front().m_text);
    derived().websocket().async_write(m_send_queue.front().m_buffer.data(), boost::beast::bind_front_handler(&WebsocketSessionBase::on_send, derived().shared_from_this()));
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_accept(boost::beast::error_code ec)
{
    if (ec)
    {
        if (boost::asio::ssl::error::stream_truncated != ec)
        {
            m_service->on_error(derived().shared_from_this(), derived().protocol(), "accept", ec.value(), ec.message().c_str());
        }
        return;
    }

    if (!m_service->on_accept(derived().shared_from_this(), m_address.m_host_port))
    {
        close();
        return;
    }

    recv();
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_recv(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
    {
        if (boost::asio::ssl::error::stream_truncated != ec && boost::beast::websocket::error::closed != ec)
        {
            m_service->on_error(derived().shared_from_this(), derived().protocol(), "recv", ec.value(), ec.message().c_str());
        }
        close();
        return;
    }

    BOOST_ASSERT(!m_recv_queue.empty());

    m_recv_queue.back().m_text = derived().websocket().got_text();
//  m_recv_queue.back().m_buffer.commit(bytes_transferred);
    ++m_recv_queue_size;

    if (!m_service->on_recv(derived().shared_from_this()))
    {
        close();
        return;
    }

    recv();
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_send(boost::beast::error_code ec, std::size_t bytes_transferred)
{
    if (ec)
    {
        if (boost::asio::ssl::error::stream_truncated != ec)
        {
            m_service->on_error(derived().shared_from_this(), derived().protocol(), "send", ec.value(), ec.message().c_str());
        }
        close();
        return;
    }

    BOOST_ASSERT(!m_send_queue.empty());

    if (m_send_queue.front().m_buffer.size() > bytes_transferred)
    {
        m_send_queue.front().m_buffer.consume(bytes_transferred);
        return;
    }

    m_send_queue.pop_front();
    --m_send_queue_size;

    if (!m_service->on_send(derived().shared_from_this()))
    {
        close();
        return;
    }

    if (!m_send_queue.empty())
    {
        send();
    }
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_close(boost::beast::error_code ec)
{
    boost::ignore_unused(ec);

    if (!m_has_close)
    {
        m_has_close = true;
        m_service->on_close(derived().shared_from_this());
    }
}

template <class Derived>
void WebsocketSessionBase<Derived>::on_post(std::shared_ptr<BufferNode> buffer_node)
{
    m_send_queue.emplace_back(std::move(*buffer_node));
    ++m_send_queue_size;

    if (1 == m_send_queue.size())
    {
        send();
    }
}

} // namespace BoostWeb end


#endif // BOOST_WEB_WEBSOCKET_SESSION_BASE_HPP
