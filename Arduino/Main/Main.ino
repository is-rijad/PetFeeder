#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Firebase.h>
#include <RTClib.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>

// #include <HX711_ADC.h>
// #include <EEPROM.h>
// #include <Stepper.h>


struct NextAktivacija {
 uint8_t tezina;
 uint8_t sat;
 uint8_t minuta;
 NextAktivacija() {
   tezina = 0;
   sat = 0;
   minuta = 0;
 }
 void setAktivaciju(uint8_t _tezina, uint8_t _sat, uint8_t _minuta)  {
   tezina = _tezina;
   sat = _sat;
   minuta = _minuta;
 }
};

void spajanjeNaInternet(bool = false);
float usIzmjeriDuzinu();
int getNivoVodePosuda();
int getNivoVodeSpremnik();
bool imaLiHraneSpremnik();
// void updateFirebase();
void updatePodatke();
void getNextAktivaciju();
// void izvrsiAktivaciju(uint8_t);
void izvrsiAktivaciju(uint8_t);
void sipajVodu();
// void postaviVagu();
// float getTezinuPosuda();
// ----------------KONSTANTE----------------
#define FIREBASE_URL "https://petfeeder-28ccf-default-rtdb.europe-west1.firebasedatabase.app/"

#define VODA_POSUDA 35
#define VODA_SPREMNIK 36
#define POSUDA_MIN 320
#define POSUDA_MAX 2400
#define SPREMNIK_MIN 350
#define SPREMNIK_MAX 480

// #define MOTOR_IN1 19
// #define MOTOR_IN2 18
// #define MOTOR_IN3 5
// #define MOTOR_IN4 17

#define PUMPA_RELEJ 25
#define MOTOR_RELEJ 17

#define US_TRIG_PIN 14
#define US_ECHO_PIN 12
#define HX711_DOUT_PIN 16
#define HX711_SCK_PIN 4

// const int stepsPerRevolution = 200;
// const int pomakniZa = 100;
const String getNextUrl = "https://api.p2362.app.fit.ba/Aktivacija/GetNextAktivaciju?MacAdresa=";
const String updatePodatakaUrl = "https://api.p2362.app.fit.ba/Uredjaj/UpdatePodataka";




// ----------------VARIJABLE----------------
Firebase firebase(FIREBASE_URL);
// Stepper stepperMotor(stepsPerRevolution, MOTOR_IN1, MOTOR_IN3, MOTOR_IN2, MOTOR_IN4);
// HX711_ADC LoadCell(HX711_DOUT_PIN, HX711_SCK_PIN);
RTC_DS3231 sat;

String macAdresa;
DateTime posljednjiUpdate = DateTime();
DateTime posljednjeSipanjeVode = DateTime();
NextAktivacija nextAktivacija = NextAktivacija();
//----------------INICIJALIZACIJA----------------

void setup() {
  Serial.begin(115200);

  spajanjeNaInternet();

  // postaviVagu();

  while (!sat.begin()) {
   Serial.println("Sat nije pronadjen!");
   delay(100);
  }
  pinMode(VODA_POSUDA, INPUT);
  pinMode(VODA_SPREMNIK, INPUT);

  pinMode(PUMPA_RELEJ, OUTPUT);
  digitalWrite(PUMPA_RELEJ, HIGH);

  pinMode(MOTOR_RELEJ, OUTPUT);
  digitalWrite(MOTOR_RELEJ, HIGH);

  pinMode(US_TRIG_PIN, OUTPUT);
  pinMode(US_ECHO_PIN, INPUT);

  // stepperMotor.setSpeed(30);

  macAdresa = WiFi.macAddress();

  getNextAktivaciju();
}

//----------------MAIN----------------
void loop() {
  if(firebase.getInt(macAdresa + "/zaboraviWifi") == 1) {
   firebase.setInt(macAdresa + "/zaboraviWifi", -1);
   spajanjeNaInternet(true);
  }
  if (firebase.getInt(macAdresa + "/uredjajAktivan") == 1) {
   if (firebase.getInt(macAdresa + "/dodajHranu") == 1) {
       izvrsiAktivaciju(firebase.getInt(macAdresa + "/dodajHraneDo"));
       firebase.setInt(macAdresa + "/dodajHranu", -1);
   }
   if (firebase.getInt(macAdresa + "/dodajVodu") == 1) {
       sipajVodu();
       firebase.setInt(macAdresa + "/dodajVodu", -1);
   }
   if(firebase.getInt(macAdresa + "/aktivacijeIzmijenjene") == 1 || nextAktivacija.tezina <= 0) {
     getNextAktivaciju();
     firebase.setInt(macAdresa + "/aktivacijeIzmijenjene", -1);
   }
   if (sat.now().hour() == nextAktivacija.sat && sat.now().minute() == nextAktivacija.minuta) {
     uint8_t brojPokusaja = 0;
     while(nextAktivacija.tezina <= 0 && brojPokusaja < 5) {
       getNextAktivaciju();
       brojPokusaja++;
     }
     if (brojPokusaja < 5) {
       izvrsiAktivaciju(nextAktivacija.tezina);
       getNextAktivaciju();
     }
   }
   if ((posljednjeSipanjeVode.hour() + 1 == sat.now().hour() && posljednjeSipanjeVode.minute() == sat.now().minute())
       || posljednjeSipanjeVode.secondstime() == 0) {
     sipajVodu();
   }
   if((posljednjiUpdate.minute() + 2 == sat.now().minute()) || posljednjiUpdate.secondstime() == 0) 
     updatePodatke();
  }
}
void spajanjeNaInternet(bool zaboraviWifi) {
 WiFiManager wm;
 if(zaboraviWifi)
   wm.resetSettings();
 WiFi.mode(WIFI_STA);
 wm.setHostname("Pet Feeder WiFi");
 wm.setConfigPortalTimeout(60);
 wm.setHostname("pf.wifi.com");
 wm.setConfigPortalTimeoutCallback(wmConfigPortalTimeout);
 WiFiManagerParameter macAdresa("macAdresa", "MAC adresa<br>Sačuvajte je radi dodavanja uređaja", WiFi.macAddress().c_str(), 20, "<input type=\"text\" readonly=\"true\"");
 wm.addParameter(&macAdresa);
 wm.autoConnect("Pet Feeder");
}
float usIzmjeriDuzinu() {
 float trajanjePulsa;

 digitalWrite(US_TRIG_PIN, HIGH);
 delayMicroseconds(50);
 digitalWrite(US_TRIG_PIN, LOW);

 trajanjePulsa = pulseIn(US_ECHO_PIN, HIGH);

 return (0.017 * trajanjePulsa);
}
int getNivoVodePosuda() {
 int rezultat = map(analogRead(VODA_POSUDA), POSUDA_MIN, POSUDA_MAX, 0, 10);
 return (rezultat > 0) ? rezultat : 0;
}
int getNivoVodeSpremnik() {
 int rezultat = map(analogRead(VODA_SPREMNIK), SPREMNIK_MIN, SPREMNIK_MAX, 0, 3);
 return (rezultat > 0) ? rezultat : 0;
}

