#include <cstdio>
#include <cassert>
#include <cstring>
#include "test_service.h"

static const char msg_blk[] = "this is a message\n";
static const std::size_t msg_len = sizeof(msg_blk) / sizeof(msg_blk[0]) - 1;

TestService::TestService(std::size_t send_times, std::size_t connection_count)
    : m_max_msg_cnt(send_times)
    , m_max_connection_cnt(connection_count)
    , m_connect_count(0)
    , m_disconnect_count(0)
    , m_send_finish_count(0)
    , m_web_manager()
{

}

TestService::~TestService()
{

}

bool TestService::target_is_path(const std::string & target)
{
    return ("/sys/" != target.substr(0, 5));
}

bool TestService::handle_request(const BoostWeb::HttpRequestBase & request, BoostWeb::HttpResponseBase & response)
{
    response.set_result(BoostWeb::http_result_t::http_ok);
    printf("method: %s\n", request.get_method_string().c_str());
    printf("target: %s\n", request.get_target().c_str());
    printf("cookie: %s\n", request.get_field_cookie().c_str());
    printf("body: {\n%s\n}\n", request.get_body().c_str());
    response.set_body("recv request (" + request.get_method_string() + ": " + request.get_target() + ", {" + request.get_body() + "})");
    return (true);
}

void TestService::on_error(const char * protocol, const char * what, int error, const char * message)
{
    show_error(protocol, what, error, message);
}

bool TestService::on_connect(BoostWeb::WebsocketConnectionSharedPtr connection, std::size_t identity)
{
    return (false);
}

bool TestService::on_accept(BoostWeb::WebsocketConnectionSharedPtr connection, unsigned short listener_port)
{
    return (insert_connection(connection));
}

bool TestService::on_recv(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    return (recv_message(connection));
}

bool TestService::on_send(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    return (true);
}

void TestService::on_error(BoostWeb::WebsocketConnectionSharedPtr connection, const char * protocol, const char * what, int error, const char * message)
{
    show_error(protocol, what, error, message);
}

void TestService::on_close(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    remove_connection(connection);
}

void TestService::show_error(const char * protocol, const char * what, int error, const char * message)
{
    printf("%s::%s error (%u) {%s}\n", protocol, what, error, message);
}

bool TestService::insert_connection(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    std::string host_ip;
    unsigned short host_port = 0;
    connection->get_host_address(host_ip, host_port);
    std::string peer_ip;
    unsigned short peer_port = 0;
    connection->get_peer_address(peer_ip, peer_port);
    printf("connect: %u, [%s:%u] -> [%s:%u]\n", static_cast<uint32_t>(++m_connect_count), host_ip.c_str(), host_port, peer_ip.c_str(), peer_port);
    connection->set_user_data(reinterpret_cast<void *>(0));
    return (true);
}

bool TestService::remove_connection(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    std::string host_ip;
    unsigned short host_port = 0;
    connection->get_host_address(host_ip, host_port);
    std::string peer_ip;
    unsigned short peer_port = 0;
    connection->get_peer_address(peer_ip, peer_port);
    printf("disconnect: %u, [%s:%u] -> [%s:%u]\n", static_cast<uint32_t>(++m_disconnect_count), host_ip.c_str(), host_port, peer_ip.c_str(), peer_port);
    std::size_t count = reinterpret_cast<std::size_t>(connection->get_user_data());
    return (true);
}

bool TestService::send_message(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    if (0 == m_max_msg_cnt)
    {
        return (true);
    }

    std::size_t count = reinterpret_cast<std::size_t>(connection->get_user_data());
    if (count >= m_max_msg_cnt)
    {
        std::string host_ip;
        unsigned short host_port = 0;
        connection->get_host_address(host_ip, host_port);
        std::string peer_ip;
        unsigned short peer_port = 0;
        connection->get_peer_address(peer_ip, peer_port);
        printf("send finish: %u, [%s:%u] -> [%s:%u]\n", static_cast<uint32_t>(++m_send_finish_count), host_ip.c_str(), host_port, peer_ip.c_str(), peer_port);
        return (false);
    }

    ++count;

    std::string data(msg_len * count, '\0');
    for (std::size_t index = 0; index < count; ++index)
    {
        memcpy(&data[msg_len * index], msg_blk, msg_len);
    }

    if (!connection->send_buffer_fill_len(true, data.c_str(), data.size()))
    {
        assert(false);
        return (false);
    }

    connection->set_user_data(reinterpret_cast<void *>(count));

    return (true);
}

bool TestService::recv_message(BoostWeb::WebsocketConnectionSharedPtr connection)
{
    assert(0 != m_max_msg_cnt);

    if (!connection->recv_buffer_has_data())
    {
        assert(false);
        return (false);
    }

    if (!connection->recv_buffer_data_is_text())
    {
        assert(false);
        return (false);
    }

    const char * data = reinterpret_cast<const char *>(connection->recv_buffer_data());
    std::size_t len = connection->recv_buffer_size();

    /*
    if (!check_message(connection, data, len))
    {
        assert(false);
        return (false);
    }
    */

    if (!connection->recv_buffer_drop_len(len))
    {
        assert(false);
        return (false);
    }

    if (!send_message(connection))
    {
        return (false);
    }

    return (true);
}

bool TestService::check_message(BoostWeb::WebsocketConnectionSharedPtr connection, const char * data, std::size_t len)
{
    std::size_t count = reinterpret_cast<std::size_t>(connection->get_user_data());
    if (count >= m_max_msg_cnt)
    {
        assert(false);
        return (false);
    }

    if (msg_len * count != len)
    {
        assert(false);
        return (false);
    }

    for (std::size_t index = 0; index < count; ++index)
    {
        if (0 != memcmp(data, msg_blk, msg_len))
        {
            assert(false);
        }
        data += msg_len;
        len -= msg_len;
    }

    return (true);
}

bool TestService::init()
{
    BoostWeb::ServiceNode server_node;
    server_node.host = "0.0.0.0";
    server_node.port = 12345;
    server_node.root = "g:/codes_vs2015/munu/munu/www";
    server_node.timeout = 15;
    server_node.protocol = BoostWeb::support_protocol_t::protocol_all;
    const char * crt_file = "f:/codes/codes/transmission/web_transmit_adapter/res/local.foxrenderfarm.com.crt";
    const char * key_file = "f:/codes/codes/transmission/web_transmit_adapter/res/local.foxrenderfarm.com.key";
    if (!m_web_manager.init(this, &server_node, 1, true, crt_file, key_file, 5))
    {
        return (false);
    }
    return (true);
}

void TestService::exit()
{
    m_web_manager.exit();
}
