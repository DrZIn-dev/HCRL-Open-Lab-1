#include <Arduino.h>      //? Main Arduino Lib Framework
#include <WiFi.h>         //? Connect WiFi
#include <PubSubClient.h> //? Client MQTT
#include <M5Stack.h>

//* Prototype Function
void callback(char *topic, byte *payload, unsigned int length);
void mqttReconnect();
void mqttSetup();
void wifiSetup();
void mqttTestPub();

void MainMenu();
void Menu();
void MenuToDoList();
void MenuTimer();
void MenuSetting();
void ToDoList();
void Timer();
void Setting();
void pressB();
void pressA();
void pressC();
void deleteTDL();
void Countdown();
void Counter();
void ChangeLED();
void changeVolume();
void TimerCounterStart();
void TimerCounterStop();
void loop_Display();
void TimerCounterCurrent();

String t1;
String t2;
String t3;
String t4;
String t5;
String t6;

int butt=0;

int notice=0,noticeCount=0,noticeShow=0;
int p=0,k=0,timcnt=0;
bool isPrint=false;

//* WIFI STA Details
#define WiFi_STA_USERNAME "LEGENDZ"   //? WiFi UserName
#define WiFi_STA_PASSWORD "123456789" //? WiFi Password

//* MQTT Server Details
#define MQTT_SERVER "soldier.cloudmqtt.com" //?Server URLS
#define MQTT_PORT 16518                     //? Server Port
#define MQTT_USERNAME "mrzombit"            //? MQTT UserName
#define MQTT_PASSWORD "game1000"            //? MQTT Password
#define MQTT_NAME "ESP32_1"                 //?Topic Name

//* Params
#define Intervals 500 //? Global Interval Time
unsigned long long previousMillis_WiFi = 0,
                   previousMillis_MQTT = 0,
                   previousMillis_PUB = 0;

//* SUB Topic
#define MQTT_SUB_TEST "/t" //? Test MQTT Only
#define MQTT_SUB_TEST1 "/t1"
#define MQTT_SUB_TEST2 "/t2"
#define MQTT_SUB_TEST3 "/t3"
#define MQTT_SUB_TEST4 "/t4"
#define MQTT_SUB_TEST5 "/t5"
#define MQTT_SUB_TEST6 "/t6"

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
    MainMenu();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (mqtt.connected() == false)
  {
    mqttReconnect();
  }
  else
  {
    loop_Display();
    mqtt.loop();

  }
  //mqttTestPub();
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
  if (topic_str.equals(MQTT_SUB_TEST1))
  {
    t1 = payload_str;
    Serial.println("t1 : " + t1);
  }
   if (topic_str.equals(MQTT_SUB_TEST2))
  {
    t2 = payload_str;
    Serial.println("t2 : " + t2);
  }
   if (topic_str.equals(MQTT_SUB_TEST3))
  {
    t3 = payload_str;
    Serial.println("t3 : " + t3);
  }
   if (topic_str.equals(MQTT_SUB_TEST4))
  {
    t4 = payload_str;
    Serial.println("t4 : " + t4);
  }
   if (topic_str.equals(MQTT_SUB_TEST5))
  {
    t5 = payload_str;
    Serial.println("t5 : " + t5);
  }
   if (topic_str.equals(MQTT_SUB_TEST6))
  {
    t6 = payload_str;
    Serial.println("t6 : " + t6);
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
    mqtt.subscribe(MQTT_SUB_TEST1);
    mqtt.subscribe(MQTT_SUB_TEST2);
    mqtt.subscribe(MQTT_SUB_TEST3);
    mqtt.subscribe(MQTT_SUB_TEST4);
    mqtt.subscribe(MQTT_SUB_TEST5);
    mqtt.subscribe(MQTT_SUB_TEST6);
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


void MainMenu()
{
    M5.lcd.fillScreen(ORANGE);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(100,30);
    M5.Lcd.print("DESK!\n  ASSISTANCE");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(100,100);
    M5.Lcd.print("Press C to enter Menu");

    if(notice>0)
    {
      for(noticeShow=noticeCount;noticeShow>noticeCount-2;noticeShow--)
      {
        M5.Lcd.setTextColor(BLUE);
        M5.Lcd.setTextSize(2);
        M5.Lcd.setCursor(120,100);
        M5.Lcd.print("Notice" +noticeCount);
      }
    }
}

void Menu()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("MENU");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.lcd.fillCircle(60,100,30,RED);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(12,140);
    M5.Lcd.print("ToDoList");

    M5.lcd.fillCircle(160,100,30,BLUE);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(133,140);
    M5.Lcd.print("Timer");

    M5.lcd.fillCircle(250,100,30,GREEN);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(210,140);
    M5.Lcd.print("Setting");

}

