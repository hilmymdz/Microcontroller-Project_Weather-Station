# Microcontroller-Project_Weather Station

Project **Ujian Akhir Semester (UAS) mata kuliah Sistem Mikrokontroler**. Sistem ini berfungsi sebagai stasiun cuaca mini berbasis IoT yang mengumpulkan data lingkungan dari berbagai sensor secara real-time, lalu mengirimkannya melalui MQTT untuk divisualisasikan di dashboard Website.

---

## Anggota
* Kelompok 9 - TIF RP 23 CID A
  * Hilmy Muhamad Dzakwan - 23552011368
  * Helmi Ahmad Fauzan - 23552011433
  * Pedrik Dendi Aparel - 24552012016

* Universitas Teknologi Bandung

## ✨ Fitur Utama
- **Monitoring Real-Time:** Membaca data Suhu, Kelembapan, Tekanan Udara, dan Intensitas Cahaya.
- **Komunikasi Ringan:** Menggunakan protokol MQTT (via shiftr.io) dengan topik terpisah untuk efisiensi *bandwidth*.
- **Otomasi Aktuator:** Kipas pendingin menyala otomatis jika suhu ruangan mencapai batas kritis (≥ 30°C).
- **Manual Override:** Kendali kipas (Auto, On, Off) secara manual melalui antarmuka web.
- **Web Dashboard:** Antarmuka responsif dan modern dengan tema gelap (*dark mode*) yang menggunakan WebSockets.

## 🛠️ Perangkat Keras (Hardware)
1. Mikrokontroler ESP32 (DOIT DevKit V1)
2. Sensor Suhu & Kelembapan: **DHT22**
3. Sensor Tekanan Udara: **BMP280** (I2C)
4. Sensor Intensitas Cahaya: **GY-30 / BH1750** (I2C)
5. Aktuator: **Modul Relay 1-Channel** & **Kipas DC**
6. Breadboard & Kabel Jumper secukupnya

### Skema Pin (Wiring)
| Komponen | Pin Komponen | Pin ESP32 | Keterangan |
| :--- | :--- | :--- | :--- |
| **DHT22** | VCC, GND, OUT | 3V3, GND, GPIO 4 | Digital 1-Wire |
| **BMP280** | VCC, GND, SCL, SDA | 3V3, GND, GPIO 22, GPIO 21 | SDO ke GND (Alamat 0x76) |
| **GY-30** | VCC, GND, SCL, SDA | 3V3, GND, GPIO 22, GPIO 21 | ADD ke GND / Kosong (0x23) |
| **Relay** | VCC, GND, IN | 5V/VIN, GND, GPIO 14 | Active Low |

## 💻 Perangkat Lunak & Library (Software)
Pastikan kamu telah menginstal *library* berikut di Arduino IDE:
- `WiFi.h` (Bawaan ESP32)
- `PubSubClient` oleh Nick O'Leary
- `DHT sensor library` oleh Adafruit
- `Adafruit BMP280 Library` oleh Adafruit
- `BH1750` oleh Christopher Laws

