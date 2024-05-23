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

    // Check if the status code is at leas3 characters long
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

int main() {
    int sockfd;
    char* request;
    char* response;
    string url;
    string session_cookie;
    string jwt_token;

    while (true) {
        string input;
        cin >> input;

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore leftover newline character

        Command command = InvalidCommand;

        auto it = commandMap.find(input);
        if (it != commandMap.end()) {
            command = it->second;
        }

        if (input == "exit") {
            break;
        }

        switch (command) {
        case Register:
        {
            string username, password;

            cout << "username=";
            cin >> username;
            cout << "password=";
            cin >> password;

            // Validate input: reject username/passsword with empty spaces.
            if (username.find(" ") != string::npos || password.find(" ") != string::npos) {
                cout << "ERROR: Username or password contain spaces!";
                continue;
            }

            // A json object containing the username and password
            json prompt_json = {
                {"username", username},
                {"password", password}
            };

            url = "/api/v1/tema/auth/register";
            request = compute_post_request(SERVER_IP, url, "application/json", prompt_json, "");

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            if (is_http_status_ok(response)) {
                cout << "SUCCESS:" << endl;
            }
            else {
                cout << "ERROR:" << endl;
            }
            cout << response << endl;

            break;
        }

        case Login:
        {
            string username, password;

            cout << "username=";
            cin >> username;
            cout << "password=";
            cin >> password;

            // Validate input: reject username/passsword with empty spaces.
            if (username.find(" ") != string::npos || password.find(" ") != string::npos) {
                cout << "ERROR: Username or password contain spaces!";
                continue;
            }

            // A json object containing the username and password
            json prompt_json = {
                {"username", username},
                {"password", password}
            };

            url = "/api/v1/tema/auth/login";
            request = compute_post_request(SERVER_IP, url, "application/json", prompt_json, "");

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            if (is_http_status_ok(response)) {
                cout << "SUCCESS:" << endl;
            }
            else {
                cout << "ERROR:" << endl;
            }
            cout << response << endl;

            // Search for the session cookie in the HTTP response header for cookies.
            session_cookie = get_cookie(response);

            break;
        }
        case EnterLibrary:
        {
            // Put the session cookie in a vector and create an HTTP GET request based on it.
            // I used a whole vector in case there were multiple cookies that needed to be send in the request.
            vector<string> cookies;
            cookies.push_back(session_cookie);
            url = "/api/v1/tema/library/access";
            request = compute_get_request(SERVER_IP, url, NULL, cookies, "");

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            if (is_http_status_ok(response)) {
                cout << "SUCCESS:" << endl;
            }
            else {
                cout << "ERROR:" << endl;
            }
            cout << response << endl;

            // Search for the jwt token in the HTTP response.
            jwt_token = get_jwt_token(response);

            break;
        }
        case GetAllBooks:
        {
            vector<string> cookies;
            url = "/api/v1/tema/library/books";
            request = compute_get_request(SERVER_IP, url, NULL, cookies, jwt_token);

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            if (is_http_status_ok(response)) {
                cout << "SUCCESS:" << endl;
            }
            else {
                cout << "ERROR:" << endl;
            }
            cout << response << endl;

            break;
        }
        case GetBook:
        {
            // Get a book's id.
            string id;

            cout << "id=";
            cin >> id;

            // Validate id.
            if (is_a_number(id) == false) {
                break;
            }

            // Add id to URL
            url = "/api/v1/tema/library/books/" + id;

            vector<string> cookies;
            request = compute_get_request(SERVER_IP, url, NULL, cookies, jwt_token);

            cout << "Request: " << request << endl << endl;

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            if (is_http_status_ok(response)) {
                cout << "SUCCESS:" << endl;
            }
            else {
                cout << "ERROR:" << endl;
            }
            cout << response << endl;

            break;
        }
        case AddBook:
        {
            // Output a prompt for creating a new book
            // and get input from the user.
            string title, author, genre, publisher, page_count;

            cout << "title=";
            getline(cin, title);
            cout << "author=";
            getline(cin, author);
            cout << "genre=";
            getline(cin, genre);
            cout << "publisher=";
            getline(cin, publisher);
            cout << "page_count=";
            getline(cin, page_count);

            // Validate page count.
            if (is_a_number(page_count) == false) {
                cout << "ERROR: Page count should be a number!";
                break;
            }

            // Create json for the book to be added.
            json new_book_json = {
                   {"title", title},
                   {"author", author},
                   {"genre", genre},
                   {"page_count", stoi(page_count)},
                   {"publisher", publisher}
            };

            // cout << "JSON: " << new_book_json << endl << endl;

            url = "/api/v1/tema/library/books";
            request = compute_post_request(SERVER_IP, url, "application/json", new_book_json, jwt_token);

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            if (is_http_status_ok(response)) {
                cout << "SUCCESS:" << endl;
            }
            else {
                cout << "ERROR:" << endl;
            }
            cout << response << endl;

            break;
        }
        case DeleteBook:
        {
            // Get a book's id.
            string id;

            cout << "id=";
            cin >> id;

            // Validate id.
            if (is_a_number(id) == false) {
                break;
            }

            // Add id to URL
            url = "/api/v1/tema/library/books/" + id;

            // Create delete request.
            request = compute_delete_request(SERVER_IP, url, jwt_token);

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            if (is_http_status_ok(response)) {
                cout << "SUCCESS:" << endl;
            }
            else {
                cout << "ERROR:" << endl;
            }
            cout << response << endl;

            break;
        }
        case Logout:
        {
            vector<string> cookies;
            cookies.push_back(session_cookie);
            url = "/api/v1/tema/auth/logout";

            request = compute_get_request(SERVER_IP, url, NULL, cookies, jwt_token);

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            if (is_http_status_ok(response)) {
                cout << "SUCCESS:" << endl;
            }
            else {
                cout << "ERROR:" << endl;
            }
            cout << response << endl;

            // Clear the session cookie and jwt token
            session_cookie = "";
            jwt_token = "";

            break;
        }
        default:
            cerr << "Error: Invalid command detected!" << endl;
            break;
        }

    }

    return 0;
}