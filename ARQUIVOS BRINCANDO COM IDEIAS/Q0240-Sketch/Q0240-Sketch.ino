#include <ESP8266WiFi.h>

const char* ssid = "NET_2GA0DC53";
const char* password = "23A0DC53";

WiFiServer server(80); //Shield will receive requisitions form pages (o padrão WEB é a porta 80)

String HTTP_req; 
String URLValue;  // variables used in sketch

void processaPorta(byte porta, byte posicao, WiFiClient cl);  // functions
void lePortaDigital(byte porta, byte posicao, WiFiClient cl);        
void lePortaAnalogica(byte porta, byte posicao, WiFiClient cl);   
String getURLRequest(String *requisicao);
bool mainPageRequest(String *requisicao);

const byte qtdePinosDigitais = 1; // number of controlled pins
byte pinosDigitais[qtdePinosDigitais] = {0}; // GPIO door
byte modoPinos[qtdePinosDigitais]     = {OUTPUT}; //door state

const byte qtdePinosAnalogicos = 1; //there's only one analogic door in NodeMCU
byte pinosAnalogicos[qtdePinosAnalogicos] = {A0};

void setup()
{         
    Serial.begin(115200); // shows serial IP on monitor, etc

    //Conexão na rede WiFi
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password); // starts connection

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi conectado!");  // show message when connects

    // start web server
    server.begin();
    Serial.println("Server iniciado");

    // show IP adress
    Serial.println(WiFi.localIP());

    //pin mode configuration
    for (int nP=0; nP < qtdePinosDigitais; nP++) {
        pinMode(pinosDigitais[nP], modoPinos[nP]);
    }
}

void loop()
{

  WiFiClient  client = server.available();  // server object with function/method "available" that allows to see if its connected

    if (client) { 
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) { 
                char c = client.read(); 
                HTTP_req += c;  
                
                if (c == '\n' && currentLineIsBlank ) { 

                    if ( mainPageRequest(&HTTP_req) ) {
                        URLValue = getURLRequest(&HTTP_req);
                        Serial.println(HTTP_req);
                                                 
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: keep-alive");              //<------ attention
                        client.println();
                        
                        //Conteudo da Página HTML
                        client.println("<!DOCTYPE html>");
                        client.println("<html>");

                        
                        client.println("<head>");
                        client.println("<title>Arduino via WEB</title>");

                        client.println("<script>");
                        client.println("function LeDadosDoArduino() {");
                        client.println("nocache = \"&nocache=\" + Math.random() * 1000000;");
                        client.println("var request = new XMLHttpRequest();");
                        client.println("var posIni;");
                        client.println("var valPosIni;");
                        client.println("var valPosFim;");
                        client.println("request.onreadystatechange = function() {");
                        client.println("if (this.readyState == 4) {");
                        client.println("if (this.status == 200) {");
                        client.println("if (this.responseText != null) {");

                        for (int nL=0; nL < qtdePinosDigitais; nL++) {                                                    //<-------new
                            client.print("posIni = this.responseText.indexOf(\"PD");
                            client.print(pinosDigitais[nL]);
                            client.println("\");");
                            client.println("if ( posIni > -1) {");
                            client.println("valPosIni = this.responseText.indexOf(\"#\", posIni) + 1;");
                            client.println("valPosFim = this.responseText.indexOf(\"|\", posIni);");
                            client.print("document.getElementById(\"pino");
                            client.print(pinosDigitais[nL]);
                            client.println("\").checked = Number(this.responseText.substring(valPosIni, valPosFim));");
                            client.println("}");
                        }

                        for (int nL=0; nL < qtdePinosAnalogicos; nL++) {                                                    //<-------new
                            
                            client.print("posIni = this.responseText.indexOf(\"PA");
                            client.print(pinosAnalogicos[nL]);
                            client.println("\");"); 
                            client.println("if ( posIni > -1) {");
                            client.println("valPosIni = this.responseText.indexOf(\"#\", posIni) + 1;");
                            client.println("valPosFim = this.responseText.indexOf(\"|\", posIni);");
                            client.print("document.getElementById(\"pino");
                            client.print(pinosAnalogicos[nL]);
                            client.print("\").innerHTML = \"Porta ");
                            client.print(pinosAnalogicos[nL]);
                            client.print(" - Valor: \" + this.responseText.substring(valPosIni, valPosFim);");
                            client.println("}");
                        }
                          
                        client.println("}}}}");
                        client.println("request.open(\"GET\", \"solicitacao_via_ajax\" + nocache, true);");
                        client.println("request.send(null);");
                        client.println("setTimeout('LeDadosDoArduino()', 1000);");
                        client.println("}");
                        client.println("</script>");
                        
                        client.println("</head>");

                        client.println("<body onload=\"LeDadosDoArduino()\">");                      //<------altered                    
                        client.println("<h1>PORTAS EM FUN&Ccedil;&Atilde;O ANAL&Oacute;GICA</h1>");

                        for (int nL=0; nL < qtdePinosAnalogicos; nL++) {

                            client.print("<div id=\"pino");                         //<----- new
                            client.print(pinosAnalogicos[nL]);
                            client.print("\">"); 
                                                         
                            client.print("Porta ");
                            client.print(pinosAnalogicos[nL]);
                            client.println(" - Valor: ");
                               
                            client.print( analogRead(pinosAnalogicos[nL]) );
                            client.println("</div>");                               //<----- new
                               
                            client.println("<br/>");                             
                        }
                        
                        client.println("<br/>");                        
                        client.println("<h1>PORTAS EM FUN&Ccedil;&Atilde;O DIGITAL</h1>");
                        client.println("<form method=\"get\">");

                        for (int nL=0; nL < qtdePinosDigitais; nL++) {
                            processaPorta(pinosDigitais[nL], nL, client);
                            client.println("<br/>");
                        }
                        
                        client.println("<br/>");
                        client.println("<button type=\"submit\">Envia para o ESP8266</button>");
                        client.println("</form>");                      
                        
                        client.println("</body>");
                        
                        client.println("</html>");

                    
                    } else if (HTTP_req.indexOf("solicitacao_via_ajax") > -1) {     //<----- new

                        Serial.println(HTTP_req);

                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: keep-alive");      
                        client.println();                      

                        for (int nL=0; nL < qtdePinosAnalogicos; nL++) {
                            lePortaAnalogica(pinosAnalogicos[nL], nL, client);                            
                        }
                        for (int nL=0; nL < qtdePinosDigitais; nL++) {
                            lePortaDigital(pinosDigitais[nL], nL, client);
                        }
                            
                    } else {

                        Serial.println(HTTP_req);
                        client.println("HTTP/1.1 200 OK");
                    }
                    HTTP_req = "";    
                    break;
                }
                
                if (c == '\n') {
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    currentLineIsBlank = false;
                }
            }
        } 
        delay(1);     
        client.stop(); 
    } 
}


