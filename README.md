# SmartCity-IoT 🌧️🤖
> **Rede de Objetos Inteligentes Conectados para Monitoramento Ambiental e Prevenção de Desastres em Centros Urbanos**

Este projeto consiste no desenvolvimento de um protótipo de sistema embarcado baseado em IoT para o monitoramento em tempo real do nível de água em galerias pluviais e rios urbanos. O sistema utiliza computação de borda (*edge computing*) para acionamento imediato de alertas locais (sonoros e mecânicos) em cenários de alagamento, mitigando riscos antes mesmo da resposta em nuvem. O projeto está estritamente alinhado com o **ODS 11 (Cidades e Comunidades Sustentáveis)**.

---

## 🚀 1. Funcionamento e Uso
O protótipo monitora continuamente a distância entre o sensor e a superfície da água. Dependendo da aproximação da água, o microcontrolador toma decisões locais e publica os dados na internet:
* **Status Normal (Distância > 50 cm):** Apenas envia dados de telemetria.
* **Status de Atenção (Distância entre 20 cm e 50 cm):** Publica um alerta de atenção via rede.
* **Status de Emergência (Distância < 20 cm):** Ativa imediatamente o Buzzer (alarme sonoro) e o Módulo Relé (representando o fechamento de comportas ou ativação de bombas de drenagem) de forma offline, além de propagar o status crítico via protocolo MQTT.

### Como Reproduzir:
1. Monte o circuito seguindo o diagrama disponível na seção de Hardware.
2. Instale a **Arduino IDE** (ou utilize o simulador online **Wokwi**).
3. Adicione a biblioteca `PubSubClient` ao seu ambiente de desenvolvimento.
4. Carregue o código fonte disponível na pasta `/src` para o seu módulo ESP32.
5. Abra o Monitor Serial (baud rate: 115200) para acompanhar a conexão Wi-Fi e o envio de dados JSON.

---

## 💻 2. Software e Lógica Embarcada
O firmware do dispositivo foi desenvolvido em linguagem C/C++ utilizando a plataforma Arduino. A lógica é estruturada em um ciclo iterativo contínuo, priorizando o processamento local antes do envio de pacotes de rede para otimizar o tempo de resposta em situações críticas de infraestrutura.

O código-fonte completo encontra-se documentado no arquivo `src/smartcity_iot.ino`.

---

## 🛠️ 3. Hardware Utilizado
O ecossistema de hardware foi selecionado visando baixo custo de implementação e eficiência energética.
* **Plataforma de Prototipagem:** Módulo de Desenvolvimento ESP32 (Processador *dual-core* Xtensa de 32 bits, com Wi-Fi nativo de 2.4GHz).
* **Sensor:** Sensor Ultrassônico HC-SR04 (Frequência de 40 kHz, alcance operacional de 2 cm a 400 cm).
* **Atuadores:** * Buzzer Ativo Piezoelétrico 5V (Sirene local de evacuação).
    * Módulo Relé de 1 Canal 5V (Chaveamento eletromecânico para sistemas de contenção/drenagem).

*Nota técnica: No caso de uma implementação física em caixas de junção municipais, recomenda-se o uso de cases com certificação IP65 para proteção contra intempéries e umidade extrema.*

---

## 🌐 4. Interfaces e Protocolo de Comunicação
A camada de rede e transporte de dados utiliza a arquitetura TCP/IP integrada ao protocolo de aplicação **MQTT (Message Queuing Telemetry Transport)**, operando sob o modelo *Publish/Subscribe*.

* **Broker MQTT Utilizado:** HiveMQ (Servidor público: `broker.hivemq.com` | Porta: 1883).
* **Tópico de Publicação:** `smartcity/monitoramento/alagamento`

### Formato do Payload (JSON):
Os dados de campo são encapsulados e enviados a cada 2 segundos no formato estruturado abaixo:
```json
{
  "distancia_cm": 18.5,
  "status": "EMERGENCIA: Alagamento detectado!"
}
```
---

## 👥 Autores
* Ivan Capelle
* Níkolas Povreslo

---
*Projeto desenvolvido como atividade prática ("Aplicando Conhecimento") para a Faculdade de Computação e Informática - Universidade Presbiteriana Mackenzie (UPM).*