bool imaLiHraneSpremnik() {
 float duzina = usIzmjeriDuzinu();
 return (duzina < 8.7 || duzina > 9.4) ? true : false;
}
// void updateFirebase() {
//    firebase.setInt(macAdresa + "/hrana", 0); //IZMJENA
//    firebase.setInt(macAdresa + "/voda", getNivoVodePosuda()); 
//    firebase.setFloat(macAdresa + "/hranaSpremnik", imaLiHraneSpremnik()); 
//    firebase.setInt(macAdresa + "/vodaSpremnik", getNivoVodeSpremnik()); 
//    firebase.setString(macAdresa + "/imaoObrok", sat.now().timestamp()); //IZMJENA
//    firebase.setFloat(macAdresa + "/imaoObrokPojeo", 0.0); //IZMJENA
//    posljednjiUpdate = sat.now();
//    firebase.setString(macAdresa + "/posljednjiUpdate", posljednjiUpdate.timestamp());
// }
void updatePodatke() {
  HTTPClient http;
  JsonDocument doc;
  doc["mac"] = macAdresa;
  doc["hranaPosuda"] = -1;
  doc["hranaSpremnik"] = imaLiHraneSpremnik();
  doc["vodaPosuda"] = getNivoVodePosuda();
  doc["vodaSpremnik"] = getNivoVodeSpremnik();
  doc["imaoObrokVrijeme"] = sat.now().timestamp();
  doc["imaoObrokPojeo"] = 0;
  doc["posljednjiUpdateVrijeme"] = sat.now().timestamp();
  String object;
  serializeJson(doc, object);
  http.POST(object);
  http.end();
}
void getNextAktivaciju() {
 HTTPClient http;
 String url = getNextUrl + macAdresa;
 http.begin(url.c_str());
 int httpResponseCode = http.GET();

 if (httpResponseCode == 200) {
   JsonDocument doc;
   deserializeJson(doc, http.getString());
   nextAktivacija.setAktivaciju(doc["tezina"], doc["sat"], doc["minuta"]);
 }
 http.end();
}
void wmConfigPortalTimeout() {
 ESP.restart();
}
// void izvrsiAktivaciju(uint8_t ciljanaTezina) {
//  if (usIzmjeriDuzinu() < ciljanaTezina - 10 && imaLiHraneSpremnik()) {
//    stepperMotor.step(-pomakniZa);
//    while(1) {
//      if (usIzmjeriDuzinu() > ciljanaTezina - 5) {
//        stepperMotor.step(pomakniZa);
//        break;
//      }
//    }
//  }
// }
void izvrsiAktivaciju(uint8_t sekundi = 10) {
  digitalWrite(MOTOR_RELEJ, HIGH);
  uint32_t vrijemePocetka = sat.now().secondstime();
  uint32_t vrijemeKraja = vrijemePocetka + sekundi;
  while(sat.now().secondstime() >= vrijemeKraja){}
  digitalWrite(MOTOR_RELEJ, LOW);
}
void sipajVodu() {
 if (getNivoVodePosuda() < 9 && getNivoVodeSpremnik() >= 1) {
   digitalWrite(PUMPA_RELEJ, LOW);
   while(1) {
     if (getNivoVodePosuda() >= 9 || getNivoVodeSpremnik() < 1) {
       digitalWrite(PUMPA_RELEJ, HIGH);
       posljednjeSipanjeVode = sat.now();
       break;
     }
   }
 }
}
// void postaviVagu() {
//   const int kalibracijaEEPROMAdresa = 0;
//   LoadCell.begin();
//   float kalibracijskiFaktor; 
//   EEPROM.begin(512);
//   EEPROM.get(kalibracijaEEPROMAdresa, kalibracijskiFaktor);

//   unsigned long vrijemeStabilizacije = 2000;
//   boolean tare = true;
//   LoadCell.start(vrijemeStabilizacije, tare);
//   while (LoadCell.getTareTimeoutFlag()) {
//     Serial.println("Neuspjesno postavljanje! Provjeri HX711!");
//   }
//   LoadCell.setCalFactor(kalibracijskiFaktor);
// }
// float getTezinuPosuda() {
//   if (LoadCell.update()) {
//     return LoadCell.getData();
//   }
//   else {
//     return -1;
//   }
// }
