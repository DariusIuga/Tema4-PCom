#include <iostream>
#include <string>
#include <sys/socket.h>
#include <vector>
#include <cstring>

// Libraries for the client
#include "helpers.hpp"
#include "requests.hpp"
#include <unordered_map>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

constexpr char SERVER_IP[] = "34.246.184.49";
// #define WEATHER_SERVER_ADDR "api.openweathermap.org"
constexpr int SERVER_PORT = 8080;

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
    string session_cookie;
    string jwt_token;

    while (true) {
        string input;
        cin >> input;


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
                continue;
            }

            // A json object containing the username and password
            json prompt_json = {
                {"username", username},
                {"password", password}
            };

            request = compute_post_request(SERVER_IP, "/api/v1/tema/auth/register", "application/json", &prompt_json, "");

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
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
                continue;
            }

            // A json object containing the username and password
            json prompt_json = {
                {"username", username},
                {"password", password}
            };

            request = compute_post_request(SERVER_IP, "/api/v1/tema/auth/login", "application/json", &prompt_json, "");

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            send_to_server(sockfd, request);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
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
            request = compute_get_request(SERVER_IP, "/api/v1/tema/library/access", NULL, cookies, "");

            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            response = receive_from_server(sockfd);

            close_connection(sockfd);

            // Print the response from the server.
            cout << response << endl;

            // Search for the jwt token in the HTTP response.
            jwt_token = get_jwt_token(response);

            break;
        }
        case GetAllBooks:
            cout << "Get all books command executed" << endl;
            break;
        case GetBook:
            cout << "Get book command executed" << endl;
            break;
        case AddBook:
            cout << "Add book command executed" << endl;
            break;
        case DeleteBook:
            cout << "Delete book command executed" << endl;
            break;
        case Logout:
            cout << "Logout command executed" << endl;
            break;
        default:
            cerr << "Error: Invalid command detected!" << endl;
            break;
        }

    }

    return 0;
}