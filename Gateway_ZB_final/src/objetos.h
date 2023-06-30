#include "bibliotecas.h"

// Configuraçoes e Endereço de Internet WIFI
WiFiServer server (8090);
//Variável para armazenar a solicitação HTTP
//String header;
// Hora Atual
//unsigned long currentTime = millis();
// Vez anterior
//unsigned long previousTime = 0; 
//Defina o tempo limite em milissegundos 
//const long timeoutTime = 1000;

//CREDENCIAIS DO AP CRIADO PELO ESP
#define AP_SSID "DSA_GATEWAY"
#define AP_PASS "12345678"


// INSTANCIA DO WIFIMANAGER
ESP_WiFiManager wifiManager;


// Status para controle de Led
//int Status_Led = LOW; 

//CONFIGURAÇÕES DO BROKER
//const char* ssid = "DanielSa";
//const char* password = "244466666";
const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883; //1883
const char* mqttUsername = "";
const char* mqttPassword = "";

//topicos
char topico_coordTemp[] = "DSA/coord/temp";
char topico_coordHum[] =  "DSA/coord/hum";
char topico_coordLed[] =  "DSA/coord/led";
char topico_coordRst[] =  "DSA/coord/rst";

char topico_r1Temp[] =    "DSA/R1/temp";
char topico_r1Led[] =     "DSA/R1/led";

char topico_r2Porta[] =   "DSA/R2/porta";

//CONFIGURAÇÕES DHT11
const int DHT_PIN = PIN_DHT;
const int DHT_TYPE = DHT11;
DHT dht(DHT_PIN, DHT_TYPE);

//CLIENT
WiFiClient espClient;
PubSubClient client(espClient);

//VARIAVEL QUE RECEBE O COMANDO BROKER PAYLOAD
String pay;
//MENSAGEM DE COMANDO PARA O BROKER
String message;


// dados xbee
byte dados[50]; // Array para armazenar os dados recebidos
int indice = 0; // Índice para controlar a posição no array
int lenPacote;
int iniDadosA;  // Inicio do byte de dados
int fimDadosA;  // Fim do byte de dados
int dadosLidos;

//endereções dos xbees usados (endMAC para capturar MAC do pacote API)
byte endMac[4];                           //Lista para receber os MSB do MAC xbee
byte mac1[4] = {0x41, 0x49, 0x6C, 0x66};  //Xbee 01 MAC
byte mac2[4] = {0x41, 0x49, 0x6C, 0xA0};  //Vbee 02 MAC

//PACOTES XBEE
byte D1LOW[] = {0x7E,0x00,0x10,0x17,0x01,0x00,0x13,0xA2,0x00,0x41,0x49,0x6C,0x66,0xFF,0xFE,0x02,0x44,0x31,0x04,0x5E};
byte D1HIGH[] = {0x7E,0x00,0x10,0x17,0x01,0x00,0x13,0xA2,0x00,0x41,0x49,0x6C,0x66,0xFF,0xFE,0x02,0x44,0x31,0x05,0x5D};