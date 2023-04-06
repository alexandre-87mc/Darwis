#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

#define pinBotao1 5  //D1

//WiFi
const char* SSID = "NET_2GA0DC53";                // SSID / wifi network to connect
const char* PASSWORD = "23A0DC53";               // wifi password
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "m12.cloudmqtt.com"; //URL broker MQTT to use server adress
int BROKER_PORT = 12525;                      // door Broker MQTT server door

#define ID_MQTT  "L1B"            //Enter a unique ID and yours. If repeated IDs are used, the last connection will override the previous one. 
#define TOPIC_PUBLISH "casa/L1"    //Enter a single Topic. If duplicate topics are used, the last one will eliminate the previous one.
PubSubClient MQTT(wifiClient);        // Instantiate the MQTT Client by passing the espClient object

//Declaração das Funções
void mantemConexoes();  //Ensures connections to WiFi and MQTT Broker remain active
void conectaWiFi();     //Do wifi connection
void conectaMQTT();     //Do connection Broker MQTT
void enviaPacote();     //send package -- PAYLOAD

void setup() {
  pinMode(pinBotao1, INPUT_PULLUP);         

  Serial.begin(115200);

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   
}

void loop() {
  mantemConexoes(); //evaluates whether the connection has been lost and tries to resume
  enviaValores(); //avoids flooding of messages and unnecessary data sent
  MQTT.loop();  //necessary for constant communication with broker
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //if no connection, try again
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Connect WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}

void enviaValores() {
static bool estadoBotao1 = HIGH;
static bool estadoBotao1Ant = HIGH;
static unsigned long debounceBotao1;

  estadoBotao1 = digitalRead(pinBotao1);
  if (  (millis() - debounceBotao1) > 30 ) {  //Eliminate Bouncing Effect
     if (!estadoBotao1 && estadoBotao1Ant) {

        //Botao Apertado     
        MQTT.publish(TOPIC_PUBLISH, "1");
        Serial.println("Botao1 APERTADO. Payload enviado.");
        
        debounceBotao1 = millis();
     } else if (estadoBotao1 && !estadoBotao1Ant) {

        //Botao Solto
        MQTT.publish(TOPIC_PUBLISH, "0");
        Serial.println("Botao1 SOLTO. Payload enviado.");
        
        debounceBotao1 = millis();
     }
     
  }
  estadoBotao1Ant = estadoBotao1;
}

