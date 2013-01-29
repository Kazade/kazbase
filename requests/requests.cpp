#include <tr1/memory>
#include <mutex>
#include <thread>
#include <openssl/crypto.h>
#include <curl/curl.h>

#include "requests.h"
#include "../string.h"
#include "../logging.h"
#include "../thread/future.h"

namespace requests {

using namespace type;

static bool initialized = false;

static std::vector<std::tr1::shared_ptr<std::mutex> > locks;
static unsigned long thread_id = 0;
static std::map<std::string, unsigned long> thread_ids;

static std::mutex thread_id_mutex;

static unsigned long openssl_thread_id() {
    /*
        Well... this is a whole bunch of fun! OpenSSL wants an ID for the
        thread that has to be an unsigned long. Boost doesn't give us a consistent
        portable id that can be easily converted to an unsigned long, so instead
        we cast the thread id to a string, and use that as a lookup. We need
        to lock the call because it is going to be called from a number of threads.
    */
    std::lock_guard<std::mutex> lock(thread_id_mutex);

    std::stringstream ios;
    ios << std::this_thread::get_id();
    std::string idx = ios.str();

    std::map<std::string, unsigned long>::iterator it = thread_ids.find(idx);
    if(it != thread_ids.end()) {
        return (*it).second;
    }

    thread_ids[idx] = ++thread_id;
    return thread_ids[idx];
}

static void openssl_thread_lock(int mode, int lock_id, const char* file, int line) {
    if(mode & CRYPTO_LOCK) {
        locks.at(lock_id)->lock();
    } else {
        locks.at(lock_id)->unlock();
    }
}

void init() {
    if(initialized) return;

    if (CRYPTO_get_locking_callback() && CRYPTO_get_id_callback()) {
        L_WARN("Not setting SSL callbacks, as some already exist");
        initialized = true;
		return;
	}

	int num_thread_locks = CRYPTO_num_locks();
	for(int i = 0; i < num_thread_locks; ++i) {
        locks.push_back(std::tr1::shared_ptr<std::mutex>(new std::mutex));
	}

	CRYPTO_set_locking_callback(&openssl_thread_lock);
	CRYPTO_set_id_callback(&openssl_thread_id);

	initialized = true;
}

void cleanup() {
    CRYPTO_set_locking_callback(NULL);
	CRYPTO_set_id_callback(NULL);
	initialized = false;
}

std::string char2hex(char dec) {
	char dig1 = (dec&0xF0)>>4;
	char dig2 = (dec&0x0F);
	if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48 in ascii
	if (10<= dig1 && dig1<=15) dig1+=65-10; //A,65 in ascii
	if ( 0<= dig2 && dig2<= 9) dig2+=48;
	if (10<= dig2 && dig2<=15) dig2+=65-10;

    std::string r;
	r.append( &dig1, 1);
	r.append( &dig2, 1);
	return r;
}

std::string url_quote(const std::string& input, const std::string& safe) {
    std::string safe_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~" + safe;

    std::string result;
    for(std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        if(safe_chars.find((*it)) != std::string::npos) {
            result += (*it);
        } else {
            result += "%" + char2hex((*it));
        }
    }

    return result;
}

std::string url_quote_plus(const std::string& input) {
    std::string result = url_quote(input, " ");
    boost::replace_all(result, " ", "+");
    return result;
}

std::string url_encode(const MultiValueDict& data) {
    std::vector<std::string> params;

    for(MultiValueDict::const_iterator it = data.begin(); it != data.end(); ++it) {
        std::vector<std::string> items = (*it).second;
        for(std::string s: items) {
            std::string k = url_quote((*it).first);
            std::string v = url_quote(s);
            params.push_back(k + "=" + v);
        }
    }
    return str::join(params, "&");
}

MultiValueDict url_decode(const std::string& query) {
    if(query.empty()) {
        return MultiValueDict();
    }

    std::vector<std::string> params = str::split(query, "&");

    MultiValueDict result;
    for(std::string s: params) {
        std::vector<std::string> values = str::split(s, "=");

        if(values.size() == 2) {
            result.append(values.at(0), values.at(1));
        } else {
            std::cerr << "Invalid query string: " << query << std::endl;
        }
    }

    return result;
}

std::pair<std::string, std::string> split_netloc(std::string url, int start) {
    std::string::size_type delim = url.length();

    std::string delims = "/?#";
    for(std::string::const_iterator c = delims.begin(); c != delims.end(); ++c) {
        std::string::size_type wdelim = url.find((*c), start);
        if(wdelim != std::string::npos) {
            delim = std::min(delim, wdelim);
        }
    }

    std::string first_part(url.begin() + start, url.begin() + delim);
    std::string second_part(url.begin() + delim, url.end());
    return std::make_pair(first_part, second_part);
}

std::map<std::string, std::string> url_split(const std::string& url) {
    std::string work = url;
    std::map<std::string, std::string> result;

    std::string::size_type it = work.find(":");

    std::string scheme;
    std::string netloc;
    std::string path;
    std::string query;
    std::string fragment;

    if(it != std::string::npos) {
        scheme = std::string(work.begin(), work.begin() + it);
        work = std::string(work.begin() + it + 1, work.end());
    }

    if(str::starts_with(work, "//")) {
        std::pair<std::string, std::string> parts = split_netloc(work, 2);
        netloc = parts.first;
        work = parts.second;
    }

    if(work.find("#") != std::string::npos) {
        std::vector<std::string> parts = str::split(work, "#");
        work = parts.at(0);
        fragment = parts.at(1);
    }

    if(work.find("?") != std::string::npos) {
        std::vector<std::string> parts = str::split(work, "?");
        work = parts.at(0);
        query = parts.at(1);
    }

    result["scheme"] = scheme;
    result["netloc"] = netloc;
    result["fragment"] = fragment;
    result["query"] = query;
    result["path"] = work;

    return result;
}

struct URLReader {
    std::string data;

