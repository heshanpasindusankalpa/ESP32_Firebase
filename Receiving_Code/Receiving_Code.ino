
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi Configuration
#define WIFI_SSID "Galaxy A13 00F4"
#define WIFI_PASSWORD "elfv3871"

// Firebase Configuration
#define API_KEY "AIzaSyCKZolSWzhB-cDVnJX6G1gGAx40ltYbh74"
#define DATABASE_URL "https://esp32-test-fc532-default-rtdb.firebaseio.com/"

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

// Connect to WiFi
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to WiFi!");
  } else {
    Serial.println("\n❌ WiFi Connection Failed!");
  }
}

// Initialize OLED
void initOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED initialization failed!");
    while (true);
  }
  Serial.println("✅ OLED Initialized!");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Fetching data...");
  display.display();
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
  initOLED();

  // Configure Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  Serial.println("Signing in to Firebase...");
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("✅ Firebase authentication successful!");
    signupOK = true;
  } else {
    Serial.printf("❌ Firebase authentication failed: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void fetchData() {
  if (Firebase.ready() && signupOK) {
    float temperature, humidity;
    
    // Get Temperature
    if (Firebase.RTDB.getFloat(&fbdo, "/sensor/temperature")) {
      temperature = fbdo.floatData();
      Serial.printf("🌡 Temperature: %.2f°C\n", temperature);
    } else {
      Serial.printf("❌ Failed to get temperature: %s\n", fbdo.errorReason().c_str());
      return;
    }

    // Get Humidity
    if (Firebase.RTDB.getFloat(&fbdo, "/sensor/humidity")) {
      humidity = fbdo.floatData();
      Serial.printf("💧 Humidity: %.2f%%\n", humidity);
    } else {
      Serial.printf("❌ Failed to get humidity: %s\n", fbdo.errorReason().c_str());
      return;
    }

    // Display on OLED
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 10);
    display.print("Temp:");
    display.print(temperature, 1);
    display.print(" C");

    display.setCursor(0, 40);
    display.print("Hum:");
    display.print(humidity, 1);
    display.print(" %");

    display.display();
  } else {
    Serial.println("❌ Firebase not ready or authentication failed!");
  }
}

void loop() {
  fetchData();
  delay(5000);
}
