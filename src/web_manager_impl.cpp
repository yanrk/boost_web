/********************************************************
 * Description : web manager implement
 * Data        : 2018-04-05 18:35:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#include <cstring>
#include <memory>
#include <type_traits>
#include <boost/assert.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/functional/factory.hpp>
#include "listener.h"
#include "web_manager_impl.h"

namespace BoostWeb { // namespace BoostWeb begin

WebManagerImpl::WebManagerImpl()
    : m_io_contexts()
    , m_signal_sets()
    , m_ssl_context(boost::asio::ssl::context::sslv23)
    , m_thread_list()
    , m_web_service(nullptr)
{

}

WebManagerImpl::~WebManagerImpl()
{

}

bool WebManagerImpl::init(WebServiceBase * web_service, const ServiceNode * service_array, std::size_t service_count, bool pass_file_not_buffer, const char * crt_file_or_buffer, const char * key_file_or_buffer, std::size_t thread_count)
{
    if (nullptr == web_service)
    {
        return (false);
    }

    if (nullptr == service_array || 0 == service_count)
    {
        return (false);
    }

    m_web_service = web_service;

    unsigned short unsupported_protocols = support_protocol_t::protocol_ssl;
    if (nullptr != crt_file_or_buffer && nullptr != key_file_or_buffer)
    {
        m_ssl_context.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2);
        try
        {
            if (pass_file_not_buffer)
            {
                m_ssl_context.use_certificate_chain_file(crt_file_or_buffer);
                m_ssl_context.use_private_key_file(key_file_or_buffer, boost::asio::ssl::context::file_format::pem);
            }
            else
            {
                m_ssl_context.use_certificate_chain(boost::asio::buffer(crt_file_or_buffer, strlen(crt_file_or_buffer)));
                m_ssl_context.use_private_key(boost::asio::buffer(key_file_or_buffer, strlen(key_file_or_buffer)), boost::asio::ssl::context::file_format::pem);
            }
        }
        catch (boost::system::error_code & ec)
        {
            m_web_service->on_error("ssl", "file", ec.value(), ec.message().c_str());
            return (false);
        }
        catch (std::exception & ex)
        {
            m_web_service->on_error("ssl", "file", 1, ex.what());
            return (false);
        }
        unsupported_protocols = 0;
    }

    thread_count = std::max<std::size_t>(service_count, thread_count);

    int concurrency_hint = static_cast<int>(thread_count);
    m_io_contexts.push_back(boost::factory<boost::asio::io_context *>()(concurrency_hint));

    int signal_interrupt = SIGINT;
    int signal_terminate = SIGTERM;
    m_signal_sets.push_back(boost::factory<boost::asio::signal_set *>()(m_io_contexts.back(), signal_interrupt, signal_terminate));
    m_signal_sets.back().async_wait([this](const boost::system::error_code &, int) { this->m_io_contexts.back().stop(); });

    for (std::size_t index = 0; index < service_count; ++index)
    {
        const ServiceNode & service_node = service_array[index];
        BOOST_ASSERT(nullptr != service_node.host);
        BOOST_ASSERT(service_node.port > 0);
        BOOST_ASSERT(nullptr != service_node.root);
        unsigned char protocol = (service_node.protocol) & (~unsupported_protocols);
        if (0 == protocol)
        {
            m_web_service->on_error("init", "protocol", 1, "no support protocol");
            return (false);
        }
        std::make_shared<Listener>(m_io_contexts.back(), m_ssl_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(service_node.host), service_node.port), service_node.root, std::max<std::size_t>(15, service_node.timeout), protocol, m_web_service)->run();
    }

    for (std::size_t index = 0; index < thread_count; ++index)
    {
        m_thread_list.push_back(boost::factory<std::thread *>()([this] { this->m_io_contexts.back().run(); }));
    }

    return (true);
}

void WebManagerImpl::exit()
{
    if (!m_io_contexts.empty())
    {
        m_io_contexts.back().stop();
        for (auto & thread : m_thread_list)
        {
            thread.join();
        }
        m_thread_list.clear();
        m_signal_sets.clear();
        m_io_contexts.clear();
    }
    m_web_service = nullptr;
}

} // namespace BoostWeb end
