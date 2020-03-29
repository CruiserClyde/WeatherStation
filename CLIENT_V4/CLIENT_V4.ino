#include <RCSwitch.h>
#include "Nextion.h"
#include "RTClib.h"
#include "DHT.h"
#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
RCSwitch client = RCSwitch();
char jours[7][12] = {"Dimanche","Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samedi"};
char mois[12][12] = {"Janvier","Février","Mars","Avril","Mai","Juin","Juillet","Août","Septembre","Octobre","Novembre","Décembre"};
int PAGE = 0;

//declaration des objets nextions(page id = 0, component id = 1, component name = "b0")
//BOUTONS RETOURS
NexButton bParam = NexButton(0, 2, "bParam");
NexButton bRetour1 = NexButton(1, 6, "bRetour");
NexButton bRetour2 = NexButton(2, 3, "bRetour");
NexButton bRetour3 = NexButton(3, 2, "bRetour");

//BOUTONS VALIDER
NexButton bValidTime = NexButton(2, 23, "bValider");
NexButton bValidPress = NexButton(3, 5, "bValider");
//BOUTONS REGLAGES
NexButton bRegDateHeure = NexButton(1, 2, "bRegDateHeure");
NexButton bRegPression = NexButton(1, 3, "bRegPression");

/************REGLAGES DATE ET HEURE************/
NexButton Pjour = NexButton(2, 7, "Pjour");//ok
NexButton Mjour = NexButton(2, 9, "Mjour");//ok
NexButton Pmois = NexButton(2, 12, "Pmois");//ok
NexButton Mmois = NexButton(2, 14, "Mmois");//ok
NexButton Pannee = NexButton(2, 13, "Pannee");//ok
NexButton Mannee = NexButton(2, 15, "Mannee");//ok
NexButton Pheure = NexButton(2, 16, "Pheure");//ok
NexButton Mheure = NexButton(2, 18, "Mheure");//ok
NexButton Pmin = NexButton(2, 17, "Pmin");//ok
NexButton Mmin = NexButton(2, 19, "Mmin");//ok

/************REGLAGES PRESSION************ (a venir...) */


//declaration des events
NexTouch *nex_listen_list[] = {
  &bParam,
  &bRetour1,
  &bRetour2,
  &bRetour3,
  &bValidTime,
  &bValidPress,
  &bRegDateHeure,
  &bRegPression,
  &Pjour,
  &Mjour,
  &Pmois,
  &Mmois,
  &Pannee,
  &Mannee,
  &Pheure,
  &Mheure,
  &Pmin,
  &Mmin,
  NULL
};

//donnees String du serveur
String pressExtStr;//ok
String tempExtStr;//ok
String humiExtStr;//ok
String lumiExtStr;//ok
String pluieExtStr;//ok
String nivpluieExtStr;//ok

//donnees Val du serveur
int pressExtVal;//ok
int tempExtVal;//ok
float humiExtVal;//ok
int lumiExtVal;//ok
int pluieExtVal;//ok
int nivpluieExtVal;

//donnees Val du client
float tempIntVal;//ok
float humiIntVal;//ok
int ANNEE;  int REG_ANNEE = 0; int ANNEE_DECONNEXION;
int MOIS;   int REG_MOIS = 0;  int MOIS_DECONNEXION;
int JOUR;   int REG_JOUR = 0;  int JOUR_DECONNEXION;
int HEURE;  int REG_HEURE = 0; int HEURE_DECONNEXION;
int MINUTE; int REG_MINUTE =0; int MIN_DECONNEXION;
int cptDeconnect = 0;

//donnees modifiables user-space
int deltaPression;//ok

//smart note
String infoJour;//ok
String infoPluie;//ok
String infoPrev;//ok
String smartNote;//ok

//watchdog connexion serveur
const long interval = 10000;//plus de 10 secondes sans reponse du serveur
int isNetworkOn = 0;

