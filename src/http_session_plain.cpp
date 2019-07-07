/********************************************************
 * Description : plain http session
 * Data        : 2018-04-03 18:15:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include "http_session_plain.h"

namespace BoostWeb { // namespace BoostWeb begin

HttpSession::HttpSession(boost::beast::tcp_stream && stream, boost::beast::flat_buffer && buffer, const std::shared_ptr<const std::string> & doc_root, Address address, std::chrono::seconds timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service)
    : HttpSessionBase<HttpSession>(std::move(buffer), doc_root, std::move(address), std::move(timeout), body_limit, protocol, service)
    , m_stream(std::move(stream))
{

}

boost::beast::tcp_stream & HttpSession::stream()
{
    return (m_stream);
}

boost::beast::tcp_stream HttpSession::release_stream()
{
    return (std::move(m_stream));
}

const char * HttpSession::protocol() const
{
    return ("http");
}

support_protocol_t::value_t HttpSession::max_support_protocol() const
{
    return (support_protocol_t::protocol_plain);
}

void HttpSession::run()
{
    recv();
}

void HttpSession::eof()
{
    boost::beast::error_code ec;
    m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}

} // namespace BoostWeb end
