#include <SoftwareSerial.h>

const byte rxPin = 3;
const byte txPin = 4;

SoftwareSerial SerialRS = SoftwareSerial(rxPin, txPin);

const int LED = 13;

const int P_LEFT = 12;    //left
const int P_UP = 11;      //up
const int P_RIGHT = 10;   //right
const int P_DOWN = 9;     //down
const int P_ZOOM_IN = 7;  //zoom in
const int P_ZOOM_OUT = 8; //zoom out

const byte address = 1; //camera address
byte speed = 100; // can be changed by pressing left+up or left+down

const byte C_STOP = 0x00;
const byte C_UP = 0x08;
const byte C_DOWN = 0x10;
const byte C_LEFT = 0x04;
const byte C_RIGHT = 0x02;

const byte C_ZOOMIN = 0x20;
const byte C_ZOOMOUT = 0x40;

const byte C_SET_PAN_POSITION = 0x4B;  // 1/100ths of degree
const byte C_SET_TILT_POSITION = 0x4D; // 1/100ths of degree

bool stoped = false;

void setup()
{
  pinMode(LED, OUTPUT);

  pinMode(P_LEFT, INPUT);     //left
  pinMode(P_UP, INPUT);       //up
  pinMode(P_RIGHT, INPUT);    //right
  pinMode(P_DOWN, INPUT);     //down
  pinMode(P_ZOOM_IN, INPUT);  //zoom in
  pinMode(P_ZOOM_OUT, INPUT); //zoom out

  Serial.begin(9600);
  SerialRS.begin(2400);
  Serial.println("Pelco D controller");

  sendPelcoDFrame(C_STOP, 0, 0);
  delay(10);
  sendPelcoDFrame(C_SET_PAN_POSITION, 23, 28);

  blinkLED();
}

void loop()
{

  if (check(P_LEFT) && check(P_DOWN))
  {
    speed = speed - 10;
    if (speed < 0)
      speed = 0;
    Serial.print("Speed:");
    Serial.println(speed);
    blinkLED();
    delay(1000);
    return;
  }

  if (check(P_LEFT) && check(P_UP))
  {
    speed = speed + 10;
    if (speed > 255)
      speed = 255;
    Serial.print("Speed:");
    Serial.println(speed);
    blinkLED();
    delay(1000);
    return;
  }

  if (check(P_LEFT))
  {
    sendPelcoDFrame(C_LEFT, speed, speed);
    Serial.println("left");
    stoped = false;
  }
  else if (check(P_RIGHT))
  {
    sendPelcoDFrame(C_RIGHT, speed, speed);
    Serial.println("right");
    stoped = false;
  }
  else if (check(P_UP))
  {
    sendPelcoDFrame(C_UP, speed, speed);
    Serial.println("up");
    stoped = false;
  }
  else if (check(P_DOWN))
  {
    sendPelcoDFrame(C_DOWN, speed, speed);
    Serial.println("down");
    stoped = false;
  }
  else if (check(P_ZOOM_IN))
  {
    sendPelcoDFrame(C_ZOOMIN, 0, 0);
    Serial.println("zoom in");
    stoped = false;
  }
  else if (check(P_ZOOM_OUT))
  {
    sendPelcoDFrame(C_ZOOMOUT, 0, 0);
    Serial.println("zoom out");
    stoped = false;
  }
  else
  {
    if (!stoped)
    {
      sendPelcoDFrame(C_STOP, 0, 0);
      delay(10);
      sendPelcoDFrame(C_STOP, 0, 0);
      Serial.println("stop");
      stoped = true;
    }
  }

  if (!stoped)
  {
    digitalWrite(LED, HIGH);
  }
  else
  {
    digitalWrite(LED, LOW);
  }

  delay(100);
}

bool check(int pin)
{
  return (digitalRead(pin) == HIGH);
}

void sendPelcoDFrame(byte command, byte data1, byte data2)
{
  byte bytes[7] = {0xFF, address, 0x00, command, data1, data2, 0x00};
  byte crc = (bytes[1] + bytes[2] + bytes[3] + bytes[4] + bytes[5]) % 0x100;
  bytes[6] = crc;

  for (int i = 0; i < 7; i++)
  {
    SerialRS.write(bytes[i]);
    // Serial.print(bytes[i], HEX); //debug
  }
  // Serial.println(); //debug
}

void blinkLED()
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }
}