void setup() {
  Serial.begin(9600);
  nexInit();
  
  bParam.attachPush(bParamPushCallback);//-> page1
  bRetour1.attachPush(bRetour1PushCallback);//-> page0
  bRetour2.attachPush(bRetour2PushCallback);//-> page1
  bRetour3.attachPush(bRetour3PushCallback);//-> page1
  bValidTime.attachPush(bValidTimePushCallback);//-> page0
  bValidPress.attachPush(bValidPressPushCallback);//-> page0
  bRegDateHeure.attachPush(bRegDateHeurePushCallback);//-> page2
  bRegPression.attachPush(bRegPressionPushCallback);//-> page3
  
  Pjour.attachPush(PjourPushCallback);
  Mjour.attachPush(MjourPushCallback);
  Pmois.attachPush(PmoisPushCallback);
  Mmois.attachPush(MmoisPushCallback);
  Pannee.attachPush(PanneePushCallback);
  Mannee.attachPush(ManneePushCallback);
  Pheure.attachPush(PheurePushCallback);
  Mheure.attachPush(MheurePushCallback);
  Pmin.attachPush(PminPushCallback);
  Mmin.attachPush(MminPushCallback);
  
  deltaPression = 0;
  client.enableReceive(0); // pin #2
  client.enableTransmit(10); //pin 10
  if (! rtc.begin()) {/*problème avec le module rtc*/ while (1);}}
  
void loop() {
  nexLoop(nex_listen_list);
  unsigned long currentMillis = millis();
  unsigned long receiveMillis;
  if(client.available()){
    int message = client.getReceivedValue();
    if(!isnan(message)){
      receiveMillis = millis();    
      /******************************* DEBUT ********************************/
      /************************** code de reception *************************/
      /**********************************************************************/
      if(client.getReceivedBitlength() == 14 && client.getReceivedProtocol() == 1){
        pressExtStr = message;ActiveReseau();  
      }
      if(client.getReceivedBitlength() == 16 && client.getReceivedProtocol() == 1){
        tempExtStr = message;ActiveReseau(); 
      }
      if(client.getReceivedBitlength() == 18 && client.getReceivedProtocol() == 1){
        humiExtStr = message;ActiveReseau();  
      }
      if(client.getReceivedBitlength() == 13 && client.getReceivedProtocol() == 1){
        lumiExtStr = message;ActiveReseau();  
      }
      if(client.getReceivedBitlength() == 7 && client.getReceivedProtocol() == 1){
        pluieExtStr = message;ActiveReseau();  
      }
      if(client.getReceivedBitlength() == 15 && client.getReceivedProtocol() == 1){
        nivpluieExtStr = message;ActiveReseau();   
      }
      /******************************** FIN *********************************/
      /************************** code de reception *************************/
      /**********************************************************************/
    }
  }
  if (abs(currentMillis - receiveMillis) >= interval) {
    DesactiveReseau();
  }
  
  
  affDate();
  affHeure();
  if(PAGE == 0){ //MAIN PAGE
    client.resetAvailable();
    /******************************* DEBUT ********************************/
    /************************ code de transmission ************************/
    /**********************************************************************/
    transmet(87,2,24);
    /******************************** FIN *********************************/
    /************************ code de transmission ************************/
    /**********************************************************************/
    delay(2000);
    /************************** DEBUT AFFICHAGE ***************************/
    affTempInt();
    affHumiInt();
    affHumiExt(humiExtStr);
    affTempExt(tempExtStr);
    affPressionExt(pressExtStr);
    affNiveauPluie(nivpluieExtStr);
    initLumiExt(lumiExtStr);
    isRaining(pluieExtStr);
    affPrevisions();
    affSmartNote();
  }
  /*************************** FIN AFFICHAGE ****************************/
}
/*********************** LES FONCTIONS ************************/
void transmet(long donnee,int protocole,int nombreBits){
  client.setProtocol(protocole);
  client.send(donnee,nombreBits);
}
////////////////////////////// LES FONCTIONS AFFICHAGES ///////////////////////
/**************************** AFFICHAGE INTERNE ******************************/
/******* AFFICHAGE HUMIDITE INTERIEURE ********/
void affHumiInt(){
  float lireHumi = dht.readHumidity();
  while(isnan(lireHumi)){lireHumi = dht.readHumidity();}//check si pas d erreur sinon recommence
  humiIntVal = lireHumi;
  String h = String (round(dht.readHumidity()*10));
  String humi;
  if(h.length()==3){
    if(String(h[2])!="0"){
      humi = String(h[0])+String(h[1])+"."+String(h[2])+"%";
    }else{
      humi = String(h[0])+String(h[1])+"%";
    }
  }else{
    if(String(h[1])!="0"){
      humi = "0"+String(h[0])+"."+String(h[1])+"%";
    }else{
      humi = "0"+String(h[0])+"%";
    }
  }
  ecrireNextion("tHumiInt",humi);
}
/******* AFFICHAGE TEMPERATURE INTERIEURE ********/
void affTempInt(){
  float lireTemp = dht.readTemperature();
  while(isnan(lireTemp)){lireTemp = dht.readTemperature();}//check si pas d erreur sinon recommence
  tempIntVal = lireTemp;
  int testCouleur = round(lireTemp);
  if(testCouleur<=0){
    ecrireNextion("tTempInt","pco",String(2047));
  }
  if(testCouleur>0 and testCouleur<=13){
    ecrireNextion("tTempInt","pco",String(65504));
  }
  if(testCouleur>13 and testCouleur<=21){
    ecrireNextion("tTempInt","pco",String(64611));
  }
  if(testCouleur>21){
    ecrireNextion("tTempInt","pco",String(63488));
  }
  long temp = lireTemp*100;
  String t;
  String tempInt;
  
  if(lireTemp<0){
    t = String(-temp);
    if(t.length()==2){
      t="00"+t;
    }
    if(t.length()==3){
      t="0"+t;
    }
    tempInt = "-"+String(t[0])+String(t[1])+"."+String(t[2])+"°C";
  }else{
    t = String(temp);
    if(t.length()==2){
      t="00"+t;
    }
    if(t.length()==3){
      t="0"+t;
    }
    tempInt = String(t[0])+String(t[1])+"."+String(t[2])+"°C";
    }
  ecrireNextion("tTempInt",tempInt);
}
/******* AFFICHAGE DE LA DATE  ********/
void affDate(){
  DateTime now = rtc.now(); // objet date
  int lireJour = now.dayOfTheWeek();
  int lireNumJour = now.day();
  int lireMois = now.month();
  int lireAnnee = now.year();
  while(isnan(lireJour)&&isnan(lireNumJour)&&isnan(lireMois)&&isnan(lireAnnee)){lireJour = now.dayOfTheWeek();lireNumJour = now.day();lireMois = now.month();lireAnnee = now.year();}
  ANNEE = lireAnnee;
  MOIS = lireMois;
  JOUR = lireNumJour;
  String date = ""+String(jours[lireJour])+" "+String(JOUR)+" "+String(mois[MOIS-1])+" "+String(ANNEE);
  ecrireNextion("tDate",date);
}
/******* AFFICHAGE DE L'HEURE  ********/
void affHeure(){
  DateTime now = rtc.now(); // objet date
  int lireMinute = now.minute();
  int lireHeure = now.hour();
  while(isnan(lireMinute)&& isnan(lireHeure)){lireMinute = now.minute();lireHeure = now.hour();}
  MINUTE = lireMinute;
  HEURE = lireHeure;
  String minutes;
  if(MINUTE<10){
    minutes = "0"+String(MINUTE);
  }else{
    minutes = String(MINUTE);
  }
  String heure = String(HEURE) + ":" + minutes;
  ecrireNextion("tHeure",heure);
}

