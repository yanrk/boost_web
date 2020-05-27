/********************************************************
 * Description : web manager class
 * Data        : 2018-04-05 21:17:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include <boost/functional/factory.hpp>
#include <boost/checked_delete.hpp>
#include <boost/lexical_cast.hpp>
#include "boost_web.h"
#include "web_manager_impl.h"

namespace BoostWeb { // namespace BoostWeb begin

WebManager::WebManager()
    : m_manager_impl(nullptr)
{

}

WebManager::~WebManager()
{
    exit();
}

bool WebManager::init(WebServiceBase * web_service, const ServiceNode * service_array, std::size_t service_count, bool pass_file_not_buffer, const char * crt_file_or_buffer, const char * key_file_or_buffer, std::size_t thread_count)
{
    if (nullptr == web_service)
    {
        return (false);
    }

    if (nullptr != m_manager_impl)
    {
        return (false);
    }

    m_manager_impl = boost::factory<WebManagerImpl *>()();
    if (nullptr == m_manager_impl)
    {
        return (false);
    }

    if (m_manager_impl->init(web_service, service_array, service_count, pass_file_not_buffer, crt_file_or_buffer, key_file_or_buffer, thread_count))
    {
        return (true);
    }

    boost::checked_delete(m_manager_impl);
    m_manager_impl = nullptr;

    return (false);
}

void WebManager::exit()
{
    if (nullptr != m_manager_impl)
    {
        m_manager_impl->exit();
        boost::checked_delete(m_manager_impl);
        m_manager_impl = nullptr;
    }
}

bool WebManager::create_ws_client(const char * host, const char * port, const char * target, const void * identity, std::size_t timeout)
{
    return (nullptr != m_manager_impl && m_manager_impl->create_ws_client(host, port, target, identity, timeout));
}

bool WebManager::create_ws_client(const char * host, unsigned short port, const char * target, const void * identity, std::size_t timeout)
{
    std::string str_port = boost::lexical_cast<std::string>(port);
    return (nullptr != m_manager_impl && m_manager_impl->create_ws_client(host, str_port.c_str(), target, identity, timeout));
}

bool WebManager::create_wss_client(const char * host, const char * port, const char * target, const void * identity, std::size_t timeout)
{
    return (nullptr != m_manager_impl && m_manager_impl->create_wss_client(host, port, target, identity, timeout));
}

bool WebManager::create_wss_client(const char * host, unsigned short port, const char * target, const void * identity, std::size_t timeout)
{
    std::string str_port = boost::lexical_cast<std::string>(port);
    return (nullptr != m_manager_impl && m_manager_impl->create_wss_client(host, str_port.c_str(), target, identity, timeout));
}

} // namespace BoostWeb end
