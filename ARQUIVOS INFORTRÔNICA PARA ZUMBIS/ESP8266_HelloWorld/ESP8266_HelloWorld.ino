////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Arquivo:   ESP8266_HelloWorld.ino
//Tipo:      Codigo-fonte para utilizar no ESP8266 atraves da IDE do Arduino
//Autor:     Marco Rabelo para o canal Infortronica Para Zumbis (www.youtube.com/c/InfortronicaParaZumbis)
//Descricao: Inicio do curso de ESP8266 - Hello World - Criacao de um web server para controle de um
//           LED atraves de qualquer browser.
//Video:     https://youtu.be/Xcg8fAQ49Uw
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

#include <ESP8266WiFi.h>

//Nome da sua rede Wifi
const char* ssid = "nome_da_sua_rede_wifi";

//Senha da rede
const char* password = "senha_da_sua_rede_wifi";

//IP do ESP (para voce acessar pelo browser)
IPAddress ip(192, 168, 43, 50);

//IP do roteador da sua rede wifi
IPAddress gateway(192, 168, 43, 1);

//Mascara de rede da sua rede wifi
IPAddress subnet(255, 255, 255, 0);

//Criando o servidor web na porta 80
WiFiServer server(80);

//Funcao que sera executada apenas ao ligar o ESP8266
void setup() {
  //Preparando o GPIO2, que esta lidago ao LED
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);

  //Conectando a� rede Wifi
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  //Verificando se esta conectado,
  //caso contrario, espera um pouco e verifica de novo.
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  //Iniciando o servidor Web
  server.begin();
}

//Funcao que sera executada indefinidamente enquanto o ESP8266 estiver ligado.
void loop() {
  //Verificando se o servidor esta pronto.
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  //Verificando se o servidor recebeu alguma requisicao
  while (!client.available()) {
    delay(1);
  }

  //Obtendo a requisicao vinda do browser
  String req = client.readStringUntil('\r');
  client.flush();

  //Iniciando o buffer que ira conter a pagina HTML que sera enviada para o browser.
  String buf = "";

  buf += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  buf += "<h3>ESP8266 Servidor Web</h3>";
  buf += "<p>LED <a href=\"?function=led2_on\"><button>LIGA</button></a><a href=\"?function=led2_off\"><button>DESLIGA</button></a></p>";
  buf += "<h4>Infortronica Para Zumbis</h4>";
  buf += "</html>\n";

  //Enviando para o browser a 'pagina' criada.
  client.print(buf);
  client.flush();

  //Analisando a requisicao recebida para decidir se liga ou desliga o LED
  if (req.indexOf("led2_on") != -1)
  {
    digitalWrite(2, 0);
  }
  else if (req.indexOf("led2_off") != -1)
  {
    digitalWrite(2, 1);
  }
  else
  {
    //Requisicao invalida!
    client.stop();
  }
}
