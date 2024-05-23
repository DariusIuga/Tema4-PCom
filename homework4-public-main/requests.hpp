#ifndef _REQUESTS_
#define _REQUESTS_

#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char* compute_get_request(const char* host, const string& url, char* query_params,
    const vector<string> cookies, const string& jwt_token);

// computes and returns a POST request string (cookies can be NULL if not needed)
char* compute_post_request(const char* host, const string& url, const string& content_type, const json& json, const string& jwt_token);

#endif


