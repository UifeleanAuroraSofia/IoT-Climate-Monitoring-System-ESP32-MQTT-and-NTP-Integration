#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHTesp.h>

// ntp
#include <WiFiUdp.h>
#include <NTPClient.h>

// -------------------- pinuri --------------------
// pinul 13 este conectat la senzorul dht22
const int DHT_PIN = 13;

// ------------------ obiect dht ------------------
// aici folosim clasa dhtesp pentru a citi valorile de la senzor
DHTesp dht;

// ----------------- credențiale wifi -------------
// ssid si parola pentru conectarea la reteaua wokwi-guest
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ------------------- emqx cloud tls -------------
// datele pentru conectarea la un broker emqx cloud pe portul tls 8883
const char* mqtt_server   = "p96f9d2e.ala.eu-central-1.emqxsl.com";
const int   mqtt_port     = 8883;
const char* mqtt_username = "Esp";
const char* mqtt_passwordM = "Cevarandom2002";

// --------- obiect wificlientsecure & pubsubclient
WiFiClientSecure espClientSecure;
PubSubClient client(espClientSecure);

// --- obiecte pentru ntp ---
// folosim wifiudp si ntpclient pentru a obtine data si ora actuala
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// interval publicare
unsigned long lastMsg = 0;
const unsigned long PUBLISH_INTERVAL = 5000; // trimitem mesaje la fiecare 5 secunde

// ------------------- topicuri -------------------
// definitiile pentru topicurile folosite
const char* topicTemp = "/PTIT_Test/p/temp1";
const char* topicHum  = "/PTIT_Test/p/hum1";
const char* topicDate = "/PTIT_Test/p/monitoring_date";

// variabile globale
// lastmonitoring stocheaza ultimul mesaj legat de data/ora monitorizarii
// validmonitoring ne indica daca avem deja o valoare valida din broker
String lastMonitoring = "Necunoscuta";
bool validMonitoring = false;

// ---------------------------------------------------------------------------
// getformatteddatetime:
// functia primeste epoca in secunde si intoarce un string in formatul "yyyy-mm-dd hh:mm:ss"
String getFormattedDateTime(unsigned long rawTime) {
  time_t t = rawTime;
  struct tm* ti = localtime(&t);

  char buffer[25];
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
          ti->tm_year + 1900,
          ti->tm_mon + 1,
          ti->tm_mday,
          ti->tm_hour,
          ti->tm_min,
          ti->tm_sec
         );
  return String(buffer);
}

// ---------------------------------------------------------------------------
// setup_wifi:
// se conecteaza la reteaua wifi folosind ssid si parola definite mai sus
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nwifi connected!");
  Serial.println(WiFi.localIP());
}

// ---------------------------------------------------------------------------
// callback:
// aceasta functie este apelata automat de pubsubclient cand primim un mesaj de la broker
void callback(char* topic, byte* payload, unsigned int length) {
  // construim stringul care contine mesajul receptionat
  String message;
  for (int i = 0; i < length; i++) {
    message += (char) payload[i];
  }

  Serial.print("mesaj nou pe topic: ");
  Serial.println(topic);
  Serial.print("continut: ");
  Serial.println(message);

  // daca topicul este cel referitor la data monitorizarii, stocam mesajul in lastmonitoring
  if (strcmp(topic, topicDate) == 0) {
    validMonitoring = true;
    lastMonitoring = message;
    Serial.println(">> ultima monitorizare retinuta: " + lastMonitoring);
  }
}

// ---------------------------------------------------------------------------
// reconnect:
// functia care se ocupa de reconectarea la brokerul mqtt in cazul in care conexiunea este pierduta
void reconnect() {
  // bucla pana cand suntem conectati
  while (!client.connected()) {
    Serial.print("attempting mqtt connection... ");
    // generam un clientid diferit pentru fiecare conexiune
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // incercam sa ne conectam la broker cu user si parola
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_passwordM)) {
      Serial.print("conectat => ");
      Serial.println(clientId);

      // ne abonam la topicul care retine data monitorizarii (retained)
      client.subscribe(topicDate);
      Serial.print("ne-am abonat la: ");
      Serial.println(topicDate);

      // odata ce ne reconectam, generam un nou mesaj care contine data curenta
      unsigned long epochTime = timeClient.getEpochTime();
      String dateTimeStr = getFormattedDateTime(epochTime);
      String msg = "monitorizare noua la data: " + dateTimeStr;

      // publicam acest mesaj cu retain = true, astfel brokerul il pastreaza pentru urmatoarea sesiune
      client.publish(topicDate, msg.c_str(), true);
      Serial.println("am publicat (retained) => " + msg);

    } else {
      Serial.print("esec (rc=");
      Serial.print(client.state());
      Serial.println("). reincearc in 5s...");
      delay(5000);
    }
  }
}

// ---------------------------------------------------------------------------
// setup:
// functia standard de initializare, se ruleaza o singura data la pornire
void setup() {
  Serial.begin(115200);

  // wifi
  setup_wifi();

  // ntp
  timeClient.begin();

  // tls insecure (nu validam certificatul)
  espClientSecure.setInsecure();

  // test handshake direct (optional), doar pentru a vedea daca ne putem conecta la server
  if (espClientSecure.connect(mqtt_server, mqtt_port)) {
    Serial.println("handshake tls ok");
    espClientSecure.stop();
  }

  // configuram pubsubclient cu datele brokerului si stabilim functia de callback
  client.setServer(mqtt_server, mqtt_port);

  // daca nu avem deja un mesaj valid de monitorizare, setam callback-ul
  if (!validMonitoring)
    client.setCallback(callback);

  // configuram dht (tip dht22)
  dht.setup(DHT_PIN, DHTesp::DHT22);
}

// ---------------------------------------------------------------------------
// loop:
// se ruleaza continuu si mentine conexiunea mqtt, citind si publicand la intervale stabilite
void loop() {
  // actualizam datele ntp pentru a avea ora curenta
  timeClient.update();

  // daca nu suntem conectati la broker, incercam reconectarea
  if (!client.connected()) {
    reconnect();
  }
  // mentinem conexiunea mqtt si procesam eventualele mesaje primite
  client.loop();

  // la fiecare publish_interval milisecunde, citim senzorul si publicam noile valori
  unsigned long now = millis();
  if (now - lastMsg > PUBLISH_INTERVAL) {
    lastMsg = now;

    // citim temperatura si umiditatea din dht22
    TempAndHumidity data = dht.getTempAndHumidity();
    float temperature = data.temperature;
    float humidity    = data.humidity;

    // publicam temperatura pe topicul topictemp
    String tempStr = String(temperature, 2);
    String msgTemp = "temperatura: ";
    msgTemp += tempStr;
    client.publish(topicTemp, msgTemp.c_str());
    Serial.print("temperatura => ");
    Serial.println(tempStr);

    // publicam umiditatea pe topicul topichum
    String humStr = String(humidity, 1);
    String msgHum = "umiditate: ";
    msgHum += humStr;
    client.publish(topicHum, msgHum.c_str());
    Serial.print("umiditate => ");
    Serial.println(humStr);

    Serial.println("----------------------");
  }
}
