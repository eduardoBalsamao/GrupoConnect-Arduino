//Desenvolvido pelo Grupo Conect 

#include <ESP8266WiFi.h> //Inclui a biblioteca do WiFi da ESP8266
#include <FirebaseESP8266.h>  //Inclui a biblioteca do FireBase https://github.com/mobizt/Firebase-ESP8266/
#include <ArduinoJson.h>
#include <WiFiManager.h>  //Inclui a biblioteca do WifiManager https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>
#include <NTPClient.h> //Biblioteca NTPClient para trabalhar com tempo

#define FIREBASE_HOST "grupo-connect-oficial-default-rtdb.firebaseio.com" 
#define FIREBASE_AUTH "pddcuoKmV8vRK8LyVm2g0rQMMnEPtRamMzZtzTd5"

WiFiUDP udp; //Socket UDP que a lib do NTP utiliza para recuperar dados sobre o horário

int rele = 2;     //Saida padrão da ESP01 - Modulo relé 5v- GPIO 0
String val, valLigar, valDesligar;      //Variavel para conferir o estado no firebase

FirebaseData firebaseData;

NTPClient ntpClient(udp, "br.pool.ntp.org", 0, 60000);

void setup()
{

  Serial.begin(115200); //Inicia a Conexão Serial

  ntpClient.begin(); //Inicia a variavel ntp
  ntpClient.setTimeOffset(-10800);
  
  pinMode(rele,OUTPUT); // Seta que o "rele" é de saida
  digitalWrite(rele, LOW);
  
  WiFiManager wifiManager; // Inicia o WiFiManager
  //wifiManager.resetSettings(); //LINHA COMENTADA (reseta as credenciais de wifi)
  wifiManager.autoConnect("Grupo Conect"); //Inicia o autoConnect com o nome Grupo Conect
  
  Serial.print("Connected with IP: "); 
  Serial.println(WiFi.localIP());//Mostra o ip conectado no monitorSerial
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
 

}


void loop() {
  ntpClient.update();
  int horas = ntpClient.getHours();
  String horasStr = String(horas);
  int minutos = ntpClient.getMinutes();
  String minutosStr = String(minutos);
  String tempo = horasStr + ':' + minutosStr;
  Serial.println(tempo);
  
 if (Firebase.getString(firebaseData, "/Torneiras/t111/status")) {  //On successful Read operation, function returns 1 
  
    val = firebaseData.stringData();
    if(val == "0"){
      digitalWrite(rele, LOW); // Desliga o rele
      Serial.println(val);
      delay(100);
    } 
    if (val == "1"){
      digitalWrite(rele, HIGH); // Liga o rele
      Serial.println(val);
      delay(100);
    }
  
  } else {
    Serial.println(firebaseData.errorReason());
  }
  
  
  
  if (Firebase.getString(firebaseData, "/Torneiras/t111/ligarAs")) {  //On successful Read operation, function returns 1 
  
    valLigar = firebaseData.stringData();
    if(valLigar == tempo){
      //digitalWrite(rele, HIGH); // Desliga o rele
      Firebase.setString(firebaseData, "/Torneiras/t111/status", "1");
      Serial.println("Ligou as " + tempo);
      delay(100);
    } 
  } else {
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getString(firebaseData, "/Torneiras/t111/desligarAs")) {  //On successful Read operation, function returns 1 
  
    valDesligar = firebaseData.stringData();
    if(valDesligar == tempo){
      //digitalWrite(rele, LOW); // Desliga o rele
      Firebase.setString(firebaseData, "/Torneiras/t111/status", "0");
      Serial.println("Desligou as " + tempo);
      delay(100);
    } 
  } else {
    Serial.println(firebaseData.errorReason());
  }


 
}
