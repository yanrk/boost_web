/********************************************************
 * Description : plain http session
 * Data        : 2018-04-03 18:15:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_HTTP_SESSION_PLAIN_H
#define BOOST_WEB_HTTP_SESSION_PLAIN_H


#include <string>
#include <chrono>
#include <memory>
#include <type_traits>
#include "http_session_base.hpp"

namespace BoostWeb { // namespace BoostWeb begin

class HttpSession : public HttpSessionBase<HttpSession>, public std::enable_shared_from_this<HttpSession>
{
public:
    HttpSession(boost::beast::tcp_stream && stream, boost::beast::flat_buffer && buffer, const std::shared_ptr<const std::string> & doc_root, Address address, std::chrono::seconds timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service);

public:
    boost::beast::tcp_stream & stream();
    boost::beast::tcp_stream release_stream();
    const char * protocol() const;
    support_protocol_t::value_t max_support_protocol() const;

public:
    void run();
    void eof();

private:
    boost::beast::tcp_stream                                                    m_stream;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_SESSION_PLAIN_H
