#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Firebase.h>
#include <RTClib.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <Wire.h>

struct NextAktivacija
{
  uint8_t sat;
  uint8_t minuta;
  void setAktivaciju(uint8_t _sat, uint8_t _minuta)
  {
    sat = _sat;
    minuta = _minuta;
  }
};
struct PodaciUredjaj
{
  uint8_t izbacivanja = 0;
  DateTime *imaoObrokVrijeme = nullptr;
  DateTime *posljednjiUpdateVrijeme = nullptr;
  void setPodatke(uint8_t _izbacivanja, DateTime *_imaoObrokVrijeme, DateTime *_posljednjiUpdateVrijeme)
  {
    izbacivanja = _izbacivanja;
    if (_imaoObrokVrijeme != nullptr)
    {
      delete _imaoObrokVrijeme;
      imaoObrokVrijeme = _imaoObrokVrijeme;
    }
    if (_posljednjiUpdateVrijeme != nullptr)
    {
      delete _posljednjiUpdateVrijeme;
      posljednjiUpdateVrijeme = _posljednjiUpdateVrijeme;
    }
  }
};

void spajanjeNaInternet(bool = false);
String saveNewApiUrl();
String getApiUrl();
// void updateFirebase();
void updatePodatke();
void getNextAktivaciju();
void izvrsiAktivaciju();
void getBrojIzbacivanja();
void onPir();
void onTaster();
// ----------------KONSTANTE----------------
#define MOTOR_PIN 26
#define PIR_PIN 32
#define TASTER_PIN 33
#define SDA_PIN 21
#define SCL_PIN 22

#define FIREBASE_URL "https://petfeeder-28ccf-default-rtdb.europe-west1.firebasedatabase.app/"

String backendUrl = "";
const String getNextUrl = "/Aktivacija/GetNextAktivaciju?MacAdresa=";
const String updatePodatakaUrl = "/Uredjaj/UpdatePodataka";
const String getPodatkeZaUredjajUrl = "/Uredjaj/GetPodatke?mac=";

// ----------------VARIJABLE----------------
Firebase firebase(FIREBASE_URL);
RTC_DS3231 sat;

String macAdresa;
NextAktivacija *nextAktivacija = nullptr;
PodaciUredjaj podaciUredjaj = PodaciUredjaj();
//----------------INICIJALIZACIJA----------------

void setup()
{
  Serial.begin(115200);

  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(TASTER_PIN, INPUT_PULLUP);
  digitalWrite(MOTOR_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), onPir, RISING);
  attachInterrupt(digitalPinToInterrupt(TASTER_PIN), onTaster, FALLING);

  spajanjeNaInternet();

  Wire.begin(SDA_PIN, SCL_PIN);
  while (!sat.begin())
  {
    Serial.println("Sat nije pronadjen!");
    delay(100);
  }

  macAdresa = WiFi.macAddress();

  getNextAktivaciju();
  getPodatkeZaUredjaj();
}

