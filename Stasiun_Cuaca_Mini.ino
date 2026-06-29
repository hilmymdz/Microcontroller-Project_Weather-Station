#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>

// ==========================================
// PENGATURAN PIN
// ==========================================
#define DHTPIN 4          
#define DHTTYPE DHT22     
#define RELAY_PIN 14      

// ==========================================
// PENGATURAN WIFI & MQTT SHIFTR.IO
// ==========================================
const char* ssid = "H Pixel 6a";         
const char* password = "pixelgo6a"; 

const char* mqtt_server = "uas-cuaca.cloud.shiftr.io";       
const char* mqtt_user = "uas-cuaca";    
const char* mqtt_password = "6c0283NBx54lgA0b";  

// Definisi Topik
const char* topic_suhu = "uas/cuaca/suhu";
const char* topic_kelembapan = "uas/cuaca/kelembapan";
const char* topic_tekanan = "uas/cuaca/tekanan";
const char* topic_cahaya = "uas/cuaca/cahaya";
const char* topic_kipas = "uas/cuaca/kipas";
const char* topic_perintah = "uas/cuaca/kendali_kipas"; // TOPIK BARU UNTUK TERIMA PERINTAH

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;
BH1750 lightMeter;

unsigned long lastMsg = 0;
const long interval = 2000; 
const float BATAS_SUHU = 30.0; 

// Variabel Mode Kipas: 0 = Auto, 1 = Manual ON, 2 = Manual OFF
int modeKipas = 0; 

// ==========================================
// FUNGSI CALLBACK (Menerima Pesan dari Web)
// ==========================================
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";
  for (int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }
  
  Serial.print("Pesan masuk di topik: ");
  Serial.print(topic);
  Serial.print(" | Isi: ");
  Serial.println(pesan);

  // Mengubah mode berdasarkan perintah dari Web
  if (String(topic) == topic_perintah) {
    if (pesan == "AUTO") {
      modeKipas = 0;
      Serial.println("Mode diubah ke: AUTO");
    } else if (pesan == "ON") {
      modeKipas = 1;
      Serial.println("Mode diubah ke: MANUAL ON");
    } else if (pesan == "OFF") {
      modeKipas = 2;
      Serial.println("Mode diubah ke: MANUAL OFF");
    }
  }
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("Terhubung ke shiftr.io!");
      // PENTING: Subscribe ke topik perintah setelah berhasil terhubung
      client.subscribe(topic_perintah); 
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); 

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); // Daftarkan fungsi callback

  Wire.begin();
  dht.begin();
  bmp.begin(0x76);
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Wajib ada agar ESP32 bisa memproses pesan masuk terus menerus

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    float suhuDHT = dht.readTemperature();
    float kelembapan = dht.readHumidity();
    float tekananUdara = bmp.readPressure() / 100.0F; 
    float intensitasCahaya = lightMeter.readLightLevel();

    if (isnan(suhuDHT) || isnan(kelembapan)) return; 

    // LOGIKA AKTUATOR BARU (Mendukung Auto & Manual)
    String statusKipas = "";
    
    if (modeKipas == 1) {
      digitalWrite(RELAY_PIN, LOW); 
      statusKipas = "ON (Manual)";
    } 
    else if (modeKipas == 2) {
      digitalWrite(RELAY_PIN, HIGH); 
      statusKipas = "OFF (Manual)";
    } 
    else {
      // Mode Auto (modeKipas == 0)
      if (suhuDHT >= BATAS_SUHU) {
        digitalWrite(RELAY_PIN, LOW); 
        statusKipas = "ON (Auto)";
      } else {
        digitalWrite(RELAY_PIN, HIGH); 
        statusKipas = "OFF (Auto)";
      }
    }

    client.publish(topic_suhu, String(suhuDHT).c_str());
    client.publish(topic_kelembapan, String(kelembapan).c_str());
    client.publish(topic_tekanan, String(tekananUdara).c_str());
    client.publish(topic_cahaya, String(intensitasCahaya).c_str());
    client.publish(topic_kipas, statusKipas.c_str());
  }
}