/********************************************************
 * Description : ssl http session
 * Data        : 2018-04-03 18:53:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include "http_session_ssl.h"

namespace BoostWeb { // namespace BoostWeb begin

HttpsSession::HttpsSession(boost::beast::tcp_stream && stream, boost::asio::ssl::context & ctx, boost::beast::flat_buffer && buffer, const std::shared_ptr<const std::string> & doc_root, Address address, std::chrono::seconds timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service)
    : HttpSessionBase<HttpsSession>(std::move(buffer), doc_root, std::move(address), std::move(timeout), body_limit, protocol, service)
    , m_stream(std::move(stream), ctx)
{

}

boost::beast::ssl_stream<boost::beast::tcp_stream> & HttpsSession::stream()
{
    return (m_stream);
}

boost::beast::ssl_stream<boost::beast::tcp_stream> HttpsSession::release_stream()
{
    return (std::move(m_stream));
}

const char * HttpsSession::protocol() const
{
    return ("https");
}

support_protocol_t::value_t HttpsSession::max_support_protocol() const
{
    return (support_protocol_t::protocol_ssl);
}

void HttpsSession::run()
{
    boost::beast::get_lowest_layer(m_stream).expires_after(m_timeout);

    /*
     * this is the buffered version of the handshake
     */
    m_stream.async_handshake(boost::asio::ssl::stream_base::server, m_recv_buffer.data(), boost::beast::bind_front_handler(&HttpsSession::on_handshake, shared_from_this()));
}

void HttpsSession::eof()
{
    boost::beast::get_lowest_layer(m_stream).expires_after(m_timeout);

    m_stream.async_shutdown(boost::beast::bind_front_handler(&HttpsSession::on_shutdown, shared_from_this()));
}

void HttpsSession::on_handshake(boost::beast::error_code ec, std::size_t bytes_used)
{
    if (ec)
    {
        if (boost::asio::ssl::error::stream_truncated != ec)
        {
            m_service->on_error(protocol(), "handshake", ec.value(), ec.message().c_str());
        }
        return;
    }

    /*
     * consume the portion of the buffer used by the handshake
     */
    m_recv_buffer.consume(bytes_used);

    recv();
}

void HttpsSession::on_shutdown(boost::beast::error_code ec)
{
    if (ec && boost::asio::ssl::error::stream_truncated != ec)
    {
        m_service->on_error(protocol(), "shutdown", ec.value(), ec.message().c_str());
    }
}

} // namespace BoostWeb end
