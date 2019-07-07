/********************************************************
 * Description : http response
 * Data        : 2018-04-08 19:38:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * Copyright(C): 2019 - 2020
 ********************************************************/

#ifndef BOOST_WEB_HTTP_RESPONSE_HPP
#define BOOST_WEB_HTTP_RESPONSE_HPP


#include "http_message.hpp"

namespace BoostWeb { // namespace BoostWeb begin

template <class Body, class Fields>
class HttpResponse : public HttpMessage<false, Body, Fields, HttpResponseBase>
{
public:
    HttpResponse(boost::beast::http::response<Body, Fields> & res);

public:
    virtual void set_result(http_result_t::value_t) override;
    virtual http_result_t::value_t get_result() const override;
    virtual std::size_t get_result_integer() const override;
    virtual void set_reason(const std::string &) override;
    virtual std::string get_reason() const override;

private:
    using HttpMessage<false, Body, Fields, HttpResponseBase>::m_message;
};

template <class Body, class Fields>
HttpResponse<Body, Fields>::HttpResponse(boost::beast::http::response<Body, Fields> & res)
    : HttpMessage<false, Body, Fields, HttpResponseBase>(res)
{
    static_assert(static_cast<std::size_t>(boost::beast::http::status::continue_) == static_cast<std::size_t>(http_result_t::http_continue), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::processing) == static_cast<std::size_t>(http_result_t::http_processing), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::ok) == static_cast<std::size_t>(http_result_t::http_ok), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::created) == static_cast<std::size_t>(http_result_t::http_created), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::accepted) == static_cast<std::size_t>(http_result_t::http_accepted), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::non_authoritative_information) == static_cast<std::size_t>(http_result_t::http_non_authoritative_information), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::no_content) == static_cast<std::size_t>(http_result_t::http_no_content), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::reset_content) == static_cast<std::size_t>(http_result_t::http_reset_content), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::partial_content) == static_cast<std::size_t>(http_result_t::http_partial_content), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::multiple_choices) == static_cast<std::size_t>(http_result_t::http_multiple_choices), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::moved_permanently) == static_cast<std::size_t>(http_result_t::http_moved_permanently), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::found) == static_cast<std::size_t>(http_result_t::http_found), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::not_modified) == static_cast<std::size_t>(http_result_t::http_not_modified), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::use_proxy) == static_cast<std::size_t>(http_result_t::http_use_proxy), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::bad_request) == static_cast<std::size_t>(http_result_t::http_bad_request), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::unauthorized) == static_cast<std::size_t>(http_result_t::http_unauthorized), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::payment_required) == static_cast<std::size_t>(http_result_t::http_payment_required), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::forbidden) == static_cast<std::size_t>(http_result_t::http_forbidden), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::not_found) == static_cast<std::size_t>(http_result_t::http_not_found), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::method_not_allowed) == static_cast<std::size_t>(http_result_t::http_method_not_allowed), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::not_acceptable) == static_cast<std::size_t>(http_result_t::http_not_acceptable), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::request_timeout) == static_cast<std::size_t>(http_result_t::http_request_timeout), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::conflict) == static_cast<std::size_t>(http_result_t::http_conflict), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::gone) == static_cast<std::size_t>(http_result_t::http_gone), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::length_required) == static_cast<std::size_t>(http_result_t::http_length_required), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::precondition_failed) == static_cast<std::size_t>(http_result_t::http_precondition_failed), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::payload_too_large) == static_cast<std::size_t>(http_result_t::http_payload_too_large), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::uri_too_long) == static_cast<std::size_t>(http_result_t::http_uri_too_long), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::internal_server_error) == static_cast<std::size_t>(http_result_t::http_internal_server_error), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::not_implemented) == static_cast<std::size_t>(http_result_t::http_not_implemented), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::service_unavailable) == static_cast<std::size_t>(http_result_t::http_service_unavailable), "http result invalid");
    static_assert(static_cast<std::size_t>(boost::beast::http::status::http_version_not_supported) == static_cast<std::size_t>(http_result_t::http_version_not_supported), "http result invalid");
}

template <class Body, class Fields>
void HttpResponse<Body, Fields>::set_result(http_result_t::value_t http_result)
{
    m_message.result(static_cast<boost::beast::http::status>(http_result));
}

template <class Body, class Fields>
http_result_t::value_t HttpResponse<Body, Fields>::get_result() const
{
    return (static_cast<http_result_t::value_t>(m_message.result()));
}

template <class Body, class Fields>
std::size_t HttpResponse<Body, Fields>::get_result_integer() const
{
    return (static_cast<std::size_t>(m_message.result_int()));
}

template <class Body, class Fields>
void HttpResponse<Body, Fields>::set_reason(const std::string & http_target)
{
    m_message.reason(http_target);
}

template <class Body, class Fields>
std::string HttpResponse<Body, Fields>::get_reason() const
{
    return (std::string(m_message.reason()));
}

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_RESPONSE_HPP
