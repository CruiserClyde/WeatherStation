#include <Adafruit_BMP085.h>
#include <RCSwitch.h>
#include "DHT.h"
#define DHTPIN 3 //pin du data du dht 22
#define DHTTYPE DHT22
#define trigPin 8
#define echoPin 9
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);
RCSwitch serveur = RCSwitch();

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("le capteur BMP085 n a pas ete trouve regarde les branchements!");
  while (1) {}
  }
  dht.begin();
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  serveur.enableReceive(0);   // pin #2
  serveur.enableTransmit(10); //pin 10
  Serial.print("Début de l echange d informations coté serveur...\n");
}

void loop() {
  if(serveur.available()){
    int message = serveur.getReceivedValue();
    if(isnan(message)){
      Serial.print("\n Erreur de transmission \n");
     }
    else {
      /******************************* DEBUT ********************************/
      /************************** code de reception *************************/
      /**********************************************************************/
      if(serveur.getReceivedBitlength() == 24 && serveur.getReceivedProtocol() == 2){
        Serial.print("\n Message reçu \n");
        Serial.print(message);
      }
      /******************************** FIN *********************************/
      /************************** code de reception *************************/
      /**********************************************************************/
    }
  }
  /******************************* DEBUT ********************************/
  /************************ code de transmission ************************/
  /**********************************************************************/
  transmet(PressionExt(1015),1,14);
  serveur.resetAvailable();
  delay(2000);
  transmet(TemperatureExt().toInt(),1,16);
  serveur.resetAvailable();
  delay(2000);
  transmet(HumiditeExt().toInt(),1,18);
  serveur.resetAvailable();
  delay(2000);
  transmet(LuminositeExt(),1,13);
  serveur.resetAvailable();
  delay(2000);
  transmet(PluieExt(),1,7);
  serveur.resetAvailable();
  delay(2000);
  transmet(NiveauPluieExt(trigPin,echoPin),1,15);
  serveur.resetAvailable();
  delay(2000);
  /******************************** FIN *********************************/
  /************************ code de transmission ************************/
  /**********************************************************************/
    
}

void transmet(long donnee,int protocole,int nombreBits){
  serveur.setProtocol(protocole);
  serveur.send(donnee,nombreBits);
  Serial.print("\n donnée envoyée : ");
  Serial.print(donnee);
}
long  PressionExt(float SeaLevelPress){// en milibar
  long mesure = bmp.readPressure();
  while(isnan(mesure)){mesure = bmp.readPressure();}
  long delta = (SeaLevelPress-mesure/100)*100;
  long pression = (delta+mesure)-100000;//on rajoute 100000 pour avoir la pression(cause de taille du paquet a transmettre)
  return pression;// en pascal
}

String TemperatureExt(){
 float lireTemp = dht.readTemperature();
 while(isnan(lireTemp)){lireTemp = dht.readTemperature();}
 
 long temp = lireTemp*100;
 String t;
 String tempExt;
 
 if(lireTemp<0){
    t = String(-temp);
    if(t.length()==2){
      t="00"+t;
    }
    if(t.length()==3){
      t="0"+t;
    }
    tempExt = "1"+String(t[0])+String(t[1])+String(t[2])+String(t[3]);
  }else{
    t = String(temp);
    if(t.length()==2){
      t="00"+t;
    }
    if(t.length()==3){
      t="0"+t;
    }
    tempExt = "2"+String(t[0])+String(t[1])+String(t[2])+String(t[3]);
    }
 return tempExt;// au format 5 chiffres 1 pour - 2 pour + suivis de 4 chiffres
}

String HumiditeExt(){
  float h = dht.readHumidity();
  while(isnan(h)){h = dht.readHumidity();}
  String humiext;
  long humi = h*10;
  if(h<10){
    humiext = "0"+String(humi);
  }else{
    humiext = String(humi);
  }
  return humiext;
}

int LuminositeExt(){
  int Analog0 = analogRead(A0);
  while(isnan(Analog0)){Analog0 = analogRead(A0);}
  int lumi = 100-map(Analog0,0,1023,0,100);//luminosite en %
  return lumi;
}

int PluieExt(){
  int Analog1 = analogRead(A1);
  while(isnan(Analog1)){Analog1 = analogRead(A1);}
  int pluie = 100-map(Analog1,0,1023,0,100);//pluie en %
  return pluie;
}
long NiveauPluieExt(int trig,int echo){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  long duration = pulseIn(echo,HIGH);
  long distance = (duration*0.034/2)*10;
  while(isnan(distance)){distance = NiveauPluieExt(trig,echo);}//recursivite
  return distance;//en mm
}
