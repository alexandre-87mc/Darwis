#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

#define pinLED1 2  //GP2 ESP-01

//WiFi
const char* SSID = "NET_2G517AFC";                // SSID / WiFi network name you want to connect
const char* PASSWORD = "F9517AFC";   // Password of the WiFi network you want to connect to
WiFiClient wifiClient;                        
 
//MQTT Server
const char* BROKER_MQTT = "iot.eclipse.org"; //URL of the MQTT broker you want to use
int BROKER_PORT = 1883;                      // Porta do Broker MQTT

#define ID_MQTT "BCI02"             //Enter a unique ID and yours. If repeated IDs are used, the last connection will override the previous one.
#define TOPIC_SUBSCRIBE "BCIBotao1" //Inform a single Topic. If duplicate topics are used, the last one will eliminate the previous one.
PubSubClient MQTT(wifiClient);      // Instantiate the MQTT Client by passing the espClient object

//Declaração das Funções
void mantemConexoes();  //Ensures connections to WiFi and MQTT Broker remain active
void conectaWiFi();     //Makes WiFi connection
void conectaMQTT();     //Makes Broker MQTT connection
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(pinLED1, OUTPUT);         

  Serial.begin(115200);

  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);  
  MQTT.setCallback(recebePacote); 
}

void loop() {
  mantemConexoes();
  MQTT.loop();
}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //if there is no connection to WiFI, the connection is redone
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
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
            MQTT.subscribe(TOPIC_SUBSCRIBE);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}

void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    String msg;

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    if (msg == "0") {
       digitalWrite(pinLED1, LOW);
    }

    if (msg == "1") {
       digitalWrite(pinLED1, HIGH);
    }
}
