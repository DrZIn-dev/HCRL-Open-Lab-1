#include <Arduino.h>
#include <M5Stack.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>         //? Connect WiFi
#include <PubSubClient.h> //? Client MQTT

#define PIN 15
#define NUMPIXELS 10

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
//Functions
void Menu();
void GOD(int);
void RELAX(int);
void CREDIT();
void colorR();
void colorL();
void callback(char *topic, byte *payload, unsigned int length);
void mqttReconnect();
void mqttSetup();
void wifiSetup();
void mqttTestPub();

//============================== GAME =================================
boolean alienShot(int num);
void down();
void drawBitmap(char img[], int imgW, int imgH, int x, int y, int scale);
void drawScore(boolean win);
boolean exceedBoundary(int num);
int findAlienX(int num);
int findAlienY(int num);
int findOldAlienX(int num);
int findOldAlienY(int num);
void fireDaLazer();
void gameOver();
void keepFirinDaLazer(int bulletNo);
void left();
void levelUp();
void loop();
void moveAliens();
void moveShip();
boolean onPlayer(int num);
void right();
void select();
void setup();
void stopFirinDaLazer(int bulletNo);
void up();

//============================= game variables =========================
unsigned long offsetM = 0;
unsigned long offsetT = 0;
unsigned long offsetF = 0;
unsigned long offsetB = 0;
unsigned long offsetA = 0;
unsigned long offsetAF = 0;
unsigned long offsetAB = 0;
unsigned long offsetS = 0;
int threshold = 40;
boolean startPrinted = false;
boolean beginGame = false;
boolean beginGame2 = true;
boolean play = false;
int score = 0;
int scoreInc = 10;
int level = 1;
//---------------------Player---------------------------------------
int shipX = 147;
int shipY = 190;
int oldShipX = 0;
int oldShipY = 0;
int changeShipX = 0;
int changeShipY = 0;
int shipSpeed = 50;
boolean doSplode = false;
boolean fire = false;
boolean can = false;
int fFireX[5] = {0, 0, 0, 0, 0};
int fFireY[5] = {0, 0, 0, 0, 0};
int fFireAge[5] = {0, 0, 0, 0, 0};
//--------------------------Aliens----------------------------------
boolean alienLive[18];
int alienLiveCount = 18;
int alienX = 7;
int alienY = 25;
int oldAlienX = 7;
int oldAlienY = 25;
int changeAlienX = 6;
int changeAlienY = 0;
int alienSpeed = 200;
int oldAlienSpeed;
int aFireX[5];
int aFireY[5];
boolean aFireAge[5];
int chanceOfFire = 2;
//================================ bitmaps ========================
//your starship
const int shipImgW = 14;
const int shipImgH = 16;
char shipImg[] = "ZZZZZZWWZZZZZZZZZZZYWWYZZZZZZZZZZWWWWZZZZZZZZZZ"
                 "WWWWZZZZZZZZZWWWWWWZZZZZZZZWWWWWWZZZZZYZZWWWWWWZZYZZYZZWWWWWWZZ"
                 "YZWWZZWWBBWWZZWWWWZZWBBBBWZZWWWWZWWBBBBWWZWWWWZWWWWWWWWZWWWWWWW"
                 "WWWWWWWWWWRWWWWWWWWWWRWZZWWWWWWWWWWZZZZZWRRWWRRWZZZ";
//flames
const int flamesImgW = 12;
const int flamesImgH = 6;
char flamesImg[] = "RZZZZZZZZZZRRZRYYRRYYRZRRZRYYRRYYRZRZZRYRZZRY"
                   "RZZZZZRZZZZRZZZZZZRZZZZRZZZ";
//alien
const int alienImgW = 14;
const int alienImgH = 11;
char alienImg[] = "GGGZZZZZZZZGGGZZZGZZZZZZGZZZZZGGGGGGGGGGZZZGGG"
                  "GGGGGGGGGZGGGZGGGGGGZGGGGGGZZGGGGZZGGGGGGGGGGGGGGGGGGGGGGGGGGGG"
                  "GGGGZZZGGZZGGZZZGZGGZGGZZGGZGGZZZZZZGZZGZZZZZ";
