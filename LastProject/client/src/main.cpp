#include <Arduino.h>
#include <M5Stack.h>
#include <Arduino.h>      //? Main Arduino Lib Framework
#include <WiFi.h>         //? Connect WiFi
#include <PubSubClient.h> //? Client MQTT
#include <ArduinoJson.h>

const unsigned int BG = M5.Lcd.color565(139, 254, 235);
const unsigned int R = M5.Lcd.color565(255, 29, 30);
const unsigned int Y = M5.Lcd.color565(255, 255, 0);
const unsigned int G = M5.Lcd.color565(18, 253, 42);
int state = 0;
int i = 0;
float t = 0.00;
int time_ = 0;

void callback(char *topic, byte *payload, unsigned int length);
void mqttReconnect();
void mqttSetup();
void wifiSetup();
void mqttTestPub();
void pubJSON();
void start();
void speed();

#define WiFi_STA_USERNAME "ILoveKmitl"       //? WiFi UserName
#define WiFi_STA_PASSWORD "1234567890" //? WiFi Password

#define MQTT_SERVER "soldier.cloudmqtt.com" //?Server URLS
#define MQTT_PORT 	16521                   //? Server Port
#define MQTT_USERNAME "Saturn"               //? MQTT UserName
#define MQTT_PASSWORD "Nut24212543"            //? MQTT Password
#define MQTT_NAME "ESP32_1"   

#define Intervals 500 //? Global Interval Time
unsigned long long previousMillis_WiFi = 0,
                   previousMillis_MQTT = 0,
                   previousMillis_PUB = 0;

#define MQTT_SUB_TEST "/distance" //? Test MQTT Only
#define MQTT_SUB_JSON "/NODE_RED/JSON"
WiFiClient client;         //? Create TCP Connection
PubSubClient mqtt(client); //? Create Mqtt over WiFiClient
float input ;

void start()
{
  M5.Lcd.fillScreen(BG);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(7, 100);
  M5.Lcd.printf("Choose your speed");
  M5.Lcd.setCursor(260, 210);
  M5.Lcd.printf(">>");
}

void speed()
{
  M5.Lcd.fillScreen(BG);
  M5.Lcd.fillCircle(157,117,60,BLUE);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(5);
  M5.Lcd.setCursor(130, 100);
  M5.Lcd.printf("%d",i);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(10, 210);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.printf("<<");
  M5.Lcd.setCursor(120, 210);
  M5.Lcd.printf("Okey");
  M5.Lcd.setCursor(270, 210);
  M5.Lcd.printf(">>");
  
}



String check(float input)
{
  String light;
  
  int a;
  t = input / (i * (5.00 / 18.00));
  time_ = (int)t / 60 ;
  a = t / 125.0;
  t = t - (a * 125);

  if (t <= 60)
  {
    light = "Green";
  }
  else if (t <= 65)
  {
    light = "Yellow";
  }
  else
  {
    light = "Red";
  }
  
  Serial.println(light);
  return light;
  
}


void ans()
{ //assight
  String a;
  a = check(input);
   // Serial.println(input);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(110, 90);
  if (a == "Green")
   { 
     M5.Lcd.fillScreen(G);
    M5.Lcd.println("Green");
    M5.Lcd.setTextSize(3);

   }
  else if (a == "Yellow")
   {
      M5.Lcd.fillScreen(Y);
      M5.Lcd.println("Yellow");
   }
  else if (a == "Red")
   { 
     M5.Lcd.fillScreen(R);
     M5.Lcd.println("Red");
    }
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(100, 130);
  M5.Lcd.printf(" %d m %d s",time_,(int)t%60);

   M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(30, 210);
  M5.Lcd.printf("Insert New Speed      >>");

}

void setup()
{
  // put your setup code here, to run once:
   Serial.begin(115200);
  Serial.println();
  wifiSetup();
  mqttSetup();
  m5.begin();
  start();
}

