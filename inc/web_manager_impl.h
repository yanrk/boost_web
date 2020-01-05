/********************************************************
 * Description : web manager implement
 * Data        : 2018-04-05 18:35:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_WEB_MANAGER_IMPLEMENT_H
#define BOOST_WEB_WEB_MANAGER_IMPLEMENT_H


#include <thread>
#include <boost/asio/ssl.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include "boost_web.h"

namespace BoostWeb { // namespace BoostWeb begin

class WebManagerImpl
{
public:
    WebManagerImpl();
    ~WebManagerImpl();

public:
    WebManagerImpl(const WebManagerImpl &) = delete;
    WebManagerImpl & operator = (const WebManagerImpl &) = delete;

public:
    bool init(WebServiceBase * web_service, const ServiceNode * service_array, std::size_t service_count, bool pass_file_not_buffer, const char * crt_file_or_buffer, const char * key_file_or_buffer, std::size_t thread_count);
    void exit();

public:
    bool create_ws_client(const char * host, const char * port, const char * target, std::size_t identity, std::size_t timeout);
    bool create_wss_client(const char * host, const char * port, const char * target, std::size_t identity, std::size_t timeout);

private:
    boost::ptr_list<boost::asio::io_context>                                    m_io_contexts;
    boost::ptr_list<boost::asio::signal_set>                                    m_signal_sets;
    boost::ptr_list<boost::asio::io_context::work>                              m_alive_works;
    boost::asio::ssl::context                                                   m_ssl_context;
    boost::ptr_list<std::thread>                                                m_thread_list;
    WebServiceBase                                                            * m_web_service;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_WEB_MANAGER_IMPLEMENT_H
