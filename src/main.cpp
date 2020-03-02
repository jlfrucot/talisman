/* Projet Talisman
  *Une petite lib pour gérer un ruban de leds
  *
  * @author 2020 Jean-Louis Frucot <frucot.jeanlouis@free.fr>
  * @see The GNU Public License (GPL)
  *
  * this program is free software you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * You should have received a copy of the GNU General Public License along
  * with this program. If not, see <http://www.gnu.org/licenses/>.
  */

 /*!
  @todo Changer de lib pour le serveur web ???
 */
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Update.h>

#define OUTPUT_SERIAL_PRINT 1 // 1 affiche les debugs, 0 ne les affiche pas

#include "data.h"

/////////////////////////////////////////// WIFI ///////////////////////
// Déclaration d'un client pour le WIFI
const char *SSID = NetworkName;         // Nom du réseau WIFI
const char *PASSWORD = NetworkPassword; // Mot de passe du réseau WIFI
const char *HOST = "esp32-01";
WiFiClient wifiClient;
void setup_wifi();
//Set your Static IP address
IPAddress local_IP(192, 168, 43, 132); // Set your Static IP address
// Set your Gateway IP address
IPAddress gateway(192, 168, 43, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(8888); // Initialise et lance le serveur web utilisé pour OTA

bool redOn();
bool redOff();
bool greenOn();
bool greenOff();
bool yellowOn();
bool yellowOff();
#include "JLF_ota.h" // Pour déporter les "pages html" du server OTA


////////////////////////////////////////////// LEDS NEOPIXELS ///////////
const uint8_t LEDSPIN = 25;
const uint16_t LEDSCOUNT = 60;
uint16_t m_currentLed = 0;
// Ruban *m_ruban;
unsigned char m_color[3] = {255, 255, 255}; // r, g, b Couleur d'affichage des leds
unsigned char m_motif[] = {1, 4, 8, 16, 32, 64, 128};
uint8_t m_nbBoucle = -1;

/////////////////////////////////////////////// Leds tests ////////////////
const uint8_t REDPIN = 25;
const uint8_t GREENPIN = 21;
const uint8_t YELLOWPIN = 19;
void setup()
{

  Serial.begin(9600);
  
  setup_wifi();
  // Service mDNS pour la résolution de nom
  if (!MDNS.begin(HOST))
  {
#if OUTPUT_SERIAL_PRINT
    Serial.println("Error setting up MDNS responder!");
#endif
    while (1)
    {
      delay(1000);
    }
  }
#if OUTPUT_SERIAL_PRINT
  Serial.println("mDNS responder started");
#endif
  //////////////////////////// OTA
  OTAServerInit(); // Initialise et lance le serveur web pour OTA (voir JLF_OTA.h)
  ////////// Tests
  pinMode(REDPIN, OUTPUT);

}

void loop()
{
  server.handleClient(); // Permet l'écoute du serveur http pour OTA

}

void setup_wifi()
{
  Serial.println("SetupWifi");
  // Juste pour moi ip 192.168.1.180 attribuée par le routeur
  delay(10);
  // On essaie de se connecter au WIFI

#if OUTPUT_SERIAL_PRINT
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);
#endif
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
#if OUTPUT_SERIAL_PRINT
    Serial.print(".");
#endif
  }
#if OUTPUT_SERIAL_PRINT
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

bool redOn()
{
  Serial.println("RED ON");
  digitalWrite(REDPIN, HIGH);
  return true;
}

bool redOff()
{
  Serial.println("RED OFF");
  digitalWrite(REDPIN, LOW);
  return true;
}

bool greenOn()
{
  Serial.println("Green ON");
  digitalWrite(GREENPIN, HIGH);
  return true;
}
bool greenOff()
{
  Serial.println("Green OFF");
  digitalWrite(GREENPIN, LOW);
  return true;
}

bool yellowOn()
{
  Serial.println("Yellow ON");
  digitalWrite(YELLOWPIN, HIGH);
  return true;
}
bool yellowOff()
{
  Serial.println("Yellow OFF");
  digitalWrite(YELLOWPIN, LOW);
  return true;
}