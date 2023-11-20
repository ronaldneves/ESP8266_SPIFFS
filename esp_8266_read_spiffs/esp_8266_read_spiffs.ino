#include "FS.h"
String nome_do_arquivo = "/teste.txt";

void setup() {
  Serial.begin(9600);
  SPIFFS.begin();

  File arquivo = SPIFFS.open(nome_do_arquivo, "r");
  
  if (arquivo) {
    Serial.println("Conteúdo do arquivo:");

    while (arquivo.available()) {
      Serial.write(arquivo.read());
    } 
  }  
  arquivo.close();
}

void loop() {
}