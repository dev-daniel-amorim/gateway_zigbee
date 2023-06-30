/* **********************************************************************************
  GATEWAY ZIGBEE:
  O GATEWAY TEM OBJETIVO DE CONCENTRAR UMA REDE MESH ZIGBEE ATRAVÉS UM XBEE COORDENADOR
  LIGADO A UM ESP32 QUE INTERPRETA OS PACOTES DE ENVIO (TX) E RECEBIMENTO (RX) E TRANSMITE
  AS INFORMAÇÕES VIA ETHERNET/WIFI PARA UM BROKER MQTT.
  DESENVOLVEDOR:
  DANIEL S AMORIM      
*************************************************************************************/
#include <Arduino.h>
#include <bibliotecas.h>
#include "pinout.h"
#include "objetos.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//DEFINE OS PINOS TX RX DO ESP PARA COMUNICAÇÃO SERIAL
SoftwareSerial mySerial (PIN_RX, PIN_TX);

//DEFININDO AS HANDLES
TaskHandle_t handleTaskLedGateway = NULL;
TaskHandle_t handleTaskLedXbee = NULL;
TaskHandle_t handleTaskResetWifi = NULL;
TaskHandle_t handleTaskDht = NULL;
TaskHandle_t handleTaskTxRxXbee = NULL;

void TaskLedGateway(void * param);
void TaskLedXbee(void * param);
void TaskResetWifi(void * param);
void TaskDht(void * param);
void TaskTxRxXbee(void * param);

/* **********************************************************************************
                                 FUNÇÕES                                    
*************************************************************************************/

//==================PROCESSA AS MENSAGENS RECEBIDAS PELO BROKER
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("==============================");
  Serial.print("Nova mensagem, topico: ");
  Serial.println(topic);
  pay = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    pay += (char)payload[i];
  }
  Serial.println();
}

//=================RECONECTA AO BROKER EM CASO DE DESCONEXÃO E SUBSCRIBE AOS TOPICOS
void reconnect() {
  //Se desconectado desliga led da porta 19
  digitalWrite(Led_WF_conectado, LOW);

  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    digitalWrite(Led_WF_conectado,   LOW);
    //conecta (abaixo modo se quiser usar usuario e senha)
    //if (client.connect("ESP32Client", mqttUsername, mqttPassword))
    if (client.connect("DSA_teste", mqttUsername, mqttPassword)) {
      Serial.println("Conectado!");
      digitalWrite(Led_WF_conectado,   HIGH);
      //Assina tópicos
      client.subscribe("DSA/coord/temp");
      client.subscribe("DSA/coord/hum");
      client.subscribe("DSA/coord/led");
      client.subscribe("DSA/coord/rst");
      client.subscribe("DSA/R1/temp");
      client.subscribe("DSA/R1/led");
      client.subscribe("DSA/R2/porta");

    } else {
      Serial.print("Falha ao connectar o MQTT, rc=");
      Serial.print(client.state());
      Serial.println("Retrying in 1 seconds...");
      delay(1000);
    }
    
  }
}

//=================FUNÇÃO PARA PISCAR ALGUM LED DE AVISO
void pisca_led(int Led, int valor) {
  for (int i = 0; i < valor; i++){
    digitalWrite(Led, HIGH); // Desliga o LED
    delay(100);
    digitalWrite(Led, LOW); // Desliga o LED
    delay(100);
  }
  return;
}

/* **********************************************************************************
                                 VOID SETUP                                     
*************************************************************************************/

