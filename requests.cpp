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

std::string compute_get_request(
    std::string &host,
    std::string &url,
    std::string &query_params,
    std::vector<std::string> &cookies,
    std::vector<std::string> &headers)
{
    std::string message;
    std::string line;

    // Step 1: write the method name, URL, request params (if any) and protocol type
    line = "GET " + url;
    if (query_params.size() > 0) {
        line += "?" + query_params;
    } 
    line += " HTTP/1.1";

    compute_message(message, line);

    // Step 2: add the host
    line = "Host: " + host;
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    for(auto h:headers)
        compute_message(message, h);
    
    if (!cookies.empty()) {
        line = "Cookie:" + cookies[0];
        for(size_t i = 1; i < cookies.size(); i++) {
            line += "; " + cookies[i];
        }
        compute_message(message, line);
    }
    // Step 4: add final new line
    line = "";
    compute_message(message, line);
    return message;
}

std::string compute_post_request(
    std::string &host, 
    std::string &url, 
    std::string &content_type, 
    std::vector<std::string> &body_data,
    std::vector<std::string> &cookies,
    std::vector<std::string> &headers){

    std::string message;
    std::string line;

    // Step 1: write the method name, URL and protocol type
    line = "POST " + url + " HTTP/1.1";
    compute_message(message, line);
    
    // Step 2: add the host
    line = "Host: " + host;
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    line = "Content-Type: " + content_type;
    compute_message(message, line);
    int content_len = 0;
    for(auto x:body_data) // calculeaza lungimea totala
        content_len += x.size();
    line = "Content-Length: " + std::to_string(content_len);
    compute_message(message, line);
    for(auto h:headers)
        compute_message(message, h);
    // Step 4 (optional): add cookies
    if (!cookies.empty()) {
        line = "Cookie:" + cookies[0];
        for(size_t i = 1; i < cookies.size(); i++) {
            line += "; " + cookies[i];
        }
        compute_message(message, line);
    }
    // Step 5: add new line at end of header
    line = "";
    compute_message(message, line);
    // Step 6: add the actual payload data
    for(auto x:body_data){
        compute_message(message,x);
    }

    return message;
}
std::string compute_delete_request(
    std::string &host,
    std::string &url,
    std::vector<std::string> &cookies,
    std::vector<std::string> &headers)
{
    std::string message;
    std::string line;

    // Step 1: write the method name, URL, request params (if any) and protocol type
    line = "DELETE " + url + " HTTP/1.1";

    compute_message(message, line);

    // Step 2: add the host
    line = "Host: " + host;
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    for(auto h:headers)
        compute_message(message, h);
    
    if (!cookies.empty()) {
        line = "Cookie:" + cookies[0];
        for(size_t i = 1; i < cookies.size(); i++) {
            line += "; " + cookies[i];
        }
        compute_message(message, line);
    }
    // Step 4: add final new line
    line = "";
    compute_message(message, line);
    return message;
}