/********************************************************
 * Description : listener
 * Data        : 2018-04-04 14:11:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include <memory>
#include <type_traits>
#include "detect_session.h"
#include "listener.h"

namespace BoostWeb { // namespace BoostWeb begin

Listener::Listener(boost::asio::io_context & ioc, boost::asio::ssl::context & ctx, boost::asio::ip::tcp::endpoint endpoint, const std::shared_ptr<const std::string> & doc_root, std::size_t timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service)
    : m_ioc(ioc)
    , m_ctx(ctx)
    , m_acceptor(boost::asio::make_strand(ioc))
    , m_doc_root(doc_root)
    , m_timeout(timeout)
    , m_body_limit(body_limit)
    , m_protocol(protocol)
    , m_service(service)
{
    BOOST_ASSERT(nullptr != service);

    boost::beast::error_code ec;

    m_acceptor.open(endpoint.protocol(), ec);
    if (ec)
    {
        m_service->on_error("listener", "open", ec.value(), ec.message().c_str());
        return;
    }

    m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
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
    accept();
}

void Listener::accept()
{
    m_acceptor.async_accept(boost::asio::make_strand(m_ioc), boost::beast::bind_front_handler(&Listener::on_accept, shared_from_this()));
}

void Listener::on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
    if (ec)
    {
        m_service->on_error("listener", "accept", ec.value(), ec.message().c_str());
    }
    else
    {
        std::make_shared<DetectSession>(std::move(socket), m_ctx, m_doc_root, m_timeout, m_body_limit, m_protocol, m_service)->run();
    }

    accept();
}

} // namespace BoostWeb end
