# Tema 4 Protocoale de Comunicatie - Client Web. Comunicatie cu REST API.

## Mentiune: Clientul a fost scris pornind de la implementarea din Laboratorul 9: protocolul HTTP.

## Biblioteci folosite

-   [nlohmann](https://github.com/nlohmann/json) (serializare/deserializare JSON)

## Structura codului

-   Am folosit biblioteca nlohmann pentru parsarea JSON-ului pentru ca are un API destul de simplu, a fost usor de inclus si e probabil cea mai populara astfel de biblioteca pentru C++. Am folosit versiunea bibliotecii cu un singur header "json.hpp", care poate fi gasita [aici](https://github.com/nlohmann/json/tree/develop/single_include/nlohmann).
-   Am modificat functiile din requests.cpp din laboratorul 9 pentru a lucra cu text JSON si pentru a transmite tokenuri JWT unde e nevoie. Am adaugat si o functie care compune o cerere HTTP DELETE, folosita pentru stergerea unei carti.
-   Fisierele buffer.hpp, buffer.cpp, helpers.hpp si helpers.cpp au functii auxiliare pentru conectarea/deconectarea la server, transmiterea si primirea pachetelor, si lucrul cu stringul. Nu le-am modificat.
-   Fisierele requests.hpp si requests.cpp contin functii de construire a cererilor HTTP de tipul GET, POST si DELETE.
-   Logica pentru client poate fi gaunordered_mapsita in client.cpp. client.hpp are functii pentru extragerea unui cookie de sesiune, a tokenului JWT, a status code de la response.

## Functionarea clientului

-   Retin fd-ul socketului cu care comunic cu serverul, bufere de scriere/citire, cookieul de sesiune si tokenul jwt pe parcursul executiei clientului.
-   Incep un loop infinit in care citesc comenzi de la utilizator, si aleg valori dintr-un enum pentru selectarea fiecarei comenzi valide.

---

-   exit:
    Dau break, procesul se termina imediat.
-   register:
    Afisez un prompt utilizatorului pentru username si parola utilizatorului,
    le citesc si verific ca acestea nu contin spatii goale.
    Creez un obiect json care contine username si password si il pun in corpul cererii
    HTTP POST atunci cand apelez compute_post_request.

    \*Deschid o conexiune cu serverul
    la adresa IP si portul specificat in cerinta, trimit requestul si primesc un response de la server,
    inchid conexiunea si printez requestul. Apelez functia is_http_status_ok pentru
    a anunta utilizatorul daca am primit o eroare de la server (daca statul code are
    forma 4xx sau 5xx).

    \*Fac acesti pasi pentru fiecare comanda. In continuare nu ii voi mai mentiona.

-   login:
    Efectuez autentificarea utilizatorului. Afisez un prompt pentru username si parola, le citesc si verific daca acestea contin spatii goale. Creez un obiect JSON cu username si password si il trimit intr-o cerere HTTP POST pentru autentificare. Salvez cookie-ul de sesiune din raspunsul serverului.

-   enter_library:
    Cer acces in biblioteca. Trimit o cerere HTTP GET cu cookie-ul de sesiune si primesc un JWT token in raspuns.

-   get_books:
    Cer toate cartile de pe server. Trimit o cerere HTTP GET folosind JWT token pentru autorizare si afisez lista de carti primita.

-   get_book:
    Cer informatie despre o carte specifica pe baza unui ID. Solicit utilizatorului sa introduca ID-ul cartii, validez ID-ul si trimit o cerere HTTP GET cu JWT token pentru autorizare.

-   add_book:
    Adaug o carte noua. Solicit utilizatorului sa introduca titlul, autorul, genul, editura si numarul de pagini. Validez inputul si trimit o cerere HTTP POST cu datele cartii si JWT token pentru autorizare.

-   delete_book:
    Sterg o carte. Solicit utilizatorului sa introduca ID-ul cartii, validez ID-ul si trimit o cerere HTTP DELETE cu JWT token pentru autorizare.

-   logout:
    Efectuez delogarea utilizatorului. Trimit o cerere HTTP GET pentru delogare folosind cookie-ul de sesiune. Dupa delogare, sterg cookie-ul de sesiune si JWT token-ul din memoria clientului.
