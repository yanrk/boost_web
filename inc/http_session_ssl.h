/********************************************************
 * Description : ssl http session
 * Data        : 2018-04-03 18:53:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_HTTP_SESSION_SSL_H
#define BOOST_WEB_HTTP_SESSION_SSL_H


#include <string>
#include <chrono>
#include <memory>
#include <type_traits>
#include "http_session_base.hpp"

namespace BoostWeb { // namespace BoostWeb begin

class HttpsSession : public HttpSessionBase<HttpsSession>, public std::enable_shared_from_this<HttpsSession>
{
public:
    HttpsSession(boost::beast::tcp_stream && stream, boost::asio::ssl::context & ctx, boost::beast::flat_buffer && buffer, const std::shared_ptr<const std::string> & doc_root, Address address, std::chrono::seconds timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service);

public:
    boost::beast::ssl_stream<boost::beast::tcp_stream> & stream();
    boost::beast::ssl_stream<boost::beast::tcp_stream> release_stream();
    const char * protocol() const;
    support_protocol_t::value_t max_support_protocol() const;

public:
    void run();
    void eof();

private:
    void on_handshake(boost::beast::error_code ec, std::size_t bytes_used);
    void on_shutdown(boost::beast::error_code ec);

private:
    boost::beast::ssl_stream<boost::beast::tcp_stream>                          m_stream;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_SESSION_SSL_H