//----------------MAIN----------------
void loop()
{
  if (firebase.getInt(macAdresa + "/zaboraviWifi") == 1)
  {
    firebase.setInt(macAdresa + "/zaboraviWifi", -1);
    spajanjeNaInternet(true);
  }
  if (sat.now().hour() == 0 && podaciUredjaj.izbacivanja > 0 && (podaciUredjaj.imaoObrokVrijeme == nullptr || podaciUredjaj.imaoObrokVrijeme->hour() != 0))
  {
    podaciUredjaj.izbacivanja = 0;
    updatePodatke();
  }
  if (firebase.getInt(macAdresa + "/uredjajAktivan") == 1)
  {
    if (firebase.getInt(macAdresa + "/dodajHranu") == 1)
    {
      izvrsiAktivaciju();
      firebase.setInt(macAdresa + "/dodajHranu", -1);
    }
    if (firebase.getInt(macAdresa + "/aktivacijeIzmijenjene") == 1)
    {
      getNextAktivaciju();
      firebase.setInt(macAdresa + "/aktivacijeIzmijenjene", -1);
    }
    if (nextAktivacija != nullptr && (sat.now().hour() == nextAktivacija->sat && sat.now().minute() == nextAktivacija->minuta))
    {
      izvrsiAktivaciju();
      getNextAktivaciju();
    }
    if (podaciUredjaj.posljednjiUpdateVrijeme == nullptr || (podaciUredjaj.posljednjiUpdateVrijeme->minute() + 2 == sat.now().minute()))
      updatePodatke();
  }
}
String saveNewApiUrl()
{
  HTTPClient http;
  http.begin("https://raw.githubusercontent.com/is-rijad/PetFeeder/refs/heads/master/urls.json");
  http.setTimeout(5000);

  int code = http.GET();
  if (code != 200)
  {
    http.end();
    return "";
  }

  JsonDocument doc;
  deserializeJson(doc, http.getString());

  url = doc["url"].as<String>();
  http.end();
  return url;
}
String getApiUrl()
{
  HTTPClient http;
  http.begin(url + "status");
  http.setTimeout(5000);
  int code = http.GET();
  if (code != 200)
  {
    url = saveNewApiUrl();
  }
  return url;
}
void spajanjeNaInternet(bool zaboraviWifi)
{
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
void updatePodatke()
{
  HTTPClient http;
  JsonDocument doc;
  doc["mac"] = macAdresa;
  doc["izbacivanja"] = podaciUredjaj.izbacivanja;
  if (podaciUredjaj.imaoObrokVrijeme != nullptr)
  {
    doc["imaoObrokVrijeme"] = podaciUredjaj.imaoObrokVrijeme->timestamp();
  }
  doc["posljednjiUpdateVrijeme"] = sat.now().timestamp();
  String object;
  serializeJson(doc, object);
  String url = getApiUrl() + updatePodatakaUrl;
  http.begin(url.c_str());
  http.POST(object);
  http.end();
}
void getNextAktivaciju()
{
  HTTPClient http;
  String url = getApiUrl() + getNextUrl + macAdresa;
  http.begin(url.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200)
  {
    JsonDocument doc;
    deserializeJson(doc, http.getString());
    nextAktivacija->setAktivaciju(doc["sat"], doc["minuta"]);
  }
  else
  {
    delete nextAktivacija;
    nextAktivacija = nullptr;
  }
  http.end();
}
void getPodatkeZaUredjaj()
{
  HTTPClient http;
  String url = getApiUrl() + getPodatkeZaUredjajUrl + macAdresa;
  http.begin(url.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200)
  {
    JsonDocument doc;
    deserializeJson(doc, http.getString());
    DateTime *_imaoObrokVrijeme = nullptr;
    DateTime *_posljednjiUpdateVrijeme = nullptr;
    if (!doc["imaoObrokVrijeme"].isNull())
    {
      _imaoObrokVrijeme = new DateTime(doc["imaoObrokVrijeme"].as<uint32_t>());
    }
    if (!doc["posljednjiUpdateVrijeme"].isNull())
    {
      _posljednjiUpdateVrijeme = new DateTime(doc["posljednjiUpdateVrijeme"].as<uint32_t>());
    }
    podaciUredjaj.setPodatke(doc["izbacivanja"], _imaoObrokVrijeme, _posljednjiUpdateVrijeme);
  }
  http.end();
}
void wmConfigPortalTimeout()
{
  ESP.restart();
}
void izvrsiAktivaciju()
{
  digitalWrite(MOTOR_PIN, HIGH);
  digitalWrite(MOTOR_PIN, LOW);
  podaciUredjaj.izbacivanja++;
  *podaciUredjaj.imaoObrokVrijeme = sat.now();
}
void onPir()
{
  if (firebase.getInt(macAdresa + "/upaljenSenzor") == 1)
  {
    izvrsiAktivaciju();
  }
}
void onTaster()
{
  podaciUredjaj.izbacivanja = 0;
  updatePodatke();
}