//ship 'sploded
const int splodedImgW = 14;
const int splodedImgH = 16;
char splodedImg[] = "ZZZZZZWWZZZZZZZZZZRYWWYRZZZYZZZRRWWRRRRZRWYZ"
                    "RRRRRYYRRRZWYZRYRYYRYYRRRZWWRYYYRYYYYYRZWWRYYRYRYYYYRRWWRYYYRWR"
                    "YBRRZRRRYRRWWWRYRWZZRYYRRBBWRYRWWZZRYYBBBRRYBWWRZZRYYYRRYYZZWZR"
                    "RWRYYRBYRZZWZZRYBRYYYYYRRZZRWWYYYWWRRRZZZZWRRWWRRRWZZZ";

#define WiFi_STA_USERNAME "Pipek_PC" //? WiFi UserName
#define WiFi_STA_PASSWORD "12345678"

//* MQTT Server Details
#define MQTT_SERVER "soldier.cloudmqtt.com" //?Server URLS
#define MQTT_PORT 16880                     //? Server Port
#define MQTT_USERNAME "zuzu2307"            //? MQTT UserName
#define MQTT_PASSWORD "741852963"           //? MQTT Password
#define MQTT_NAME "ESP32_1"                 //?Topic Name

//* Params
#define Intervals 500 //? Global Interval Time
unsigned long long previousMillis_WiFi = 0,
                   previousMillis_MQTT = 0,
                   previousMillis_PUB = 0;

#define MQTT_SUB_TEST "/ESP32_1/Test" //? Test MQTT Only
#define MQ "/CE/GOD"

WiFiClient client;         //? Create TCP Connection
PubSubClient mqtt(client); //? Create Mqtt over WiFiClient

int state_MENU = 0, state = 0, state_GOD = 0, state_RELAX = 0, state_GAME = 0;

