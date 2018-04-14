/********************************************************
 * Description : ssl http session
 * Data        : 2018-04-03 18:53:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#ifndef BOOST_WEB_HTTP_SESSION_SSL_H
#define BOOST_WEB_HTTP_SESSION_SSL_H


#include <string>
#include <chrono>
#include <memory>
#include <type_traits>
#include <boost/asio/ip/tcp.hpp>
#include "common/ssl_stream.hpp"
#include "http_session_base.hpp"

namespace BoostWeb { // namespace BoostWeb begin

class HttpsSession : public HttpSessionBase<HttpsSession>, public std::enable_shared_from_this<HttpsSession>
{
public:
    HttpsSession(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context & ctx, boost::beast::flat_buffer buffer, const std::string & doc_root, Address address, std::chrono::seconds timeout, unsigned char protocol, WebServiceBase * service);

public:
    ssl_stream<boost::asio::ip::tcp::socket> & stream();
    ssl_stream<boost::asio::ip::tcp::socket> release_stream();
    const char * protocol() const;
    support_protocol_t::value_t max_support_protocol() const;

public:
    void run();
    void eof();
    void shutdown(boost::system::error_code ec);
    void timeout();

private:
    void on_handshake(boost::system::error_code ec, std::size_t bytes_used);
    void on_shutdown(boost::system::error_code ec);

private:
    ssl_stream<boost::asio::ip::tcp::socket>                                    m_stream;
    bool                                                                        m_eof;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_SESSION_SSL_H