void MenuToDoList()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("MENU");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.lcd.fillCircle(60,100,30,RED);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(12,140);
    M5.Lcd.print("ToDoList");

    M5.lcd.fillCircle(160,100,30,BLUE);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(133,140);
    M5.Lcd.print("Timer");

    M5.lcd.fillCircle(250,100,30,GREEN);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(210,140);
    M5.Lcd.print("Setting");

    M5.lcd.fillCircle(60,100,20,WHITE);

}

void MenuTimer()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("MENU");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.lcd.fillCircle(60,100,30,RED);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(12,140);
    M5.Lcd.print("ToDoList");

    M5.lcd.fillCircle(160,100,30,BLUE);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(133,140);
    M5.Lcd.print("Timer");

    M5.lcd.fillCircle(250,100,30,GREEN);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(210,140);
    M5.Lcd.print("Setting");
    M5.lcd.fillCircle(160,100,20,WHITE);

}

void MenuSetting()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("MENU");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.lcd.fillCircle(60,100,30,RED);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(12,140);
    M5.Lcd.print("ToDoList");

    M5.lcd.fillCircle(160,100,30,BLUE);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(133,140);
    M5.Lcd.print("Timer");

    M5.lcd.fillCircle(250,100,30,GREEN);
    M5.Lcd.setTextColor(NAVY);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(210,140);
    M5.Lcd.print("Setting");
    M5.lcd.fillCircle(250,100,20,WHITE);
}

void ToDoList()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(220,192, 100, 70,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,60);
    M5.Lcd.print("  " + t1);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,80);
    M5.Lcd.print("  " + t2);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,100);
    M5.Lcd.print("  " + t3);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,120);
    M5.Lcd.print("  " + t4);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,140);
    M5.Lcd.print("  " + t5);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,160);
    M5.Lcd.print("  " + t6);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TO DO LIST");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(240,207);
    M5.Lcd.print("DELETE");
}

void Timer()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TIMER");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,80);
    M5.Lcd.print("PRESS > TO SELECT MODE");
}

void Setting()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("SETTING");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(15,80);
    M5.Lcd.print("PRESS + TO ADJUST VOLUME\n  AND CHANGE LED COLOR");
}

void DeleteToDoList()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TO DO LIST");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print(" NO");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(278,207);
    M5.Lcd.print("YES");

    M5.Lcd.fillRect(0,60, 360, 120,WHITE);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,80);
    M5.Lcd.print("  DO YOU WANT\n  TO DELETE THIS\n   TO DO LIST?");
}

void TimerCountDown()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 400, 60,BLACK);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(220,192, 100, 70,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TIMER");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(130,207);
    M5.Lcd.print("SELECT");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50,80);
    M5.Lcd.print("COUNTDOWN MODE");

}

void TimerCounter()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 400, 60,BLACK);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(220,192, 100, 70,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TIMER");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(130,207);
    M5.Lcd.print("SELECT");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50,80);
    M5.Lcd.print("COUNTER MODE");

}

void TimerPomodoro()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 400, 60,BLACK);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(220,192, 100, 70,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TIMER");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(130,207);
    M5.Lcd.print("SELECT");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50,80);
    M5.Lcd.print("POMODORO MODE");
}

void setVolume()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 400, 60,BLACK);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    M5.Lcd.fillRect(50,120, 210, 30,BLACK);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("SETTING");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(130,207);
    M5.Lcd.print("   +");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50,80);
    M5.Lcd.print("VOLUME");
}

void setNoticeColor()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 400, 60,BLACK);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(256,192, 100, 60,NAVY);

    //M5.Lcd.fillRect(50,120, 210, 30,BLACK);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("SETTING");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(130,207);
    M5.Lcd.print("   +");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(281,207);
    M5.Lcd.print(">");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50,80);
    M5.Lcd.print("LED COLOR");
}

void setup_2()
{
    M5.begin();
    MainMenu();
}

