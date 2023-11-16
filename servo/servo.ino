#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Servo.h>

//Servo setting
#define servoPin 2

Servo myservo;
int pos = 0;

// Replace the next variables with your SSID/Password combination
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "wjw";
const char* password = "1234223432";

const char* mqtt_server = "test.mosquitto.org";
String topic = "F112119108/Home/Restroom/Toilet/AutoFlush";
String client_id = "AutoFlush_Servo";

char msg[50];

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client_id += String(WiFi.macAddress());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}

void servo_run_cycle(int i){
  myservo.attach(servoPin);
  
  for (pos = 30; pos <= i; pos++){
    myservo.write(pos);
    //Serial.print("pos: ");
    //Serial.println(pos);
    delay(15);
  }
  for (pos = i; pos >= 30; pos--){
    myservo.write(pos);
    //Serial.print("pos: ");
    //Serial.println(pos);
    delay(15);
  }
  myservo.detach();
  
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  myservo.attach(servoPin);
  myservo.write(30);
  myservo.detach();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(client_id.c_str())) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(topic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
    
  }
  Serial.println(messageTemp);

  String s = "";
  s = messageTemp.c_str();
  s.trim();
  DynamicJsonDocument root(1024);
  deserializeJson(root, s);
  
  String TYPE = root["TYP"];
  String NAME = root["NAME"];
  int ENABLE = root["ENABLE"];
  
  if (TYPE == "CONTROL"){
    if (NAME == "Dashbutton" && ENABLE == 1){
      servo_run_cycle(150);
    }
  }
  
}
