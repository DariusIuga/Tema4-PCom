#include <bits/stdc++.h>
using namespace std;

int main() {
    // Get a book's id.
    string id, url;

    cout << "id=";
    // cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore leftover newline character
    getline(cin, id);
    cout << "ID: " << id << endl << endl;

    // // Validate id.
    // if (is_a_number(id) == false) {
    //     break;
    // }

    cout << "ID: " << id << endl << endl;

    // Add id to URL
    url = "/api/v1/tema/library/books/" + id;
    cout << "URL: " << url << endl << endl;

    return 0;
}
