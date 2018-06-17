/********************************************************
 * Description : http request
 * Data        : 2018-04-08 18:51:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#ifndef BOOST_WEB_HTTP_REQUEST_HPP
#define BOOST_WEB_HTTP_REQUEST_HPP


#include "http_message.hpp"

namespace BoostWeb { // namespace BoostWeb begin

template <class Body, class Fields>
class HttpRequest : public HttpMessage<true, Body, Fields, HttpRequestBase>
{
public:
    HttpRequest(boost::beast::http::request<Body, Fields> & req);

public:
    virtual void set_method(http_method_t::value_t) override;
    virtual http_method_t::value_t get_method() const override;
    virtual std::string get_method_string() const override;
    virtual void set_target(const std::string &) override;
    virtual std::string get_target() const override;

private:
    using HttpMessage<true, Body, Fields, HttpRequestBase>::m_message;
};

template <class Body, class Fields>
HttpRequest<Body, Fields>::HttpRequest(boost::beast::http::request<Body, Fields> & req)
    : HttpMessage<true, Body, Fields, HttpRequestBase>(req)
{
    static_assert(static_cast<std::size_t>(boost::beast::http::verb::unknown) == static_cast<std::size_t>(http_method_t::http_unsupported), "http method invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::verb::delete_) == static_cast<std::size_t>(http_method_t::http_delete), "http method invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::verb::get) == static_cast<std::size_t>(http_method_t::http_get), "http method invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::verb::head) == static_cast<std::size_t>(http_method_t::http_head), "http method invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::verb::post) == static_cast<std::size_t>(http_method_t::http_post), "http method invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::verb::connect) == static_cast<std::size_t>(http_method_t::http_connect), "http method invalid");
}

template <class Body, class Fields>
void HttpRequest<Body, Fields>::set_method(http_method_t::value_t http_method)
{
    m_message.method(static_cast<boost::beast::http::verb>(http_method));
}

template <class Body, class Fields>
http_method_t::value_t HttpRequest<Body, Fields>::get_method() const
{
    return (static_cast<http_method_t::value_t>(m_message.method()));
}

template <class Body, class Fields>
std::string HttpRequest<Body, Fields>::get_method_string() const
{
    return (m_message.method_string().to_string());
}

template <class Body, class Fields>
void HttpRequest<Body, Fields>::set_target(const std::string & http_target)
{
    m_message.target(http_target);
}

template <class Body, class Fields>
std::string HttpRequest<Body, Fields>::get_target() const
{
    return (m_message.target().to_string());
}

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_REQUEST_HPP
