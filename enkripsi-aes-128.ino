#include <WiFi.h>
#include <AESLib.h>   // AESLib by Matej Sychra

AESLib aesLib;

// --- ENKRIPSI KONFIGURASI ---
// Kunci AES (16 byte untuk AES-128)
byte encryptionKey[16] = {
  0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
  0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

// Initialization Vector (IV) - harus 16 byte
byte iv[16] = {
  0x00, 0x01, 0x02, 0x03,
  0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B,
  0x0C, 0x0D, 0x0E, 0x0F
};
// --- AKHIR ENKRIPSI KONFIGURASI ---


// WiFi
#define WIFI_SSID "tebakne"
#define WIFI_PASSWORD "hpelitkuotasulit"

// NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 8 * 3600; // GMT+8 (WITA)
const int daylightOffset_sec = 0;


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Mulai");

  // Config WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected to IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // SetUp NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}


// Fungsi untuk enkripsi data pakai AESLib (CBC + Base64)
String encryptData(String plainText) {
  int msgLen = plainText.length() + 1; // termasuk null terminator
  char plainBuf[msgLen];
  plainText.toCharArray(plainBuf, msgLen);

  // Buffer output, harus lebih besar (AES + Base64 butuh lebih banyak space)
  char encrypted[2 * msgLen];
  memset(encrypted, 0, sizeof(encrypted));

  // 🔑 Format yang benar untuk AESLib::encrypt64
  aesLib.encrypt64((byte*)plainBuf, msgLen, encrypted, encryptionKey, 128, iv);

  return String(encrypted);
}


void loop() {
  // Ambil waktu
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Gagal mengambil waktu dari NTP");
    delay(1000);
    return;
  }
  time_t now = mktime(&timeinfo); // UNIX timestamp

  // --- PREPARASI DATA UNTUK ENKRIPSI ---
  String rawDataJson = "{\"Timestamp\":" + String((int)now) + "}";

  // --- ENKRIPSI DATA ---
  String encryptedData = encryptData(rawDataJson);
  Serial.print("Raw Data JSON: ");
  Serial.println(rawDataJson);
  Serial.print("Data Terenkripsi (Base64): ");
  Serial.println(encryptedData);

  delay(5000);
}
