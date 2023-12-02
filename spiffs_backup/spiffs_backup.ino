/*
1. Criar função que gere dados aleatorios para simular um sensor de nivel numa caixa d'água (1 a 3, sendo 1 = vazio); -- OK
2. Criar função para contar quantas vezes a caixa d'água ficou vazia no dia (leitura == 1); -- OK
3. Criar função para que a cada X tempo, esse contador seja armazenado num arquivo .txt na memória flash; -- OK
4. Ao inicializar, a ESP8266 precisa checar o valor do contador armazenado anteriormente e dar continuidade ao contador; -- OK
*/
// BIBLIOTECAS
#include "FS.h"


// VARIAVEIS
int leitura_sensor = 0;
int contador_evento = 0;
int comparador_evento = 1; // 1 significa tanque vazio. quero contar quantas vezes o tanque ficou vazio
unsigned long millis_sensor = 0;
unsigned long tempo_sensor = 2000;
unsigned long tempo_backup = 5000;
unsigned long millis_backup = 0;
const char* nome_do_arquivo = "/backup.txt";
String template_backup = "Backup do contador: ";

void setup() {
  Serial.begin(9600);
  SPIFFS.begin();
  contador_evento = restaura_backup(nome_do_arquivo, contador_evento);
}

void loop() {
  if ((millis() - millis_sensor) > tempo_sensor) {
    millis_sensor = millis();
    leitura_sensor = simulador_sensor(leitura_sensor);
    Serial.print("Valor de leitura do sensor da caixa d'água: ");
    Serial.println(leitura_sensor);
    Serial.print("Valor do contador de eventos: ");
    contador_evento = contador(leitura_sensor, contador_evento, comparador_evento);
    Serial.println(contador_evento);
  } 

  if ((millis() - millis_backup) > tempo_backup) {
    millis_backup = millis();
    salva_backup(contador_evento, nome_do_arquivo);
  }
}


// FUNÇÕES
// gerar dados aleatorios para simular sensor
int simulador_sensor(int sensor) {
  sensor = random(1, 4); // 4 não incluso. ele conta de 1 a 3 (1, 2, 3)
  return sensor;
}


// função para monitorar eventos desejados
int contador(int sensor, int contador_evento, int comparador) {
  if (sensor == comparador) {
    contador_evento += 1;
    Serial.print("Evento detectado. ");
  }

  return contador_evento;
}


// função para salvar o valor da contagem num backup
void salva_backup(int contador, const char* nome_do_arquivo) {
  File backup = SPIFFS.open(nome_do_arquivo, "w");
  backup.println(contador);
  backup.close();

  Serial.println("Backup realizado com sucesso.");
}


// função de restauro de backup
int restaura_backup(const char* nome_do_arquivo, int contador) {
  if (SPIFFS.exists(nome_do_arquivo)) {
    Serial.println("Arquivo de backup encontrado. Restaurando valor do contador.");
    File backup = SPIFFS.open(nome_do_arquivo, "r");

    while(backup.available()) {
      String linha = backup.readStringUntil('\n');
      contador = linha.toInt();        
    }
  }
  return contador;
}