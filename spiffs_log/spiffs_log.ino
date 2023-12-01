// BIBLIOTECAS
#include "ESP8266WiFi.h"
#include "NTPClient.h"
#include "TimeLib.h" // biblioteca para fazer conversões de tempo para data
#include "WiFiUdp.h"
#include "FS.h"


// VARIÁVEIS
int botao_acesso = 4;
int estado_botao = 1;
int ultimo_estado_botao = 1;
unsigned long debounce_delay = 50;
unsigned long debounce = 0;
int botao_com_debounce = 0;

const char* nome_wifi = ""; // substituir por nome da wifi
const char* senha_wifi = ""; // substituir por senha da wifi
const char* servidor_ntp = "pool.ntp.org";
unsigned long ajuste_fuso = (-3 * 3600);
WiFiUDP udp;
NTPClient cliente_tempo(udp, "pool.ntp.org", ajuste_fuso);
String hora_data = "";

int controle_impressao = 0;
const char* nome_do_arquivo = "/log.txt";
String log_template = "";


// FUNÇÕES
int debounce_botao(int porta_do_botao) {
  int leitura_botao = digitalRead(porta_do_botao);
  
  if (leitura_botao != ultimo_estado_botao) {
    debounce = millis();
  }

  if ((millis() - debounce) > debounce_delay) {
    if (leitura_botao != estado_botao) {
      estado_botao = leitura_botao;      
      }
    }

  ultimo_estado_botao = leitura_botao;
  return estado_botao;
}


void setup_wifi(const char* rede, const char* senha) {
  WiFi.begin(rede, senha);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.print("Conectado na rede: ");
  Serial.println(rede);
}


String pega_data_hora(NTPClient cliente) { 
  unsigned long epochTime = cliente.getEpochTime(); // epochTime = estrutura de tempo unix, conta segundos passados desde 1900
  struct tm *ptm = gmtime((time_t *)&epochTime);
  tmElements_t tm;
  breakTime(epochTime + (7 * 24 * 3600), tm); // criação do objeto dia / mês / ano
  int dia = (tm.Day);
  int mes = (tm.Month);
  int ano = (tm.Year + 1970);

  String data = (String(dia) + "/" + String(mes) + "/" + String(ano));
  String hora = cliente_tempo.getFormattedTime();
  String data_hora = (hora + " " + data);

  return data_hora;
}


void checa_arquivo(const char* nome_do_arquivo) {
  if (!SPIFFS.exists(nome_do_arquivo)) {
    File arquivo_inicial = SPIFFS.open(nome_do_arquivo, "w");
    arquivo_inicial.println("Início do Log de Acesso.");
    arquivo_inicial.close();
    Serial.println("Arquivo base criado com sucesso.");
  }
  else {
    Serial.println("Arquivo base encontrado na memória.");
  }
}


void salva_log(const char* nome_arquivo, String log_hora_data) {
  File arquivo = SPIFFS.open(nome_arquivo, "a");
  arquivo.println(log_hora_data);
  arquivo.close();
  Serial.println("Log gerado.");
}


// SETUP
void setup() {
  Serial.begin(9600);
  pinMode(botao_acesso, INPUT_PULLUP);
  setup_wifi(nome_wifi, senha_wifi);

  cliente_tempo.begin();
  SPIFFS.begin();

  checa_arquivo(nome_do_arquivo);
}


// LOOP
void loop() {
  cliente_tempo.update();  
  botao_com_debounce = debounce_botao(botao_acesso);

  if (botao_com_debounce == 0 && controle_impressao == 0) {
    log_template = "Botão pressionado: " + pega_data_hora(cliente_tempo);
    salva_log(nome_do_arquivo, log_template);
    controle_impressao = 1;  // controle de impressão
  }  
  else if (botao_com_debounce != 0) {
    controle_impressao = 0;  // reset do controle 
  }
}