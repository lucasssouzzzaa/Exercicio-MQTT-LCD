#include <Arduino.h>
#include <WiFi.h>
#include "internet.h"
#include "senha.h"

const unsigned long tempoConexao = 10000;
const unsigned long tempoReconexao = 5000;

void conexaoWifi(){
  Serial.printf("Conectando ao wifi: %s", SSID);

  WiFi.begin(SSID, SENHA);

  unsigned long tempoWifi = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoWifi < tempoConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Wifi conectado com sucesso!");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Falha ao se conectar");
  }
}

void reconexaoWifi(){
  unsigned long tempoAtual = millis();
  unsigned long tempoUltimaConexao = 0;

  if (tempoAtual - tempoUltimaConexao > tempoReconexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Conexao Wifi perdida");
      conexaoWifi();
    }
    tempoUltimaConexao = tempoAtual;
  }
}