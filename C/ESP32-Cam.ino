    //YouTube: https://www.youtube.com/brianlough

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

//------- Replace the following! ------
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

// Initialize Wifi connection to the router
char ssid[] = "Itubenar";     // your network SSID (name)
char password[] = "itubenarlo"; // your network key
unsigned long startMillis;  
unsigned long currentMillis;
const unsigned long period = 10000;

// Initialize Telegram BOT
#define BOTtoken "6243097277:AAFG37wKeMM-D8Q5zycsmedGK_ABIjx4rMM"  // your Bot Token (Get from Botfather)

#include "camera_pins.h"
#include "camera_code.h"
#define FLASH_LED_PIN 4

String user = "1558390207";

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 10; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

bool flashState = LOW;

camera_fb_t * fb = NULL;

bool isMoreDataAvailable();

int currentByte;
uint8_t* fb_buffer;
size_t fb_length;

bool isMoreDataAvailable() {
  return (fb_length - currentByte);
}

uint8_t getNextByte() {
  currentByte++;
  return (fb_buffer[currentByte - 1]);
}

bool dataAvailable = false;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/subs") {
      bot.longPoll = 60;
      String pesan = "Silahkan kirim id Telegram anda";
      bot.sendMessage(chat_id, pesan, "Markdown");
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      bot.longPoll = 0;
      while (numNewMessages) {
        Serial.println("got response");
        String text = bot.messages[i].text;
        String pesan = "user dari bot ini telah diubah menjadi " + text;
        bot.sendMessage(chat_id, pesan, "Markdown");
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        user=text;
        
        break;
      }
    }

    if (text == "/start") {
      String welcome = "Selamat datang di bot Overspeed Warning TIV Klaten. Bot ini akan mengirimkan foto ketika terjadi pelanggaran kecepatan.\n\n";
      welcome += "/start : memunculkan pesan ini\n";
      welcome += "/subs  : mengubah user tujuan dari bot ini\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(13,INPUT);
  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState); //defaults to low

  if (!setupCamera()) {
    Serial.println("Camera Setup Failed!");
    while (true) {
      delay(100);
    }
  }
  
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Make the bot wait for a new message for up to 60seconds
  bot.longPoll = 0;
}

void loop() {
    Serial.println("nunggu pesan");    
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.println("ngapain");
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    currentMillis = millis();
    startMillis=currentMillis+period;
    while (currentMillis<startMillis)
    {
    Serial.println("nunggu kecepatan");
    if(digitalRead(13)==HIGH)
    {
    fb = NULL;

      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);
      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);

      // Take Picture with Camera
      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        bot.sendMessage(user, "Camera capture failed", "");
        return;
      }

      currentByte = 0;
      fb_length = fb->len;
      fb_buffer = fb->buf;

      bot.sendPhotoByBinary(user, "image/jpeg", fb_length,
                              isMoreDataAvailable, getNextByte,
                              nullptr, nullptr);
      
      dataAvailable = true;
      String pesan = "Telah terjadi pelanggaran kecepatan\n";
      bot.sendMessage(user, pesan, "Markdown");
      Serial.println("done!");
      esp_camera_fb_return(fb);
      startMillis=currentMillis;
    }currentMillis = millis();
    }
}