void processaPorta(byte porta, byte posicao, WiFiClient cl)
{
static boolean LED_status = 0;
String cHTML;

    cHTML = "P";
    cHTML += porta;
    cHTML += "=";
    cHTML += porta;

    if (modoPinos[posicao] == OUTPUT) { 
        
        if (URLValue.indexOf(cHTML) > -1) { 
           LED_status = HIGH;
        } else {
           LED_status = LOW;
        }
        digitalWrite(porta, LED_status);
    } else {

        LED_status = digitalRead(porta);
    }
        
    cl.print("<input type=\"checkbox\" name=\"P");
    cl.print(porta);
    cl.print("\" value=\"");
    cl.print(porta);
    
    cl.print("\"");

    cl.print(" id=\"pino");           //<------new
    cl.print(porta);
    cl.print("\"");
    
    if (LED_status) { 
        cl.print(" checked ");
    }

    if (modoPinos[posicao] != OUTPUT) { 
        cl.print(" disabled ");
    }
    
    cl.print(">Porta ");
    cl.print(porta);

    cl.println();
}

void lePortaDigital(byte porta, byte posicao, WiFiClient cl)
{
    if (modoPinos[posicao] != OUTPUT) { 
       cl.print("PD");
       cl.print(porta);
       cl.print("#");
       
       if (digitalRead(porta)) {
          cl.print("1");
       } else {
          cl.print("0");
       }
       cl.println("|");
    }
}


void lePortaAnalogica(byte porta, byte posicao, WiFiClient cl)
{
   cl.print("PA");
   cl.print(porta);
   cl.print("#");
   
   cl.print(analogRead(porta));

   //format door A0 value
   if (porta == A0) { 
      cl.print(" (");
      cl.print(map(analogRead(A0),0,1023,0,179)); 
      cl.print("&deg;)");
   }
   
   cl.println("|");   
}


String getURLRequest(String *requisicao) {
int inicio, fim;
String retorno;

  inicio = requisicao->indexOf("GET") + 3;
  fim = requisicao->indexOf("HTTP/") - 1;
  retorno = requisicao->substring(inicio, fim);
  retorno.trim();

  return retorno;
}

bool mainPageRequest(String *requisicao) {
String valor;
bool retorno = false;

  valor = getURLRequest(requisicao);
  valor.toLowerCase();

  if (valor == "/") {
     retorno = true;
  }

  if (valor.substring(0,2) == "/?") {
     retorno = true;
  }  

  if (valor.substring(0,10) == "/index.htm") {
     retorno = true;
  }  

  return retorno;
}