/**************************** AFFICHAGE EXTERNE ******************************/
/******* AFFICHAGE TEMPERATURE EXTERIEURE a revoir********/ 
void affTempExt(String tempExt){
  int testCouleur;
  String temp;
  if(String(tempExt[0])=="1"){
    testCouleur = -(String(tempExt[1])+String(tempExt[2])).toInt();
    temp = "-"+String(tempExt[1])+String(tempExt[2])+"."+String(tempExt[3])+"°C";
  }else{
    testCouleur = (String(tempExt[1])+String(tempExt[2])).toInt();
    temp = String(tempExt[1])+String(tempExt[2])+"."+String(tempExt[3])+"°C";
  }
  tempExtVal = testCouleur;
  if(testCouleur<=0){
    ecrireNextion("tTempExt","pco",String(2047));
  }
  if(testCouleur>0 and testCouleur<=13){
    ecrireNextion("tTempExt","pco",String(65504));
  }
  if(testCouleur>13 and testCouleur<=21){
    ecrireNextion("tTempExt","pco",String(64611));
  }
  if(testCouleur>21){
    ecrireNextion("tTempExt","pco",String(63488));
  }
  ecrireNextion("tTempExt",temp);
}

/******* AFFICHAGE HUMIDITE EXTERIEURE ********/
void affHumiExt(String humiExt){//495 49.5% ou 049 4.9% sera tj a 3 chiffres
   int humiVal;
   String humi;
     if(String(humiExt[2])!="0"){
       humi = String(humiExt[0])+String(humiExt[1])+"."+String(humiExt[2])+"%";
     }else{
       humi = String(humiExt[0])+String(humiExt[1])+"%";
     }
     humiVal = humiExt.toInt();
     humiExtVal = humiVal/10;
     ecrireNextion("tHumiExt",humi);
}
   
