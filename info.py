import requests
import json
import os
import time
import random
from colorama import init, Fore, Style

init(autoreset=True)

# === CONFIGURATION ===
API_TOKEN = "7576981793:2WnIfAmi"
LANG = "ru"
LIMIT = 300
URL = "https://leakosintapi.com/"

# === Typing Animation ===
def type_effect(text, delay=0.002):
    for char in text:
        print(char, end='', flush=True)
        time.sleep(delay)
    print()

# === Swipe-style Text Animation ===
def swipe_effect(text, delay=0.01):
    for line in text.splitlines():
        for char in line:
            print(char, end='', flush=True)
            time.sleep(delay)
        print()

# === Banner ===
def show_banner():
    os.system("clear")
    ascii_banner = f"""{Fore.GREEN}

           _   _ _____  _____ _    _     ________   _______  _      ____ _____ _______ _____  
     /\   | \ | |_   _|/ ____| |  | |   |  ____\ \ / /  __ \| |    / __ \_   _|__   __/ ____| 
    /  \  |  \| | | | | (___ | |__| |   | |__   \ V /| |__) | |   | |  | || |    | | | (___   
   / /\ \ | . ` | | |  \___ \|  __  |   |  __|   > < |  ___/| |   | |  | || |    | |  \___ \  
  / ____ \| |\  |_| |_ ____) | |  | |   | |____ / . \| |    | |___| |__| || |_   | |  ____) | 
 /_/    \_\_| \_|_____|_____/|_|  |_|   |______/_/ \_\_|    |______\____/_____|  |_| |_____/  

"""
    credit = f"{Fore.RED}{Style.BRIGHT}➤ Credit by ANISH EXPLOITS\n"
    type_effect(ascii_banner, delay=0.0005)
    print(credit)

# === JavaScript-style Formatter ===
def format_as_js(data):
    js_lines = []
    for key, value in data.items():
        key_str = key
        value_str = json.dumps(value)
        js_lines.append(f"  {key_str}: {value_str}")
    return "{\n" + ",\n".join(js_lines) + "\n}"

# === Random Color Swipe Printer ===
def print_colored(text, delay=0.01):
    colors = [Fore.YELLOW, Fore.CYAN, Fore.MAGENTA, Fore.LIGHTBLUE_EX, Fore.LIGHTGREEN_EX]
    for line in text.splitlines():
        color = random.choice(colors)
        for char in line:
            print(color + char, end='', flush=True)
            time.sleep(delay)
        print()

# === Leak Report ===
def generate_report(query):
    data = {
        "token": API_TOKEN,
        "request": query.strip(),
        "limit": LIMIT,
        "lang": LANG
    }

    try:
        response = requests.post(URL, json=data).json()
    except Exception as e:
        print(Fore.RED + f"\n❌ API ERROR: {e}")
        return

    if "Error code" in response:
        print(Fore.RED + f"\n🚫 API Error: {response['Error code']}")
        return

    for db in response["List"].keys():
        db_title = "Professor Anish" if db.lower() == "1win" else db
        db_name = f"\n=== [ DATABASE: {db_title} ] ===\n"
        type_effect(Fore.LIGHTRED_EX + db_name)

        if db != "No results found":
            for entry in response["List"][db]["Data"]:
                formatted = format_as_js(entry)
                print_colored(formatted)

# === Main CLI ===
def main():
    os.system("clear")
    show_banner()
    while True:
        try:
            type_effect(Fore.BLUE + "\n[+] Send Your Target Number (email, phone, etc):")
            target = input(Fore.LIGHTGREEN_EX + "Target ➤ ")
            if target.lower() == "exit":
                break
            generate_report(target)
            print(Fore.LIGHTBLACK_EX + "\nType another target or 'exit' to quit.\n")
        except KeyboardInterrupt:
            print(Fore.RED + "\n⛔ Exiting...")
            break

# === Entry Point ===
if __name__ == "__main__":
    main()