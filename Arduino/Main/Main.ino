#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Firebase.h>
#include <RTClib.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Wire.h>

struct NextAktivacija {
  uint8_t sat;
  uint8_t minuta;
  void setAktivaciju(uint8_t _sat, uint8_t _minuta) {
    sat = _sat;
    minuta = _minuta;
  }
};
struct PodaciUredjaj {
  uint8_t izbacivanja = 0;
  DateTime* imaoObrokVrijeme = nullptr;
  void setPodatke(uint8_t _izbacivanja, DateTime* _imaoObrokVrijeme) {
    izbacivanja = _izbacivanja;
    if (_imaoObrokVrijeme != nullptr) {
      delete _imaoObrokVrijeme;
      imaoObrokVrijeme = _imaoObrokVrijeme;
    }
  }
};

void spajanjeNaInternet(bool = false);
String saveNewApiUrl();
String getApiUrl();
void updatePodatke();
void getNextAktivaciju(int* izvrsenaHour, int* izvrsenaMinute);
void izvrsiAktivaciju();
// ----------------KONSTANTE----------------
#define MOTOR_PIN 26
#define SDA_PIN 21
#define SCL_PIN 22

#define FIREBASE_URL "https://petfeeder-28ccf-default-rtdb.europe-west1.firebasedatabase.app/"

const String getNextUrl = "/Aktivacija/GetNextAktivaciju?MacAdresa=";
const String updatePodatakaUrl = "/Uredjaj/UpdatePodataka";
const String getPodatkeZaUredjajUrl = "/Uredjaj/GetPodatke?mac=";

// ----------------VARIJABLE----------------
Firebase firebase(FIREBASE_URL);
RTC_DS3231 sat;

String macAdresa;
String backendUrl = "";
NextAktivacija* nextAktivacija = nullptr;
PodaciUredjaj podaciUredjaj = PodaciUredjaj();
//----------------INICIJALIZACIJA----------------

void setup() {
  Serial.begin(115200);

  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  spajanjeNaInternet();
  macAdresa = WiFi.macAddress();

  Wire.begin(SDA_PIN, SCL_PIN);
  while (!sat.begin()) {
    Serial.println("Sat nije pronadjen!");
    delay(100);
  }

  backendUrl = getApiUrl();
  getNextAktivaciju(nullptr, nullptr);
  getPodatkeZaUredjaj();
}

