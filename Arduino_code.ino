#include <Wire.h>
#include "MAX30105.h" // A lib chama o header de 30105, mas funciona para o 30102
#include "heartRate.h"

MAX30105 particleSensor;

// --- Configuração dos Pinos ---
// Defina aqui onde você conectará o fio que vai para o transistor Q1 (Oscilador 1)
const int PINO_ALARME_OSC = D5; 

// Defina aqui onde ficará o botão (conecte um lado no pino e outro no GND)
const int PINO_BOTAO = D3;      

// --- Variáveis de Controle ---
const byte RATE_SIZE = 4; // Quantidade de leituras para fazer a média
byte rates[RATE_SIZE]; // Array de batimentos
byte rateSpot = 0;
long lastBeat = 0; // Momento do último batimento

float beatsPerMinute;
int beatAvg;

// Limite de batimento para disparar alarme (ex: 100 BPM para teste, ajuste para idoso)
const int LIMITE_BPM = 70; 

// Controle de Estado do Alarme
bool alarmeAtivo = false;    // Se o alarme deveria estar tocando
bool alarmeSilenciado = false; // Se o cuidador apertou o botão

void setup() {
  Serial.begin(115200);
  Serial.println("Inicializando Monitor Cardíaco...");

  // Inicializa Sensor
  // Tenta usar a Wire padrão (D1=SCL, D2=SDA no Wemos)
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { 
    Serial.println("MAX30102 não encontrado. Verifique a fiação (SDA=D2, SCL=D1).");
    while (1);
  }
  
  // Configurações do sensor (brilho do LED, média de amostras, etc)
  particleSensor.setup(); 
  particleSensor.setPulseAmplitudeRed(0x0A); // Baixo para evitar leitura falsa
  particleSensor.setPulseAmplitudeGreen(0); // Desliga LED verde

  // Configuração dos Pinos de Saída e Entrada
  pinMode(PINO_ALARME_OSC, OUTPUT);
  pinMode(PINO_BOTAO, INPUT_PULLUP); // Usa resistor interno. Botão pressionado = LOW

  // Estado inicial do Alarme: SILÊNCIO
  // Conforme o PDF: Nível Lógico 1 (HIGH) satura transistor e aterra o 555 (desliga som) 
  digitalWrite(PINO_ALARME_OSC, HIGH); 
}

void loop() {
  long irValue = particleSensor.getIR();

  // Verifica se há um dedo no sensor (valor IR muito baixo indica ausência)
  if (irValue < 50000) {
    beatsPerMinute = 0;
    beatAvg = 0;
    // Sem dedo, reseta estados e garante silêncio
    digitalWrite(PINO_ALARME_OSC, HIGH); 
    alarmeSilenciado = false;
    Serial.println("Nenhum dedo detectado");
  } 
  else {
    // Detecta batimento
    if (checkForBeat(irValue) == true) {
      long delta = millis() - lastBeat;
      lastBeat = millis();

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute; // Adiciona ao array
        rateSpot %= RATE_SIZE; // Cicla o índice

        // Tira a média
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }
    
    // Mostra no Serial Plotter/Monitor [cite: 28]
    // Serial.print("IR: "); Serial.print(irValue);
    Serial.print(" BPM:"); Serial.print(beatsPerMinute);
    Serial.print(" Media BPM: "); Serial.print(beatAvg);
    Serial.print(", Status Alarme: "); Serial.println(alarmeAtivo ? "ON" : "OFF");

    // --- LÓGICA DO ALARME ---
    
    // 1. Verifica se passou do limite
    if (beatAvg > LIMITE_BPM) {
      alarmeAtivo = true;
    } else {
      alarmeAtivo = false;
      alarmeSilenciado = false; // Reseta o silêncio se o batimento normalizar
    }

    // 2. Verifica Botão (Cuidador)
    // Se botão pressionado (LOW), silencia o alarme atual
    if (digitalRead(PINO_BOTAO) == LOW && alarmeAtivo) {
       alarmeSilenciado = true;
       Serial.println("ALARME SILENCIADO PELO CUIDADOR");
       delay(500); // Debounce simples
    }

    // 3. Atuação Física no Pino (Baseado na lógica invertida do PDF)
    if (alarmeAtivo && !alarmeSilenciado) {
       // Alarme deve tocar -> Saída LOW (0V) para permitir oscilação 
       digitalWrite(PINO_ALARME_OSC, LOW); 
    } else {
       // Alarme mudo -> Saída HIGH (5V/3.3V) para travar oscilação 
       digitalWrite(PINO_ALARME_OSC, HIGH);
    }
  }
}