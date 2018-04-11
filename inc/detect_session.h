/********************************************************
 * Description : detect session
 * Data        : 2018-04-04 08:40:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#ifndef BOOST_WEB_DETECT_SESSION_H
#define BOOST_WEB_DETECT_SESSION_H


#include <string>
#include <chrono>
#include <memory>
#include <boost/logic/tribool.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/bind_executor.hpp>
#include "common/detect_ssl.hpp"
#include "boost_web.h"

namespace BoostWeb { // namespace BoostWeb begin

class DetectSession : public std::enable_shared_from_this<DetectSession>
{
public:
    explicit DetectSession(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context & ctx, const std::string & doc_root, std::size_t timeout, unsigned char protocol, WebServiceBase * service);

public:
    void run();

private:
    void on_detect(boost::system::error_code ec, boost::tribool result);

private:
    boost::asio::ip::tcp::socket                                                m_socket;
    boost::asio::ssl::context                                                 & m_ctx;
    boost::asio::strand<boost::asio::io_context::executor_type>                 m_strand;
    const std::string                                                         & m_doc_root;
    boost::beast::flat_buffer                                                   m_buffer;
    std::chrono::seconds                                                        m_timeout;
    unsigned char                                                               m_protocol;
    WebServiceBase                                                            * m_service;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_DETECT_SESSION_H
