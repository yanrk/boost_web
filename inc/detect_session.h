/********************************************************
 * Description : detect session
 * Data        : 2018-04-04 08:40:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_DETECT_SESSION_H
#define BOOST_WEB_DETECT_SESSION_H


#include <string>
#include <chrono>
#include <memory>
#include <boost/logic/tribool.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include "address.h"
#include "boost_web.h"

namespace BoostWeb { // namespace BoostWeb begin

class DetectSession : public std::enable_shared_from_this<DetectSession>
{
public:
    explicit DetectSession(boost::asio::ip::tcp::socket && socket, boost::asio::ssl::context & ctx, const std::shared_ptr<const std::string> & doc_root, std::size_t timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service);

public:
    void run();

private:
    void on_detect(boost::beast::error_code ec, bool result);

private:
    boost::beast::tcp_stream                                                    m_stream;
    boost::asio::ssl::context                                                 & m_ctx;
    std::shared_ptr<const std::string>                                          m_doc_root;
    boost::beast::flat_buffer                                                   m_buffer;
    Address                                                                     m_address;
    std::chrono::seconds                                                        m_timeout;
    uint64_t                                                                    m_body_limit;
    unsigned char                                                               m_protocol;
    WebServiceBase                                                            * m_service;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_DETECT_SESSION_H
