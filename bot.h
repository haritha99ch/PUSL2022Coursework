//Telegram Bot
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

String sendPhotoTelegram();
String _chatId = "980828375";
String accessToken = "5268188026:AAGHMx8RZZgQYmGiLo6UG8eqt7I0QYkbFBk";
bool sendPhoto = false;

WiFiClientSecure clientTCP;
UniversalTelegramBot bot(accessToken, clientTCP);

void handleNewMessages(int numNewMessages) {
  Serial.println(xPortGetCoreID());
  Serial.print("New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chatId = String(bot.messages[i].chat_id);
    if (chatId != _chatId) {
      bot.sendMessage(_chatId, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String botMessage = bot.messages[i].text;
    Serial.println(botMessage);

    String fromName = bot.messages[i].from_name;

    if (botMessage == "/lock") {
        door.attach(14);
        door.write(0);
        Serial.println("Door Locked");
    }else if (botMessage == "/unlock") {
        door.attach(14);
        door.write(180);
        prevMillis=millis();
        Serial.println("Door Unlocked");
    }
    if (botMessage == "/photo") {
      sendPhoto = true;
      Serial.println("New photo  request");
    }
    if (botMessage == "/start") {
      String welcome = "Welcome to the ESP32-CAM Telegram bot.\n";
      bot.sendMessage(chatId, welcome, "");
    }
  }
}

//SendPhoto
String sendPhotoToTelegram(){
  const char* myDomain = "api.telegram.org";
  String getAll = "";
  String getBody = "";

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
    return "Camera capture failed";
  }  
  
  Serial.println("Connect to " + String(myDomain));

  if (clientTCP.connect(myDomain, 443)) {
    Serial.println("Connection successful");
    
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"chat_id\"; \r\n\r\n" + _chatId + "\r\n--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"photo\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint16_t imageLen = fb->len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;
  
    clientTCP.println("POST /bot"+accessToken+"/sendPhoto HTTP/1.1");
    clientTCP.println("Host: " + String(myDomain));
    clientTCP.println("Content-Length: " + String(totalLen));
    clientTCP.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    clientTCP.println();
    clientTCP.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0;n<fbLen;n=n+1024) {
      if (n+1024<fbLen) {
        clientTCP.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        clientTCP.write(fbBuf, remainder);
      }
    }  
    
    clientTCP.print(tail);
    
    esp_camera_fb_return(fb);
    
    int waitTime = 10000;   // timeout 10 seconds
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + waitTime) > millis()){
      Serial.print(".");
      delay(100);      
      while (clientTCP.available()) {
        char c = clientTCP.read();
        if (state==true) getBody += String(c);        
        if (c == '\n') {
          if (getAll.length()==0) state=true; 
          getAll = "";
        } 
        else if (c != '\r')
          getAll += String(c);
        startTimer = millis();
      }
      if (getBody.length()>0) break;
    }
    clientTCP.stop();
    Serial.println(getBody);
  }
  else {
    getBody="api.telegram.org connection failed.";
    Serial.println(getBody);
  }
  return getBody;
}

void readBot() {
  if (sendPhoto) {
    Serial.println("Taking Photo");
    sendPhoto = false;
    sendPhotoToTelegram();
  }
  int botRequestDelay = 1000;   // mean time between scan messages
  long lastTimeBotRan;

  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
