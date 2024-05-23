#include <iostream>
#include <string>
#include <sys/socket.h>
#include <vector>
#include <cstring>
#include <unordered_map>

// Libraries for the client
#include "helpers.hpp"
#include "requests.hpp"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;


constexpr char SERVER_IP[] = "34.246.184.49";
// #define WEATHER_SERVER_ADDR "api.openweathermap.org"
constexpr int SERVER_PORT = 8080;

bool is_http_status_ok(const char* response) {
    if (response == nullptr) {
        return false;
    }

    // Find the first space to get the end of the HTTP version string
    const char* spacePos = strchr(response, ' ');
    if (spacePos == nullptr) {
        return false;
    }

    // Move past the space to get the status code
    const char* statusCodePos = spacePos + 1;

    // Check if the status code is at least 3 characters long
    if (strlen(statusCodePos) < 3) {
        return false;
    }

    // Check that the status code is a number
    for (int i = 0; i < 3; ++i) {
        if (!isdigit(statusCodePos[i])) {
            return false;
        }
    }

    // Check the first digit of the status code
    char firstDigit = statusCodePos[0];
    return (firstDigit == '1' || firstDigit == '2' || firstDigit == '3');
}

const string get_jwt_token(char* response) {
    char* token = strstr(response, "token");
    char const delimitator[] = ":\"";

    if (token != NULL) {
        strtok(token, delimitator);
        // Return the jwt token.
        return string(strtok(NULL, delimitator));
    }

    return "";
}

// Gets the cookie from the Set-Cookie header of an HTTP response.
// If the header doesn't exist (in the case of an error), it returns "".
const string get_cookie(char* response) {
    char* session_cookie = strstr(response, "Set-Cookie: ");
    char const delimitator[] = " ;";

    if (session_cookie != NULL) {
        strtok(session_cookie, delimitator);
        // The cookie is the content of the header, after ':'.
        return string(strtok(NULL, delimitator));
    }

    return "";
}

bool is_a_number(string str) {
    if (str.empty()) {
        // Check for empty strings
        return false;
    }

    // num should contain what's after the parsed number.
    // So, if it points to \0 after strtol, the whole string was indeed a number.
    char* num;
    strtol(str.c_str(), &num, 10);

    if (*num == 0) {
        return true;
    }
    else {
        return false;
    }
}

// Used for selecting which action the client will do
enum Command {
    Register,
    Login,
    EnterLibrary,
    GetAllBooks,
    GetBook,
    AddBook,
    DeleteBook,
    Logout,
    Exit,
    InvalidCommand
};

// Maps user input to enum values
unordered_map<string, Command> commandMap = {
    {"register", Register},
    {"login", Login},
    {"enter_library", EnterLibrary},
    {"get_books", GetAllBooks},
    {"get_book", GetBook},
    {"add_book", AddBook},
    {"delete_book", DeleteBook},
    {"logout", Logout},
};
