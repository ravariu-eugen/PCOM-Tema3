#ifndef _REQUESTS_
#define _REQUESTS_
#include <vector>
#include <string>
// computes and returns a GET request string 
std::string compute_get_request(
    std::string &host,
    std::string &url,
    std::string &query_params,
    std::vector<std::string> &cookies, 
    std::vector<std::string> &headers);
// computes and returns a POST request string 
std::string compute_post_request(
    std::string &host, 
    std::string &url, 
    std::string &content_type, 
    std::vector<std::string> &body_data,
    std::vector<std::string> &cookies, 
    std::vector<std::string> &headers);
// computes and returns a DELETE request string 
std::string compute_delete_request(
    std::string &host,
    std::string &url,
    std::vector<std::string> &cookies,
    std::vector<std::string> &headers);
#endif
