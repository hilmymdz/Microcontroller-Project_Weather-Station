#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>

// --- DEKLARASI PIN ESP32 ---
#define DHTPIN 4          // Pin GPIO 4 untuk DHT22
#define DHTTYPE DHT22     // Jenis sensor DHT
#define RELAY_PIN 14      // Pin GPIO 18 untuk Modul Relay

// --- INISIALISASI OBJEK ---
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;      // Menggunakan komunikasi I2C bawaan
BH1750 lightMeter;

// Batas suhu untuk menyalakan kipas
const float BATAS_SUHU = 30.0; 

void setup() {
  // Memulai komunikasi serial dengan baud rate 115200
  Serial.begin(115200);
  Serial.println("\n--- Memulai Tes Stasiun Cuaca ---");

  // Memulai komunikasi I2C (Pin 21 SDA, Pin 22 SCL)
  Wire.begin();

  // Setup pin Relay
  pinMode(RELAY_PIN, OUTPUT);
  // Catatan: Kebanyakan modul relay Arduino menggunakan logika "Active Low"
  // Artinya LOW = Menyala, HIGH = Mati. Kita set HIGH di awal agar kipas mati.
  digitalWrite(RELAY_PIN, HIGH); 

  // Memulai DHT22
  dht.begin();
  
  // Memulai BMP280 (0x76 adalah alamat I2C umum jika pin SDO ke GND)
  if (!bmp.begin(0x76)) {
    Serial.println("Peringatan: Sensor BMP280 tidak terdeteksi! Cek kabel SDA/SCL.");
  } else {
    Serial.println("BMP280 OK!");
  }
  
  // Memulai GY-30 / BH1750
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23)) {
    Serial.println("Peringatan: Sensor Cahaya GY-30 tidak terdeteksi! Cek kabel SDA/SCL.");
  } else {
    Serial.println("GY-30 OK!");
  }
  
  Serial.println("---------------------------------");
  delay(2000); // Waktu jeda sebelum masuk ke loop
}

void loop() {
  // 1. Membaca Data Sensor
  // DHT22 butuh waktu sekitar 2 detik antar pembacaan
  float kelembapan = dht.readHumidity();
  float suhuDHT = dht.readTemperature();
  
  // Membaca BMP280
  float suhuBMP = bmp.readTemperature();
  float tekananUdara = bmp.readPressure() / 100.0F; // Konversi ke hPa (HectoPascal)
  
  // Membaca GY-30
  float intensitasCahaya = lightMeter.readLightLevel();

  // 2. Validasi Pembacaan DHT22
  // Jika bernilai NaN (Not a Number), berarti gagal terbaca
  if (isnan(kelembapan) || isnan(suhuDHT)) {
    Serial.println("Gagal membaca data dari sensor DHT!");
  } else {
    // 3. Menampilkan Data ke Serial Monitor
    Serial.print("Suhu DHT: "); Serial.print(suhuDHT); Serial.print(" °C | ");
    Serial.print("Suhu BMP: "); Serial.print(suhuBMP); Serial.print(" °C | ");
    Serial.print("Kelembapan: "); Serial.print(kelembapan); Serial.print(" % | ");
    Serial.print("Tekanan Udara: "); Serial.print(tekananUdara); Serial.print(" hPa | ");
    Serial.print("Cahaya: "); Serial.print(intensitasCahaya); Serial.println(" lx");

    // 4. Logika Aktuator (Otomatisasi Kipas)
    if (suhuDHT >= BATAS_SUHU) {
      digitalWrite(RELAY_PIN, HIGH); // Diubah ke HIGH untuk menyalakan (Active High)
      Serial.println(">> STATUS KIPAS: MENYALA (Suhu di atas batas!)");
    } else {
      digitalWrite(RELAY_PIN, LOW);  // Diubah ke LOW untuk mematikan
      Serial.println(">> STATUS KIPAS: MATI (Suhu aman)");
    }
  }
  
  Serial.println("------------------------------------------------------------------");
  
  // Jeda 2 detik agar sensor stabil dan Serial Monitor tidak terlalu cepat
  delay(2000);
}
