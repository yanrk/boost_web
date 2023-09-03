/********************************************************
 * Description : listener
 * Data        : 2018-04-04 14:11:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_LISTENER_H
#define BOOST_WEB_LISTENER_H


#include <string>
#include <boost/asio/ssl.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include "boost_web.h"

namespace BoostWeb { // namespace BoostWeb begin

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener(boost::asio::io_context & ioc, boost::asio::ssl::context & ctx, boost::asio::ip::tcp::endpoint endpoint, const std::shared_ptr<const std::string> & doc_root, std::size_t timeout, uint64_t body_limit, unsigned char protocol, WebServiceBase * service);

public:
    bool run();
    void accept();

private:
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);

private:
    boost::asio::io_context                                                   & m_ioc;
    boost::asio::ssl::context                                                 & m_ctx;
    boost::asio::ip::tcp::acceptor                                              m_acceptor;
    std::shared_ptr<const std::string>                                          m_doc_root;
    std::size_t                                                                 m_timeout;
    uint64_t                                                                    m_body_limit;
    unsigned char                                                               m_protocol;
    WebServiceBase                                                            * m_service;
    bool                                                                        m_good;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_LISTENER_H

