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
struct SJ {
  int shi;
  int fen;
  int miao;
};
//========================================================================
const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
SoftwareSerial esp(6, 7); //定义虚拟串口名为serial,rx为6号端口,tx为7号端口
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char a;
struct ML *p1 = new struct ML, *p2 = p1;
struct XS *xs = new struct XS;
SJ  sj = {0, 0, 0}, lssj = {0, 0, 0};
unsigned int t = 0;
const char fg = ',';                     //分隔符
char hc[200] = {0};
int  hc_i    = 0;
char ml[200] = {0};
int  ml_i    = 0;
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
  void shanchu(XS * p);
  int sjjg(SJ p);
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
}//========================================================================
void cch(char* p, char* c, int x) //字符串转整数-p中第x个数字,数字间用","(分隔符)隔开
{
  int s = 0, fh = 1, i;
  for (i = 0; i < 200; i++) {
    if (!x) break;
    if (p[i] == fg) x--;
  }
  if (i == 199) {
    *c = '\0';
    return;
  }
  p += i;
  while (*p != fg && *p != '\r') {
    *c = *p;
    c++;
    p++;
  }
  *c = '\0';
}
//========================================================================
int mlcl(char* s, const char* ml)          //命令处理-判断s的开头是否为ml 是-将s中ml删掉
{
  const char* p1 = s, * p2 = ml;
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
  char s[40] = "chushihua";
  int i;
  xs->p = xs;
  esp.begin(9600);                       //初始化虚拟串口
  Serial.begin(9600);                    //初始化Arduino默认串口
  lcd.begin(16, 02);                     //初始化lcd
  lcd.clear();                           //清空显示的内容
  lcd.setCursor(13, 0);                  //将光标移至初始位置
  lcd.print("Yxg");                      //显示初始内容
  for (i = 0; i < 40; i++) xs -> s[i] = s[i];
  xs -> i = -1;
  xs -> t = 5;
}
//========================================================================
void loop()
{
  int i;
  char s[40];
  ML *p = NULL;
  XS *ls = NULL;
  unsigned int t1 = millis();
  while (t1 - t >= 1000) {
    t += 1000;
    sj.miao++;
    if (sj.miao == 60) {
      sj.miao = 0;
      sj.fen++;
    }
    if (sj.fen == 60) {
      sj.fen = 0;
      sj.shi++;
    }
  }
  if (t > t1) t = 0;
  if (sj.shi < 0) sj.shi = 0;
  digitalWrite(13, !(sj.miao % 10));
  if (xs->t <= sjjg(sj)) {
    ls = xs;
    xs = xs->p;
    if (ls->i == 0) shanchu(ls);
    if (xs->i != -1)xs->i--;
    lssj = sj;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(xs->s);
  }
  while (Serial.available()) {
    hc[hc_i] = Serial.read();
    if (hc[hc_i] == '\n' && hc_i != 0) {
      p2->p = new ML;
      for (i = 0; i < hc_i; i++) p2->ml[i] = hc[i];
      p2 = p2->p;
      hc_i = -1;
    }
    hc_i++;
  }
  while (esp.available()) {
    ml[ml_i] = esp.read();
    if (ml[ml_i] == '\n' && ml_i != 0) {
      p2->p = new ML;
      for (i = 0; i < ml_i; i++) p2->ml[i] = ml[i];
      p2 = p2->p;
      ml_i = -1;
    }
    ml_i++;
  }
  while (p1->p) {
    if (mlcl(p1->ml, "lcdjr:")) {
      cch(p1->ml, s, 2);
      jiaru(s, cin(p1->ml, 0), cin(p1->ml, 1));
    }

    p = p1;
    p1 = p1->p;
    delete p;
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
  while (ls->p != p) ls = ls->p;
  ls->p = p->p;
  xs = p->p;
  delete p;
}
//========================================================================
int sjjg(SJ p) {
  int shic = p.shi - lssj.shi;
  int fenc = p.fen - lssj.fen;
  int miaoc = p.miao - lssj.miao;
  return miaoc + 60 * (fenc + 60 * shic);
}
