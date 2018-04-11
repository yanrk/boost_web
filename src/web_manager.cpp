/********************************************************
 * Description : web manager class
 * Data        : 2018-04-05 21:17:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include <boost/functional/factory.hpp>
#include <boost/checked_delete.hpp>
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

} // namespace BoostWeb end
