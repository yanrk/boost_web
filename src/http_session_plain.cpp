/********************************************************
 * Description : plain http session
 * Data        : 2018-04-03 18:15:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include "http_session_plain.h"

namespace BoostWeb { // namespace BoostWeb begin

HttpSession::HttpSession(boost::asio::ip::tcp::socket socket, boost::beast::flat_buffer buffer, const std::string & doc_root, Address address, std::chrono::seconds timeout, unsigned char protocol, WebServiceBase * service)
    : HttpSessionBase<HttpSession>(socket.get_executor().context(), std::move(buffer), doc_root, std::move(address), std::move(timeout), protocol, service)
    , m_socket(std::move(socket))
{

}

boost::asio::ip::tcp::socket & HttpSession::stream()
{
    return (m_socket);
}

boost::asio::ip::tcp::socket HttpSession::release_stream()
{
    return (std::move(m_socket));
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
    start_timer();

    recv();
}

void HttpSession::eof()
{
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}

void HttpSession::timeout()
{
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_socket.close(ec);
}

} // namespace BoostWeb end
