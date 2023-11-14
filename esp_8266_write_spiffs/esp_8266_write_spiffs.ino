#include "FS.h"
String nome_do_arquivo = "teste.txt";

void setup() {
  SPIFFS.begin();
  File novo_arquivo = SPIFFS.open(nome_do_arquivo, "w");
  
  if (novo_arquivo) {
    novo_arquivo.print("Hello world!");
    novo_arquivo.close();
  }
}

void loop() {
}