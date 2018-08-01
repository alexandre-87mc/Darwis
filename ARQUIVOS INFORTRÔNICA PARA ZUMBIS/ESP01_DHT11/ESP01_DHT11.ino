////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Arquivo:   ESP01_DHT11.ino
//Tipo:      Codigo-fonte para utilizar no ESP8266 atraves da IDE do Arduino
//Autor:     Marco Rabelo para o canal Infortronica Para Zumbis (www.youtube.com/c/InfortronicaParaZumbis)
//Descricao: Curso de ESP8266 - Lendo dados de sensores de umidade e temperatura (DHT11 e DHT22), postando
//           estas leituras na internet e recuperando de forma grafica no celular ou PC.
//Video:     https://youtu.be/uzcL80nT5cU
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
Equivalencia das saidas Digitais entre NodeMCU e ESP8266 (na IDE do Arduino)
NodeMCU - ESP8266
D0 = 16;
D1 = 5;
D2 = 4;
D3 = 0;
D4 = 2;
D5 = 14;
D6 = 12;
D7 = 13;
D8 = 15;
D9 = 3;
D10 = 1;
*/

// Importando as bibliotecas necessarias
#include "ESP8266WiFi.h"
#include "DHT.h"

//Nome da sua rede Wifi
const char* ssid = "Nome da sua rede wifi";

//Senha da rede
const char* password = "senha da sua rede wifi";

// Pino do DHT
#define DHTPIN 2

// Definindo o sensor DHT11
#define DHTTYPE DHT11

// Inicializando o sensor DHT
DHT dht(DHTPIN, DHTTYPE, 15);

// Site que receber� os dados
const char* host = "dweet.io";

void setup() {
  // Iniciando o DHT 
  dht.begin();

  // Conectando na rede wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void loop() {
  // Criando uma conexao TCP
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    return;
  }
  
  // Lendo a umidade
  int u = dht.readHumidity();

  //Verificando se houve falha na leitura
  if(u == 2147483647){
    delay(10);
    return;
  }
  
  // Lendo a temperatura em graus Celsius
  int t = dht.readTemperature();

  // Calculando o indice de calor
  float f = dht.readTemperature(true);
  float ic = dht.computeHeatIndex(f, u);
  float icC = dht.convertFtoC(ic);
  
  // Enviando para o dweet as leituras efetuadas
  client.print(String("GET /dweet/for/nome_do_seu_modulo?temperatura=") + String(t) +
                "&umidade=" + String(u) + "&indice=" + String(icC) +
                " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  
  // Repetindo a cada 6 segundos
  delay(6000);
}
