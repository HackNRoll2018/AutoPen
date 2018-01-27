import json
import requests
import time
import urllib


class AutoPen:

    def __init__(self):
        return

    def get_json_from_url(self, url):
        content = self.get_url(url)
        js = json.loads(content)
        return js

    def get_updates(self, offset=None):
        url = self.url_prefix + "getUpdates?timeout=100"
        if offset:
            url += "&offset={}".format(offset)
        js = self.get_json_from_url(url)
        return js

    def send_message(self, text, chat_id):
        text = urllib.parse.quote_plus(text)
        url = self.url_prefix + "sendMessage?text={}&chat_id={}&parse_mode=Markdown".format(text, chat_id)
        self.get_url(url)

    def handle_updates(self, updates):
        for update in updates["result"]:
            chat = update["message"]["chat"]["id"]
            self.db.add_user(chat)
            try:
                text = str(update["message"]["text"]).strip()
            except KeyError: # if the update has no text (gif, sticker, image, audio, etc)
                self.send_message("please do not do that", chat)
                return

    @staticmethod
    def get_url(url):
        response = requests.get(url)
        content = response.content.decode("utf8")
        return content

    @staticmethod
    def get_last_update_id(updates):
        update_ids = []
        for update in updates["result"]:
            update_ids.append(int(update["update_id"]))
        return max(update_ids)

    @staticmethod
    def get_last_chat_id_and_text(updates):
        num_updates = len(updates["result"])
        last_update = num_updates - 1
        text = updates["result"][last_update]["message"]["text"]
        chat_id = updates["result"][last_update]["message"]["chat"]["id"]
        return text, chat_id


def main():
    token = "541193212:AAFcR1kpHMYdI1MvFjIxdY4Qm-uwrW5t7bw"
    bot = AutoPen("https://api.telegram.org/bot{}/".format(token))
    last_update_id = None
    while True:
        updates = bot.get_updates(last_update_id)
        if len(updates["result"]) > 0:
            last_update_id = bot.get_last_update_id(updates) + 1
            bot.handle_updates(updates)
        time.sleep(0.4)


if __name__ == '__main__':
    main()
