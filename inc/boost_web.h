/********************************************************
 * Description : boost web
 * Data        : 2018-04-05 18:32:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_H
#define BOOST_WEB_H


#include <cstddef>
#include <string>
#include <memory>

#ifdef _MSC_VER
    #define BOOST_WEB_CDECL             __cdecl
    #define BOOST_WEB_STDCALL           __stdcall
    #ifdef EXPORT_BOOST_WEB_DLL
        #define BOOST_WEB_API           __declspec(dllexport)
    #else
        #ifdef USE_BOOST_WEB_DLL
            #define BOOST_WEB_API       __declspec(dllimport)
        #else
            #define BOOST_WEB_API
        #endif // USE_BOOST_WEB_DLL
    #endif // EXPORT_BOOST_WEB_DLL
#else
    #define BOOST_WEB_CDECL
    #define BOOST_WEB_STDCALL
    #define BOOST_WEB_API
#endif // _MSC_VER

namespace BoostWeb { // namespace BoostWeb begin

struct http_version_t
{
    enum value_t
    {
        http_v10 = 10, 
        http_v11 = 11  
    };
};

struct http_method_t
{
    enum value_t
    {
        http_unsupported = 0, 
        http_delete, 
        http_get, 
        http_head, 
        http_post, 
        http_put, 
        http_connect 
    };
};

struct http_result_t
{
    enum value_t
    {
        http_continue = 100, 
        http_processing = 102, 
        http_ok = 200, 
        http_created = 201, 
        http_accepted = 202, 
        http_non_authoritative_information = 203, 
        http_no_content = 204, 
        http_reset_content = 205, 
        http_partial_content = 206, 
        http_multiple_choices = 300, 
        http_moved_permanently = 301, 
        http_found = 302, 
        http_not_modified = 304, 
        http_use_proxy = 305, 
        http_bad_request = 400, 
        http_unauthorized = 401, 
        http_payment_required = 402, 
        http_forbidden = 403, 
        http_not_found = 404, 
        http_method_not_allowed = 405, 
        http_not_acceptable = 406, 
        http_request_timeout = 408, 
        http_conflict = 409, 
        http_gone = 410, 
        http_length_required = 411, 
        http_precondition_failed = 412, 
        http_payload_too_large = 413, 
        http_uri_too_long = 414, 
        http_internal_server_error = 500, 
        http_not_implemented = 501, 
        http_service_unavailable = 503, 
        http_version_not_supported = 505 
    };
};

class BOOST_WEB_API HttpMessageBase
{
public:
    HttpMessageBase() = default;
    virtual ~HttpMessageBase() = default;

public:
    virtual void set_version(http_version_t::value_t) = 0;
    virtual http_version_t::value_t get_version() const = 0;
    virtual void set_field_server(const std::string &) = 0;
    virtual std::string get_field_server() const = 0;
    virtual void set_field_accept(const std::string &) = 0;
    virtual std::string get_field_accept() const = 0;
    virtual void set_field_cookie(const std::string &) = 0;
    virtual std::string get_field_cookie() const = 0;
    virtual void set_field_user_agent(const std::string &) = 0;
    virtual std::string get_field_user_agent() const = 0;
    virtual void set_field_content_type(const std::string &) = 0;
    virtual std::string get_field_content_type() const = 0;
    virtual void set_field_content_md5(const std::string &) = 0;
    virtual std::string get_field_content_md5() const = 0;
    virtual void set_body(const std::string &) = 0;
    virtual const std::string & get_body() const = 0;
    virtual void set_body(std::string &&) = 0;
    virtual std::string && get_body() = 0;
};

class BOOST_WEB_API HttpRequestBase : public HttpMessageBase
{
public:
    HttpRequestBase() = default;
    virtual ~HttpRequestBase() = default;

public:
    virtual void set_method(http_method_t::value_t) = 0;
    virtual http_method_t::value_t get_method() const = 0;
    virtual std::string get_method_string() const = 0;
    virtual void set_target(const std::string &) = 0;
    virtual std::string get_target() const = 0;
};

class BOOST_WEB_API HttpResponseBase : public HttpMessageBase
{
public:
    HttpResponseBase() = default;
    virtual ~HttpResponseBase() = default;

public:
    virtual void set_result(http_result_t::value_t) = 0;
    virtual http_result_t::value_t get_result() const = 0;
    virtual std::size_t get_result_integer() const = 0;
    virtual void set_reason(const std::string &) = 0;
    virtual std::string get_reason() const = 0;
};

class BOOST_WEB_API WebsocketConnectionBase
{
public:
    WebsocketConnectionBase() = default;
    virtual ~WebsocketConnectionBase() = default;

public:
    virtual void set_user_data(void * user_data) = 0;
    virtual void * get_user_data() const = 0;

public:
    virtual void get_host_address(std::string & ip, unsigned short & port) const = 0;
    virtual void get_peer_address(std::string & ip, unsigned short & port) const = 0;

public:
    virtual bool recv_buffer_has_data() = 0;
    virtual bool recv_buffer_data_is_text() = 0;
    virtual const void * recv_buffer_data() = 0;
    virtual std::size_t recv_buffer_size() = 0;
    virtual bool recv_buffer_drop() = 0;
    virtual bool send_buffer_fill(bool text, const void * data, std::size_t len) = 0;

public:
    virtual void close() = 0;
};

typedef std::shared_ptr<WebsocketConnectionBase> WebsocketConnectionSharedPtr;
typedef std::weak_ptr<WebsocketConnectionBase> WebsocketConnectionWeakPtr;

class BOOST_WEB_API HttpConnectionBase
{
public:
    HttpConnectionBase() = default;
    virtual ~HttpConnectionBase() = default;

public:
    virtual void get_host_address(std::string & ip, unsigned short & port) const = 0;
    virtual void get_peer_address(std::string & ip, unsigned short & port) const = 0;
};

class BOOST_WEB_API WebServiceBase
{
public:
    WebServiceBase() = default;
    virtual ~WebServiceBase() = default;

public: /* http(s) */
    virtual bool target_is_path(const std::string & target) = 0;
    virtual bool handle_request(const HttpConnectionBase & connection, const HttpRequestBase & request, HttpResponseBase & response) = 0;
    virtual void on_error(const char * protocol, const char * what, int error, const char * message) = 0;