/******* AFFICHAGE PRESSION EXTERIEURE ********/
void affPressionExt(String pressionExt){
  String pression;
  pression= String(pressionExt)+" hPa";
  pressExtVal = pressionExt.toInt()+deltaPression;
  ecrireNextion("tPression",pression);
}

/******* AFFICHAGE NIVEAU PLUIE ********/
void affNiveauPluie(String nivPluieExt){
  String rainLevel;
  rainLevel = nivPluieExt+"mm";
  ecrireNextion("tPluieMm",rainLevel);
}

/******* INITIALISE LUMI EXTERIEURE ********/
void initLumiExt(String lumiExt){
  String note;
  lumiExtVal = lumiExt.toInt();
  if(lumiExtVal<10){
    note = "Il fait nuit";
  }else{
    note = "Il fait jour";
  }
  infoJour = note;
}
/******* INITIALISE PLUIE EXTERIEURE ********/
void isRaining(String pluieEXT){
  String note;
  String effet;
  pluieExtVal = pluieEXT.toInt();
  if(pluieExtVal<40){
    note = "Temps sec";
    effet = "";
  }else{
    if(tempExtVal<=0 && pluieExtVal >= 40){
      note = "Il neige";
      effet = "   *     *        *    *   *         *         *  *   *      *      *             *";
    }else{
      note = "Il pleut";
      effet = "   '     '        '    '   '         '         '  '   '      '      '             ' ";
    }
  }
  infoPluie = note;
  ecrireNextion("gMeteo",effet);
}

/******* AFFICHAGE METEO ********/
void affPrevisions(){
  String note;
  String photo;
  if(pressExtVal<=1006){
    note = "Risque de pluie/orage";
    if(lumiExtVal<10){
      photo = "24";
    }else{ photo = "25";}
  }
  if(pressExtVal>1006 && pressExtVal<=1013){
    note = "Risque de pluie";
    if(lumiExtVal<10){
      photo = "24";
    }else{ photo = "21";}
  }
  if(pressExtVal>1013 && pressExtVal<=1020){
    note = "Assez beau temps";
    if(lumiExtVal<10){
      photo = "24";
    }else{ photo = "20";}
  }
  if(pressExtVal>1020){
    note = "Beau temps";
    if(lumiExtVal<10){
      photo = "23";
    }else{ photo = "26";}
  }
   infoPrev = note;
   ecrireNextion("pMeteo","pic",photo);
}
/******* AFFICHAGE RESEAU ********/
void DesactiveReseau(){
  isNetworkOn = 0;
  if(cptDeconnect == 0){
    cptDeconnect = 1;
  }
  ecrireNextion("pReseau","pic","18");
}
void ActiveReseau(){
  isNetworkOn = 1;
  cptDeconnect = 0;
  ecrireNextion("pReseau","pic","17");
}

/******* AFFICHAGE SMART NOTE ********/
void affSmartNote(){
  if (isNetworkOn == 1){
    smartNote = infoJour+" "+infoPluie+" "+infoPrev;
  }else{
    String minutes;
    String jour;
    String mois;
    if(cptDeconnect == 1){
      HEURE_DECONNEXION = HEURE;
      MIN_DECONNEXION = MINUTE;
      JOUR_DECONNEXION = JOUR;
      MOIS_DECONNEXION = MOIS;
      ANNEE_DECONNEXION = ANNEE;
      cptDeconnect = 2;
    }
    if(MIN_DECONNEXION<10){
      minutes = "0"+String(MIN_DECONNEXION);
    }else{
      minutes = String(MIN_DECONNEXION);
    }
    if(JOUR_DECONNEXION<10){
      jour = "0"+String(JOUR_DECONNEXION);
    }else{
     jour = String(JOUR_DECONNEXION);
    }
    if(MOIS_DECONNEXION<10){
      mois = "0"+String(MOIS_DECONNEXION);
    }else{
      mois = String(MOIS_DECONNEXION);
    }  
    smartNote = "Serveur déconnecté depuis le "+String(jour)+"/"+String(mois)+"/"+String(ANNEE_DECONNEXION)+" à "+String(HEURE_DECONNEXION)+":"+String(minutes);
  }
  ecrireNextion("gSmartNote",smartNote);
}
/*******************************************************************************************************************************************/
/****************************************************** FONCTIONS CALLBACK USER-SPACE ******************************************************/
/*******************************************************************************************************************************************/
//ecrireNextion("tHeure",String(PAGE));delay(2000);
//////////////// GO TO PAGE 0////////////////
void bRetour1PushCallback(void *ptr) {
  PAGE = 0;
}
void bValidTimePushCallback(void *ptr) {
  PAGE = 0;
  rtc.adjust(DateTime(REG_ANNEE, REG_MOIS, REG_JOUR, REG_HEURE, REG_MINUTE, 0));
}
void bValidPressPushCallback(void *ptr) {
  PAGE = 0;
  //code a venir
}

