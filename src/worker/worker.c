/*
** worker.c for Cherokee
**
** Made by BOURDALE Jules
** Login   <bourda_j@etna-alternance.net>
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#include "../threadpool/thpool.h"
#include "../log/log.h"
#include "../parser/parser.h"
#include "worker.h"


void worker(int skt, c_config* config)
{
    int client_skt;
    char buf[1024];
    c_request *req;
    c_struct_thpool *params;
    threadpool  thpool;

    params = malloc(sizeof(c_struct_thpool));

    log_info("Worker successfuly started.");
    thpool = thpool_init(config->nbthreadperworker);
    log_info("Thread pool successfuly initialized (%d threads).", config->nbthreadperworker);
    while(1) {
        log_info("Worker ready to accept connection");
        client_skt = accept(skt, NULL, NULL);
        log_info("New client connection accepted");
        if (client_skt < 0) {
            perror("accept failed");
            log_error("Failed to accept connection");
            exit(1);
        }

        memset(buf, '\0', 1024);
        int read = recv(client_skt, buf, 1024, 0);
        if (read < 0) {
            log_error("Unable to read data");
            perror("Client read failed\n");
        }
        log_debug("GOT REQUEST:\n%s", buf);

        req = parse(buf);

        params->client_skt = client_skt;
        params->config = config;
        params->req = req;

        thpool_add_work(thpool, (void*)work_in_thread, (void*)params);
    }

    thpool_destroy(thpool);
    log_error("Worker exiting");
    exit(0);
}

void work_in_thread(c_struct_thpool* params_struct) {
        log_info("Thread #%u on work !",(int)pthread_self());
        c_struct_thpool *params;
        params = (c_struct_thpool*) params_struct;
        if (params->req == NULL) {
            close(params->client_skt);
            return;
        }

        resolve_http_decision_diagram(params->config, params->req);
        log_debug("BUILD RESPONSE");
        int res_len = build_response(params->req);
        log_debug("BUILD RESPONSE DONE");

        int err = send(params->client_skt, params->req->response->raw, res_len, 0);
        if (err < 0) {
            log_error("Client write failed");
        } else {
            if (params->req->body != NULL) {
                log_info("HTTP/%d.%d %s %s (%d) Sent %d bytes (%d)", params->req->version.major, params->req->version.minor, methodToStr(params->req->method), params->req->url, strlen(params->req->body), params->req->response->status.code);
            } else {
                log_info("HTTP/%d.%d %s %s (%d)", params->req->version.major, params->req->version.minor, methodToStr(params->req->method), params->req->url, params->req->response->status.code);
            }
        }

        free_request(params->req);
        close(params->client_skt);
}

pid_t spawn_worker(int skt, c_config* config)
{
    pid_t pid = create_process();

    if (pid == -1)
    {
        perror("fork");
        return -1;
    } else if (pid == 0)
    {
        worker(skt, config);
    }

    return pid;
}

void spawn_multiple_workers(pid_t *pids, int skt, c_config* config) {
    int i;
    pid_t pid;

    log_info("Spawning %d workers", config->workers);

    for (i = 0; i < config->workers; i++)
    {
        pid = spawn_worker(skt, config);
        if (pid == 0) {
            break;
        } else {
            pids[i] = pid;
        }
    }
}