void setup()
{
  // put your setup code here, to run once:
  M5.begin();
  pixels.begin();
  M5.Lcd.setBrightness(200);
  M5.Speaker.setVolume(10);
  M5.Speaker.update();
  Serial.begin(115200);
  wifiSetup();
  mqttSetup();
  Menu();
  // memset(alienLive, true, 18);
  // memset(aFireX, 0, 5);
  // memset(aFireY, 0, 5);
  // memset(aFireAge, 0, 5);
  // M5.Lcd.setRotation(1); //M5.Lcd.setRotation(3);
  // M5.Lcd.fillScreen(ILI9341_BLACK);
  // M5.Lcd.setTextColor(0x5E85);
  // M5.Lcd.setTextSize(4);
  randomSeed(analogRead(6));
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  pinMode(BUTTON_C_PIN, INPUT_PULLUP);
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
  }

  if (state == 0)
  {
    if (M5.BtnC.wasPressed())
    {

      colorR();
      state_MENU += 1;
      Menu();
      if (state_MENU > 2)
      {
        state_MENU = 0;
      }
    }
    else if (M5.BtnA.wasPressed())
    {
      colorL();
      // M5.Speaker.beep();
      state_MENU -= 1;
      Menu();
      if (state_MENU < 0)
      {
        state_MENU = 2;
      }
    }
    else if (M5.BtnB.wasPressed())
    {
      switch (state_MENU)
      {
      case 0:
        GOD(state_GOD);
        state = 1;
        break;
      case 1:
        RELAX(state_RELAX);
        state = 2;
        break;
      case 2:
        CREDIT();
        state = 3;
        break;
      }
    }
    if (state == 0)
    {
      switch (state_MENU)
      {
      case 0:
        M5.Lcd.setCursor(110, 200);
        M5.Lcd.print("CE_GOD");
        break;
      case 1:
        M5.Lcd.setCursor(110, 200);
        M5.Lcd.print("RELAX");
        break;
      case 2:
        M5.Lcd.setCursor(110, 200);
        M5.Lcd.print("CREDIT");
        break;
      }
    }
  }
  else if (state == 1)
  {

    if (M5.BtnC.wasPressed())
    {
      state_GOD += 1;

      if (state_GOD > 6)
      {
        state_GOD = 0;
      }
      GOD(state_GOD);
    }
    else if (M5.BtnA.wasPressed())
    {
      state_GOD -= 1;
      if (state_GOD < 0)
      {
        state_GOD = 6;
      }
      GOD(state_GOD);
    }
    else if (M5.BtnA.isPressed() && M5.BtnC.isPressed())
    {
      state = 0;
      Menu();
    }
    else if (M5.BtnB.wasPressed())
    {
      switch (state_GOD)
      {
      case 0:
        mqtt.publish(MQ, "Toey");
        break;
      case 1:
        mqtt.publish(MQ, "Ou");
        break;
      case 2:
        mqtt.publish(MQ, "Time");
        break;
      case 3:
        mqtt.publish(MQ, "Pai");
        break;
      case 4:
        mqtt.publish(MQ, "Boy");
        break;
      case 5:
        mqtt.publish(MQ, "Zin");
        break;
      case 6:
        mqtt.publish(MQ, "Kan");
        break;
      }
    }
  }
  else if (state == 2)
  {
    if (M5.BtnC.wasPressed() && state_GAME == 0)
    {
      state_RELAX += 1;

      if (state_RELAX > 1)
      {
        state_RELAX = 0;
      }
      RELAX(state_RELAX);
    }
    else if (M5.BtnA.wasPressed() && state_GAME == 0)
    {
      state_RELAX -= 1;
      if (state_RELAX < 0)
      {
        state_RELAX = 1;
      }
      RELAX(state_RELAX);
    }
    else if (M5.BtnA.isPressed() && M5.BtnC.isPressed() && state_GAME == 0)
    {
      state = 0;
      Menu();
    }
    else if (M5.BtnB.wasPressed() && state_GAME == 0)
    {
      switch (state_RELAX)
      {
      case 0:
        memset(alienLive, true, 18);
        memset(aFireX, 0, 5);
        memset(aFireY, 0, 5);
        memset(aFireAge, 0, 5);
        M5.Lcd.setRotation(1); //M5.Lcd.setRotation(3);
        M5.Lcd.fillScreen(ILI9341_BLACK);
        M5.Lcd.setTextColor(0x5E85);
        M5.Lcd.setTextSize(4);
        state_GAME = 1;
        break;
      case 1:
        mqtt.publish(MQ, "Ou");
        break;
      }
    }
    else if (state == 3 && state_GAME == 0)
    {
      if (M5.BtnB.wasPressed())
      {
        state = 0;
        Menu();
      }
    }
    else if (state_GAME == 1)
    {
      if (state_RELAX == 0)
      {
        if (M5.BtnA.isPressed())
        {
          left();
        }
        if (M5.BtnB.isPressed())
        {
          right();
        }
        if (M5.BtnC.isPressed())
        {
          select();
        }
        //-------------Start Screen--------------
        if (millis() - offsetS >= 900 and !beginGame)
        {
          if (!startPrinted)
          {
            M5.Lcd.setCursor(77, 105);
            M5.Lcd.print(">START<");
            startPrinted = true;
            offsetS = millis();
          }
          else
          {
            M5.Lcd.fillRect(77, 105, 244, 32, ILI9341_BLACK);
            startPrinted = false;
            offsetS = millis();
          }
        }
        if (beginGame and beginGame2)
        {
          M5.Lcd.fillRect(77, 105, 244, 32, ILI9341_BLACK);
          beginGame2 = false;
          play = true;
        }
        //-------------Player-----------------------------------------------
        if (millis() - offsetM >= shipSpeed and play)
        {
          moveShip();
          offsetM = millis();
        }
        if (oldShipX != shipX or oldShipY != shipY)
        {
          M5.Lcd.fillRect(oldShipX, oldShipY, 28, 44, ILI9341_BLACK);
          oldShipX = shipX;
          oldShipY = shipY;
          drawBitmap(shipImg, shipImgW, shipImgH, shipX, shipY, 2);
        }
        if (fire and play)
        {
          fireDaLazer();
        }
        if (millis() - offsetB >= 50)
        {
          for (int i = 0; i < 5; i++)
          {
            if (fFireAge[i] < 20 and fFireAge[i] > 0)
            {
              keepFirinDaLazer(i);
            }
            if (fFireAge[i] == 20)
            {
              stopFirinDaLazer(i);
            }
          }
          offsetB = millis();
        }
        if (millis() - offsetT > 50)
        {
          changeShipX = 0;
          changeShipY = 0;
        }
        //---------------Aliens--------------------------------------------
        if (millis() - offsetA >= alienSpeed and play)
        {
          moveAliens();
          offsetA = millis();
        }
        if (findAlienX(5) >= 294)
        {
          changeAlienX = -3;
          changeAlienY = 7;
        }
        if (alienX <= 6)
        {
          changeAlienX = 3;
          changeAlienY = 7;
        }
        alienLiveCount = 0;
        for (int i = 0; i < 18; i++)
        {
          if (alienLive[i])
          {
            alienLiveCount += 1;
            if (alienShot(i))
            {
              M5.Lcd.fillRect(findOldAlienX(i), findOldAlienY(i), 28, 22, BLACK);
              alienLiveCount -= 1;
              alienLive[i] = false;
              score += scoreInc;
            }
            if (onPlayer(i) or exceedBoundary(i))
            {
              gameOver();
              delay(1500);
              state_GAME = 0;
              M5.Lcd.setTextSize(3);
              RELAX(state_RELAX);
              can = false;
            }
          }
        }
        if (alienLiveCount == 1)
        {
          oldAlienSpeed = alienSpeed;
          if (alienSpeed > 50)
          {
            alienSpeed -= 10;
          }
          else
          {
            alienSpeed = 20;
          }
        }
        if (alienLiveCount == 0)
        {
          levelUp();
        }
      }
    }
  }

  M5.update();
}

