#include <cstdio>
#include <cassert>
#include <cstring>
#include <string>
#include <algorithm>
#include "echo_service.h"

static const char hello_data[] = "welcome to access ws://a.b.c.d:12345/";
static const std::size_t hello_size = sizeof(hello_data) - 1;

EchoService::EchoService()
    : m_connect_count(0)
    , m_disconnect_count(0)
    , m_web_manager()
{

}

EchoService::~EchoService()
{

}

bool EchoService::target_is_path(const std::string & target)
{
    assert(false);
    return (false);
}

bool EchoService::handle_request(const BoostWeb::HttpConnectionBase & connection, const BoostWeb::HttpRequestBase & request, BoostWeb::HttpResponseBase & response)
{
    assert(false);
    return (false);
}

void EchoService::on_error(const char * protocol, const char * what, int error, const char * message)
{
    show_error(protocol, what, error, message);
}

bool EchoService::on_connect(BoostWeb::WebsocketConnectionSharedPtr connection, const void * identity)
{
    assert(false);
    return (false);
}

bool EchoService::on_accept(BoostWeb::WebsocketConnectionSharedPtr connection, unsigned short listener_port)
{
    return (!!connection && insert_connection(connection) && send_message(connection, true, hello_data, hello_size));
}

bool EchoService::on_recv(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    return (recv_message(connection));
}

bool EchoService::on_send(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    return (true);
}

void EchoService::on_error(BoostWeb::WebsocketConnectionSharedPtr connection, const char * protocol, const char * what, int error, const char * message)
{
    show_error(protocol, what, error, message);
}

void EchoService::on_close(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    remove_connection(connection);
}

void EchoService::show_error(const char * protocol, const char * what, int error, const char * message)
{
    printf("%s::%s error (%d) {%s}\n", protocol, what, error, message);
}

bool EchoService::insert_connection(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    std::string host_ip;
    unsigned short host_port = 0;
    connection->get_host_address(host_ip, host_port);
    std::string peer_ip;
    unsigned short peer_port = 0;
    connection->get_peer_address(peer_ip, peer_port);
    printf("websocket connect: %u, [%s:%d] -> [%s:%d]\n", static_cast<uint32_t>(++m_connect_count), host_ip.c_str(), host_port, peer_ip.c_str(), peer_port);
    return (true);
}

bool EchoService::remove_connection(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    std::string host_ip;
    unsigned short host_port = 0;
    connection->get_host_address(host_ip, host_port);
    std::string peer_ip;
    unsigned short peer_port = 0;
    connection->get_peer_address(peer_ip, peer_port);
    printf("websocket disconnect: %u, [%s:%d] -> [%s:%d]\n", static_cast<uint32_t>(++m_disconnect_count), host_ip.c_str(), host_port, peer_ip.c_str(), peer_port);
    return (true);
}

bool EchoService::send_message(BoostWeb::WebsocketConnectionSharedPtr connection, bool text, const void * data, std::size_t size)
{
    if (!connection->send_buffer_fill(text, data, size))
    {
        assert(false);
        return (false);
    }

    if (text)
    {
        printf("send text data, content {%s}\n", reinterpret_cast<const char *>(data));
    }
    else
    {
        printf("send binary data, size (%zu)\n", size);
    }

    return (true);
}

bool EchoService::recv_message(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    if (!connection->recv_buffer_has_data())
    {
        assert(false);
        return (false);
    }

    bool text = connection->recv_buffer_data_is_text();
    std::string data(reinterpret_cast<const char *>(connection->recv_buffer_data()), connection->recv_buffer_size());

    if (!connection->recv_buffer_drop())
    {
        assert(false);
        return (false);
    }

    if (text)
    {
        printf("recv text data, content {%s}\n", data.c_str());
    }
    else
    {
        printf("recv binary data, size (%zu)\n", data.size());
    }

    if (!send_message(connection, text, data.c_str(), data.size()))
    {
        return (false);
    }

    return (true);
}

bool EchoService::init()
{
    BoostWeb::ServiceNode service_node;
    service_node.host = "0.0.0.0";
    service_node.port = 12345;
    service_node.root = "/";
    service_node.timeout = 15;
    service_node.body_limit = 0;
    service_node.protocol = BoostWeb::support_protocol_t::protocol_wsss;
    if (!m_web_manager.init(this, &service_node, 1, false, true, nullptr, nullptr, 1))
    {
        return (false);
    }
    return (true);
}

void EchoService::exit()
{
    m_web_manager.exit();
}
