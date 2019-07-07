/********************************************************
 * Description : detect session
 * Data        : 2018-04-04 08:40:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include <type_traits>
#include "detect_session.h"
#include "http_session_plain.h"
#include "http_session_ssl.h"

namespace BoostWeb { // namespace BoostWeb begin

DetectSession::DetectSession(boost::asio::ip::tcp::socket && socket, boost::asio::ssl::context & ctx, const std::shared_ptr<const std::string> & doc_root, std::size_t timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service)
    : m_stream(std::move(socket))
    , m_ctx(ctx)
    , m_doc_root(doc_root)
    , m_buffer()
    , m_address()
    , m_timeout(timeout)
    , m_body_limit(body_limit)
    , m_protocol(protocol)
    , m_service(service)
{
    boost::beast::error_code ec;
    m_address.m_host_ip = m_stream.socket().local_endpoint(ec).address().to_string(ec);
    m_address.m_host_port = m_stream.socket().local_endpoint(ec).port();
    m_address.m_peer_ip = m_stream.socket().remote_endpoint(ec).address().to_string(ec);
    m_address.m_peer_port = m_stream.socket().remote_endpoint(ec).port();

    BOOST_ASSERT(nullptr != service);
}

void DetectSession::run()
{
    m_stream.expires_after(m_timeout);

    boost::beast::async_detect_ssl(m_stream, m_buffer, boost::beast::bind_front_handler(&DetectSession::on_detect, shared_from_this()));
}

void DetectSession::on_detect(boost::beast::error_code ec, boost::tribool result)
{
    if (ec)
    {
        m_service->on_error("detector", "detect", ec.value(), ec.message().c_str());
        return;
    }

    if (result)
    {
        if (0x0 == (support_protocol_t::protocol_ssl & m_protocol))
        {
            m_service->on_error("detector", "detect", 1, "not support ssl protocol");
        }
        else
        {
            std::make_shared<HttpsSession>(std::move(m_stream), m_ctx, std::move(m_buffer), m_doc_root, std::move(m_address), std::move(m_timeout), m_body_limit, m_protocol, m_service)->run();
        }
    }
    else
    {
        if (0x0 == (support_protocol_t::protocol_plain & m_protocol))
        {
            m_service->on_error("detector", "detect", 1, "not support plain protocol");
        }
        else
        {
            std::make_shared<HttpSession>(std::move(m_stream), std::move(m_buffer), m_doc_root, std::move(m_address), std::move(m_timeout), m_body_limit, m_protocol, m_service)->run();
        }
    }
}

} // namespace BoostWeb end
