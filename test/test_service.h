#ifndef TEST_SERVICE_H
#define TEST_SERVICE_H


#include <string>
#include <atomic>
#include "boost_web.h"

class TestService : public BoostWeb::WebServiceBase
{
public:
    TestService(bool server, bool text, bool ssl, std::size_t send_times, std::size_t connection_count);
    virtual ~TestService();

public:
    bool init();
    void exit();

public: /* http(s) */
    virtual bool target_is_path(const std::string & target) override;
    virtual bool handle_request(const BoostWeb::HttpConnectionBase & connection, const BoostWeb::HttpRequestBase & request, BoostWeb::HttpResponseBase & response) override;
    virtual void on_error(const char * protocol, const char * what, int error, const char * message) override;

public: /* websocket(s) */
    virtual bool on_connect(BoostWeb::WebsocketConnectionSharedPtr connection, std::size_t identity) override;
    virtual bool on_accept(BoostWeb::WebsocketConnectionSharedPtr connection, unsigned short listener_port) override;
    virtual bool on_recv(BoostWeb::WebsocketConnectionSharedPtr connection) override;
    virtual bool on_send(BoostWeb::WebsocketConnectionSharedPtr connection) override;
    virtual void on_error(BoostWeb::WebsocketConnectionSharedPtr connection, const char * protocol, const char * what, int error, const char * message) override;
    virtual void on_close(BoostWeb::WebsocketConnectionSharedPtr connection) override;

private:
    bool insert_connection(BoostWeb::WebsocketConnectionSharedPtr connection);
    bool remove_connection(BoostWeb::WebsocketConnectionSharedPtr connection);

private:
    bool check_message(BoostWeb::WebsocketConnectionSharedPtr connection, const char * data, std::size_t len);

private:
    bool send_message(BoostWeb::WebsocketConnectionSharedPtr connection);
    bool recv_message(BoostWeb::WebsocketConnectionSharedPtr connection);

private:
    void show_error(const char * protocol, const char * what, int error, const char * message);

private:
    void parse_file_body(const std::string & boundary, const std::string & body);

private:
    bool                                                        m_server;
    bool                                                        m_text;
    bool                                                        m_ssl;
    std::size_t                                                 m_max_msg_cnt;
    std::size_t                                                 m_max_connection_cnt;
    std::atomic_long                                            m_connect_count;
    std::atomic_long                                            m_disconnect_count;
    std::atomic_long                                            m_send_finish_count;
    BoostWeb::WebManager                                        m_web_manager;
};


#endif // TEST_SERVICE_H
