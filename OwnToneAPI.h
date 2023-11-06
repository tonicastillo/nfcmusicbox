#include "config.h"

#ifndef OwnToneAPI_h
#define OwnToneAPI_h
// This one for make API Requests
#include <HTTPClient.h>

// OwnTone stuff
String owntoneapi = OWNTONE_API_URL;
const char *owntoneusername = OWNTONE_API_USERNAME;
const char *owntonepassword = OWNTONE_API_PASSWORD;
const char *owntonespeaker = OWNTONE_API_SPEAKER;
HTTPClient http;
void ownClearQueue()
{
  http.begin(owntoneapi + "/api/queue/clear");
  
  http.setAuthorization(owntoneusername, owntonepassword);
  int statusCode = http.PUT("PUT sent from ESP32");
  if (statusCode > 0)
  {
    // l("ownClearQueue\n");
    // l("Able to send HTTP request out of the board.");

    if (statusCode == HTTP_CODE_OK)
    {
      // l("Server responded with HTTP status 200.");
    }
    else
    
    {
      //Serial.printf("%d /api/queue/clear", owntoneapi);
      Serial.printf("Got HTTP status: %d", statusCode);
    }
  }
  else
  {
    //Serial.printf("%d /api/queue/clear", owntoneapi);
    Serial.printf("Error occurred while sending HTTP Get: %s\n", http.errorToString(statusCode).c_str());
  }
  // l("\n ownClearQueue end \n");
  http.end();
}

void ownPlay()
{
  http.begin(owntoneapi + "/api/player/play");
  http.setAuthorization(owntoneusername, owntonepassword);
  int statusCode = http.PUT("PUT sent from ESP32");
  if (statusCode > 0)
  {
    // l("Able to send HTTP request out of the board.");

    if (statusCode == HTTP_CODE_OK)
    {
      // l("Server responded with HTTP status 200.");
    }
    else
    {
      //Serial.printf("Got HTTP status: %d", statusCode);
    }
  }
  else
  {
    //Serial.printf("Error occurred while sending HTTP Get: %s\n", http.errorToString(statusCode).c_str());
  }
  // l(" \n ownPlay end \n");
  http.end();
}

void ownAddItemFromLibrary(String typeOfItem, String itemToAdd)
{
  http.begin(owntoneapi + "/api/queue/items/add?uris=library:" + typeOfItem + ":" + itemToAdd);
  http.setAuthorization(owntoneusername, owntonepassword);
  int statusCode = http.POST("POST sent from ESP32");
  if (statusCode > 0)
  {
    // l("ownAddItemFromLibrary\n");
    // l("Able to send HTTP request out of the board.\n");

    if (statusCode == HTTP_CODE_OK)
    {
      // l("Server responded with HTTP status 200.\n");
    }
    else
    {
      //Serial.printf("Got HTTP status: %d", statusCode);
    }
  }
  else
  {
    //Serial.printf("Error occurred while sending HTTP Get: %s\n", http.errorToString(statusCode).c_str());
  }
  // l(" \n ownAddItemFromLibrary end \n ");
  http.end();
}

void ownDisableShuffle(){

     http.begin(owntoneapi+"/api/player/shuffle?state=false");
      http.setAuthorization(owntoneusername, owntonepassword);
      http.addHeader("Content-Type", "application/json");

    int statusCode = http.PUT("PUT sent from ESP32");
    if (statusCode > 0) {
      Serial.printf("ownDisableShuffle\n");
      ////Serial.printf("Able to send HTTP request out of the board.");

      if(statusCode == HTTP_CODE_OK) {
        Serial.printf("Server responded with HTTP status 200.");

      }
      else {
        Serial.printf("Got HTTP status: %d", statusCode);

      }
    }
    else {
      Serial.printf("Error occurred while sending HTTP Get: %s\n", http.errorToString(statusCode).c_str());
    }
    ////Serial.printf("\n ownDisableShuffle end \n");
    http.end();

}

void ownChangeOutput(){
   //const String speaker="175525119465970";

     http.begin(owntoneapi+"/api/outputs/"+owntonespeaker);
      http.setAuthorization(owntoneusername, owntonepassword);
      http.addHeader("Content-Type", "application/json");

    int statusCode = http.PUT("{\"selected\":true,\"volume\":100}");
    if (statusCode > 0) {
      //Serial.printf("ownChangeOutput\n");
     // //Serial.printf("Able to send HTTP request out of the board.");

      if(statusCode == HTTP_CODE_OK) {
       // //Serial.printf("Server responded with HTTP status 200.");

      }
      else {
        ////Serial.printf("Got HTTP status: %d", statusCode);

      }
    }
    else {
      //Serial.printf("Error occurred while sending HTTP Get: %s\n", http.errorToString(statusCode).c_str());
    }
    ////Serial.printf("\n ownChangeOutput end \n");
    http.end();

}

#endif