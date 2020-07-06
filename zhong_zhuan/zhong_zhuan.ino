#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
//========================================================================
struct ML {
  char ml[200] = {'\0'};
  struct ML *p = NULL;
};
struct XS {
  char s[40];   //显示内容
  int  t;       //显示时间
  int  i;       //显示次数
  XS*  p;
};
//========================================================================
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
SoftwareSerial esp(6, 7); //定义虚拟串口名为serial,rx为6号端口,tx为7号端口
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char a;
struct ML *p1 = new struct ML, *p2 = p1;
struct XS *xs = new struct XS;
const char fg = ',';                     //分隔符
char hc[200] = {0};
int  hc_i    = 0;
//========================================================================
void tssc(const char *p) {        //调试输出-将字符串常量p输出到LCD屏下行
  lcd.setCursor(0, 1);            //将光标移至初始位置
  lcd.print(p);                   //显示初始内容
}
//========================================================================
int jk(const char *p1, const char *p2) {        //监控 监测Wifi串口 与 字符串常量 p1,p2对比 缓冲区大小为100
  int i, s_t;
  char s[100] = "";
  s_t = 0;
  //while(esp.available()) esp.read();          //清空Wifi串口内容
  while (1) {
    if (esp.available()) {
      s[s_t] = esp.read();
      s_t++;
      if (s_t > 100) return  -1;
      for (i = 0; i < s_t; i++) if (mlcl(s + i, p1 + 1)) return 1;
      for (i = 0; i < s_t; i++) if (mlcl(s + i, p2 + 1)) return 2;
    }
  }
}
//========================================================================
void cwtz() {         //错误处理
XS* jiaru(char *s, int t, int i);
void shanchu(XS* p);
}
//========================================================================
void esp8266_csh() {        //ESP-01初始化             -------------          未完成
  esp.print("AT+CWMODE=1\r\n");  // OK
  tssc("esp:1");
  if (jk("OK", "OK") == -1) cwtz();
  esp.print("AT+CWJAP=\"Yxg-164\",\"1641420800\"\r\n");  // OK  or  ERROR
  tssc("esp:2");
  if (jk("OK", "IP") == -1) cwtz();
  esp.print("AT+CIPMUX=0\r\n");  // OK  or  Link is builded
  tssc("esp:3");
  if (jk("OK", "Link is builded") != 1) cwtz();
  esp.print("AT+CIPMODE=1\r\n");  // OK  or  Link is builded
  tssc("esp:4");
  if (jk("OK", "Link is builded") != 1) cwtz();
  esp.print("AT+CIPSTART=\"TCP\",\"192.168.1.100\",16414\r\n");    // OK  or  ERROR
  tssc("esp:5");
  if (jk("OK", "ERROR") != 1) cwtz();
  esp.print("AT+CIPSEND\r\n");
  tssc("esp:6");
  delay(20);
  esp.print("hi=159168974976707559569020789401076400678587331858799877810869,che,1\r\n");    // OK  or  ERROR
  tssc("esp:7");
  if (jk("ok", "ok") == -1) cwtz();
  tssc("GO     ");
}
//========================================================================
int cin(char* p, int x) //字符串转整数-p中第x个数字,数字间用","(分隔符)隔开
{
  int s = 0, fh = 1, i;
  for (i = 0; i < 200; i++) {
    if (!x) break;
    if (p[i] == fg) x--;
  }
  if (i == 199) return -1;
  p += i;
  if (*p == '-') {
    fh = -1;
    p++;
  }
  while (*p >= '0' && *p <= '9') {
    s *= 10;
    s += *p - '0';
    p++;
  }
  return s * fh;
}
//========================================================================
int mlcl(char* s, char* ml)           //命令处理-判断s的开头是否为ml 是-将s中ml删掉
{
  char* p1 = s, * p2 = ml;
  while (*p1 == *p2) {
    p1++, p2++;
    if (*p1 == '\0') break;
  }
  if (*p2 != '\0') return 0;
  else if (*p1 == '\0') return 1;
  else for (int i = 0; p1[i - 1] != '\0'; i++) s[i] = p1[i];
  return 2;
}
//========================================================================
void setup()
{
  xs->p = xs;
  esp.begin(9600);                       //初始化虚拟串口
  Serial.begin(9600);                    //初始化Arduino默认串口
  lcd.begin(16, 02);                     //初始化lcd
  lcd.clear();                           //清空显示的内容
  lcd.setCursor(13, 0);                  //将光标移至初始位置
  lcd.print("Yxg");                      //显示初始内容
}
//========================================================================
void loop()
{
  int i;
  ML *p = NULL;
  while (Serial.available()) {
    hc[hc_i] = Serial.read();
    Serial.print(hc[hc_i]);
    if (hc[hc_i] == '\n' && hc_i != 0) {
      p2->p = new ML;
      for (i = 0; i < hc_i; i++) p2->ml[i] = hc[i];
      p2 = p2->p;
      hc_i = -1;
    }
    hc_i++;
  }
  while (p1->p) {
    Serial.println(p1->ml);

    p = p1;
    p1 = p1->p;
    delete p;
  }
  if (esp.available()) //虚拟串口的用法和默认串口的用法基本一样
  {
    a = esp.read();
    Serial.print(a);
  }

}
//========================================================================
XS* jiaru(char *s, int t, int i) {
  XS *p = new XS;
  p->p = xs->p;
  xs->p = p;
  for (int i = 0; i < 40; i++) p->s[i] = s[i];
  p->t = t;
  p->i = i;
  return p;
}
//========================================================================
void shanchu(XS* p) {
  XS *ls = p;
  while(ls->p != p) ls = ls->p;
  ls->p = p->p;
  sx = p->p;
  delete p;
}
