/********************************************************
 * Description : listener
 * Data        : 2018-04-04 14:11:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include <memory>
#include <type_traits>
#include "detect_session.h"
#include "listener.h"

namespace BoostWeb { // namespace BoostWeb begin

Listener::Listener(boost::asio::io_context & ioc, boost::asio::ssl::context & ctx, boost::asio::ip::tcp::endpoint endpoint, const std::string & doc_root, std::size_t timeout, unsigned char protocol, WebServiceBase * service)
    : m_ctx(ctx)
    , m_acceptor(ioc)
    , m_socket(ioc)
    , m_doc_root(doc_root)
    , m_timeout(timeout)
    , m_protocol(protocol)
    , m_service(service)
{
    BOOST_ASSERT(nullptr != service);

    boost::system::error_code ec;

    m_acceptor.open(endpoint.protocol(), ec);
    if (ec)
    {
        m_service->on_error("listener", "open", ec.value(), ec.message().c_str());
        return;
    }

    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    if (ec)
    {
        m_service->on_error("listener", "reuse", ec.value(), ec.message().c_str());
        return;
    }

    m_acceptor.bind(endpoint, ec);
    if (ec)
    {
        m_service->on_error("listener", "bind", ec.value(), ec.message().c_str());
        return;
    }

    m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec)
    {
        m_service->on_error("listener", "listen", ec.value(), ec.message().c_str());
        return;
    }
}

void Listener::run()
{
    if (!m_acceptor.is_open())
    {
        return;
    }

    accept();
}

void Listener::accept()
{
    m_acceptor.async_accept(m_socket, std::bind(&Listener::on_accept, shared_from_this(), std::placeholders::_1));
}

void Listener::on_accept(boost::system::error_code ec)
{
    if (ec)
    {
        m_service->on_error("listener", "accept", ec.value(), ec.message().c_str());
    }
    else
    {
        std::make_shared<DetectSession>(std::move(m_socket), m_ctx, m_doc_root, m_timeout, m_protocol, m_service)->run();
    }

    accept();
}

} // namespace BoostWeb end
