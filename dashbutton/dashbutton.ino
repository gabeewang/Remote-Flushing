#include <WiFi.h>
#include <PubSubClient.h>

//Button Setting
const int buttonPin = 22;

int btn_State = 0; //觸發狀態
unsigned long startTime = 0;  // 用於儲存按下按鈕的開始時間
unsigned long duration = 500; // 500毫秒的持續時間

//Wi-Fi and MQTT Setting
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "wjw";
const char* password = "1234223432";

const char* mqtt_server = "test.mosquitto.org";
String topic = "F112119108/Home/Restroom/Toilet/AutoFlush";
String client_id = "AutoFlush_Dashborad";

char msg[50];

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client_id += String(WiFi.macAddress());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(buttonPin, INPUT);
  
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (digitalRead(buttonPin) == HIGH) {
    if (btn_State == 0) {
      startTime = millis();  // 紀錄按下按鈕的開始時間
      btn_State = 1;
    }
    // 檢查按鈕按下的時間是否超過500毫秒
    if (millis() - startTime >= duration && btn_State == 1) {
      mqtt_publish();
      btn_State = 2;  // 表示已經執行過 mqtt_publish()，避免重複觸發
    }
  }
  else {
    btn_State = 0;
  }
  
}

void mqtt_publish(){
  String msg = "{";
  msg += "\"TYP\":\"CONTROL\",";
  msg += "\"NAME\":\"Dashbutton\",";
  msg += "\"ENABLE\":1";
  msg += "}";
  
  client.publish(topic.c_str(), msg.c_str());
 
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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
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
  
}
