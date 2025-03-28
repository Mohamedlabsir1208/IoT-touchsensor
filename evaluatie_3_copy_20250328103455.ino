// ===== Blynk template instellingen =====
#define BLYNK_TEMPLATE_ID "user7"
#define BLYNK_TEMPLATE_NAME "user7@server.wyns.it"
#define BLYNK_PRINT Serial  // Voor debug info in Serial Monitor

// ===== Inclusies van libraries =====
#include <WiFi.h>                 // WiFi functionaliteit
#include <BlynkSimpleEsp32.h>     // Blynk bibliotheek voor ESP32
#include <Wire.h>                 // I2C-communicatie
#include <LiquidCrystal_I2C.h>    // LCD via I2C-bus

// ===== Blynk gebruikersgegevens =====
char auth[] = "cprYrmoKFJSwvZxmdoOrCAkz7fWUBI1E";  // Auth token uit Blynk
char ssid[] = "telenet-63E5C";                    // WiFi netwerknaam
char pass[] = "MaRiAm25";                         // WiFi wachtwoord

// ===== Pin-definities =====
#define TOUCH_PIN 33      // GPIO33 → Touch draad voor bezet-detectie
#define LED_PIN 2         // GPIO2 → Fysieke LED

// ===== Variabelen =====
bool manualOverride = false;  // Als dit true is, negeer touchsensor
int manualState = 0;          // Status van manuele knop in Blynk (0 = uit, 1 = aan)

BlynkTimer timer;  // Timer-object voor periodieke updates
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD scherm met I2C-adres 0x27, 16 kolommen, 2 rijen

// ===== Manuele aansturing via Blynk-knop (V2) =====
BLYNK_WRITE(V2) {
  manualState = param.asInt();                  // Lees knopstatus (0 of 1)
  manualOverride = (manualState == 1);          // Alleen override als knop op AAN staat
}

// ===== Status check functie =====
void checkStatus() {
  int touchValue = touchRead(TOUCH_PIN);  // Lees aanraakwaarde van GPIO33
  Serial.print("Touch waarde: ");
  Serial.println(touchValue);             // Print naar Serial Monitor

  // Voorbereiden van LCD-scherm
  lcd.setCursor(0, 0);
  lcd.print("Status:          ");          // Leegmaken regel 1

  if (manualOverride) {
    // === Manuele override via Blynk-knop ===
    if (manualState == 1) {
      digitalWrite(LED_PIN, HIGH);         // Zet LED aan
      lcd.setCursor(8, 0);
      lcd.print("Manueel AAN ");           // Toon op LCD
      Blynk.virtualWrite(V0, "Manueel AAN");  // Stuur tekst naar Blynk label (V0)
      Blynk.virtualWrite(V1, 255);             // Zet virtuele LED (V1) aan
    }
  } else {
    // === Automatische detectie via touch ===
    if (touchValue < 30) {                 // Drempelwaarde voor aanraking
      digitalWrite(LED_PIN, HIGH);         // Zet LED aan
      lcd.setCursor(8, 0);
      lcd.print("Bezet       ");           // Toon "Bezet" op LCD
      Blynk.virtualWrite(V0, "Bezet");     // Label V0 in Blynk
      Blynk.virtualWrite(V1, 255);         // Virtuele LED V1 aan
    } else {
      digitalWrite(LED_PIN, LOW);          // Zet LED uit
      lcd.setCursor(8, 0);
      lcd.print("Vrij        ");           // Toon "Vrij" op LCD
      Blynk.virtualWrite(V0, "Vrij");      // Label V0 in Blynk
      Blynk.virtualWrite(V1, 0);           // Virtuele LED V1 uit
    }
  }
}

// ===== Setup-functie (1x bij opstart) =====
void setup() {
  Serial.begin(115200);  // Start de seriële communicatie
  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081); // Verbinden met Blynk-server

  pinMode(LED_PIN, OUTPUT);  // Zet LED-pin als uitgang

  lcd.init();                // Start het LCD-scherm
  lcd.backlight();           // Zet de achtergrondverlichting aan
  lcd.setCursor(0, 0);
  lcd.print("Starten...");   // Welkomsttekst
  delay(1000);               // Korte wachttijd

  // Start een timer die elke 500ms de status controleert
  timer.setInterval(500L, checkStatus);
}

// ===== Hoofdlus (loopt constant) =====
void loop() {
  Blynk.run();   // Verwerking van Blynk-verbinding
  timer.run();   // Laat de timer functie uitvoeren
}
