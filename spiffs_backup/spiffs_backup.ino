// LIBS
#include "FS.h"

// VARIAVEIS
int leitura_sensor = 0;
int contador_evento = 0;
int valor_de_interesse = 7;
unsigned long tempo_sensor = 1000;
unsigned long tempo_backup = 5000;
unsigned long millis_sensor = 0;
unsigned long millis_backup = 0;

String texto_template = "Backup contador: ";
const char* nome_do_arquivo = "/log.txt";


// FUNÇÕES
int controle_contagem(int sensor, int contador, int comparador) {
  if (sensor == comparador) {
    contador += 1;
    Serial.print("Evento ocorrido. Valor do contador: ");
    Serial.println(contador_evento);
  }
  return contador;
}


void salva_backup(int contador, const char* nome_do_arquivo, String template_texto) {
  File backup = SPIFFS.open(nome_do_arquivo, "w");
  backup.print(template_texto);
  backup.println(contador);
  backup.close();

  Serial.println("Backup realizado com sucesso");
}


int checa_arquivo(const char* nome_do_arquivo, int contador, String template_texto) { // se arquivo não existir, cria. se existir, restaura o valor na variavel.
  if (!SPIFFS.exists(nome_do_arquivo)) {
    Serial.println("Arquivo de backup não encontrado. Criando novo arquivo de backup");
    File arquivo_inicial = SPIFFS.open(nome_do_arquivo, "w");
    arquivo_inicial.println("Backup do contador de ocorrências.");
    arquivo_inicial.close();
  }

  else {
    Serial.println("Arquivo de backup encontrado. Recuperando valor do contador");
    File backup = SPIFFS.open(nome_do_arquivo, "r");

    while(backup.available()) {
      String linha = backup.readStringUntil('\n');
      int posicao = linha.indexOf(template_texto);

      if (posicao != -1) {
        int tamanho = template_texto.length();
        contador = linha.substring(posicao + tamanho).toInt();  
      }
    }
  }
  return contador;
}


// SETUP
void setup() {
  Serial.begin(9600);
  SPIFFS.begin();
  contador_evento = checa_arquivo(nome_do_arquivo, contador_evento, texto_template);
}


// LOOP
void loop() {
  if (millis() - millis_sensor > tempo_sensor) {
    millis_sensor = millis();
    leitura_sensor = random(0, 11);
    Serial.println(leitura_sensor);

    contador_evento = controle_contagem(leitura_sensor, contador_evento, valor_de_interesse);    
  }


  if (millis() - millis_backup > tempo_backup) {
    millis_backup = millis();
    salva_backup(contador_evento, nome_do_arquivo, texto_template);
  }
}