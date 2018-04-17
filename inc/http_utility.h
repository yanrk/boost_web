/********************************************************
 * Description : http utility
 * Data        : 2018-04-07 22:39:00
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2018
 ********************************************************/

#ifndef BOOST_WEB_HTTP_UTILITY_H
#define BOOST_WEB_HTTP_UTILITY_H


#include <string>
#include <algorithm>
#include <boost/beast/http.hpp>
#include "boost_web.h"
#include "http_request.hpp"
#include "http_response.hpp"

namespace BoostWeb { // namespace BoostWeb begin

extern const std::string & get_mime_type(const std::string & filename);
extern std::string path_catenate(const std::string & base, const std::string & path);

template <class Body, class Fields, class Send>
void handle_request(WebServiceBase * service, const std::string & doc_root, const HttpConnectionBase & connection, boost::beast::http::request<Body, Fields> && req, Send && send)
{
    auto const bad_request = [&req](boost::beast::string_view why)
    {
        boost::beast::http::response<boost::beast::http::string_body> res{ boost::beast::http::status::bad_request, req.version() };
        res.set(boost::beast::http::field::server, "boost web server 1.0 by yanrk");
        res.set(boost::beast::http::field::content_type, "text/html");
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.body() = why.to_string();
        res.prepare_payload();
        return (res);
    };

    auto const not_found = [&req](boost::beast::string_view target)
    {
        boost::beast::http::response<boost::beast::http::string_body> res{ boost::beast::http::status::not_found, req.version() };
        res.set(boost::beast::http::field::server, "boost web server 1.0 by yanrk");
        res.set(boost::beast::http::field::content_type, "text/html");
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + target.to_string() + "' was not found";
        res.prepare_payload();
        return (res);
    };

    auto const server_error = [&req](boost::beast::string_view what)
    {
        boost::beast::http::response<boost::beast::http::string_body> res{ boost::beast::http::status::internal_server_error, req.version() };
        res.set(boost::beast::http::field::server, "boost web server 1.0 by yanrk");
        res.set(boost::beast::http::field::content_type, "text/html");
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + what.to_string() + "'";
        res.prepare_payload();
        return (res);
    };

    const std::string request_target(req.target().to_string());

    std::string request_target_append_slash(request_target + "/");
    std::replace(request_target_append_slash.begin(), request_target_append_slash.end(), '\\', '/');
    if (request_target.empty() || '/' != request_target.front() || std::string::npos != request_target_append_slash.find("/../"))
    {
        return (send(bad_request("Illegal request-target '" + request_target + "'")));
    }

    if (service->target_is_path(request_target))
    {
        std::string path = path_catenate(doc_root, request_target);
        if ('/' == path.back())
        {
            path.append("index.html");
        }

        boost::beast::error_code ec;
        boost::beast::http::file_body::value_type body;
        body.open(path.c_str(), boost::beast::file_mode::scan, ec);

        if (boost::system::errc::no_such_file_or_directory == ec)
        {
            return (send(not_found(req.target())));
        }

        if (ec)
        {
            return (send(server_error(ec.message())));
        }

        /*
         * cache the body size since we need it after the move operate
         */
        const uint64_t body_size = body.size();

        if (boost::beast::http::verb::head == req.method())
        {
            boost::beast::http::response<boost::beast::http::empty_body> res{ boost::beast::http::status::ok, req.version() };
            res.set(boost::beast::http::field::server, "boost web server 1.0 by yanrk");
            res.set(boost::beast::http::field::content_type, get_mime_type(path));
            res.set(boost::beast::http::field::access_control_allow_origin, "*");
            res.content_length(body_size);
            res.keep_alive(req.keep_alive());
            return (send(std::move(res)));
        }
        else if (boost::beast::http::verb::get == req.method())
        {
            boost::beast::http::response<boost::beast::http::file_body> res{ std::piecewise_construct, std::make_tuple(std::move(body)), std::make_tuple(boost::beast::http::status::ok, req.version()) };
            res.set(boost::beast::http::field::server, "boost web server 1.0 by yanrk");
            res.set(boost::beast::http::field::content_type, get_mime_type(path));
            res.set(boost::beast::http::field::access_control_allow_origin, "*");
            res.content_length(body_size);
            res.keep_alive(req.keep_alive());
            return (send(std::move(res)));
        }
        else
        {
            return (send(bad_request("Unsupported HTTP-method '" + req.method_string().to_string() + "' while request file")));
        }
    }
    else
    {
        if (boost::beast::http::verb::connect < req.method())
        {
            return (send(bad_request("Unsupported HTTP-method '" + req.method_string().to_string() + "'")));
        }

        boost::beast::http::response<boost::beast::http::string_body, Fields> res;
        res.set(boost::beast::http::field::server, "boost web server 1.0 by yanrk");
        res.set(boost::beast::http::field::content_type, "text/html");
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        HttpRequest<Body, Fields> request(req);
        HttpResponse<boost::beast::http::string_body, Fields> response(res);
        service->handle_request(connection, request, response);
        res.keep_alive(req.keep_alive());
        res.prepare_payload();
        return (send(std::move(res)));
    }
}

} // namespace BoostWeb end


#endif // BOOST_WEB_HTTP_UTILITY_H
