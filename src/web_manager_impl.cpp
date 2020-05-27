/********************************************************
 * Description : web manager implement
 * Data        : 2018-04-05 18:35:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#include <cstring>
#include <memory>
#include <type_traits>
#include <boost/assert.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/functional/factory.hpp>
#include "listener.h"
#include "web_manager_impl.h"
#include "websocket_connector_plain.h"
#include "websocket_connector_ssl.h"

namespace BoostWeb { // namespace BoostWeb begin

WebManagerImpl::WebManagerImpl()
    : m_io_contexts()
    , m_signal_sets()
    , m_alive_works()
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

    m_web_service = web_service;

    thread_count = std::max<std::size_t>(service_count, thread_count);

    int concurrency_hint = static_cast<int>(thread_count);
    m_io_contexts.push_back(boost::factory<boost::asio::io_context *>()(concurrency_hint));

    int signal_interrupt = SIGINT;
    int signal_terminate = SIGTERM;
    m_signal_sets.push_back(boost::factory<boost::asio::signal_set *>()(m_io_contexts.back(), signal_interrupt, signal_terminate));
    m_signal_sets.back().async_wait([this](const boost::beast::error_code &, int) { this->m_io_contexts.back().stop(); });

    m_alive_works.push_back(boost::factory<boost::asio::io_context::work *>()(m_io_contexts.back()));

    if (nullptr == service_array || 0 == service_count)
    {
        if (nullptr != crt_file_or_buffer)
        {
            m_ssl_context.set_options(boost::asio::ssl::context::sslv23_client);
            try
            {
                if (pass_file_not_buffer)
                {
                    m_ssl_context.load_verify_file(crt_file_or_buffer);
                }
                else
                {
                    m_ssl_context.add_certificate_authority(boost::asio::buffer(crt_file_or_buffer, strlen(crt_file_or_buffer)));
                }
            }
            catch (boost::beast::error_code & ec)
            {
                m_web_service->on_error("ssl", "file", ec.value(), ec.message().c_str());
                return (false);
            }
            catch (std::exception & ex)
            {
                m_web_service->on_error("ssl", "file", 1, ex.what());
                return (false);
            }
        }
    }
    else
    {
        unsigned char unsupported_protocols = support_protocol_t::protocol_ssl;
        if (nullptr != crt_file_or_buffer && nullptr != key_file_or_buffer)
        {
            m_ssl_context.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3 | boost::asio::ssl::context::no_tlsv1);
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
            catch (boost::beast::error_code & ec)
            {
                m_web_service->on_error("ssl", "file", ec.value(), ec.message().c_str());
                return (false);
            }
            catch (std::exception & ex)
            {
                m_web_service->on_error("ssl", "file", 1, ex.what());
                return (false);
            }
            unsupported_protocols = 0x0;
        }

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
            std::make_shared<Listener>(m_io_contexts.back(), m_ssl_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(service_node.host), service_node.port), std::make_shared<std::string>(service_node.root), service_node.timeout, service_node.body_limit, protocol, m_web_service)->run();
        }
    }

    auto thread_func = [this] { this->m_io_contexts.back().run(); };
    for (std::size_t index = 0; index < thread_count; ++index)
    {
        m_thread_list.push_back(boost::factory<std::thread *>()(thread_func));
    }

    return (true);
}

void WebManagerImpl::exit()
{
    m_alive_works.clear();
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

bool WebManagerImpl::create_ws_client(const char * host, const char * port, const char * target, const void * identity, std::size_t timeout)
{
    if (nullptr == host || nullptr == port || nullptr == target || '/' != *target)
    {
        return (false);
    }
    std::make_shared<WebsocketConnector>(m_io_contexts.back(), m_web_service, identity, timeout, host, port, target)->run();
    return (true);
}

bool WebManagerImpl::create_wss_client(const char * host, const char * port, const char * target, const void * identity, std::size_t timeout)
{
    if (nullptr == host || nullptr == port || nullptr == target || '/' != *target)
    {
        return (false);
    }
    std::make_shared<WebsocketsConnector>(m_io_contexts.back(), m_ssl_context, m_web_service, identity, timeout, host, port, target)->run();
    return (true);
}

} // namespace BoostWeb end