void loop()
{
  if (mqtt.connected() == false)
  {
    mqttReconnect();
  }
  else
  {
    mqtt.loop();
    pubJSON();
  }



  if (state == 0 && M5.BtnC.wasPressed())
  {

    state++;
    speed();
  }

  if (state == 1 && M5.BtnC.wasReleased())
  {
    i += 10;
    speed();
  }
  else if (state == 1 && M5.BtnA.wasReleased() && i != 10)
  {
    i -= 10;
    speed();
  }

  if (state == 1 && M5.BtnB.wasReleased())
  {
    if (i > 90)
    { /*
      M5.Lcd.setCursor(30, 110);
      M5.Lcd.fillScreen(R);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setTextSize(5);
      M5.Lcd.println("FUCK YOU");
      */
      M5.Speaker.tone(300);
      delay(150);
      M5.Speaker.mute();
      
      M5.Lcd.setCursor(10, 110);
      M5.Lcd.fillScreen(R);
      M5.Lcd.setTextColor(WHITE);
      M5.Lcd.setTextSize(3);
      M5.Lcd.println("Your speed illegal");
      delay(2000);
      i = 0;
      state=0;

      start();
    }
    else
    {
      state++;
      ans();
    }
  }
  if (state == 2 && M5.BtnC.wasReleased())
  {
    state=0;
    start();
    i=0;
  }
  M5.update();
}

void wifiSetup()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WiFi_STA_USERNAME);

  //* WiFi Connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(WiFi_STA_USERNAME, WiFi_STA_PASSWORD);

  Serial.print("Status: [");
  while (WiFi.status() != WL_CONNECTED)
  {
    //* Time Loop Uss Millis
    unsigned long currentMillis = millis(); //? Store Current time
    if (currentMillis - previousMillis_WiFi > Intervals)
    {
      previousMillis_WiFi = currentMillis; //? Save Current time
      Serial.print("=");
    }
  }
  Serial.println("] 100%");

  Serial.println("--- WiFi Connected ---");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void mqttSetup()
{
  mqtt.setServer(MQTT_SERVER, MQTT_PORT); //? Set Destination Server
  mqtt.setCallback(callback);             //? Set Callback Function
  Serial.println("--- MQTT Connected ---");
}

void callback(char *topic, byte *payload, unsigned int length)
{
  payload[length] = '\0';
  String topic_str = topic, payload_str = (char *)payload;
  Serial.println("[" + topic_str + "]: " + payload_str);

  if (topic_str ==MQTT_SUB_JSON)
  {
    Serial.println("---Start JSON Process ---");
    const size_t capacity = JSON_OBJECT_SIZE(3);
    DynamicJsonDocument doc(capacity);

    deserializeJson(doc,payload,length);
  const char *title,*label,*status;

  title = doc["title"];
  label = doc["label"];
  status = doc["status_"];

  Serial.println("title : "+String(title)+" | "+
                  "label : "+String(label)+" | "+
                  "status : "+String(status));
  Serial.println("---END JSON Process ---");
  }
  if(topic_str.equals("/distance")){
    input = payload_str.toFloat();
    Serial.println("Print :" +String(input) );
  }
}

void mqttReconnect()
{
  Serial.println("MQTT Connecting ");
  Serial.print("Status: [");
  while (mqtt.connected() == false)
  {
    mqtt.connect(MQTT_NAME, MQTT_USERNAME, MQTT_PASSWORD); //? Reconnect To Server

    //* Time Loop
    unsigned long currentMillis = millis(); //? Store Current time
    if (currentMillis - previousMillis_MQTT > Intervals)
    {
      previousMillis_MQTT = currentMillis; //? Save Current time
      Serial.print("=");
    }
  }
  Serial.println("] 100%");
  Serial.println("--- MQTT Reconnected ---");

  if (mqtt.connect(MQTT_NAME, MQTT_USERNAME, MQTT_PASSWORD))
  {
    Serial.println("connected : " + String(MQTT_SUB_TEST));
    mqtt.subscribe(MQTT_SUB_TEST);
     Serial.println("connected : " + String(MQTT_SUB_JSON));
    mqtt.subscribe(MQTT_SUB_JSON);
  }
  else
  {
    Serial.println("--- failed ---");
  }
}

void mqttTestPub()
{
  unsigned long currentMillis = millis(); //? Store Current time
  if (currentMillis - previousMillis_PUB > Intervals)
  {
    previousMillis_PUB = currentMillis; //? Save Current time

    mqtt.publish(MQTT_SUB_TEST, "Hello From M5GO");
    }
}

void pubJSON()

{

  unsigned long currentMillis = millis(); //? Store Current time

  if (currentMillis - previousMillis_PUB > 500)

  {

    previousMillis_PUB = currentMillis; //? Save Current time

    const size_t capacity = JSON_OBJECT_SIZE(3);

    DynamicJsonDocument doc(capacity);

    char buffer[512];



    doc["title"] = "test1";

    doc["duration"] = 10;

    doc["status"] = "Todo";



    serializeJson(doc, buffer);



    //mqtt.publish("/in", buffer);

  }

}