#ifndef REQUESTS_H_INCLUDED
#define REQUESTS_H_INCLUDED

#include <future>
#include "../types.h"

namespace requests {

typedef std::map<std::string, std::string> Dict;
using type::MultiValueDict;

const std::string MULTIPART_CONTENT = "multipart/form-data";

namespace status {
    typedef int Code;

    const Code CONTINUE = 100;
    const Code SWITCHING_PROTOCOLS = 101;
    const Code OK = 200;
    const Code CREATED = 201;
    const Code ACCEPTED = 202;
    const Code NON_AUTHORITATIVE_INFO = 203;
    const Code NO_CONTENT = 204;
    const Code RESET_CONTENT = 205;
    const Code PARTIAL_CONTENT = 206;
    const Code MULTIPLE_CHOICES = 300;
    const Code MOVED_PERMANENTLY = 301;
    const Code FOUND = 302;
    const Code SEE_OTHER = 303;
    const Code NOT_MODIFIED = 304;
    const Code USE_PROXY = 305;
    const Code TEMPORARY_REDIRECT = 307;
    const Code BAD_REQUEST = 400;
    const Code UNAUTHORIZED = 401;
    const Code PAYMENT_REQUIRED = 402;
    const Code FORBIDDEN = 403;
    const Code NOT_FOUND = 404;
    const Code NOT_ACCEPTABLE = 406;
    const Code PROXY_AUTH_REQUIRED = 407;
    const Code REQUEST_TIMEOUT = 408;
    const Code CONFLICT = 409;
    const Code GONE = 410;
    const Code LENGTH_REQUIRED = 411;
    const Code PRECONDITION_FAILED = 412;
    const Code REQUEST_ENTITY_TOO_LARGE = 413;
    const Code REQUEST_URI_TOO_LONG = 414;
    const Code UNSUPPORTED_MEDIA_TYPE = 415;
    const Code REQUESTED_RANGE_NOT_SATISFIABLE = 416;
    const Code EXPECTATION_FAILED = 417;
    const Code INTERNAL_SERVER_ERROR = 500;
    const Code NOT_IMPLEMENTED = 501;
    const Code BAD_GATEWAY = 502;
    const Code SERVICE_UNAVAILABLE = 503;
    const Code GATEWAY_TIMEOUT = 504;
    const Code HTTP_VERSION_NOT_SUPPORTED = 505;
}

class Client;

class Response {
public:
    std::string get_content() const { return content_; }
    status::Code get_status_code() const { return status_code_; }
    long get_connect_code() const { return connect_code_; }
    std::string get_final_url() const { return final_url_; }

    Response():
        content_(""),
        status_code_(status::OK),
        connect_code_(0) {}
private:
    void set_content(const std::string& content) { content_ = content; }
    void set_status_code(const status::Code code) { status_code_ = code; }
    void set_connect_code(const long code) { connect_code_ = code; }
    void set_final_url(const std::string& final_url) { final_url_ = final_url; }

    std::string content_;
    status::Code status_code_;
    long connect_code_;
    std::string final_url_;

    friend class Client;
};

class Client {
public:
    Response get(const std::string& path,
        const type::MultiValueDict& data=type::MultiValueDict(),
        const Dict& headers=Dict(),
        const int timeout=45
    );

    Response get_to_file(const std::string& path,
        const std::string& output_filename,
        const type::MultiValueDict& data=type::MultiValueDict(),
        const Dict& headers=Dict(),
        const int timeout=45
    );

    std::shared_future<Response> get_async(const std::string& path,
        const type::MultiValueDict& data=type::MultiValueDict(),
        const Dict& headers=Dict(),
        const int timeout=45
    );

    std::shared_future<Response> get_to_file_async(const std::string& path,
        const std::string& output_filename,
        const type::MultiValueDict& data=type::MultiValueDict(),
        const Dict& headers=Dict(),
        const int timeout=45
    );

    Response post(const std::string& path,
        const type::MultiValueDict& data=type::MultiValueDict(),
        const std::string& content_type=MULTIPART_CONTENT,
        const Dict& headers=Dict()
    );
};

//Must be called before using the client interface
void init();
void cleanup();

std::map<std::string, std::string> url_split(const std::string& url);
std::pair<std::string, std::string> split_netloc(std::string url, int start = 0);
type::MultiValueDict url_decode(const std::string& query);
std::string url_encode(const type::MultiValueDict& data);
std::string url_quote(const std::string& input, const std::string& safe="");
std::string url_quote_plus(const std::string& input);
}

#endif // REQUESTS_H_INCLUDED
