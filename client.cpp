#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <vector>
#include <string>
#include "helpers.hpp"
#include "requests.hpp"
#include "json.hpp"
#include <iostream>
using json = nlohmann::json;

/**
 * @brief verifica daca un string este un intreg pozitiv
 * 
 * @param nr 
 * @return true 
 * @return false 
 */
bool isNumber(std::string& nr){
        for(auto c:nr)
            if(std::isdigit(c) == 0) return false;
        return true;
    }
    
class LibraryServer{

    private:
    std::vector<std::string> cookies;
    std::string JWT; // token acces biblioteca
    std::string serverIP;
    short port;
    int sockfd;
    /**
     * @brief deschide conexiunea la server
     * 
     */
    void open(){
        this->sockfd = open_connection(this->serverIP, port,AF_INET, SOCK_STREAM, 0);
    }
    
    /**
     * @brief obtine codul reply-ului
     * 
     * @param response 
     * @return int 
     */
    int get_code(const std::string& response){
        return std::stoi(response.substr(response.find(" ")+1, 3));
    }
    
    /**
     * @brief adauga cookie-urile din reply
     * 
     * @param response 
     */
    void add_cookies(std::string &response){
        int poz = 0;
        int end;
        while(1){
            poz = response.find("Set-Cookie:", poz);
            if(poz == -1)break;
            poz += sizeof("Set-Cookie:");
            end = response.find("\r\n", poz);
            std ::string c = response.substr(poz, end - poz);
            //std::cout << c << std::endl;
            cookies.push_back(c);
        }
    }
    
    
    public:

    LibraryServer(std::string host, short port){
        this->port = port;
        this->serverIP = host;
        this->JWT = std::string("null");
        
    
    }

    ~LibraryServer(){
        close_connection(sockfd);
    }
    /**
     * @brief inregistreaza user la server cu parola data
     * 
     * @param user 
     * @param password 
     * 
     */
    void registerUser(std::string &user, std::string &password){
        open();
        json payload;
        payload["username"] = user;
        payload["password"] = password;
        std::string data = payload.dump();

        std::string url("/api/v1/tema/auth/register");
        std::string type("application/json");
        std::vector<std::string> body = {data};
        std::vector<std::string> headers;

        std::string message =  compute_post_request(serverIP, url, 
                                    type, body, cookies, headers);
        send_to_server(sockfd, message);

        std::string response = receive_from_server(sockfd);

        int code = get_code(response);
        switch(code){
            case 200: printf("200: OK\n"); break;
            case 400: printf("400: Username taken!\n"); break;
            default : printf("unknown: %d\n", code);
        }
    }
    /**
     * @brief autentifica utilizatorul
     * 
     * @param user 
     * @param password 
     * 
     */
    void login(std::string &user, std::string &password){
        open();
        json payload;
        payload["username"] = user;
        payload["password"] = password;
        std::string data = payload.dump();

        std::string url("/api/v1/tema/auth/login");
        std::string type("application/json");
        std::vector<std::string> body = {data};
        std::vector<std::string> headers;

        std::string message =  compute_post_request(serverIP, url, 
                                    type, body, cookies, headers);
        send_to_server(sockfd, message);

        std::string response = receive_from_server(sockfd);

        int code = get_code(response);
        switch(code){
            case 200: printf("200: OK\n"); add_cookies(response); break;
            case 204: printf("204: Already logged in\n"); break;
            case 400: printf("400: Bad credentials\n"); break;
            default : printf("unknown: %d\n", code);
        }
    }
    /**
     * @brief obtine token-ul de acces la biblioteca
     * 
     * 
     */
    void get_access_token(){
        open();
        std::string url("/api/v1/tema/library/access");
        std::string query_p;
        std::vector<std::string> headers;
        
        std::string message =  compute_get_request(serverIP, url, 
                                        query_p, cookies, headers);
        send_to_server(sockfd, message);
        
        std::string response = receive_from_server(sockfd);

        int code = get_code(response);
        std::string data;
        switch(code){
            case 200: 
                data = basic_extract_json_response(response);
                this->JWT = data.substr(10, data.rfind("\"}")-10);
                printf("200: OK\n"); 
                break;
            case 401: printf("401: Not logged in\n"); break;
            default : printf("unknown: %d\n", code);
        }
    }
    /**
     * @brief afiseaza numele si id-ul tuturor cartilor
     * 
     * 
     */
    void view_books_summary(){
        open();
        std::string url("/api/v1/tema/library/books");
        std::string query_p;
        std::vector<std::string> headers = {std::string("Authorization: Bearer " + this->JWT)};
        
        std::string message =  compute_get_request(serverIP, url, 
                                        query_p, cookies, headers);
        send_to_server(sockfd, message);

        std::string response = receive_from_server(sockfd);

        int code = get_code(response);
        std::string data;
        switch(code){
            case 200: 
                printf("200: OK\n");
                data = response.substr(response.find("["));
                std::cout << data << std::endl; 
                break;
            case 500: printf("500: No access\n"); break;
            default : printf("unknown: %d\n", code);
        }
    }
    /**
     * @brief afiseaza datele unei carti
     * 
     * @param id id-ul cartii
     * 
     */
    void get_book_data(int id){
        open();
        std::string url("/api/v1/tema/library/books/"+std::to_string(id));
        std::string query_p;
        std::vector<std::string> headers = {std::string("Authorization: Bearer " + this->JWT)};
        
        std::string message =  compute_get_request(serverIP, url, 
                                        query_p, cookies, headers);
        send_to_server(sockfd, message);
        
        std::string response = receive_from_server(sockfd);

        int code = get_code(response);
        std::string data;
        switch(code){
            case 200: 
                printf("200: OK\n"); 
                data = basic_extract_json_response(response);
                std::cout << data.substr(0,data.find("]")) << std::endl;
                break;
            case 404: printf("404: Book %d Not Found\n", id); break;
            case 500: printf("500: No access\n"); break;
            default : printf("unknown: %d\n", code);
        }
        
        
        
    }
    /**
     * @brief adauga o carte la biblioteca
     * 
     * @param title 
     * @param author 
     * @param genre 
     * @param publisher 
     * @param page_count 
     * 
     */
    void add_book(std::string &title, 
                std::string &author, 
                std::string &genre, 
                std::string &publisher, 
                int page_count){
        open();
        json payload;
        payload["title"] = title;
        payload["author"] = author;
        payload["genre"] = genre;
        payload["publisher"] = publisher;
        payload["page_count"] = page_count;
        std::string data = payload.dump();

        std::string url("/api/v1/tema/library/books");
        std::string type("application/json");
        std::vector<std::string> body = {data};
        std::vector<std::string> headers = {std::string("Authorization: Bearer " + this->JWT)};
        
        std::string message =  compute_post_request(serverIP, url, 
                                    type, body, cookies, headers);
        send_to_server(sockfd, message);
        
        std::string response = receive_from_server(sockfd);

        int code = get_code(response);
        switch(code){
            case 200: printf("200: OK\n"); break;
            case 500: printf("500: No access\n"); break;
            default : printf("unknown: %d\n", code);
        }
    }
    /**
     * @brief sterge o carte din biblioteca
     * 
     * @param id id-ul cartii
     * 
     */
    void delete_book(int id){
        open();

        std::string url("/api/v1/tema/library/books/"+std::to_string(id));
        std::vector<std::string> headers = {std::string("Authorization: Bearer " + this->JWT)};
        
        std::string message =  compute_delete_request(serverIP, url, 
                                                cookies, headers);
        send_to_server(sockfd, message);

        std::string response = receive_from_server(sockfd);

        int code = get_code(response);
        switch(code){
            case 200: printf("200: OK\n"); break;
            case 404: printf("404: Book %d Not Found\n", id); break;
            case 500: printf("500: No access\n"); break;
            default : printf("unknown: %d\n", code);
        }
    }
    /**
     * @brief efectueaza logout
     * 
     * 
     */
    void logout(){
        open();
        std::string url("/api/v1/tema/auth/logout");
        std::string query_p;
        std::vector<std::string> headers;

        std::string message =  compute_get_request(serverIP, url, 
                                        query_p, cookies, headers);
        send_to_server(sockfd, message);

        std::string response = receive_from_server(sockfd);

        int code = get_code(response);
        if(code == 200) {
            cookies.clear();
            JWT = std::string("null");
        }
        switch(code){
            case 200: 
                cookies.clear();
                JWT = std::string("null");
                printf("200: OK\n"); 
                break;
            case 400: printf("400: Not logged in\n"); break;
            default : printf("unknown: %d\n", code);
        }
    }
    
    
};