void loop_Display()
{

        if (M5.BtnA.wasPressed())
        {
            if(p==0||p==1||p==2||p==3||p==4)
            {
              k=p;
              p--;
            }
            else pressA();
            isPrint=false;
        }
        if(M5.BtnB.wasPressed())
        {
            butt =1;
            pressB();
            isPrint=false;
        }
        if (M5.BtnC.wasPressed())
        {
            if(p==0||p==1||p==2||p==3)
            {
              k=p;
              p++;
            }
            else pressC();
            isPrint=false;
        }

        if(isPrint==false)
        {
          switch(p)
          {
            case  0: MainMenu();        isPrint=true; break;
            case  1: Menu();            isPrint=true; break;
            case  2: MenuToDoList();    isPrint=true; break;
            case  3: MenuTimer();       isPrint=true; break;
            case  4: MenuSetting();     isPrint=true; break;
            case  5: ToDoList();        isPrint=true; break;
            case  6: Timer();           isPrint=true; break;
            case  8: DeleteToDoList();  isPrint=true; break;
            case  7: Setting();         isPrint=true; break;
            case  9: TimerCountDown();  isPrint=true; break;
            case 10: TimerCounter();    isPrint=true; break;
            case 11: TimerPomodoro();   isPrint=true; break;
            case 12: setVolume();       isPrint=true; break;
            case 13: setNoticeColor();  isPrint=true; break;
            // case 14: TimerCounterStart();  isPrint=true; break;
            // case 15: TimerCounterCurrent();  isPrint=true; break;
            // case 16: TimerCounterStop();  isPrint=true; break;
            // case 12:  isPrint=true; break;
            // case 13:  isPrint=true; break;

            default : p=k; break;
          }
        }
        M5.update();
}

void pressB()
{
  k=p;
  switch(p)
  {
    case  2: p=5;           break;
    case  3: p=6;           break;
    case  4: p=7;           break;
    case  10: p=14;          break;
    // case  14: p=15;          break;
    // case  15: p=16;           break;
  }
}

void pressA()
{
    k=p;
    switch(p)
    {
     case  5: p=2;    break;
     case  6: p=3;    break;
     case  7: p=4;    break;
     case  8: p=5;    break;
     case  9: p=3;    break;
     case 10: p=9;    break;
     case 11:p=10;    break;
     case 12: p=4;    break;
     case 13:p=12;    break;
    //  case 14: p=10;          break;
    //  case 15: p=10;           break;
    //  case 16: p=10;           break;
    }
}

void pressC()
{
    k=p;
    switch(p)
    {
      case  4: p=2;    break;
      case  5: p=8;    break;
      case  6: p=9;    break;
      case  7: p=12;   break;
      case  8: p=5;    break;
      case  9: p=10;   break;
      case 10: p=11;   break;
      case 11: p=6;    break;
      case 12: p=13;   break;
      case 13: p=7 ;   break;
    }
}


void DeleteTDL()
{

}


void Counter()
{   
    // for(butt==0)
    // {
    //   if(millis()>1000)
    //   {
    //     timcnt+=1;
    //     M5.Lcd.setTextColor(WHITE);
    //     M5.Lcd.setTextSize(3);
    //     M5.Lcd.setCursor(20,207);
    //     M5.Lcd.println(" " +timcnt,YELLOW);
    //   }
    // }
}


void Countdown()
{

}
void changeVolume()
{

}

void ChangeLED()
{

}


void TimerCounterStart()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 400, 60,BLACK);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(220,192, 100, 70,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TIME COUNTER MODE");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(130,207);
    M5.Lcd.print("START");

}

void TimerCounterCurrent()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 400, 60,BLACK);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(220,192, 100, 70,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TIME COUNTER MODE");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");
    
    Counter();

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(130,207);
    M5.Lcd.print("STOP");

}

void TimerCounterStop()
{
    M5.lcd.fillScreen(YELLOW);
    M5.Lcd.fillRect(0,0, 320, 55,ORANGE);
    M5.Lcd.fillRect(0,192, 400, 60,BLACK);
    M5.Lcd.fillRect(0,192, 100, 60,NAVY);
    M5.Lcd.fillRect(220,192, 100, 70,NAVY);

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(30,20);
    M5.Lcd.print("TIME COUNTER MODE");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(25,207);
    M5.Lcd.print("BACK");

    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(278,207);
    M5.Lcd.print("LAP");

    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(50,80);
    M5.Lcd.print("TOTALTIME : "+timcnt);

}

