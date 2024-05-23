#include "client.hpp"

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

        // Ignore leftover newline character for stdin
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        Command command = InvalidCommand;

        // Associate the command string to an enum value
        auto it = commandMap.find(input);
        if (it != commandMap.end()) {
            command = it->second;
        }

        // Close the client
        if (input == "exit") {
            break;
        }

        switch (command) {
            // Register a new account
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

            // The steps from starting a new connection and printing the response are the same for all commands.
            // Open a TCP connection
            sockfd = open_connection(SERVER_IP, SERVER_PORT, AF_INET,
                SOCK_STREAM, 0);

            // Send the HTTP request to the server
            send_to_server(sockfd, request);

            // Get the HTTP response from the server
            response = receive_from_server(sockfd);

            // Close the connection
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
        // Login to existing account
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
        // Get access to the library
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
        // Fetch information about all books
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
        // Get information about a book based on id
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
        // Add a new book to the user's collection
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
        // Delete the book with the given id
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
        // Logout
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
        // Print error message for a wrong command
        default:
            cerr << "ERROR: Invalid command detected!" << endl;
            break;
        }

    }

    return 0;
}