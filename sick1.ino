#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <WiFiClientSecure.h>

// WiFi credentials
const char* Nama_WIFI = "CS 2.0";
const char* Kata_Sandi = "12345678";

// MQTT credentials and server details
const char* Hive_url = "c195ef0f5f534d53b72397d524b180f3.s1.eu.hivemq.cloud";
const char* Hive_nama = "hivemq.webclient.1718283629828";
const char* Hive_pw = "3bRf;8VvK#qd7$AH0<Mu";
const int Hive_port = 8883;

// CA certificate for SSL/TLS
static const char* sertifikat_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// DHT sensor settings
#define PinDht 27
#define TypeDht DHT11
DHT dht(PinDht, TypeDht);

WiFiClientSecure espclient;
PubSubClient client(espclient);
unsigned long last = 0;

void setup_wifi() {
  Serial.begin(115200);
  delay(1000);
  Serial.print("\n connecting WIFI ");
  Serial.println(Nama_WIFI);

  WiFi.mode(WIFI_STA);
  WiFi.begin(Nama_WIFI, Kata_Sandi);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("GET di [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("connecting MQTT...");
    if (client.connect("ESP32Client", Hive_nama, Hive_pw)) {
      Serial.println("MQTT connected!");
      client.subscribe("testTopic");
    } else {
      Serial.print("Error code..");
      Serial.print(client.state());
      Serial.println(". try again at 5s");
      delay(5000);
    }
  }
}

void sendToFlask(String data) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = "http://192.168.1.13:5000/receive-data";

    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(data);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP response... ");
      Serial.println(httpResponseCode);
      Serial.print("Respon: ");
      Serial.println(response);
    } else {
      Serial.print("POST error.... ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  setup_wifi();

  espclient.setCACert(sertifikat_ca);
  client.setServer(Hive_url, Hive_port);
  client.setCallback(callback);

  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - last > 2000) {
    last = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Error sensor DHT!");
      return;
    }

    String payload = "{\"temperature\": ";
    payload += String(t);
    payload += ", \"humidity\": ";
    payload += String(h);
    payload += "}";

    Serial.print("Publishing message: ");
    Serial.println(payload);

    client.publish("sensor/suhu",  temperaturePayload.c_str());
     client.publish("sensor/lembab", humidityPayload.c_str());

    sendToFlask(payload);
  }
}
