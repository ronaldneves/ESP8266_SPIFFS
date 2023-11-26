#include "FS.h"
String nome_do_arquivo = "";

void setup() {
  Serial.begin(9600);
  SPIFFS.begin();  

  Dir dir = SPIFFS.openDir("/");

  while (dir.next()) {
    Serial.println(dir.fileName());
  }

  Serial.println("Listagem de arquivos concluída.");
}

void loop() {
}
