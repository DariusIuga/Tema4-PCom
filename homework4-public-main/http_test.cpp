#include <iostream>
#include <cstring>
#include <cctype>

bool isHttpResponseOk(const char* response) {
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

int main() {
    const char* response1 = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    const char* response2 = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";

    std::cout << std::boolalpha;
    std::cout << "Response 1 is OK: " << isHttpResponseOk(response1) << std::endl;
    std::cout << "Response 2 is OK: " << isHttpResponseOk(response2) << std::endl;

    return 0;
}
