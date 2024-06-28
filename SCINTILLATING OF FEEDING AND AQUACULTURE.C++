#include <Wire.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
const int doSensorPin = A2;
LiquidCrystal lcd(8,9,10,11,12,13);
const int pingPin =6; // Trigger Pin of Ultrasonic Sensor
const int echoPin =7; // Echo Pin of Ultrasonic Sensor
long duration;
int distance;
const int bz=4;
int kk=0;
Servo feed;
int ts=A5;
int ps=A1;
int os=A2;
int is=A4;
int mp1=3;
int mp2=2;
int ams=A0;
void setup() {   
    Wire.begin();
    Serial.begin(9600);
    Serial.begin(115200);
    sensors.begin();
    lcd.begin(16, 2);
    lcd.setCursor(0,0);
    lcd.print("AQUA MNTRNG");
    feed.attach(5);
    pinMode(mp1,OUTPUT);
    pinMode(ams,INPUT);
    pinMode(pingPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(mp2,OUTPUT);
    pinMode(bz,OUTPUT);
     wifi_init();
    feed.write(0);
    digitalWrite(bz,0);
    digitalWrite(mp1,0);
    digitalWrite(mp2,0);
    pinMode(ts,INPUT);
    pinMode(ps,INPUT);
    pinMode(is,INPUT);
   // delay(2000);
}
void loop() {
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance= (duration/2)/29.1;  
    int tbval=analogRead(ts)/20;
    int pval=analogRead(ps)/55;
    int irval=1-digitalRead(is);
    int aval=analogRead(ams)/10;
    sensors.requestTemperatures();
    int tval = sensors.getTempCByIndex(0);
    int doValue = analogRead(doSensorPin);
  // Convert analog value to voltage
  float voltage = doValue * (5.0 / 1023.0); // Arduino UNO has 10-bit ADC
  // Convert voltage to dissolved oxygen concentration (in mg/L)
  // This conversion formula is specific to the Gravity: Analog DO Dissolved Oxygen Sensor
  float doConcentration = (voltage - 0.5) * 200.0;
  // Print dissolved oxygen concentration
  Serial.print("Dissolved Oxygen Concentration: ");
  Serial.print(doConcentration);
  Serial.println(" mg/L");
  delay(1000);
    lcd.clear();
    lcd.print("L:"+ String(distance) + " TB:"+ String(tbval) + " P:"+ String(pval));
    lcd.setCursor(0,1);
    lcd.print("T:" + String(tval) + " A:" + String(aval)+ " O:" + String(doConcentration));
    if(irval==1)
    {
      feed.write(90);
      delay(200);
    }
    else
    {
      digitalWrite(bz,1);
      feed.write(0);
      delay(200); 
      digitalWrite(bz,0);
    }
    if(distance>20)
    {
      digitalWrite(bz,1);
    }
    else
    {
      digitalWrite(bz,0);
    }
    if(tbval>35 && tbval<45)
    {
      analogWrite(mp1,60);
      digitalWrite(mp2,0);
      digitalWrite(bz,1);
    }
    else
    {
      digitalWrite(mp1,0);
      digitalWrite(mp2,0);
      digitalWrite(bz,0);
    }
    delay(300);
    if(kk>25)
    {
      kk=0;   
    upload(tval,pval,doConcentration,tbval,distance,irval,aval);
}
kk=kk+1;
}
void wifi_init()
 {
  Serial.println("AT+RST");
  delay(4000);
  Serial.println("AT+CWMODE=3");
  delay(4000);
  Serial.print("AT+CWJAP=");
  Serial.write('"');
  Serial.print("Shannu"); // ssid/user name
  Serial.write('"');
  Serial.write(',');
  Serial.write('"');
  Serial.print("12345678"); //password
  Serial.write('"');
  Serial.println();
  delay(1000);
 }
void upload(int x, int y, int z,int p ,int q,int r,int s) //ldr copied int to - x   and gas copied into -y
{
  
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  Serial.println(cmd);
  delay(1000);
  String getStr ="GET /update?api_key=LD3GO9Y3OV694GEO&field1=";
  getStr += String(x);
  getStr +="&field2=";
  getStr += String(y);
  getStr +="&field3=";
  getStr += String(z);
  getStr +="&field4=";
  getStr += String(p);
  getStr +="&field5=";
  getStr += String(q);
   getStr +="&field6=";
  getStr += String(r);
   getStr +="&field7=";
  getStr += String(s);
  getStr += "\r\n\r\n";
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  Serial.println(cmd);
  delay(1000);
  Serial.println(getStr);
}