public: /* websocket(s) */
    virtual bool on_connect(WebsocketConnectionSharedPtr connection, std::size_t identity) = 0;
    virtual bool on_accept(WebsocketConnectionSharedPtr connection, unsigned short listener_port) = 0;
    virtual bool on_recv(WebsocketConnectionSharedPtr connection) = 0;
    virtual bool on_send(WebsocketConnectionSharedPtr connection) = 0;
    virtual void on_error(WebsocketConnectionSharedPtr connection, const char * protocol, const char * what, int error, const char * message) = 0;
    virtual void on_close(WebsocketConnectionSharedPtr connection) = 0;
};

struct support_protocol_t
{
    enum value_t
    {
        protocol_http   = 0x01, 
        protocol_https  = 0x02, 
        protocol_httpss = protocol_http | protocol_https, 
        protocol_ws     = 0x04, 
        protocol_wss    = 0x08, 
        protocol_wsss   = protocol_ws | protocol_wss, 
        protocol_plain  = protocol_http | protocol_ws, 
        protocol_ssl    = protocol_https | protocol_wss, 
        protocol_all    = protocol_httpss | protocol_wsss
    };
};

struct BOOST_WEB_API ServiceNode
{
    const char     * host;
    unsigned short   port;
    const char     * root;
    std::size_t      timeout;
    uint64_t         body_limit;
    unsigned char    protocol;
};

class WebManagerImpl;

class BOOST_WEB_API WebManager
{
public:
    WebManager();
    ~WebManager();

public:
    WebManager(const WebManager &) = delete;
    WebManager & operator = (const WebManager &) = delete;

public:
    bool init(WebServiceBase * web_service, const ServiceNode * service_array, std::size_t service_count = 1, bool pass_file_not_buffer = true, const char * crt_file_or_buffer = nullptr, const char * key_file_or_buffer = nullptr, std::size_t thread_count = 5);
    void exit();

public:
    bool create_ws_client(const char * host, const char * port, const char * target, std::size_t identity, std::size_t timeout = 30);
    bool create_ws_client(const char * host, unsigned short port, const char * target, std::size_t identity, std::size_t timeout = 30);
    bool create_wss_client(const char * host, const char * port, const char * target, std::size_t identity, std::size_t timeout = 30);
    bool create_wss_client(const char * host, unsigned short port, const char * target, std::size_t identity, std::size_t timeout = 30);

private:
    WebManagerImpl                                * m_manager_impl;
};

} // namespace BoostWeb end


#endif // BOOST_WEB_H