//////////////// GO TO PAGE 1////////////////
void bParamPushCallback(void *ptr) {
  PAGE = 1;
}
void bRetour2PushCallback(void *ptr) {
  PAGE = 1;
}
void bRetour3PushCallback(void *ptr) {
  PAGE = 1;
}

//////////////// GO TO PAGE 2////////////////
void bRegDateHeurePushCallback(void *ptr) {
  PAGE = 2;
  ecrireNextion("jour","val",String(JOUR));
  ecrireNextion("mois","val",String(MOIS));
  ecrireNextion("annee","val",String(ANNEE));
  ecrireNextion("heure","val",String(HEURE));
  ecrireNextion("minut","val",String(MINUTE));
  REG_JOUR = JOUR;
  REG_MOIS = MOIS;
  REG_ANNEE = ANNEE;
  REG_HEURE = HEURE;
  REG_MINUTE = MINUTE;
}

//////////////// GO TO PAGE 3////////////////
void bRegPressionPushCallback(void *ptr) {
  PAGE = 3;
}

////////////////// REG TIME /////////////////
void PjourPushCallback(void *ptr) {
  if(REG_JOUR<32){
    REG_JOUR += 1;
  }
  if(REG_JOUR==32){
    REG_JOUR = 1;
  }
  ecrireNextion("jour","val",String(REG_JOUR));
}
void MjourPushCallback(void *ptr) {
  if(REG_JOUR>0){
    REG_JOUR -= 1;
  }
  if(REG_JOUR==0){
    REG_JOUR = 31;
  }
  ecrireNextion("jour","val",String(REG_JOUR));
}
void PmoisPushCallback(void *ptr) {
  if(REG_MOIS<13){
    REG_MOIS += 1;
  }
  if(REG_MOIS==13){
    REG_MOIS = 1;
  }
  ecrireNextion("mois","val",String(REG_MOIS));
}
void MmoisPushCallback(void *ptr) {
  if(REG_MOIS>0){
    REG_MOIS -= 1;
  }
  if(REG_MOIS==0){
    REG_MOIS = 12;
  }
  ecrireNextion("mois","val",String(REG_MOIS));
}
void PanneePushCallback(void *ptr) {
  REG_ANNEE += 1;
  ecrireNextion("annee","val",String(REG_ANNEE));
} 
void ManneePushCallback(void *ptr) {
  REG_ANNEE -= 1;
  ecrireNextion("annee","val",String(REG_ANNEE));
} 
void PheurePushCallback(void *ptr) {
  if(REG_HEURE<24){
    REG_HEURE += 1;
  }
  if(REG_HEURE==24){
    REG_HEURE = 0;
  }
  ecrireNextion("heure","val",String(REG_HEURE));
}
void MheurePushCallback(void *ptr) {
  if(REG_HEURE>-1){
    REG_HEURE -= 1;
  }
  if(REG_HEURE==-1){
    REG_HEURE = 23;
  }
  ecrireNextion("heure","val",String(REG_HEURE));
}
void PminPushCallback(void *ptr) {
  if(REG_MINUTE<60){
    REG_MINUTE += 1;
  }
  if(REG_MINUTE==60){
   REG_MINUTE = 0;
  }
  ecrireNextion("minut","val",String(REG_MINUTE));
}
void MminPushCallback(void *ptr) {
  if(REG_MINUTE>-1){
    REG_MINUTE -= 1;
  }
  if(REG_MINUTE==-1){
    REG_MINUTE = 59;
  }
  ecrireNextion("minut","val",String(REG_MINUTE));
}
////////////////// REG BAROMETRE /////////////////
//code a venir

//FONCTIONS TRAITEMENT SPECIAL NEXTION
void ecrireNextion(String label_item,String texte){
  Serial.print(label_item);
  Serial.print(".txt=\"");
  Serial.print(texte);
  Serial.print("\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}
void ecrireNextion(String label_item,String ext,String nombre){
  Serial.print(label_item);
  Serial.print(".");
  Serial.print(ext);
  Serial.print("=");
  Serial.print(nombre);
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}
