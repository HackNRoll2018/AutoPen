#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Servo.h>
#include <time.h>
#include <stdlib.h>

// Initialize Wifi connection to the router
char ssid[] = "AutoPen";     // your network SSID (name)
char password[] = "autopeniscool"; // your network key

// Initialize Telegram BOT
#define BOTtoken "541193212:AAFcR1kpHMYdI1MvFjIxdY4Qm-uwrW5t7bw"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 100; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;
bool state = false;

int servoPin = D6;
int speakerPin = D1;
int redPin = D2;
int greenPin = D4;
int bluePin = D3;
int colorMode = 0;

String keyboardJson = "[[\"/clickpen\"], [\"/help\"], [\"/changecolor\"]]";

Servo servo;
int servoAngle = 0;   // servo position in degrees

void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/clickpen") {
      //control the servo's direction and the position of the motor
      if (state) {
        tone(speakerPin, 523.25, 450);
      } else {
        tone(speakerPin, 349.23, 450);
      }
      servo.write(90);      // Turn SG90 servo Left to 45 degrees
      delay(500);
      if (state) {
        tone(speakerPin, 349.23, 500);
      } else {
        tone(speakerPin, 523.25, 500);
      }
      servo.write(0);
      state = !state;
      bot.sendMessageWithReplyKeyboard(chat_id, "Pen is clicked", "", keyboardJson, true, false);
    }

    else if (text == "/start") {
      String welcome = "Welcome to AutoPen, " + from_name + ".\n";
      welcome += "This Telegram bot helps you to automate the most mundane part of using a pen.\n\n";
      welcome += "/clickpen : to click pen\n";
      welcome += "/changecolor : to change the color of the LED\n";
      welcome += "/help : shows all commands\n";
      bot.sendMessageWithReplyKeyboard(chat_id, welcome, "Markdown", keyboardJson, true, false);
    }

    else if (text == "/changecolor") {
      switch (colorMode) {
        case 0:
          setColor(255, 0, 0);
          bot.sendMessageWithReplyKeyboard(chat_id, "Color is changed", "", keyboardJson, true, false);
          break;
        case 1:
          setColor(0, 255, 0);
          bot.sendMessageWithReplyKeyboard(chat_id, "Color is changed", "", keyboardJson, true, false);
          break;
        case 2:
          setColor(0, 0, 255);
          bot.sendMessageWithReplyKeyboard(chat_id, "Color is changed", "", keyboardJson, true, false);
          break;
        case 3:
          setColor(0, 0, 0);
          bot.sendMessageWithReplyKeyboard(chat_id, "LED is turned off", "", keyboardJson, true, false);
          break;
      }

      colorMode = (colorMode + 1) % 4;
    }

    else if (text == "/help") {
      String help = "Commands:\n\n";
      help += "/start : to see the welcome message\n";
      help += "/clickpen : to click pen\n";
      help += "/changecolor : to change the color of the LED\n";
      bot.sendMessage(chat_id, help, "Markdown");
    }

    else {
      bot.sendMessageWithReplyKeyboard(chat_id, "Unknown command. see /help", "", keyboardJson, true, false);
    }

  }
}

void setup() {
  Serial.begin(9600);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  srand(time(NULL));

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  servo.attach(servoPin);
  pinMode(servoPin, OUTPUT); // initialize digital servoPin as an output.
  servo.write(0);
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
