//
// file_handler.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "file_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

#include "../threads/ThreadWeb.h"
#include "../common.h"
#include "../classes/Log.h"

namespace http {
    namespace server4 {

        void file_handler::operator()(const request& req, reply& rep) {
            vector<string> uri;
            boost::algorithm::split(uri, req.uri, boost::algorithm::is_any_of("?"));

            map<string, string> parameters;
            if (uri.size() > 1) {
                string p;
                string v;
                vector<string> params;
                boost::algorithm::split(params, uri[1], boost::algorithm::is_any_of("&"));
                for (vector<string>::iterator i = params.begin(); i != params.end(); i++) {
                    vector<string> pv;
                    boost::algorithm::split(pv, *i, boost::algorithm::is_any_of("="));
                    if (pv.size() == 2) {
                        url_decode(pv[0], p);
                        url_decode(pv[1], v);
                        parameters[p] = v;
                    } else if (pv.size() == 2) {
                        url_decode(pv[0], p);
                        parameters[p] = "";
                    }
                }
            }

            // Decode url to path.
            std::string request_path;
            if (uri.size() == 0 || !url_decode(uri[0], request_path)) {
                rep = reply::stock_reply(reply::bad_request);
                return;
            }

            // Request path must be absolute and not contain "..".
            if (request_path.empty() || request_path[0] != '/'
                    || request_path.find("..") != std::string::npos) {
                rep = reply::stock_reply(reply::bad_request);
                return;
            }

            stringstream html;
            if (request_path == "/") {
                ThreadWeb::handlerHome(html);
            } else if (request_path == "/config") {
                ThreadWeb::handlerConfig(html);
            } else if (request_path == "/task") {
                if (!ThreadWeb::handlerTask(html, parameters)) {
                    rep = reply::stock_reply(reply::not_found);
                    return;
                }
            } else if (request_path == "/counters") {
                ThreadWeb::handlerCounters(html);
            } else if (request_path == "/client") {
                ThreadWeb::handlerClient(html, parameters);
            } else {
                rep = reply::stock_reply(reply::not_found);
                return;
            }
            rep.status = reply::ok;
            rep.content.append(html.str());
            rep.headers.resize(3);
            rep.headers[0].name = "Cache-Control";
            rep.headers[0].value = "no-store, no-cache, must-revalidate";
            rep.headers[1].name = "Expires";
            rep.headers[1].value = "Mon, 26 Jul 1997 05:00:00 GMT";
            rep.headers[2].name = "Content-Length";
            rep.headers[2].value = boost::lexical_cast<std::string>(rep.content.size());
        }

        bool file_handler::url_decode(const std::string& in, std::string& out) {
            out.clear();
            out.reserve(in.size());
            for (std::size_t i = 0; i < in.size(); ++i) {
                if (in[i] == '%') {
                    if (i + 3 <= in.size()) {
                        int value = 0;
                        std::istringstream is(in.substr(i + 1, 2));
                        if (is >> std::hex >> value) {
                            out += static_cast<char> (value);
                            i += 2;
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }
                } else if (in[i] == '+') {
                    out += ' ';
                } else {
                    out += in[i];
                }
            }
            return true;
        }

    } // namespace server4
} // namespace http
