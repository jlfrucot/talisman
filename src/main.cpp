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
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WebServer.h>
#include <Update.h>

#define OUTPUT_SERIAL_PRINT 1 // 1 affiche les debugs, 0 ne les affiche pas

#include "data.h"
#include "ruban.h"

volatile bool interruptCounter;

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

/////////////////////////////////////////// Timer //////////////////////
#if OUTPUT_SERIAL_PRINT
unsigned long totalInterruptCounter;
#endif
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
// Fonction de callback pour le timer
void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

////////////////////////////////////////////// LEDS NEOPIXELS ///////////
const uint8_t LEDSPIN = 19;
const uint16_t LEDSCOUNT = 60;
uint16_t m_currentLed = 0;
Ruban *m_ruban;
unsigned char m_color[3] = {255, 255, 255}; // r, g, b Couleur d'affichage des leds
unsigned char m_motif[] = {1, 4, 8, 16, 32, 64, 128};
uint8_t m_nbBoucle = -1;

/////////////////////////////////////////////// Leds tests ////////////////
const uint8_t REDPIN = 22;
const uint8_t GREENPIN = 21;
const uint8_t YELLOWPIN = 19;
void setup()
{

  Serial.begin(9600);
  // Déclaration et initialisation du timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 2500000, true); // 25 ms
  timerAlarmEnable(timer);
  // Initialisation du ruban
  m_ruban = new Ruban(LEDSCOUNT, LEDSPIN);
  m_ruban->setColor(m_color[0], m_color[1], m_color[2]);
  m_ruban->setMotif(m_motif);

  // Déclaration d'un client pour le WIFI
  // WiFiClient wifiClient;
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
  pinMode(GREENPIN, OUTPUT);
  pinMode(YELLOWPIN, OUTPUT);
}

void loop()
{
  server.handleClient(); // Permet l'écoute du serveur http pour OTA
  // WiFiClient client = server.available();
  // if (client) {

  //   if (client.connected()) {
  //     Serial.println("Connected to client");
  //   }

  // // close the connection:
  // client.stop();
  // }
  if (interruptCounter)
  {
    // C'est ici que l'on demande au ruban d'avancer d'un pas
    portENTER_CRITICAL(&timerMux);
    interruptCounter = false;
    portEXIT_CRITICAL(&timerMux);

    ////////// TEst
    // Serial.println("digitalWrite");
    // digitalWrite(REDPIN, HIGH);
#if OUTPUT_SERIAL_PRINT
    totalInterruptCounter++; //
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
#endif
    //On gère le ruban m_stripe
    //     if (m_ruban->isStopped() == false) // on ne fait rien si le ruban est arrêté
    //     {
    //       if (m_nbBoucle == -1) // nombre de boucles infini
    //       {
    //         m_currentLed = m_ruban->nextStep();
    // #if OUTPUT_SERIAL_PRINT
    //         Serial.println("m_ruban : boucle infinie");
    // #endif
    //       }
    //       else
    //       {
    //         if (m_nbBoucle > 0) // Il reste des boucles à faire
    //         {
    //           m_currentLed = m_ruban->nextStep();
    // #if OUTPUT_SERIAL_PRINT
    //           Serial.print("Boucle n°");
    //           Serial.print(m_nbBoucle);
    //           Serial.print("   Led n°");
    //           Serial.println(m_currentLed);
    // #endif
    //           if (m_currentLed - LEDSCOUNT == 0) // A-t-on fini une boucle ?
    //           {
    //             m_nbBoucle--; // boucle suivante
    //           }
    //         }
    //         else // C'est la dernière boucle
    //         {
    //           if (m_currentLed == LEDSCOUNT) //On est arrivé à la fin du ruban
    //           {
    //             m_currentLed = m_ruban->stop();
    // #if OUTPUT_SERIAL_PRINT
    //             Serial.print("Plus de boucle");
    //             Serial.print("     Arrêté à la Led n°");
    //             Serial.println(m_currentLed);
    //             Serial.print("m_stripe stoppé ");
    //             Serial.println(m_ruban->isStopped());
    // #endif
    //           }
    //         }
    //       }
    //     }
  }
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