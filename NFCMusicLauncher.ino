
// Wifi stuff
const char *ssid = "";
const char *password = "";



// mDNS name, to access to WebServer. (Only compatible in Linux, Macox, iOS...)
const char *mDNShostname = "musicbox";

//https://github.com/fabestine1/MyThing/blob/main/ftg-main-master/Arduino/libraries/NDEF/Ndef.h
#include "MifareUltralight.h"

//https://github.com/miguelbalboa/rfid
#include <MFRC522.h>

#include <SPI.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>

// WebSockets By Markus Sattler https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsServer.h>

// ArduinoJson By Benoit Blanchon https://github.com/bblanchon/ArduinoJson
#include <ArduinoJson.h>



//These files are not libraries

// WebServerTemplate
#include "htmlandcss.h"

// OwnToneAPI
#include "OwnToneAPI.h"

int led = 47;

// From https://github.com/espressif/arduino-esp32

// WebSockets stuff
WebSocketsServer webSocket = WebSocketsServer(81);

// Json Stuff
StaticJsonDocument<200> doc_to_send;
StaticJsonDocument<200> doc_received;

String mode = "read";
String prev_mode = "";
WebServer server(80);

// RFID stuff
#define SS_PIN 5
#define RST_PIN 27
using namespace ndef_mfrc522;
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

NdefMessage message = NdefMessage();
String mediatype;
String itemid;


void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PC
  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522 card
  l(F("Working..."));
  WiFi.begin(ssid, password);
  l("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  l("");
  Serial.print("Connected to WiFi network with IP Address: ");
  l(String(WiFi.localIP()));

  if (!MDNS.begin(mDNShostname))
  {
    l("Error starting mDNS");
  }
  l("mDNS started");

  server.on("/", handle_root);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  server.handleClient();
  webSocket.loop();
  if (mode != prev_mode)
  {
    String jsonString = "";
    JsonObject object = doc_to_send.to<JsonObject>();
    object["action"] = "mode_change";
    object["mode"] = mode;
    serializeJson(doc_to_send, jsonString);
    Serial.println(jsonString);
    webSocket.broadcastTXT(jsonString);
    prev_mode = mode;
  }

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return;
  if (mode == "read")
  {
    MifareUltralight reader = MifareUltralight(mfrc522);
    NfcTag tag = reader.read();
    tag.print();
    String dataAction = "";

    if (tag.hasNdefMessage()) // every tag won't have a message
    {
      NdefMessage message = tag.getNdefMessage();
      NdefRecord mediatypeRecord = message.getRecord(0);
      NdefRecord itemIdRecord = message.getRecord(1);
      int mediatypePayloadLength = mediatypeRecord.getPayloadLength();
      byte mediatypePayload[mediatypePayloadLength];
      mediatypeRecord.getPayload(mediatypePayload);
      String mediatypePayloadAsString = "";
      for (int c = 0; c < mediatypePayloadLength; c++)
      {
        mediatypePayloadAsString += (char)mediatypePayload[c];
      }
      int itemIdPayloadLength = itemIdRecord.getPayloadLength();
      byte itemIdPayload[itemIdPayloadLength];
      itemIdRecord.getPayload(itemIdPayload);
      String itemIdPayloadAsString = "";
      for (int c = 0; c < itemIdPayloadLength; c++)
      {
        itemIdPayloadAsString += (char)itemIdPayload[c];
      }

      if (WiFi.status() == WL_CONNECTED)
      {
        String getMediatype = mediatypePayloadAsString.substring(1);
        String getItemId = itemIdPayloadAsString.substring(1);

        l("ownClearQueue");
        ownClearQueue();
        // ownChangeOutput();
        l("ownAddItemFromLibrary");
        l(getMediatype);
        l(getItemId);
        ownAddItemFromLibrary(getMediatype, getItemId);
        l("ownPlay");
        ownPlay();
        l("END");
      }
      else
      {
        Serial.println("WiFi Disconnected");
      }
    }
    mfrc522.PICC_HaltA();
  }

  if (mode == "write")
  {
    NdefMessage message = NdefMessage();

    Serial.println(mediatype);
    Serial.println(itemid);
    message.addUriRecord(mediatype);
    message.addUriRecord(itemid);

    MifareUltralight writer = MifareUltralight(mfrc522);
    bool cleaning = writer.clean();
    if (cleaning)
    {
      l(F("Cleaned. "));
    }
    else
    {
      l(F("Cleaned failure?"));
    }
    bool success = writer.write(message);
    if (success)
    {
      Serial.println(F("Success. Now read the tag with your phone."));
      mode = "read";
    }
    else
    {
      lError(F("Failure. Click for restart Music box."));
      // todo TRY AGAIN
    }
    delay(5000); // avoids duplicate scans
  }
}

void l(String msg)
{
  Serial.println(msg);
  webSocketSendMsg(msg, false);
}

void lError(String msg)
{
  Serial.println(msg);
  webSocketSendMsg(msg, true);
}

void webSocketSendMsg(String msg, bool isError)
{
  String jsonString = "";
  JsonObject object = doc_to_send.to<JsonObject>();
  object["action"] = isError ? "msgError" : "msg";
  object["msg"] = msg;
  serializeJson(doc_to_send, jsonString);
  Serial.println(jsonString);
  webSocket.broadcastTXT(jsonString);
}

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_TEXT:
    DeserializationError error = deserializeJson(doc_received, payload);
    if (error)
    {
      Serial.println("Error");
      return;
    }
    else
    {
      const char *action = doc_received["action"];
      if (String(action) == "setmodewrite")
      {
        const char *mediatypechar = doc_received["mediatype"];
        mediatype = String(mediatypechar);
        const char *itemidchar = doc_received["itemid"];
        itemid = String(itemidchar);
        mode = "write";
      }
      if (String(action) == "restart")
      {
        ESP.restart();
      }
    }
    break;
  }
}

// Handle root url (/)
void handle_root()
{
  mode = "read";
  server.send(200, "text/html", indexHTML);
}



