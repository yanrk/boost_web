/********************************************************
 * Description : detect session
 * Data        : 2018-04-04 08:40:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include <type_traits>
#include "detect_session.h"
#include "http_session_plain.h"
#include "http_session_ssl.h"

namespace BoostWeb { // namespace BoostWeb begin

DetectSession::DetectSession(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context & ctx, const std::string & doc_root, std::size_t timeout, unsigned char protocol, WebServiceBase * service)
    : m_socket(std::move(socket))
    , m_ctx(ctx)
    , m_strand(m_socket.get_executor())
    , m_doc_root(doc_root)
    , m_buffer()
    , m_timeout(timeout)
    , m_protocol(protocol)
    , m_service(service)
{
    BOOST_ASSERT(nullptr != service);
}

void DetectSession::run()
{
    async_detect_ssl(m_socket, m_buffer, boost::asio::bind_executor(m_strand, std::bind(&DetectSession::on_detect, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
}

void DetectSession::on_detect(boost::system::error_code ec, boost::tribool result)
{
    if (ec)
    {
        m_service->on_error("detector", "detect", ec.value(), ec.message().c_str());
        return;
    }

    if (result)
    {
        if (0 == (support_protocol_t::protocol_ssl & m_protocol))
        {
            m_service->on_error("detector", "detect", 1, "not support ssl protocol");
        }
        else
        {
            std::make_shared<HttpsSession>(std::move(m_socket), m_ctx, std::move(m_buffer), m_doc_root, std::move(m_timeout), m_protocol, m_service)->run();
        }
    }
    else
    {
        if (0 == (support_protocol_t::protocol_plain & m_protocol))
        {
            m_service->on_error("detector", "detect", 1, "not support plain protocol");
        }
        else
        {
            std::make_shared<HttpSession>(std::move(m_socket), std::move(m_buffer), m_doc_root, std::move(m_timeout), m_protocol, m_service)->run();
        }
    }
}

} // namespace BoostWeb end
