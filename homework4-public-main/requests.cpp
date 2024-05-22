#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>

#include "helpers.hpp"
#include "requests.hpp"

using namespace std;
using json = nlohmann::json;


char* compute_get_request(const char* host, const char* url, char* query_params,
    const vector<string> cookies, const string& jwt_token) {
    char* message = (char*)calloc(BUFLEN, sizeof(char));
    char* line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    }
    else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, sizeof(char) * LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    // Parse cookies
    if (cookies.empty() == false) {
        for (const string& cookie : cookies) {
            memset(line, 0, sizeof(char) * LINELEN);
            sprintf(line, "Cookie: %s", cookie.c_str());
            compute_message(message, line);
        }
    }

    // Parse jwt token
    if (jwt_token.empty() == false) {
        memset(line, 0, sizeof(char) * LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt_token.c_str());
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char* compute_post_request(const char* host, const char* url, char* const content_type,
    const json* json, const string& jwt_token) {
    char* message = (char*)calloc(BUFLEN, sizeof(char));
    char* line = (char*)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    memset(line, 0, sizeof(char) * LINELEN);
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, sizeof(char) * LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    // dump converts a json object to a string (serialization)
    string payload = json->dump();

    memset(line, 0, sizeof(char) * LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, sizeof(char) * LINELEN);
    sprintf(line, "Content-Length: %zu", payload.length());
    compute_message(message, line);

    // Step 4: add the JWT token
    if (jwt_token.empty() == false) {
        memset(line, 0, sizeof(char) * LINELEN);
        sprintf(line, "Authorization: Bearer %s", jwt_token.c_str());
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    strcat(message, payload.c_str());

    free(line);
    return message;
}
