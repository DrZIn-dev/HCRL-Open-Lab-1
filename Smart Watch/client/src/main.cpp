#include <Arduino.h>      //? Main Arduino Lib Framework
#include <WiFi.h>         //? Connect WiFi
#include <PubSubClient.h> //? Client MQTT
#include <ArduinoJson.h>

#include <M5Stack.h>

//* Prototype Function
void callback(char *topic, byte *payload, unsigned int length);
void mqttReconnect();
void mqttSetup();
void mqttTestPub(char *sub,char *msg);
void pubJSON();
void wifiSetup();

void menu(int screen);
void updateClock();
void updateToDo();
void PicNotification(int x,int y);
void PicCalendar(int x,int y);
void PicMenu(int x,int y);
void SetText(int x,int y,char *text,int color,int size);

//* WIFI STA Details
#define WiFi_STA_USERNAME "Hello"       //? WiFi UserName
#define WiFi_STA_PASSWORD "nE0n.mEe" //? WiFi Password

//* MQTT Server Details
#define MQTT_SERVER "soldier.cloudmqtt.com" //?Server URLS
#define MQTT_PORT 16522                     //? Server Port
#define MQTT_USERNAME "root"               //? MQTT UserName
#define MQTT_PASSWORD "toor"            //? MQTT Password
#define MQTT_NAME "ESP32_1"                 //?Topic Name

//* Params
#define Intervals 500 //? Global Interval Time
unsigned long long previousMillis_WiFi = 0,
                   previousMillis_MQTT = 0,
                   previousMillis_PUB = 0;
const char *hours,*minutes,*seconds,*date,*month,*year;
const char *todo;
const unsigned int Y1 = M5.Lcd.color565(255, 220, 50);
const unsigned int Y2 = M5.Lcd.color565(200, 150, 0);
const unsigned int BLUESKY = M5.Lcd.color565(0, 204, 204);
const unsigned int PINK1 = M5.Lcd.color565(255, 200, 230);
int duration;
int screen = 0;
unsigned long long int idle = 0;

//* SUB Topic
#define MQTT_SUB_TEST "/NODE_RED/Test" //? Test MQTT Only
#define MQTT_SUB_JSON "/NODE_RED/JSON" //? JSON Format
#define MQTT_PUB_JSON "/ESP32_1/JSON_PUB"