//----------------MAIN----------------
void loop() {
  if (firebase.getInt(macAdresa + "/zaboraviWifi") == 1) {
    firebase.setInt(macAdresa + "/zaboraviWifi", -1);
    spajanjeNaInternet(true);
  }
  if (firebase.getInt(macAdresa + "/uredjajAktivan") == 1) {
    if (sat.now().hour() == 0 && sat.now().minute() == 0) {
      getNextAktivaciju(nullptr, nullptr);
    }
    if (firebase.getInt(macAdresa + "/resetujIzbacivanja") == 1) {
      podaciUredjaj.izbacivanja = 0;
      firebase.setInt(macAdresa + "/resetujIzbacivanja", -1);
    }
    if (firebase.getInt(macAdresa + "/dodajHranu") == 1) {
      izvrsiAktivaciju();
      updatePodatke();
      firebase.setInt(macAdresa + "/dodajHranu", -1);
    }
    if (firebase.getInt(macAdresa + "/aktivacijeIzmijenjene") == 1) {
      getNextAktivaciju(nullptr, nullptr);
      firebase.setInt(macAdresa + "/aktivacijeIzmijenjene", -1);
    }
    if (nextAktivacija != nullptr && (sat.now().hour() == nextAktivacija->sat && sat.now().minute() == nextAktivacija->minuta)) {
      izvrsiAktivaciju();
      updatePodatke();
      getNextAktivaciju(new int(sat.now().hour()), new int(sat.now().minute()));
    }
  }
}
String saveNewApiUrl() {
  HTTPClient http;
  http.begin("https://raw.githubusercontent.com/is-rijad/PetFeeder/refs/heads/master/urls.json");
  http.setTimeout(5000);

  int code = http.GET();
  if (code <= 0) {
    http.end();
    backendUrl = "";
    return "";
  }

  JsonDocument doc;
  deserializeJson(doc, http.getString());

  backendUrl = (doc["url"].as<String>()) + "/api";
  http.end();
  return backendUrl;
}
String getApiUrl() {
  HTTPClient http;
  http.begin(backendUrl + "/status");
  http.setTimeout(5000);
  int code = http.GET();
  if (code <= 0) {
    backendUrl = saveNewApiUrl();
  }
  return backendUrl;
}
void spajanjeNaInternet(bool zaboraviWifi) {
  WiFiManager wm;
  if (zaboraviWifi)
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
void updatePodatke() {
  HTTPClient http;
  JsonDocument doc;
  doc["mac"] = macAdresa;
  doc["izbacivanja"] = podaciUredjaj.izbacivanja;
  if (podaciUredjaj.imaoObrokVrijeme != nullptr) {
    char buf[25];
    snprintf(buf, sizeof(buf),
             "%04d-%02d-%02dT%02d:%02d:%02dZ",
             podaciUredjaj.imaoObrokVrijeme->year(),
             podaciUredjaj.imaoObrokVrijeme->month() + 1,
             podaciUredjaj.imaoObrokVrijeme->day(),
             podaciUredjaj.imaoObrokVrijeme->hour(),
             podaciUredjaj.imaoObrokVrijeme->minute(),
             podaciUredjaj.imaoObrokVrijeme->second());
    doc["imaoObrokVrijeme"] = String(buf);
  }
  String object;
  serializeJson(doc, object);
  String url = getApiUrl() + updatePodatakaUrl;
  http.begin(url.c_str());
  http.addHeader("Content-Type", "application/json");
  http.POST(object);
  http.end();
  firebase.setInt(macAdresa + "/hranaIzbacena", 1);
}
void getNextAktivaciju(int* izvrsenaHour, int* izvrsenaMinute) {
  HTTPClient http;
  String izvrsenaQueryParam = "";
  if (izvrsenaHour != nullptr && izvrsenaMinute != nullptr) {
    izvrsenaQueryParam = String("&izvrsenaHour=") + *izvrsenaHour + String("&izvrsenaMinute=") + *izvrsenaMinute;
  }
  String url = getApiUrl() + getNextUrl + macAdresa + izvrsenaQueryParam;
  http.begin(url.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    JsonDocument doc;
    deserializeJson(doc, http.getString());
    if (nextAktivacija != nullptr) {
      delete nextAktivacija;
    }
    nextAktivacija = new NextAktivacija();
    nextAktivacija->setAktivaciju(doc["sat"], doc["minuta"]);
  } else {
    delete nextAktivacija;
    nextAktivacija = nullptr;
  }
  delete izvrsenaHour, izvrsenaMinute;
  http.end();
}
void getPodatkeZaUredjaj() {
  HTTPClient http;
  String url = getApiUrl() + getPodatkeZaUredjajUrl + macAdresa;
  http.begin(url.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    JsonDocument doc;
    deserializeJson(doc, http.getString());
    podaciUredjaj.setPodatke(doc["izbacivanja"], nullptr);
  }
  http.end();
}
void wmConfigPortalTimeout() {
  ESP.restart();
}
void izvrsiAktivaciju() {
  if (podaciUredjaj.izbacivanja == 4) {
    return;
  }
  digitalWrite(MOTOR_PIN, HIGH);
  delay(30);
  digitalWrite(MOTOR_PIN, LOW);
  podaciUredjaj.izbacivanja++;
  if (podaciUredjaj.imaoObrokVrijeme != nullptr) {
    delete podaciUredjaj.imaoObrokVrijeme;
  }
  podaciUredjaj.imaoObrokVrijeme = new DateTime(sat.now());
}