int main(int argc, char *argv[])
{
    char command[100];
    LibraryServer library("34.241.4.235", 8080);
        
    while(1){
        std::cin >> command;
        if(strcmp(command, "register") == 0){
            std::string username,password;
            printf("username=");
            std::cin >> username;
            printf("password=");
            std::cin >> password;
            library.registerUser(username, password);
            
            
        }else if(strcmp(command, "login") == 0){
            std::string username,password;
            printf("username=");
            std::cin >> username;
            printf("password=");
            std::cin >> password;
            library.login(username, password);
            
        }else if(strcmp(command, "enter_library") == 0){
            library.get_access_token();
            
        }else if(strcmp(command, "get_books") == 0){
            library.view_books_summary();
            
        }else if(strcmp(command, "get_book") == 0){
            std::string id;
            printf("id=");
            std::cin >> id;
            if(!isNumber(id)){
                
                printf("Error: id must be a number\n");
                continue;
            }
            library.get_book_data(std::stoi(id));

        }else if(strcmp(command, "add_book") == 0){
            std::string title, author, genre, publisher, page_count;
            printf("title=");
            std::cin >> title;
            printf("author=");
            std::cin >> author;
            printf("genre=");
            std::cin >> genre;
            printf("publisher=");
            std::cin >> publisher;
            printf("page_count=");
            std::cin >> page_count;
            if(!isNumber(page_count)){
                printf("Error: page_count must be a number\n");
                continue;
            }
            library.add_book(title, author, genre, publisher, std::stoi(page_count));
            
        }else if(strcmp(command, "delete_book") == 0){
            std::string id;
            printf("id=");
            std::cin >> id;
            if(!isNumber(id)){
                
                printf("Error: id must be a number\n");
                continue;
            }
            library.delete_book(std::stoi(id));
            
        }else if(strcmp(command, "logout") == 0){
            library.logout();
            
        }else if(strcmp(command, "exit") == 0){
            break;
        }else{ // default
            printf("error: unknown command\n");
        }
    }

    return 0;
}
