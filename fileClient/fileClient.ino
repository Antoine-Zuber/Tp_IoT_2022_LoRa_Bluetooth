#include <WiFi.h>
#include <PubSubClient.h>

#define ssid "OPPO"
#define pass "canard25"
#define mySsid "Lise"
#define myPass NULL

const byte LIGHT_PIN = 25;
// Pin to control the light with
const char *ID = "Zuber";
// Name of our device, must be unique
const char *TOPIC = "Lise";
// Topic to subcribe to
const char *STATE_TOPIC = "Lise/state";

int phase = 1 ; 

String RESPONSE ; 

// Topic to publish the light state to
IPAddress broker(192,168,43,159); // IP address of your MQTT broker
WiFiClient wclient;

PubSubClient client(wclient); // Setup MQTT client
// Handle incomming messages from the broker



void callback(char* topic, byte* payload, unsigned int length) {
  String response;
  
  for (int i = 0; i < length; i++) {
    response += (char)payload[i];
  }
  Serial.print("Msg [");
  Serial.print(topic); Serial.print("] ");
  Serial.println(response);

  //Address IP for connection 
  RESPONSE = response ;  
  phase = 2 ; 
  client.disconnect();
  
}

// Connect to WiFi network
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass); // Connect to network
  
  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500); Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Connect to WiFi ESP32 network
void setup_esp32() {
  Serial.print("\nConnecting to ");
  Serial.println(mySsid);
  WiFi.begin(mySsid, myPass); // Connect to network
  
  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500); Serial.print(".");
  }
  
  Serial.println();
  Serial.println("ESP32 connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to client
void reconnect() {
// Loop until we're reconnected
  client.setServer("test.mosquitto.org",1883);
  
  while (!client.connected()) {
    
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if(client.connect(ID)) {
      client.subscribe(TOPIC);
      Serial.println("connected");
      Serial.print("Subcribed to: ");
      Serial.println(TOPIC);
      Serial.println('\n');
    } 
    else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600); // Start serial communication at 115200 baud
  pinMode(LIGHT_PIN, OUTPUT); // Configure LIGHT_PIN as an output
  delay(100);
  setup_wifi(); // Connect to network
  client.setServer(broker, 1883);
  client.setCallback(callback);// Initialize the callback routine
}

void loop() {
  if ((!client.connected()) or (WiFi.status() != WL_CONNECTED)) {

    if (phase == 1) {
      reconnect();
    }
    else {
      setup_esp32(); 
    }
  }
  client.loop();
}
