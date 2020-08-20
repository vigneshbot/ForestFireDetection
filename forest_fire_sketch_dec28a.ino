#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define LED6_PIN D7
#define DSEN1_PIN D1
String dsen1;
int dsenc1;

// Update these with values suitable for your network.
const char* ssid = ""; //enter wifi name
const char* password = ""; //enter password
const char* mqtt_server = "test.mosquitto.org";
//const char* mqtt_server = "iot.eclipse.org";
//const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(100);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "smfr";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(DSEN1_PIN, INPUT);
  pinMode(LED6_PIN, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    dsen1 = digitalRead(DSEN1_PIN);
    dsenc1 = digitalRead(DSEN1_PIN);
    char message1[4];
    dsen1.toCharArray(message1, 4);
    client.publish("smfr1", message1);
    Serial.print("fire=");
    Serial.println(dsen1);
    Serial.println("");
    if (dsenc1 == 0)
    {
      Serial.println("FIRE DETECTED");
      digitalWrite(LED6_PIN, HIGH);
      client.publish("smfr2", "FIRE DETECTED");
      delay(500);
    }
    if (dsenc1 == 1)
    {
      Serial.println("NO FIRE");
      digitalWrite(LED6_PIN, LOW);
      client.publish("smfr2", "NO FIRE");
      delay(500);
    }
  }
}