    URLReader():
        data(std::string()) {

    }

    size_t read(void *ptr, size_t size, size_t nmemb) {
        size_t to_read = size*nmemb;

        char* data_to_read = (char*)ptr;
        data.insert(data.end(), data_to_read, data_to_read+to_read);

        return to_read;
    }

    static size_t read_callback(void *ptr, size_t size, size_t nmemb, void* data) {
        URLReader* _this = (URLReader*) data;
        return _this->read(ptr, size, nmemb);
    }
};

Response Client::get(const std::string& path, const MultiValueDict& data, const Dict& headers, const int timeout) {
    assert(initialized && "You must initialize the requests library");

    CURL *curl;
    CURLcode res;
    URLReader reader_;

    struct curl_slist* header_list = NULL;

    curl = curl_easy_init();
    if(!curl) {
        throw std::runtime_error("Could not initialize CURL");
    }

    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, NULL);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &URLReader::read_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reader_);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 15);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1);

    if(!headers.empty()) {
        typedef std::pair<std::string, std::string> HeaderPair;
        for(HeaderPair item: headers) {
            std::string header = item.first + ": " + item.second;
            header_list = curl_slist_append(header_list, header.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }

    std::string url = path;
    if(!data.empty()) {
        if(!str::ends_with(url, "/")) {
            url += "/";
        }
        url = url + url_encode(data);
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    L_DEBUG("CURL: About to perform request");
    res = curl_easy_perform(curl);
    L_DEBUG("CURL: Request finished - result: " + boost::lexical_cast<std::string>(res));

    int status_code = 0;
    long connect_code = 0;
    char* final_url = NULL;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_getinfo(curl, CURLINFO_HTTP_CONNECTCODE, &connect_code);
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &final_url);

    Response resp;
    resp.set_content(reader_.data);
    resp.set_status_code(status_code);
    resp.set_connect_code(connect_code);
    resp.set_final_url(std::string(final_url));

    if(header_list) {
        curl_slist_free_all(header_list);
    }

    curl_easy_cleanup(curl);

    return resp;
}

Response Client::get_to_file(const std::string& path,
                                 const std::string& output_filename,
                                 const MultiValueDict& data,
                                 const Dict& headers,
                                 const int timeout) {

    Response resp = get(path, data, headers, timeout);
    if(resp.get_status_code() != status::OK) {
        return resp;
    }

    std::ofstream file_out(output_filename.c_str(), std::ios::binary);
    std::string content = resp.get_content();
    file_out.write(content.c_str(), content.length());
    file_out.close();

    return resp;
}

std::shared_future<Response> Client::get_async(const std::string& path, const MultiValueDict& data, const Dict& headers, const int timeout) {
    std::shared_future<Response> res = thread::submit_task(std::bind(&Client::get, this, path, data, headers, timeout));
    return res;
}

std::shared_future<Response> Client::get_to_file_async(const std::string& path, const std::string& output_filename,
        const MultiValueDict& data, const Dict& headers, const int timeout) {

    std::shared_future<Response> res = thread::submit_task(std::bind(&Client::get_to_file, this, path, output_filename, data, headers, timeout));
    return res;
}

}
