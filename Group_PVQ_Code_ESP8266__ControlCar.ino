
#define BLYNK_PRINT Serial
/*
  /#define BLYNK_AUTH_TOKEN "SX2EsnvjqOgdAN8O-8xpnnel9NmlycGP */
#include<ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include<DHT.h>
#include<Servo.h>
/* Khai Bao cho Servo */
Servo myServo;
Servo myServo1;

#define DHTTYPE DHT11
SimpleTimer timer;
char ssid[] =  "WiFii";
char pass[] =  "201920222";
char auth[] = "w4FJI0s9i5rToL5ntildAuh_nax9u01-";
/* Khai báo chân cho động cơ Motor */
// Motor 1
int motor1Pin1 = 16;// chạy tới
int motor1Pin2 = 5; // Chạy lùi
int ENA = 14;
// Motor 2
int motor2Pin1 = 0;
int motor2Pin2 = 2;
int ENB = 12;
/* Khai báo và thiết lập các giá trị liên quan đến việc điều khiển thông qua blynk */
bool Forward = 0;
bool Backward = 0;
bool Left = 0;
bool Right = 0;
// Biến dùng để điều chỉnh tốc độ của xe
int speed = 255 ;
//Do nhiet do va do am
int PinDHT = 13 ;
DHT dht(PinDHT, DHTTYPE);
float Temperature = 0 ;
float Humidity = 0 ;
uint16_t second = 0;
// khai báo các giá trị cho cảm biến siêu âm (HCSR-04)
int Triger_Pin = 4;
int Echo_Pin = 15;
unsigned long duration ;
int distance ;
int goc;
/* Nguyên mẫu hàm */
void ControlCar();
void HcSr04();
void ReadTempAndHumi();
void Forwardd();
void Backwardd();
void Leftt();
void Rightt();
/*----------------------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------------------*/
void setup()
{
//
//  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
//    Serial.print(F("."));
  }
//  Serial.println(F("WiFi connected"));
//  Serial.print(WiFi.localIP());
  Blynk.begin(auth, ssid, pass, "blynk-server.com", 8080);
  /* Cấu hình trạng thái các chân I/O cho robot*/
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
 /* Thiết lập trạng thái ban đầu cho robot */
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW );
  digitalWrite(motor2Pin2, LOW);
  /* Sử dụng Timer đếm thời gian lấy nhiệt độ và độ ẩm */
  dht.begin();
  timer.setInterval(2000, ReadTempAndHumi);
  pinMode(PinDHT, OUTPUT);
  /* Cấu hình các chân chức năng cho cảm biến siêu âm (HCSR04) */
  pinMode(Triger_Pin, OUTPUT);
  pinMode(Echo_Pin, INPUT);
//  servo.attach(9,544,2400); // NodeMCU D8 pin
//  servo1.attach(10,544,2400);
  
}
/*----------------------------------------------------------------------------------------------------------------------*/
void loop()
{ Blynk.run();
  ControlCar();
  timer.run();
}
/*----------------------------------------------------------------------------------------------------------------------*/
/* Sử dụng câu lệnh BLYNK_WRITE(port) (port:V0,1....,200) thay đổi mức logic của các biến để điều khiển xe */
BLYNK_WRITE(V0)
{
  Forward = param.asInt();
}
BLYNK_WRITE(V1)
{
  Backward = param.asInt();
}
BLYNK_WRITE(V2)
{
  Left = param.asInt();
}
BLYNK_WRITE(V3)
{
  Right = param.asInt();
}
BLYNK_WRITE(V4)
  {

  speed = param.asInt();
  }
// điều khiển góc cho Servo qua Blynk 
BLYNK_WRITE(V8)
{
  myServo.attach(9,544,2400); 
  myServo.write(param.asInt());
}
BLYNK_WRITE(V9)
{
  myServo.attach(10,544,2400); 
  myServo1.write(param.asInt());
}
/*----------------------------------------------------------------------------------------------------------------------*/
void ControlCar()
{ 
/* Gọi hàm HCSR04 để đọc khoảng cách hiện tại của xe */
  HcSr04();
  if (Forward == 1)
  {
    
    if (distance > 20 )
    {
//         Serial.print("Forward");
      Forwardd();
    }
    else
      Stopp();
  }
  else if (Backward == 1)
  {
    
//      Serial.print("Backward");
      Backwardd();
  }
  else if (Left == 1)
  {  
//    Serial.print("Left");
     Leftt();
     delay(100);
    if (distance > 20)
    {
//      Serial.print("Left");
      Leftt();
    }
    else
    {
      Stopp();
    }

  }
  else if (Right == 1)
  {
//      Serial.print("Right");
      Rightt();
      delay(100);
    if (distance > 20)
    {
//      Serial.print("Right");
      Rightt();
    }
    else
    {
      Stopp();
    }

  }
  else
  {
//    Serial.print("Stop\n");
    //    delay(10000);
    Stopp();
  }
}
/*----------------------------------------------------------------------------------------------------------------------*/
/* Định nghĩa hàm thực hiện chức năng đo khoảng cách và hiển thị trên Blynk */
void HcSr04()
{
  digitalWrite(Triger_Pin, LOW);
  delayMicroseconds(2);
  //
  digitalWrite(Triger_Pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Triger_Pin, LOW);
  delayMicroseconds(2);
  duration = pulseIn(Echo_Pin, HIGH);
  distance = int(duration / 2 / 29.412); //cm
//    Serial.print("\nKhoang cach: ");
//    Serial.print(distance);
   Blynk.virtualWrite(V7,distance);
}
/**********************************************************************/
/* Định nghĩa hàm thức hiện chức năng đo nhiệt độ và độ ẩm */
void ReadTempAndHumi()
{
  Temperature = dht.readTemperature() ;
  Humidity = dht.readHumidity() ;
//  Serial.print("\nTemperature= ");
//  Serial.print(Temperature);
//  Serial.print("\tHumidity = ");
//  Serial.print(Humidity);
  Blynk.virtualWrite(V5, Temperature);
  Blynk.virtualWrite(V6, Humidity);
}
/*----------------------------------------------------------------------------------------------------------------------*/
void Forwardd ()
{
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
   analogWrite(ENA,speed);
   analogWrite(ENB,speed);
}
void Backwardd()
{
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW );
  digitalWrite(motor2Pin2, HIGH);
   analogWrite(ENA,speed);
   analogWrite(ENB,speed);
}
void Leftt()
{
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW );
  digitalWrite(motor2Pin2, LOW);
   analogWrite(ENA,speed);
   analogWrite(ENB,speed);
}
void Rightt()
{
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH );
  digitalWrite(motor2Pin2, LOW);
   analogWrite(ENA,speed);
   analogWrite(ENB,speed);
}
void Stopp()
{
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW );
  digitalWrite(motor2Pin2, LOW);
}
