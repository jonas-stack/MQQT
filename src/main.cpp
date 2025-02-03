#include <WiFi.h>
#include <PubSubClient.h>

// WiFi and MQTT credentials
const char* ssid = "JonasTelefon";
const char* password = "12345678";
const char* mqttServer = "mqtt.flespi.io";
const int mqttPort = 1883;
const char* mqttUser = "3CsaLQcbO01Qxi0Hlto2nW6gVPz2jqYfHdDPFojGaZNi2oKhaTejHLllIi6FXl9U";
const char* mqttPassword = "";


//dette er en kommentar, da jeg tester mit commit til git


WiFiClient espClient;
PubSubClient client(espClient);

const int buttonPins[] = {D13, D12, D11, D10}; // Pins for buttons
const char* satisfactionLevels[] = {"Very Dissatisfied", "Dissatisfied", "Satisfied", "Very Satisfied"};

unsigned long lastDebounceTime[4] = {0, 0, 0, 0}; // the last time the output pin was toggled
unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers
int buttonState[4] = {HIGH, HIGH, HIGH, HIGH}; // the current reading from the input pin
int lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH}; // the previous reading from the input pin


void printInitialButtonStates() {
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(buttonPins[i]);
    Serial.print("Button ");
    Serial.print(i);
    Serial.print(" initial reading: ");
    Serial.println(reading);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.publish("esp/test", "Hello from ESP32");

  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  printInitialButtonStates();
}



void sendSatisfaction(int buttonIndex) {
  Serial.print("Button ");
  Serial.print(buttonIndex);
  Serial.print(" pressed: ");
  Serial.println(satisfactionLevels[buttonIndex]);

  client.publish("esp/satisfaction", satisfactionLevels[buttonIndex]);
}

void loop() {
  unsigned long currentMillis = millis();

  // Read button states
  for (int i = 0; i < 4; i++) {
    int reading = digitalRead(buttonPins[i]);

    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = currentMillis;
    }

    if ((currentMillis - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;
        if (buttonState[i] == LOW) {
          sendSatisfaction(i);
        }
      }
    }

    lastButtonState[i] = reading;
  }
}