/********************************************************
 * Description : plain http session
 * Data        : 2018-04-03 18:15:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#ifndef BOOST_WEB_HTTP_SESSION_PLAIN_H
#define BOOST_WEB_HTTP_SESSION_PLAIN_H


#include <string>
#include <chrono>
#include <memory>
#include <type_traits>
#include <boost/asio/ip/tcp.hpp>
#include "http_session_base.hpp"

namespace BoostWeb { // namespace BoostWeb begin

class HttpSession : public HttpSessionBase<HttpSession>, public std::enable_shared_from_this<HttpSession>
{
public:
    HttpSession(boost::asio::ip::tcp::socket socket, boost::beast::flat_buffer buffer, const std::string & doc_root, std::chrono::seconds timeout, unsigned char protocol, WebServiceBase * service);

public:
    boost::asio::ip::tcp::socket & stream();
    boost::asio::ip::tcp::socket release_stream();
    const char * protocol() const;
    support_protocol_t::value_t max_support_protocol() const;

public:
    void run();
    void eof();
    void timeout();

private:
    boost::asio::ip::tcp::socket                                                m_socket;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_SESSION_PLAIN_H
