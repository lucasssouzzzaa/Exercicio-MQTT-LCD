#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

#include "internet.h"

const char *mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

const char *mqtt_client_id = "ESP32-LCD";
const char *mqtt_topic = "PedraoArduino/sub";

LiquidCrystal_I2C lcd(0x27, 20, 4);

WiFiClient client;
PubSubClient mqtt(client);

String dispositivo = "Aguardando";

float temperatura = 0;
float umidade = 0;
int luminosidade = 0;

bool recebeuDados = false;

unsigned long tempoAnterior = 0;

void receberDados(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensagem recebida em ");
  Serial.print(topic);
  Serial.print(": ");

  String msgRecebida = "";

  for (unsigned int i = 0; i < length; i++)
  {
    msgRecebida += (char)payload[i];
  }

  Serial.println(msgRecebida);

  JsonDocument dados;

  DeserializationError erro =
      deserializeJson(dados, msgRecebida);

  if (erro)
  {
    return;
  }

  dispositivo = dados["dispositivo"].as<String>();

  temperatura = dados["temperatura"];
  umidade = dados["umidade"];
  luminosidade = dados["luminosidade"];
  Serial.println(dispositivo);

  recebeuDados = true;
}


void conectaMqtt()
{
  while (!client.connected())
  {
    Serial.println("Conectando ao MQTT");

    if (mqtt.connect(mqtt_client_id))
    {
      Serial.println("Conectado com Sucesso!");

      mqtt.subscribe(mqtt_topic);
    }
    else
    {
      Serial.println("Erro ao conectar!");
      Serial.println(mqtt.state());
      Serial.println("Tentando novamente em 5s");
      delay(5000);
    }
  }
}

void exibirLCD()
{
  lcd.clear();
  Serial.println(recebeuDados);

  if (!recebeuDados)
  {
    lcd.setCursor(0, 0);
    lcd.print("Aguardando MQTT");

    lcd.setCursor(0, 1);
    lcd.print("Dados...");

    return;
  }

  
    lcd.setCursor(0, 0);
    lcd.print(dispositivo.substring(0, 16));

    lcd.setCursor(0, 1);
    lcd.print("Temp:");
    lcd.print(temperatura, 1);
    lcd.print(" C");
    lcd.setCursor(0, 2);
    lcd.print("Umid:");
    lcd.print(umidade, 1);
    lcd.print("%");

    lcd.setCursor(0, 3);
    lcd.print("LDR:");
    lcd.print(luminosidade);
}

void setup()
{
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  conexaoWifi();

  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(receberDados);
}

void loop()
{
  reconexaoWifi();

  if (!mqtt.connected())
  {
    conectaMqtt();
  }

  mqtt.loop();

  if (millis() - tempoAnterior >= 3000)
  {
    tempoAnterior = millis();


    exibirLCD();
  }
}