#include <WiFi.h>
#include <PubSubClient.h>

// --- Configurações de Hardware ---
const int PIN_TRIG   = 5;   // Pino de disparo do sensor ultrassônico
const int PIN_ECHO   = 18;  // Pino de retorno do eco ultrassônico
const int PIN_RELAY  = 19;  // Pino de controle do Módulo Relé
const int PIN_BUZZER = 21;  // Pino de controle do Buzzer Ativo

// --- Configurações de Rede e MQTT ---
const char* ssid        = "Wokwi-GUEST";       // SSID padrão do simulador Wokwi
const char* password    = "";                  // Senha (vazia no Wokwi)
const char* mqtt_server = "broker.hivemq.com"; // Endereço do Broker público HiveMQ
const char* topic_pub   = "smartcity/monitoramento/alagamento"; // Tópico definido no artigo

WiFiClient espClient;
PubSubClient client(espClient);

// --- Função para Medição de Distância (Sensor HC-SR04) ---
float lerDistancia() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  // Mede o tempo de viagem da onda sonora em microssegundos
  long duracao = pulseIn(PIN_ECHO, HIGH);
  
  // Calcula a distância em centímetros com base na velocidade do som
  return duracao * 0.034 / 2; 
}

// --- Rotina de Inicialização do Wi-Fi ---
void setup_wifi() {
  delay(10);
  Serial.print("\nConectando-se à rede: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado com sucesso!");
}

// --- Rotina de Conexão/Reconexão ao Broker MQTT ---
void reconnect() {
  // Loop até estabelecer conexão estável
  while (!client.connected()) {
    Serial.print("Tentando estabelecer conexão MQTT com HiveMQ...");
    
    // Tenta conectar com um ID de cliente único
    if (client.connect("ESP32_SmartCity_GroupClient")) {
      Serial.println("conectado com sucesso!");
    } else {
      Serial.print("falhou, erro rc=");
      Serial.print(client.state());
      Serial.println(". Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

// --- Configurações Iniciais do Sistema ---
void setup() {
  Serial.begin(115200);
  
  // Configuração do comportamento dos pinos do ESP32
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // Garante que os atuadores iniciem desligados (nível lógico BAIXO)
  digitalWrite(PIN_RELAY, LOW);
  digitalWrite(PIN_BUZZER, LOW);

  // Inicializa as conexões de rede
  setup_wifi();
  client.setServer(mqtt_server, 1883); // Configura o servidor na porta padrão
}

// --- Loop Principal de Execução ---
void loop() {
  // Garante a resiliência da conexão com o Broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Executa o sensoriamento em borda
  float distancia = lerDistancia();
  String status_msg = "";

  // --- Máquina de Estados / Lógica de Alerta de Borda ---
  if (distancia < 20) {
    // ESTADO DE EMERGÊNCIA: Acionamento imediato e local de hardware
    status_msg = "EMERGENCIA: Alagamento detectado!";
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_RELAY, HIGH);
  } 
  else if (distancia >= 20 && distancia <= 50) {
    // ESTADO DE ATENÇÃO: Alerta preventivo via rede
    status_msg = "ATENCAO: Nivel da agua subindo.";
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_RELAY, LOW);
  } 
  else {
    // ESTADO NORMAL: Operação padrão do ecossistema urbano
    status_msg = "Status: Normal";
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_RELAY, LOW);
  }

  // --- Serialização dos Dados em Formato JSON ---
  String payload = "{\"distancia_cm\": " + String(distancia) + 
                   ", \"status\": \"" + status_msg + "\"}";
  
  // Exibe as informações localmente no Monitor Serial
  Serial.print("Payload enviado: ");
  Serial.println(payload);
  
  // Publica o payload via MQTT para o broker público
  client.publish(topic_pub, payload.c_str());

  // Janela de amostragem de 2 segundos recomendada para telemetria urbana
  delay(2000); 
}
