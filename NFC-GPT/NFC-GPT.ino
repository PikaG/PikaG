#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Audio.h"

#include <SPI.h>
#include "MFRC522.h"

#define SS_PIN  5  // ESP32 pin GPIO5 assigned to SS
#define RST_PIN 33 // ESP32 pin GPIO33 assigned to RST

#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

const char* ssid = "Readyplayer";
const char* password = "guojingyi518";
const char* chatgpt_token = "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";
const char* temperature = "0";
const char* max_tokens = "45";

Audio audio;

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte const BUFFERSIZE = 18; // Buffer size for reading NFC tag data

String keywords[3]; // Array to store keywords from each NFC tag
int readCount = 0; // Counter for the number of reads
String Question = "";
String combinedKeywords = "";

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  while (!Serial);

  // wait for WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(100);

  SPI.begin();            
  mfrc522.PCD_Init(); 
  Serial.println(F("Scan a lot:"));
}

void loop()
{

  /*
  Serial.print("Enter 3 keywords, one at a time, followed by ENTER : ");

  // Collect 3 keywords
  String keywords[3];

  for (int i = 0; i < 3; i++) {
    Serial.print(i+1); Serial.print(") ");
    while (!Serial.available()) {
      audio.loop();
    }
    while (Serial.available()) {
      char add = Serial.read();
      if (add == '\n' || add == '\r') {
        break; // Break on newline or carriage return
      }
      keywords[i] += add;
      delay(1);
    }
  }
  */

  audio.loop();

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    //Serial.println("NFC tag detected.");

    byte dataBuffer[BUFFERSIZE];
    if (readNFCTagData(dataBuffer)) {
      // Extract the keyword from the NFC tag data
      String keyword = parseNFCTagData(dataBuffer);
      if (!keyword.isEmpty()) {
        keywords[readCount] = keyword;
        readCount++;
        Serial.println("Keyword read: " + keyword);
      } else {
        Serial.println("No keyword found on lot.");
      }
    } else {
      Serial.println("Failed to read lot.");
    }

    // Check if this is the third read
    if (readCount == 3) {
      // Concatenate the keywords into a single string and print them
      combinedKeywords = keywords[0] + ", " + keywords[1] + ", " + keywords[2];
      //Serial.println("Combined Keywords: " + combinedKeywords);

      Question = combinedKeywords;
      //int len = Question.length();
      //Question = Question.substring(0, (len - 1));
      Question = "Write a poem that includes the following three keywords '" + Question + "'. The poem should weave these elements together in a cohesive and evocative manner, capturing the essence of each keyword in the imagery and themes of the poem.";
      Question = "\"" + Question + "\"";
      //Serial.println(Question);
      Serial.println("Writing your poem...");

      HTTPClient https;

      //Serial.print("[HTTPS] begin...\n");
      if (https.begin("https://api.openai.com/v1/completions")) {  // HTTPS

      https.addHeader("Content-Type", "application/json");
      String token_key = String("Bearer ") + chatgpt_token;
      https.addHeader("Authorization", token_key);

      String payload = String("{\"model\": \"gpt-3.5-turbo-instruct\", \"prompt\": ") + 
      Question + String(", \"temperature\": ") + temperature + String(", \"max_tokens\": ") + 
      max_tokens + String("}"); 
      //Instead of TEXT as Payload, can be JSON as Paylaod

      //Serial.print("[HTTPS] GET...\n");

      // start connection and send HTTP header
      int httpCode = https.POST(payload);

      // httpCode will be negative on error
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        //Serial.println(payload);

        DynamicJsonDocument doc(1024);


        deserializeJson(doc, payload);
        String Answer = doc["choices"][0]["text"];
        Answer = Answer.substring(2);
        Serial.println(Answer);
        audio.connecttospeech(Answer.c_str(), "en"); // Google TTS

      }
      else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
      }
      else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      Question = "";

      // Reset for the next set of reads
      readCount = 0;
    }

    mfrc522.PICC_HaltA(); // Halt PICC
    mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD

    Serial.println("Place the next lot:");
    delay(1000); // Delay to avoid immediate re-reads
  }

}

void audio_info(const char *info) {
  //Serial.print("audio_info: "); Serial.println(info);
}

bool readNFCTagData(byte *dataBuffer) {
  MFRC522::StatusCode status;
  byte buffer[BUFFERSIZE];
  byte byteCount = sizeof(buffer);
  
  // Attempt to read from block 4 (adjust as necessary for your tag)
  status = mfrc522.MIFARE_Read(4, buffer, &byteCount);
  if (status == MFRC522::STATUS_OK) {
    memcpy(dataBuffer, buffer, byteCount);
    return true;
  }
  return false;
}

String parseNFCTagData(byte *dataBuffer) {
  bool enFound = false; // Flag to indicate the "en" sequence was found
  String result = ""; // Will hold the extracted text after "en"
  String temp = ""; // Temporary string for assembling characters after "en"

  for (int i = 0; i < BUFFERSIZE - 1; i++) { // Use BUFFERSIZE - 1 to avoid overflow
    // Check for "en" sequence
    if (!enFound && dataBuffer[i] == 'e' && dataBuffer[i + 1] == 'n') {
      enFound = true; // Set flag to start recording after this
      i++; // Skip the 'n' of "en" to avoid including it in the result
      continue; // Move to the next iteration
    }

    // If "en" has been found, start appending characters
    if (enFound) {
      // Stop reading and trim if a period is encountered
      if (dataBuffer[i] == '.') {
        temp.trim(); // Remove any leading or trailing whitespace before the period
        break; // Exit the loop early upon finding a period
      }
      
      // Only append if character is not a space or newline
      if (dataBuffer[i] > 32 && dataBuffer[i] != '\n' && dataBuffer[i] != '\r') {
        temp += (char)dataBuffer[i];
      }
    }
  }

  // Trim spaces or new lines at the end of the stored info (in case of no period found)
  temp.trim(); // The trim() function removes both leading and trailing whitespace

  // If "en" was found and there is text after it
  if (enFound && temp.length() > 0) {
    result = temp; // Use the accumulated text as the result
  }
  
  return result;
}