void setup() {
  //INICIA AS SERIAIS
  Serial.begin(115200);
  mySerial.begin(115200);

  //DEFINIÇÃO DOS PINOS
  pinMode(Led_WF_conectado,   OUTPUT);
  pinMode(Btn_01, INPUT);//SEM USO
  
  //SE DESCOMENTADO APAGA AS CREDENCIAIS SALVAS PARA O PONTO DE ACESSO
  //wifiManager.resetSettings (); 

  //INICIA O WIFI, SE CREDENCIAIS SALVAS ENTRA EM MODO WIFI_STA (WiFi.mode(WIFI_STA))
  Serial.println("=============================="); 
  Serial.println("Verificando conexão WIFI...");
  WiFi.begin();
  vTaskDelay(3000);

  //SE NAO CONECTADO CRIA O AP PROPRIO , SE CONECTADO INICIA A CONEXÃO COM ROTEADOR
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_AP); //ENTRA EM MODO AP PARA PASSARMOS AS CREDENCIAIS WIFI
    Serial.println("==============================");
    Serial.println("Conecte ao AP e escolha a WIFI ");
    wifiManager.autoConnect(AP_SSID, AP_PASS);
  }else{
    Serial.println("==============================");
    Serial.println("Conectado ao WiFi escolhido");
    Serial.println(WiFi.localIP());

    //Se conectado liga led da porta 19
    digitalWrite(Led_WF_conectado,   HIGH);
    vTaskDelay(500);
    //server.begin();
  }

  //WiFi.begin();

  //DEFINE O BROKER E A PORTA
  client.setServer(mqttServer, mqttPort);

  //FUNÇÃO DE RECEBIMENTO DAS MENSAGENS PLUBLISH DO BROKER
  client.setCallback(callback);

  
  //========================= TASKS EXECUTANDO NO NÚCLEO 0
  //TASK TX RX XBEE
  xTaskCreatePinnedToCore(TaskTxRxXbee, "TaskTxRxXbee", configMINIMAL_STACK_SIZE+1024*30, NULL, 10, &handleTaskTxRxXbee, APP_CPU_NUM);
  
  //========================= TASKS EXECUTANDO NO NÚCLEO 1
  //TASK LED GATEWAY
  xTaskCreatePinnedToCore(TaskLedGateway, "TaskLedGateway", configMINIMAL_STACK_SIZE+200, NULL, 4, &handleTaskLedGateway, PRO_CPU_NUM);
  //TASK LED XBEE
  xTaskCreatePinnedToCore(TaskLedXbee, "TaskLedXbee", 1500, NULL, 3, &handleTaskLedXbee, PRO_CPU_NUM);
  //TASK RESETA CREDENCIAIS WIFI
  xTaskCreatePinnedToCore(TaskResetWifi, "TaskResetWifi", 1024, NULL, 1, &handleTaskResetWifi, PRO_CPU_NUM);
  //TASK ENVIA DADOS SENSOR DHT
  xTaskCreatePinnedToCore(TaskDht, "TaskDht", 1024*15, NULL, 2, &handleTaskDht, PRO_CPU_NUM);
}


