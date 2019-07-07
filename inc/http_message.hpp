/********************************************************
 * Description : http message
 * Data        : 2018-04-08 09:51:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_HTTP_MESSAGE_HPP
#define BOOST_WEB_HTTP_MESSAGE_HPP


#include <string>
#include <boost/beast/http.hpp>
#include "boost_web.h"

namespace BoostWeb { // namespace BoostWeb begin

template <bool IsRequest, class Body, class Fields, class MessageBase>
class HttpMessage : public MessageBase
{
public:
    HttpMessage(boost::beast::http::message<IsRequest, Body, Fields> & message);

public:
    virtual void set_version(http_version_t::value_t) override;
    virtual http_version_t::value_t get_version() const override;
    virtual void set_field_server(const std::string &) override;
    virtual std::string get_field_server() const override;
    virtual void set_field_accept(const std::string &) override;
    virtual std::string get_field_accept() const override;
    virtual void set_field_cookie(const std::string &) override;
    virtual std::string get_field_cookie() const override;
    virtual void set_field_user_agent(const std::string &) override;
    virtual std::string get_field_user_agent() const override;
    virtual void set_field_content_type(const std::string &) override;
    virtual std::string get_field_content_type() const override;
    virtual void set_field_content_md5(const std::string &) override;
    virtual std::string get_field_content_md5() const override;
    virtual void set_body(const std::string &) override;
    virtual const std::string & get_body() const override;
    virtual void set_body(std::string &&) override;
    virtual std::string && get_body() override;

protected:
    boost::beast::http::message<IsRequest, Body, Fields>                      & m_message;
};

template <bool IsRequest, class Body, class Fields, class MessageBase>
HttpMessage<IsRequest, Body, Fields, MessageBase>::HttpMessage(boost::beast::http::message<IsRequest, Body, Fields> & message)
    : m_message(message)
{

}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_version(http_version_t::value_t http_version)
{
    m_message.version(http_version);
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
http_version_t::value_t HttpMessage<IsRequest, Body, Fields, MessageBase>::get_version() const
{
    return (static_cast<http_version_t::value_t>(m_message.version()));
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_field_server(const std::string & server)
{
    m_message.set(boost::beast::http::field::server, server);
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
std::string HttpMessage<IsRequest, Body, Fields, MessageBase>::get_field_server() const
{
    return (m_message[boost::beast::http::field::server].to_string());
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_field_accept(const std::string & accept)
{
    m_message.set(boost::beast::http::field::accept, accept);
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
std::string HttpMessage<IsRequest, Body, Fields, MessageBase>::get_field_accept() const
{
    return (m_message[boost::beast::http::field::accept].to_string());
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_field_cookie(const std::string & cookie)
{
    m_message.set(boost::beast::http::field::cookie, cookie);
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
std::string HttpMessage<IsRequest, Body, Fields, MessageBase>::get_field_cookie() const
{
    return (m_message[boost::beast::http::field::cookie].to_string());
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_field_user_agent(const std::string & user_agent)
{
    m_message.set(boost::beast::http::field::user_agent, user_agent);
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
std::string HttpMessage<IsRequest, Body, Fields, MessageBase>::get_field_user_agent() const
{
    return (m_message[boost::beast::http::field::user_agent].to_string());
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_field_content_type(const std::string & content_type)
{
    m_message.set(boost::beast::http::field::content_type, content_type);
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
std::string HttpMessage<IsRequest, Body, Fields, MessageBase>::get_field_content_type() const
{
    return (m_message[boost::beast::http::field::content_type].to_string());
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_field_content_md5(const std::string & content_md5)
{
    m_message.set(boost::beast::http::field::content_md5, content_md5);
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
std::string HttpMessage<IsRequest, Body, Fields, MessageBase>::get_field_content_md5() const
{
    return (m_message[boost::beast::http::field::content_md5].to_string());
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_body(const std::string & body)
{
    m_message.body() = body;
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
const std::string & HttpMessage<IsRequest, Body, Fields, MessageBase>::get_body() const
{
    return (m_message.body());
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
void HttpMessage<IsRequest, Body, Fields, MessageBase>::set_body(std::string && body)
{
    m_message.body() = std::move(body);
}

template <bool IsRequest, class Body, class Fields, class MessageBase>
std::string && HttpMessage<IsRequest, Body, Fields, MessageBase>::get_body()
{
    return (std::move(m_message.body()));
}

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_MESSAGE_HPP