void Menu()
{
  M5.Lcd.drawJpgFile(SD, "/mm.jpg");
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(50, 200);
  M5.Lcd.print("<");
  M5.Lcd.setCursor(250, 200);
  M5.Lcd.print(">");
}

void RELAX(int n)
{
  switch (n)
  {
  case 0:
    M5.Lcd.drawJpgFile(SD, "/space.jpg");
    M5.Lcd.setCursor(110, 200);
    M5.Lcd.print("START");
    break;
  case 1:
    M5.Lcd.drawJpgFile(SD, "/Ou.jpg");
    M5.Lcd.setCursor(140, 200);
    M5.Lcd.print("Ou");
    break;
  }
}

void CREDIT()
{
  M5.Lcd.drawJpgFile(SD, "/credit.jpg");
}

void GOD(int n)
{
  switch (n)
  {
  case 0:
    M5.Lcd.drawJpgFile(SD, "/toey.jpg");
    M5.Lcd.setCursor(120, 200);
    M5.Lcd.print("Toey");
    break;
  case 1:
    M5.Lcd.drawJpgFile(SD, "/Ou.jpg");
    M5.Lcd.setCursor(140, 200);
    M5.Lcd.print("Ou");
    break;
  case 2:
    M5.Lcd.drawJpgFile(SD, "/Time.jpg");
    M5.Lcd.setCursor(120, 200);
    M5.Lcd.print("Time");
    break;
  case 3:
    M5.Lcd.drawJpgFile(SD, "/pai.jpg");
    M5.Lcd.setCursor(125, 200);
    M5.Lcd.print("Pai");
    break;
  case 4:
    M5.Lcd.drawJpgFile(SD, "/boy.jpg");
    M5.Lcd.setCursor(125, 200);
    M5.Lcd.print("Boy");
    break;
  case 5:
    M5.Lcd.drawJpgFile(SD, "/Zin.jpg");
    M5.Lcd.setCursor(125, 200);
    M5.Lcd.print("Zin");
    break;
  case 6:
    M5.Lcd.drawJpgFile(SD, "/Karn.jpg");
    M5.Lcd.setCursor(125, 200);
    M5.Lcd.print("Kan");
    break;
  }
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(50, 200);
  M5.Lcd.print("<");
  M5.Lcd.setCursor(250, 200);
  M5.Lcd.print(">");
}

void colorR()
{
  pixels.setPixelColor(0, pixels.Color(100, 100, 100));
  pixels.setPixelColor(2, pixels.Color(100, 100, 100));
  pixels.setPixelColor(4, pixels.Color(100, 100, 100));
  pixels.show();
  delay(200);
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.setPixelColor(2, pixels.Color(0, 0, 0));
  pixels.setPixelColor(4, pixels.Color(0, 0, 0));
  pixels.show();
}

