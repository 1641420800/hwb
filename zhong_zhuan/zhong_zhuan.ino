#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
SoftwareSerial esp(6, 7); //定义虚拟串口名为serial,rx为6号端口,tx为7号端口
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
void setup()
{
  esp.begin(9600); //初始化虚拟串口
  Serial.begin(9600); //初始化Arduino默认串口
  lcd.begin(16, 02);                     //初始化lcd
  lcd.clear();                           //清空显示的内容
  lcd.setCursor(13, 0);                  //将光标移至初始位置
  lcd.print("Yxg");                      //显示初始内容
}
char a;
void loop()
{
  if (esp.available()) //虚拟串口的用法和默认串口的用法基本一样
  {
    a = esp.read();
    Serial.print(a);
  }
  /*
  esp.println("123,456");
  delay(1000);
  */
}