/* **********************************************************************************
                         INICIALIZAÇÃO DAS TASKS                                     
*************************************************************************************/
//========================ACENDE LED NO GATEWAY
void TaskLedGateway(void * param){

  pinMode(Led_green,   OUTPUT);
  while(1){

    if (pay == "cliga") {
      digitalWrite(Led_green, HIGH); // Liga o LED
      pay = "";// EVITA ENTRAR NOVAMENTE NO IF JA QUE ESTAMOS NUM LOOP DA TASK
    }
    if (pay == "cdesliga") {
      digitalWrite(Led_green, LOW); // Desliga o LED
      pay = "";
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

//========================ACENDE LED NO XBEE ROUTER
void TaskLedXbee(void * param){
  


  while(1){
    //ENVIA PACOTES XBEE
    if (pay == "xbeeliga") {
      mySerial.write(D1HIGH,20);
      pay = "";// EVITA ENTRAR NOVAMENTE NO IF JA QUE ESTAMOS NUM LOOP DA TASK
    }
    if (pay == "xbeedesliga") {
      mySerial.write(D1LOW,20);
      pay = "";
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

//=======================RESETA O WIFI POR COMANDO MQTT OU BOTÃO
void TaskResetWifi(void * param){
  //BOTÃO SEM RESISTOR FISICO NO GND ENTAO USA-SE RESISTOR PULLDOWN
  pinMode(Btn_02, INPUT_PULLDOWN);
  while(1){
    if (digitalRead(Btn_02) == HIGH || pay == "hardreset") {
      //pisca_led(Led_green, 4);
      Serial.println("************************************");
      Serial.println("ATENÇAO: WIFI RESETADO, RECONFIGURE!");
      Serial.println("************************************");
      vTaskDelay(3000);
      wifiManager.resetSettings ();   // Apaga todas redes Salvas do Wifi
      pay = "";
      setup();
    }
    yield();
    vTaskDelay(pdMS_TO_TICKS(3500));
  }
}

//=======================FAZ A LEITURA DO DHT11
void TaskDht(void * param){
  //CONFIGURAÇÕES DO DHT11
  pinMode(DHT_PIN, INPUT);
  dht.begin();

  while(1){

    float h = dht.readHumidity();
    // Read temperature as Celsius
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Falha ao ler DHT sensor!");
    }else{
      message = h;
      client.publish(topico_coordHum, message.c_str());
      message = t;
      client.publish(topico_coordTemp, message.c_str());
    }

    yield();
    vTaskDelay(pdMS_TO_TICKS(10000)); //  EXECUTA A TASK A CADA 10 SEG
  }
}

//=======================FAZ A LEITURA DOS PACOTES XBEE NA SERIAL
void TaskTxRxXbee(void * param){

  //HIGH WATER MARK PARA MEDIÇÃO DE CONSUMO DE MEMORIA PELA TASK
  // UBaseType_t uxHighWaterMark; 

  while(1){

    //LER O PACOTE XBEE E ENVIA A LEITURA DO ANALÓGICO
    if (mySerial.available() > 15) {
      indice=0;
      if (mySerial.read() == 0x7E) {          //Detectar byte de inicio do pacote
        while (mySerial.available()) {        //Enquanto pacote disponivel
          dados[indice] = (mySerial.read());  //Armazenas os bytes recebidos na lista DADOS
          Serial.print(dados[indice], HEX);
          Serial.print(", ");
          indice++;
        }
        Serial.println();

        lenPacote = indice;
        iniDadosA = lenPacote -3;
        fimDadosA = lenPacote -2;
        Serial.print("Tamanho do pacote: ");
        Serial.println(lenPacote);

        dadosLidos = (dados[iniDadosA] << 8) | dados[fimDadosA];
        int cont = 0;
        for(int i = 7; i < 11; i++){
          endMac[cont] = dados[i];
          cont++;
        }
        //Verifica o MAC de quem enviou o pacote
        bool xbee1 = true;
        for (int i = 0; i < 4; i++) {
          if (mac1[i] != endMac[i]) {
              xbee1 = false;
              break;        
          }
        }
        //Verifica o MAC de quem enviou o pacote
        bool xbee2 = true;
        for (int i = 0; i < 4; i++) {
          if (mac2[i] != endMac[i]) {
              xbee2 = false;
              break;        
          }
        }

        if (xbee1){
          message = dadosLidos;
          client.publish(topico_r1Temp, message.c_str());
        }
        if (xbee2){
          if (dadosLidos == 1){
            std::string message = "r2close";
            client.publish(topico_r2Porta, message.c_str());         
          }
          else{
            std::string message = "r2open";
            client.publish(topico_r2Porta, message.c_str()); 
          }
        }
      }
    }
    //IMPRIME NA SERIAL A MEMORIA UTILIZADA PELA TASK EM BYTES
    // uxHighWaterMark= uxTaskGetStackHighWaterMark(NULL);
    // Serial.print("Memory Task Tx Rx: ");
    // Serial.print(uxHighWaterMark);
    // Serial.println(" Bytes");

    yield(); // chamada para reinicializar o temporizador de vigilância e evitar timeout
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

/* **********************************************************************************
                                  VOID LOOP                                    
*************************************************************************************/
void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  yield(); // chamada para reinicializar o temporizador de vigilância e evitar timeout
  //vTaskDelay(pdMS_TO_TICKS(10));
}