WiFiClient client;         //? Create TCP Connection
PubSubClient mqtt(client); //? Create Mqtt over WiFiClient

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  wifiSetup();
  mqttSetup();
	
	M5.begin();
  menu(screen);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (mqtt.connected() == false) {
    mqttReconnect();
  } else {
    mqtt.loop();
    // pubJSON();
  }

	if(idle == 100000){
		screen = 0;
		menu(screen);
		mqttTestPub("/M5GO/GetTime","Get Time");
	}

	if(screen == 0){
		idle = 0;
		if(M5.BtnA.wasPressed()||M5.BtnB.wasPressed()||M5.BtnC.wasPressed()){
			screen = 1;
			menu(screen);
			mqttTestPub("/M5GO/GetTime","Get Time");
		}
	}else if(screen == 1){
		if(M5.BtnA.wasPressed()){
			screen = 2;
			menu(screen);
			mqttTestPub("/M5GO/GetTime","Get Time");
		}else if(M5.BtnB.wasPressed()){
			screen = 0;
			menu(screen);
			mqttTestPub("/M5GO/GetTime","Get Time");
		}else if(M5.BtnC.wasPressed()){
			screen = 3;
			menu(screen);
			// mqttTestPub("/M5GO/GetTime","Get Time");
		}
		idle++;
	}else if(screen == 2){
		idle = 0;
		if(M5.BtnA.wasPressed()||M5.BtnB.wasPressed()||M5.BtnC.wasPressed()){
			screen = 1;
			menu(screen);
			mqttTestPub("/M5GO/GetTime","Get Time");
		}
	}else if(screen == 3){
		idle = 0;
		if(M5.BtnA.wasPressed()||M5.BtnB.wasPressed()||M5.BtnC.wasPressed()){
			screen = 1;
			menu(screen);
			mqttTestPub("/M5GO/GetTime","Get Time");
		}
	}else{
		idle++;
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
  if (topic_str == "/M5GO/SetTime")
  {
    Serial.println("--- Start JSON Process ---");
    const size_t capacity = JSON_OBJECT_SIZE(6); //? Object Size
    DynamicJsonDocument doc(capacity);           //? Declare variable for store json

    deserializeJson(doc, payload, length); //? deserialize JSON

    hours = doc["hours"];
    minutes = doc["minutes"];
    seconds = doc["seconds"];
    date = doc["date"];
    month = doc["month"];
    year = doc["year"];

    Serial.println("hours : " + String(hours) + " | " +
                   "minutes : " + String(minutes) + " | " +
                   "seconds : " + String(seconds) + " | " +
                   "date : " + String(date) + " | " +
                   "month : " + String(month) + " | " +
                   "year : " + String(year)); //?Print Output

    Serial.println("--- END JSON Process ---");

		updateClock();
  }
	
  if (topic_str == "/M5GO/ToDo")
  {
    Serial.println("--- Start JSON Process ---");
    const size_t capacity = JSON_OBJECT_SIZE(6); //? Object Size
    DynamicJsonDocument doc(capacity);           //? Declare variable for store json

    deserializeJson(doc, payload, length); //? deserialize JSON

    todo = doc["detail"];

    Serial.println("ToDo : " + String(todo)); //?Print Output

    Serial.println("--- END JSON Process ---");

		updateToDo();
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

  mqttTestPub("/M5GO/GetTime","Get Time");

  if (mqtt.connect(MQTT_NAME, MQTT_USERNAME, MQTT_PASSWORD))
  {
    Serial.println("connected : " + String("/M5GO/SetTime"));
    mqtt.subscribe("/M5GO/SetTime");
    Serial.println("connected : " + String("/M5GO/ToDo"));
    mqtt.subscribe("/M5GO/ToDo");
  }
  else
  {
    Serial.println("--- failed ---");
  }
}

void mqttTestPub(char *sub,char *msg)
{
  unsigned long currentMillis = millis(); //? Store Current time
  if (currentMillis - previousMillis_PUB > Intervals)
  {
    previousMillis_PUB = currentMillis; //? Save Current time

    mqtt.publish(sub, msg);
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

    mqtt.publish(MQTT_PUB_JSON, buffer);
  }
}

void menu(int screen){
    switch (screen){
			case 0:
				M5.Lcd.fillScreen(BLACK);
				M5.Lcd.setTextColor(WHITE,BLACK);
				M5.Lcd.setTextSize(7);
				M5.Lcd.setCursor(55,35);
				M5.Lcd.print("00:00");

				M5.Lcd.fillRect(35,120,250,100,M5.Lcd.color565(255, 255, 153));
				M5.Lcd.setCursor(45,130);
				M5.Lcd.setTextSize(3);
				M5.Lcd.setTextColor(BLACK,M5.Lcd.color565(255, 255, 153));
				M5.Lcd.print("ToDo --------");
				break;
			case 1:
				M5.Lcd.fillScreen(BLACK);
				M5.Lcd.setTextColor(WHITE,BLACK);
				M5.Lcd.setTextSize(7);
				M5.Lcd.setCursor(55,35);
				M5.Lcd.print("00:00");

				M5.Lcd.fillRect(30,120,260,40,M5.Lcd.color565(255, 255, 153));
				M5.Lcd.setCursor(45,130);
				M5.Lcd.setTextSize(2);
				M5.Lcd.setTextColor(BLACK,M5.Lcd.color565(255, 255, 153));
				M5.Lcd.print("ToDo : ");

				M5.Lcd.setTextColor(WHITE,BLACK);
				M5.Lcd.setTextSize(7);
				M5.Lcd.setCursor(55,35);

				PicNotification(30,170);
				PicCalendar(220,170);
				PicMenu(125,170);
				break;
			case 2:
				M5.Lcd.fillScreen(WHITE);
				M5.Lcd.fillRect(0,0,320,60,BLUE);

				SetText(32,22,"To Do Lists",BLACK,3);
				SetText(30,20,"To Do Lists",WHITE,3);
				break;
			case 3:
				M5.Lcd.fillScreen(WHITE);
				M5.Lcd.fillRect(0,0,320,60,RED);

				SetText(32,22,"Calendar",BLACK,3);
				SetText(30,20,"Calendar",WHITE,3);
				M5.Lcd.fillRect(20,70,280,2,BLACK);
				M5.Lcd.fillRect(20,100,280,2,BLACK);
				M5.Lcd.fillRect(20,130,280,2,BLACK);
				M5.Lcd.fillRect(20,160,280,2,BLACK);
				M5.Lcd.fillRect(20,190,280,2,BLACK);
				M5.Lcd.fillRect(20,220,280,2,BLACK);

				M5.Lcd.fillRect(20,70,2,150,BLACK);
				M5.Lcd.fillRect(60,70,2,150,BLACK);
				M5.Lcd.fillRect(100,70,2,150,BLACK);
				M5.Lcd.fillRect(140,70,2,150,BLACK);
				M5.Lcd.fillRect(180,70,2,150,BLACK);
				M5.Lcd.fillRect(220,70,2,150,BLACK);
				M5.Lcd.fillRect(260,70,2,150,BLACK);
				M5.Lcd.fillRect(300,70,2,150,BLACK);

				SetText(30,80,"1",BLACK,2); SetText(70,80,"2",BLACK,2); SetText(110,80,"3",BLACK,2); SetText(150,80,"4",BLACK,2); SetText(190,80,"5",BLACK,2); SetText(230,80,"6",BLACK,2); SetText(270,80,"7",BLACK,2);
				SetText(30,110,"8",BLACK,2); SetText(70,110,"9",BLACK,2); SetText(110,110,"10",BLACK,2); SetText(150,110,"11",BLACK,2); SetText(190,110,"12",BLACK,2); SetText(230,110,"13",BLACK,2); SetText(270,110,"14",BLACK,2);
				SetText(30,140,"15",BLACK,2); SetText(70,140,"16",BLACK,2); SetText(110,140,"17",BLACK,2); SetText(150,140,"18",BLACK,2); SetText(190,140,"19",BLACK,2);SetText(230,140,"20",BLACK,2); SetText(270,140,"21",BLACK,2);
				SetText(30,170,"22",BLACK,2); SetText(70,170,"23",BLACK,2); SetText(110,170,"24",BLACK,2); 
				// SetText(150,170,"25",BLACK,2);
				M5.Lcd.setTextColor(BLACK,YELLOW);
				M5.Lcd.setTextSize(2);
				M5.Lcd.setCursor(150,170);
				M5.Lcd.print(25);
				SetText(190,170,"26",BLACK,2);SetText(230,170,"27",BLACK,2); SetText(270,170,"28",BLACK,2); 
				SetText(30,200,"29",BLACK,2); SetText(70,200,"30",BLACK,2); SetText(110,200,"31",BLACK,2);
				break;
			default:
					break;
    }
}

void updateClock(){
	if(screen == 0 || screen == 1){
		M5.Lcd.setTextColor(WHITE,BLACK);
		M5.Lcd.setTextSize(7);
		M5.Lcd.setCursor(55,35);
		if(String(hours)[2] == '"'){
			M5.Lcd.print("0");
			M5.Lcd.print(String(hours)[1]);
		}else{
			M5.Lcd.print(String(hours)[1]);
			M5.Lcd.print(String(hours)[2]);
		}
		M5.Lcd.print(":");
		if(String(minutes)[2] == '"'){
			M5.Lcd.print("0");
			M5.Lcd.print(String(minutes)[1]);
		}else{
			M5.Lcd.print(String(minutes)[1]);
			M5.Lcd.print(String(minutes)[2]);
		}
	}
}

void updateToDo(){
	if(screen == 0){
		M5.Lcd.fillRect(35,120,250,100,M5.Lcd.color565(255, 255, 153));
		M5.Lcd.setCursor(45,130);
		M5.Lcd.setTextSize(3);
		M5.Lcd.setTextColor(BLACK,M5.Lcd.color565(255, 255, 153));
		M5.Lcd.print("ToDo --------");

		M5.Lcd.setCursor(65,180);
		M5.Lcd.setTextSize(2);
		M5.Lcd.setTextColor(RED,M5.Lcd.color565(255, 255, 153));
		M5.Lcd.print(String(todo));
	}else if(screen == 1){
		M5.Lcd.fillRect(30,120,260,40,M5.Lcd.color565(255, 255, 153));
		M5.Lcd.setCursor(45,130);
		M5.Lcd.setTextSize(2);
		M5.Lcd.setTextColor(BLACK,M5.Lcd.color565(255, 255, 153));
		M5.Lcd.print("ToDo : ");

		M5.Lcd.setCursor(130,130);
		M5.Lcd.setTextSize(2);
		M5.Lcd.setTextColor(RED,M5.Lcd.color565(255, 255, 153));
		M5.Lcd.print(String(todo));
	}else if(screen == 2){
		M5.Lcd.setCursor(50,80);
		M5.Lcd.setTextSize(2);
		M5.Lcd.setTextColor(RED);
		M5.Lcd.print(String(todo));
	}
}

void PicNotification(int x,int y){
  M5.Lcd.fillRect(x,y,70,50,BLUE);
  M5.Lcd.fillCircle(x+34,y+24,17,WHITE);
  
  M5.Lcd.fillCircle(x+34,y+36,3,Y1);   
    M5.Lcd.fillRoundRect(x+23,y+31,22,5,1,Y2);
  M5.Lcd.fillRect(x+27,y+15,15,14,Y1);
  M5.Lcd.fillCircle(x+34,y+16,7,Y1);
    M5.Lcd.fillCircle(x+34,y+10,2,Y2);
}

void PicCalendar(int x,int y){
  M5.Lcd.fillRect(x,y,70,50,RED);
  M5.Lcd.fillCircle(x+34,y+24,17,WHITE);

  M5.Lcd.fillRect(x+23,y+16,23,19,Y1);
  
  M5.Lcd.fillRect(x+23,y+16,23,2,BLACK);
  M5.Lcd.fillRect(x+23,y+33,23,2,BLACK);
  M5.Lcd.fillRect(x+23,y+16,2,19,BLACK);
  M5.Lcd.fillRect(x+44,y+16,2,19,BLACK);

  M5.Lcd.fillRect(x+28,y+13,2,6,BLACK);
  M5.Lcd.fillRect(x+33,y+13,2,6,BLACK);
  M5.Lcd.fillRect(x+38,y+13,2,6,BLACK); 

  M5.Lcd.fillRect(x+33,y+23,3,3,BLACK);
  M5.Lcd.fillRect(x+38,y+23,3,3,BLACK);    
  M5.Lcd.fillRect(x+33,y+29,3,3,BLACK);
  M5.Lcd.fillRect(x+38,y+29,3,3,BLACK); 
  M5.Lcd.fillRect(x+28,y+23,3,3,BLACK);
  M5.Lcd.fillRect(x+28,y+29,3,3,BLACK);
  
}

void PicMenu(int x,int y){
  M5.Lcd.fillRect(x,y,70,50,Y1);
  M5.Lcd.fillRect(x+7,y+10,55,33,WHITE);

  SetText(x+11,y+20,"Back",BLACK,2);
}

void SetText(int x,int y,char *text,int color,int size){
  M5.Lcd.setTextColor(color);
  M5.Lcd.setTextSize(size);
  M5.Lcd.setCursor(x,y);
  M5.Lcd.print(text);
}