// IrrigAção: Instituto Federal de Ciência e Tecnologia de Rondônia - Campus Cacoal
// GPPDS: carlosdde2004@gmail.com

#include "TRIGGER_GOOGLESHEETS.h" // Biblioteca para a sincronização com Google Planilhas
#define BLYNK_PRINT Serial   
#include <SPI.h>
#include <ESP8266WiFi.h>        // Biblioteca para Wi-Fi da placa
#include <BlynkSimpleEsp8266.h> // Biblioteca para autenticar o código com o Blynk App
#include <DHT.h>                // Biblioteca para o Sensor DHT (Humidade e Temperatura do ar)
#define BLYNK_PRINT Serial      // Biblioteca adicional para melhor funcionamento
#include <OneWire.h>            // Biblioteca adicional
#include <DallasTemperature.h>  // Biblioteca adicional para melhor funcionamento
#define ONE_WIRE_BUS D2         // Debugger
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

char column_name_in_sheets[ ][20] = {"value1","value2","value3"};// Define os parâmetros de linha para o Google Planilhas;  
String Sheets_GAS_ID = "AKfycbzjqAFMBLIjPODpV8Q7DG0hSoVRxZvbjDTVtUYjWtIZukMCuBi1Ts3sZp4BFzitWYa8";// URL Google Scripts;                          
int No_of_Parameters = 3; // Quantidade de parâmetros enviados para o Google Planilhas

char auth[] = "6B-TJN7x91at0MZJomcGb5xpVUlwW8R1";           //Código de Autenticação Blynk;
char ssid[] = "Domingues Eleoterio";                        //Nome da Rede WiFi;
char pass[] = "ca183381";                                   //Senha do Wi-Fi;


#define pirPin D1    //Sensor de Movimento (PIR);
int pirValue;
int pinValue;                   

#define sensorPin D3        //Sensor de Humidade do Solo ;
int sensorState = 0;        //Estado do Sensor de Humidade do Solo;
int lastState = 0;          //Último estado do Sensor;
#define DHTPIN D5           //Sensor de Temperatura e Humidade do Ar (DHT);
#define DHTTYPE DHT22       //Define a versão do Sensor DHT;
DHT dht(DHTPIN, DHTTYPE);   //Declara o pino e tipo do sensor em uma constante;
BlynkTimer timer;           //Declara o Timer do Blynk como timer

BLYNK_WRITE(V0)   // Escreve as informações do sensor PIR (Movimento) no Pino Virtual 0
{
 pinValue = param.asInt();
} 

void sendSensor() // Captura e envia os valores dos sensores para o usuário final:
{
  float h = dht.readHumidity();     // Lê a humidade do DHT e aloca na variável h;
  float t = dht.readTemperature();  // Lê a temperatura do DHT e aloca na variável t;

  if (isnan(h) || isnan(t)) {       // Verifica se houve alguma falha ao ler o sensor DHT;
    Serial.println("Falha ao ler o sensor DHT");
    return;
  }
 
  Blynk.virtualWrite(V5, h);  // Envia a informação de Humidade do Ar para o Pino Virtual 5;
  Blynk.virtualWrite(V6, t);  // Envia a informação de Temperatura do Ar para o Pino Virtual 6;
  Data_to_Sheets(No_of_Parameters,  t,  h, analogRead(A0)); // Envia as informações dos sensores para o Google Planilhas;
}
void setup()
{
  Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters ); // Inicializa a comunicação com o Google Planilhas;
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass); // Inicializa a comunicação com o Blynk App;
   pinMode(sensorPin, INPUT);    // Define o modo do sensor de Humidade do Solo como entrada de dados;
  dht.begin();                   //Inicializa o sensor DHT

  timer.setInterval(2000L, sendSensor); //Define o intervalo de leitura do Sensor de Humidade do Solo (1000L = 1 segundo)
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
   sensors.begin();
}
int sensor=0;
void sendTemps()
{
sensor=analogRead(A0);                   //Aloca a leitura analógica do Sensor de Humidade do Solo na variável sensor
sensors.requestTemperatures();
float temp = sensors.getTempCByIndex(0); 
Serial.println(temp);     //Printa as informações do Sensor DHT
Serial.println(sensor);   //Printa as informações do Sensor de Humidade do Solo
Blynk.virtualWrite(V1, temp);
Blynk.virtualWrite(V2,sensor);
delay(1000);
}

void getPirValue(void)              //Get Informações do Sensor PIR
  {
   pirValue = digitalRead(pirPin);  //Lê o estado do Sensor PIR (Movimento) e aloca na variável pirValue;
    if (pirValue) //Verifica se existe movimento, se sim notifica o usuário;
     { 
       Serial.println("Movimento detectado");
       Blynk.notify("Foi detectado um movimento da sua horta!");  
     }
  }
  
void loop()
{
  Blynk.run();          //Inicializa a sincronização com o Blynk App;
  timer.run();          //Inicializa o temporizador Blynk;
  sendTemps();          //Chama a biblioteca de leitura/envio de informação dos sensores;
sensorState = digitalRead(sensorPin);
Serial.println(sensorState);

if (sensorState == 1 && lastState == 0) {         //Verifica a condição de água do sensor, caso esteja seco, notifica o usuário;
  Serial.println("Muito seco, precisa de água");
  Blynk.notify("Suas plantas estão precisando de água!");
  lastState = 1;
  delay(1000);
//Envia notificação
    
  } 
  else if (sensorState == 1 && lastState == 1) {
    //Não faz nada. Porém ainda não foi regado;
  Serial.println("Ainda não foi regado");
  delay(1000);
  }
  else {
    //st
    Serial.println("Não precisa de água");
    lastState = 0;
    delay(1000);
  }

  if (pinValue == HIGH)    
      {
        getPirValue();
      }
      Blynk.run();
  delay(100);
}
