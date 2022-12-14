/*
** is_method.c for Cherokee
**
** Made by BOURDALE Jules
** Login   <bourda_j@etna-alternance.net>
**
*/

#include "./httpdd.h"
#include "../../log/log.h"

int is_method(c_request *req, Http_Method method) {
    if (req != NULL && req->method == method) {
        return HTTPDD_TRUE;
    }
    return HTTPDD_FALSE;
}

int is_get_method(__attribute__((unused)) c_config *config, c_request *req) {
    return is_method(req, GET);
}

int is_get_or_head_method(__attribute__((unused)) c_config *config, c_request *req) {
    int x = (is_method(req, GET) || is_method(req, HEAD));
    log_debug("IS GET OR HEAD METHOD : %d", x);
    return x;
}

int is_post_method(__attribute__((unused)) c_config *config, c_request *req) {
    return is_method(req, POST);
}

int is_put_method(__attribute__((unused)) c_config *config, c_request *req) {
    return is_method(req, PUT);
}

int is_delete_method(__attribute__((unused)) c_config *config, c_request *req) {
    return is_method(req, DELETE);
}

int is_head_method(__attribute__((unused)) c_config *config, c_request *req) {
    return is_method(req, HEAD);
}
