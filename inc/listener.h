/********************************************************
 * Description : listener
 * Data        : 2018-04-04 14:11:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#ifndef BOOST_WEB_LISTENER_H
#define BOOST_WEB_LISTENER_H


#include <string>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include "boost_web.h"

namespace BoostWeb { // namespace BoostWeb begin

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener(boost::asio::io_context & ioc, boost::asio::ssl::context & ctx, boost::asio::ip::tcp::endpoint endpoint, const std::string & doc_root, std::size_t timeout, unsigned char protocol, WebServiceBase * service);

public:
    void run();
    void accept();

private:
    void on_accept(boost::system::error_code ec);

private:
    boost::asio::ssl::context                                                 & m_ctx;
    boost::asio::ip::tcp::acceptor                                              m_acceptor;
    boost::asio::ip::tcp::socket                                                m_socket;
    const std::string                                                           m_doc_root;
    std::size_t                                                                 m_timeout;
    unsigned char                                                               m_protocol;
    WebServiceBase                                                            * m_service;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_LISTENER_H

