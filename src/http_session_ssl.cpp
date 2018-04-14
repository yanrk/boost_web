/********************************************************
 * Description : ssl http session
 * Data        : 2018-04-03 18:53:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include "http_session_ssl.h"

namespace BoostWeb { // namespace BoostWeb begin

HttpsSession::HttpsSession(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context & ctx, boost::beast::flat_buffer buffer, const std::string & doc_root, Address address, std::chrono::seconds timeout, unsigned char protocol, WebServiceBase * service)
    : HttpSessionBase<HttpsSession>(socket.get_executor().context(), std::move(buffer), doc_root, std::move(address), std::move(timeout), protocol, service)
    , m_stream(std::move(socket), ctx)
{

}

ssl_stream<boost::asio::ip::tcp::socket> & HttpsSession::stream()
{
    return (m_stream);
}

ssl_stream<boost::asio::ip::tcp::socket> HttpsSession::release_stream()
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
    start_timer();

    m_timer.expires_after(m_timeout);

    /*
     * this is the buffered version of the handshake
     */
    m_stream.async_handshake(boost::asio::ssl::stream_base::server, m_recv_buffer.data(), boost::asio::bind_executor(m_strand, std::bind(&HttpsSession::on_handshake, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));
}

void HttpsSession::eof()
{
    m_eof = true;

    m_timer.expires_after(m_timeout);

    m_stream.async_shutdown(boost::asio::bind_executor(m_strand, std::bind(&HttpsSession::on_shutdown, shared_from_this(), std::placeholders::_1)));
}

void HttpsSession::timeout()
{
    if (m_eof)
    {
        return;
    }

    start_timer();

    eof();
}

void HttpsSession::on_handshake(boost::system::error_code ec, std::size_t bytes_used)
{
    if (ec)
    {
        if (boost::asio::error::operation_aborted != ec)
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

void HttpsSession::on_shutdown(boost::system::error_code ec)
{
    if (ec && boost::asio::error::operation_aborted != ec)
    {
        m_service->on_error(protocol(), "shutdown", ec.value(), ec.message().c_str());
    }
}

} // namespace BoostWeb end
