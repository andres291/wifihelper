
#include "WiFi.h"
#include "WiFiHelper.h"
#include <ESPmDNS.h>


String Hostname;
bool myWiFiFirstConnect = true;
long previousMillis = 0;
long interval = 30000;

WiFiHelper::WiFiHelper(String hostname){
    Hostname = hostname;
}

void WiFiHelper::tryToConnect(){
    wl_status_t state;

    state = WiFi.status();
    if (state != WL_CONNECTED) {  // We have no connection
      if (state == WL_NO_SHIELD) {  // WiFi.begin wasn't called yet
        Serial.println("Connecting WiFi");

        WiFi.mode(WIFI_STA);
        WiFi.begin();
        delay(150);
        WiFi.setHostname(Hostname.c_str());

      } else if (state == WL_CONNECT_FAILED) {  // WiFi.begin has failed (AUTH_FAIL)
        Serial.println("Disconnecting WiFi");

        WiFi.disconnect(true);

      } else if (state == WL_DISCONNECTED) // WiFi.disconnect was done or Router.WiFi got out of range
      {
        if (!myWiFiFirstConnect) // Report only once
        {
          myWiFiFirstConnect = true;
          Serial.println("WiFi disconnected");
        }
        Serial.println("No Connection -> Wifi Reset");
        WiFi.persistent(false);
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_STA);
        WiFi.begin();
        delay(150);
        WiFi.setHostname(Hostname.c_str());
        delay(3000);	// Wait 3 Seconds, WL_DISCONNECTED is present until new connect!
      }
      vTaskDelay(250); // Check again in about 250ms

    } else { // We have connection
      if (myWiFiFirstConnect) {  // Report only once
        myWiFiFirstConnect = false;

        Serial.println("Connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.println("");

        MDNS.end();
        if(!MDNS.begin(Hostname.c_str())) {
          Serial.println("Error starting mDNS");
          return;
        }
        MDNS.addService("_http", "_tcp", 80);
        Serial.println("MDNS.addService('_http', '_tcp', 80)");
      }

      vTaskDelay (5000); // Check again in about 5s
    }
}

void WiFiHelper::firstConnect(){
    int count = 0;
    while (WiFi.status() != WL_CONNECTED && count < 10)
    {
        tryToConnect();
        count++;
        delay(6000);
    }
}
