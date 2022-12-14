/*
** main.c for Cherokee
**
** Made by BOURDALE Jules
** Login   <bourda_j@etna-alternance.net>
**
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <signal.h>

#include "./worker/worker.h"
#include "./parser/parser.h"
#include "./log/log.h"
#include "./config/config.h"
#include "./custom/custom.h"

static volatile int skt;

void sigint_handler(__attribute__((unused)) int i) {
    close(skt);
}

int main(void){
    struct sockaddr_in server;
    int nb_workers;
    pid_t *pids;
    c_config *config;

    signal(SIGINT, sigint_handler);

    config = new_config();
    nb_workers = config->workers;
    pids = malloc(nb_workers * sizeof(pid_t));

    log_set_fp(fopen(config->logfile, "a+"));
    log_set_level(config->loglevel);

    config->router = get_custom_router(config);
    log_router(config->router);

    log_info("Starting Cherokee");

    // Creating the socket
    skt = socket(AF_INET, SOCK_STREAM, 0);
    if (skt == -1)
    {
        close(skt);
        log_error("socket creation");
        exit(-1);
    }
    int reuse = 1;
    if (setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        log_error("setsockopt(SO_REUSEADDR) failed");
        exit(-1);
    }

    if (setsockopt(skt, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)  {
        log_error("setsockopt(SO_REUSEPORT) failed");
        exit(-1);
    }


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(config->port);

    //Bind
    if(bind(skt, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("bind failed");
        close(skt);
        exit(-1);
    }

    // start listening on socket
    listen(skt, config->backlog);
    spawn_multiple_workers(pids, skt, config);

    int i;
    for (i = 0; i < nb_workers; i++) {
        waitpid(pids[i], NULL, 0);
    }
    log_info("Closing Cherokee");
}