void colorL()
{
  pixels.setPixelColor(5, pixels.Color(100, 100, 100));
  pixels.setPixelColor(7, pixels.Color(100, 100, 100));
  pixels.setPixelColor(9, pixels.Color(100, 100, 100));
  pixels.show();
  delay(200);
  pixels.setPixelColor(5, pixels.Color(0, 0, 0));
  pixels.setPixelColor(7, pixels.Color(0, 0, 0));
  pixels.setPixelColor(9, pixels.Color(0, 0, 0));
  pixels.show();
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
    mqtt.subscribe(MQ);
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

// functions =======================================================
void gameOver()
{
  play = false;
  if (doSplode)
  {
    drawBitmap(splodedImg, splodedImgW, splodedImgH, shipX, shipY, 2);
  }
  M5.Lcd.fillScreen(ILI9341_BLACK);

  drawScore(false);
  delay(1000);
  M5.Lcd.setCursor(70, 168);
  M5.Lcd.setTextSize(2);
  M5.Lcd.print("(B to replay)");
}
//==================================================================
void drawScore(boolean win)
{
  M5.Lcd.setCursor(53, 40);
  M5.Lcd.setTextColor(ILI9341_WHITE);
  M5.Lcd.setTextSize(4);
  if (win)
  {
    M5.Lcd.print("LEVEL UP!");
  }
  else
  {
    M5.Lcd.print("GAME OVER");
  }
  for (; millis() - offsetM <= 1000;)
    M5.Lcd.setCursor(59, 89);
  M5.Lcd.setTextSize(3);
  M5.Lcd.print("Score: ");
  M5.Lcd.print(score);
  offsetM = millis();
  for (; millis() - offsetM <= 1000;)
  {
  }
  M5.Lcd.setCursor(71, 128);
  M5.Lcd.print("Level: ");
  M5.Lcd.print(level);
}
//==================================================================
void levelUp()
{
  play = false;
  memset(alienLive, true, 18);
  memset(aFireX, 0, 5);
  memset(aFireY, 0, 5);
  memset(aFireAge, 0, 5);
  alienX = 7;
  alienY = 25;
  oldAlienX = 7;
  oldAlienY = 25;
  alienSpeed = oldAlienSpeed;
  if (alienSpeed > 100)
  {
    alienSpeed -= 10;
    chanceOfFire -= 10;
  }
  else if (alienSpeed > 50)
  {
    alienSpeed -= 10;
    chanceOfFire -= 5;
  }
  else if (alienSpeed > 25)
  {
    alienSpeed -= 5;
    chanceOfFire -= 1;
  }
  score += 50;
  scoreInc += 5;
  changeShipX = 0;
  changeShipY = 0;
  for (unsigned long i = millis(); millis() - i <= 1600;)
  {
    if (millis() - offsetM >= 20)
    {
      M5.Lcd.fillRect(oldShipX, oldShipY, 28, 44, ILI9341_BLACK);
      drawBitmap(shipImg, shipImgW, shipImgH, shipX, shipY, 2);
      drawBitmap(flamesImg, flamesImgW, flamesImgH, shipX + 1,
                 shipY + 32, 2);
      oldShipX = shipX;
      oldShipY = shipY;
      shipY -= 6;
      offsetM = millis();
    }
  }
  drawScore(true);
  level += 1;
  shipX = 147;
  shipY = 190;
  for (; millis() - offsetM <= 4000;)
  {
  }
  M5.Lcd.fillScreen(ILI9341_BLACK);
  offsetM = millis();
  play = true;
}
//==================================================================
boolean alienShot(int num)
{
  for (int i; i < 5; i++)
  {
    if (fFireAge[i] < 20 and fFireAge[i] > 0)
    {
      if (fFireX[i] > findAlienX(num) - 4 and fFireX[i] < findAlienX(num) + 28 and fFireY[i] < findAlienY(num) + 22 and fFireY[i] > findAlienY(num) + 4)
      {
        fFireAge[i] = 20;
        return true;
      }
    }
  }
  return false;
}
//==================================================================
boolean onPlayer(int num)
{
  if (findAlienX(num) - shipX < 24 and findAlienX(num) - shipX > -28 and findAlienY(num) - shipY < 32 and
      findAlienY(num) - shipY > -22)
  {
    doSplode = true;
    return true;
  }
  else
  {
    return false;
  }
}
//==================================================================
boolean exceedBoundary(int num)
{
  if (findAlienY(num) > 218)
  {
    return true;
  }
  else
  {
    return false;
  }
}
//==================================================================
void moveAliens()
{
  for (int i = 0; i < 18; i++)
  {
    if (alienLive[i])
    {
      M5.Lcd.fillRect(findOldAlienX(i), findOldAlienY(i), 28, 22, BLACK);
      drawBitmap(alienImg, alienImgW, alienImgH, findAlienX(i),
                 findAlienY(i), 2);
    }
  }
  oldAlienX = alienX;
  oldAlienY = alienY;
  alienX += changeAlienX;
  alienY += changeAlienY;
  if (changeAlienY != 0)
  {
    changeAlienY = 0;
  }
}
//==================================================================
int findAlienX(int num) { return alienX + 42 * (num % 6); }
//==================================================================
int findAlienY(int num) { return alienY + 33 * (num / 6); }
//==================================================================
int findOldAlienX(int num) { return oldAlienX + 42 * (num % 6); }
//==================================================================
int findOldAlienY(int num) { return oldAlienY + 33 * (num / 6); }
//---------------------------Player---------------------------------
void fireDaLazer()
{
  int bulletNo = -1;
  for (int i = 0; i < 4; i++)
  {
    if (fFireAge[i] == 0)
    {
      bulletNo = i;
    }
  }
  if (bulletNo != -1)
  {
    fFireAge[bulletNo] = 1;
    fFireX[bulletNo] = shipX + 13;
    fFireY[bulletNo] = shipY - 4;
    M5.Lcd.fillRect(fFireX[bulletNo], fFireY[bulletNo], 4, 3, MAGENTA);
  }
  fire = false;
}
//==================================================================
void keepFirinDaLazer(int bulletNo)
{
  M5.Lcd.fillRect(fFireX[bulletNo], fFireY[bulletNo], 4, 4, BLACK);
  fFireY[bulletNo] -= 8;
  M5.Lcd.fillRect(fFireX[bulletNo], fFireY[bulletNo], 4, 4, MAGENTA);
  fFireAge[bulletNo] += 1;
}
//==================================================================
void stopFirinDaLazer(int bulletNo)
{
  M5.Lcd.fillRect(fFireX[bulletNo], fFireY[bulletNo], 4, 4, BLACK);
  fFireAge[bulletNo] = 0;
}
//==================================================================
void moveShip()
{
  if (shipX + changeShipX < 288 and shipX + changeShipX > 6 and changeShipX != 0)
  {
    shipX += changeShipX;
  }
  if (shipY + changeShipY > 24 and shipY + changeShipY < 192 and changeShipY != 0)
  {
    shipY += changeShipY;
  }
  if (oldShipX != shipX or oldShipY != shipY)
  {
    M5.Lcd.fillRect(oldShipX, oldShipY, 28, 44, ILI9341_BLACK);
    oldShipX = shipX;
    oldShipY = shipY;
    drawBitmap(shipImg, shipImgW, shipImgH, shipX, shipY, 2);
  }
}
//==================================================================
void drawBitmap(char img[], int imgW, int imgH, int x, int y, int scale)
{
  uint16_t cellColor;
  char curPix;
  for (int i = 0; i < imgW * imgH; i++)
  {
    curPix = img[i];
    if (curPix == 'W')
    {
      cellColor = ILI9341_WHITE;
    }
    else if (curPix == 'Y')
    {
      cellColor = ILI9341_YELLOW;
    }
    else if (curPix == 'B')
    {
      cellColor = ILI9341_BLUE;
    }
    else if (curPix == 'R')
    {
      cellColor = ILI9341_RED;
    }
    else if (curPix == 'G')
    {
      cellColor = 0x5E85;
    }
    if (curPix != 'Z' and scale == 1)
    {
      M5.Lcd.drawPixel(x + i % imgW, y + i / imgW, cellColor);
    }
    else if (curPix != 'Z' and scale > 1)
    {
      M5.Lcd.fillRect(x + scale * (i % imgW), y + scale * (i / imgW), scale, scale, cellColor);
    }
  }
}
//=========================== button functions =====================
void up()
{
  if (millis() - offsetT >= 50 and play)
  {
    changeShipX = 0;
    changeShipY = -6;
    offsetT = millis();
  }
}
//==================================================================
void down()
{
  if (millis() - offsetT >= 50 and play)
  {
    changeShipX = 0;
    changeShipY = 6;
    offsetT = millis();
  }
}
//==================================================================
void left()
{
  if (millis() - offsetT >= 50 and play)
  {
    changeShipX = -6;
    changeShipY = 0;
    offsetT = millis();
  }
}
//==================================================================
void right()
{
  if (millis() - offsetT >= 50 and play)
  {
    changeShipX = 6;
    changeShipY = 0;
    offsetT = millis();
  }
}
//==================================================================
void select()
{
  if (millis() - offsetF >= 500 and play)
  {
    fire = true;
    offsetF = millis();
  }
  if (!beginGame)
  {
    beginGame = true;
  }
}
//==================================================================
