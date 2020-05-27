#ifndef ECHO_SERVICE_H
#define ECHO_SERVICE_H


#include <string>
#include <atomic>
#include "boost_web.h"

class EchoService : public BoostWeb::WebServiceBase
{
public:
    EchoService();
    virtual ~EchoService();

public:
    bool init();
    void exit();

public: /* http(s) */
    virtual bool target_is_path(const std::string & target) override;
    virtual bool handle_request(const BoostWeb::HttpConnectionBase & connection, const BoostWeb::HttpRequestBase & request, BoostWeb::HttpResponseBase & response) override;
    virtual void on_error(const char * protocol, const char * what, int error, const char * message) override;

public: /* websocket(s) */
    virtual bool on_connect(BoostWeb::WebsocketConnectionSharedPtr connection, const void * identity) override;
    virtual bool on_accept(BoostWeb::WebsocketConnectionSharedPtr connection, unsigned short listener_port) override;
    virtual bool on_recv(BoostWeb::WebsocketConnectionSharedPtr connection) override;
    virtual bool on_send(BoostWeb::WebsocketConnectionSharedPtr connection) override;
    virtual void on_error(BoostWeb::WebsocketConnectionSharedPtr connection, const char * protocol, const char * what, int error, const char * message) override;
    virtual void on_close(BoostWeb::WebsocketConnectionSharedPtr connection) override;

private:
    bool insert_connection(BoostWeb::WebsocketConnectionSharedPtr connection);
    bool remove_connection(BoostWeb::WebsocketConnectionSharedPtr connection);

private:
    bool send_message(BoostWeb::WebsocketConnectionSharedPtr connection, bool text, const void * data, std::size_t size);
    bool recv_message(BoostWeb::WebsocketConnectionSharedPtr connection);

private:
    void show_error(const char * protocol, const char * what, int error, const char * message);

private:
    std::atomic_long                                            m_connect_count;
    std::atomic_long                                            m_disconnect_count;
    BoostWeb::WebManager                                        m_web_manager;
};


#endif // ECHO_SERVICE_H
