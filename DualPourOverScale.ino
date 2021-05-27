#include "stdlib.h"
#include <Arduino.h>
//按钮初始化
#include <AceButton.h>
using namespace ace_button;
//HX711模数转换初始化
#include <HX711_ADC.h>
int weighingMode = 0;       //称重模式两态
char coffee[10];            //咖啡重量


float GRIND_COFFEE_WEIGHT = 0.0;

const int BUTTON1_PIN = 2;
const int BUTTON2_PIN = 3;

const int HX711_sda_upper =  A2;
const int HX711_scl_upper =  A3;
const int HX711_sda_lower =  A0;
const int HX711_scl_lower =  A1;

#include <StopWatch.h>
StopWatch stopWatch;
const char minsec[] = "00:00";
char *sec2minsec(int n) {
  int minute = 0;
  int second = 0;
  if (n < 99 * 60 + 60) {
    if (n >= 60) {
      minute = n / 60;
      n = n % 60;
    }
    second = n;
  } else {
    minute = 99;
    second = 59;
  }
  snprintf(minsec, 6, "%02d:%02d", minute, second);
  //  Serial.print("Serial.println :");
  //  Serial.print(minsec);
  return minsec;
}

//电子秤校准参数
//const float calibrationValue = 1118.68; //白色39元称参数
//const float calibrationValue = 1028.52; //黑色珠宝称参数
//const float calibrationValue = 472.01;  //实验亚克力称参数
const float calibrationValueLower = 1043.60; //黑色珠宝称参数
const float calibrationValueUpper = 954.60; //上称参数

//电子秤外壳对屏幕的遮盖像素补偿
//5 6 0 0 顶底左右 黑色珠宝称参数
int Margin_Top = 3;
int Margin_Bottom = 3;
int Margin_Left = 5;
int Margin_Right = 5;

//设置字体 https://github.com/olikraus/u8g2/wiki/fntlistall
#define FONT_S u8g2_font_trixel_square_tf
#define FONT_M u8g2_font_7x14B_tf
//#define FONT_L u8g2_font_logisoso24_tn
int FONT_M_HEIGHT;
int FONT_S_HEIGHT;
int FONT_L_HEIGHT;

//电子秤参数和计时点
char wu[10];
char wl[10];
char r[10];
char wuwl[10];
unsigned long t0 = 0;               //开始萃取打点
unsigned long t1 = 0;               //下液第一滴打点
unsigned long t2 = 0;               //下液结束打点
unsigned long t_ReadyToBrew = 0;    //准备好冲煮的时刻
float w0 = 0.0; //咖啡粉重（g）
float w1 = 0.0;                 //下液重量（g）
float r0 = 0.0;   //粉水比 w1/w0
int tareCounter = 0; //不稳计数器
float aWeightUpper = 0;          //稳定状态比对值（g）
float aWeightDiffUpper = 0.05;    //稳定停止波动值（g）
float atWeightUpper = 0;         //自动归零比对值（g）
float atWeightDiffUpper = 0.3;   //自动归零波动值（g）
float asWeightUpper = 0;         //下液停止比对值（g）
float asWeightDiffUpper = 0.1;   //下液停止波动值（g）
float rawWeightUpper = 0.0;      //原始读出值（g）
float aWeightLower = 0;          //稳定状态比对值（g）
float aWeightDiffLower = 0.05;    //稳定停止波动值（g）
float atWeightLower = 0;         //自动归零比对值（g）
float atWeightDiffLower = 0.3;   //自动归零波动值（g）
float asWeightLower = 0;         //下液停止比对值（g）
float asWeightDiffLower = 0.1;   //下液停止波动值（g）
float rawWeightLower = 0.0;      //原始读出值（g）
unsigned long autoTareMarker = 0;       //自动归零打点
unsigned long autoStopMarker = 0;       //下液停止打点
unsigned long scaleStableMarker = 0;    //稳定状态打点
unsigned long timeOutMarker = 0;        //超时打点
unsigned long t = 0;                  //最后一次重量输出打点
unsigned long oledRefreshMarker = 0;   //最后一次oled刷新打点
const int autoTareInterval = 500;       //自动归零检测间隔（毫秒）
const int autoStopInterval = 400;       //下液停止检测间隔（毫秒）
const int scaleStableInterval = 100;   //稳定状态监测间隔（毫秒）
const int timeOutInterval = 10000;      //超时检测间隔（毫秒）
int oledPrintInterval = 0;     //oled刷新间隔（毫秒）
const int serialPrintInterval = 0;  //称重输出间隔（毫秒）
//固定参数
float fix_upper = 0.0;
float fix_lower = 0.0;
float fix_total = 0.0;
float fix_ratio = 0.0;
char *fix_time;
bool readyToBrew = false;



HX711_ADC scaleUpper(HX711_sda_upper, HX711_scl_upper);
HX711_ADC scaleLower(HX711_sda_lower, HX711_scl_lower);




#ifdef ESP32
// Different ESP32 boards use different pins
const int LED_PIN = 2;
#else
const int LED_PIN = LED_BUILTIN;
#endif

// LED states. Some microcontrollers wire their built-in LED the reverse.
const int LED_ON = HIGH;
const int LED_OFF = LOW;

ButtonConfig config1;
ButtonConfig config2;
ButtonConfig config3;
ButtonConfig config4;
AceButton button1(&config1);
AceButton button2(&config2);
AceButton button3(&config3);
AceButton button4(&config4);
// Forward reference to prevent Arduino compiler becoming confused.
//void handleEvent1(AceButton*, uint8_t, uint8_t);
//void handleEvent2(AceButton*, uint8_t, uint8_t);
//void handleEvent3(AceButton*, uint8_t, uint8_t);
//void handleEvent4(AceButton*, uint8_t, uint8_t);

//显示屏初始化 https://github.com/olikraus/u8g2/wiki/u8g2reference
#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* reset=*/ U8X8_PIN_NONE);


//文本对齐 AC居中 AR右对齐 AL左对齐 T为要显示的文本
#define LCDWidth  u8g2.getDisplayWidth()
#define LCDHeight u8g2.getDisplayHeight()
#define AC(T)     ((LCDWidth - (u8g2.getStrWidth(T))) / 2 - Margin_Left - Margin_Right)
#define AR(T)     (LCDWidth -  u8g2.getStrWidth(T) - Margin_Right)
#define AL(T)     (u8g2.getStrWidth(T) + Margin_Left)

//自定义trim消除空格
char *ltrim(char *s) {
  while (isspace(*s)) s++;
  return s;
}

char *rtrim(char *s) {
  char* back = s + strlen(s);
  while (isspace(*--back));
  *(back + 1) = '\0';
  return s;
}

char *trim(char *s) {
  return rtrim(ltrim(s));
}

void button_init() {
  pinMode(LED_PIN, OUTPUT);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  //  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  //  pinMode(BUTTON4_PIN, INPUT_PULLUP);

  button1.init(BUTTON1_PIN);
  button2.init(BUTTON2_PIN);
  //  button3.init(BUTTON3_PIN);
  //  button4.init(BUTTON4_PIN);

  config1.setEventHandler(handleEvent1);
  config1.setFeature(ButtonConfig::kFeatureClick);

  config2.setEventHandler(handleEvent2);
  config2.setFeature(ButtonConfig::kFeatureClick);

  //  config3.setEventHandler(handleEvent3);
  //  config3.setFeature(ButtonConfig::kFeatureClick);
  //
  //  config4.setEventHandler(handleEvent4);
  //  config4.setFeature(ButtonConfig::kFeatureClick);
}

void handleEvent1(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
      digitalWrite(LED_PIN, LED_ON);
      Serial.println(F("按钮1"));
      b1();
      break;
  }
}

void handleEvent2(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
      digitalWrite(LED_PIN, LED_ON);
      Serial.println(F("按钮2"));
      b2();
      break;
  }
}


void b1() {
  if (stopWatch.isRunning() == false) {
    //不在计时中 也就是说没有冲煮 因此在粉量大于0时 可以设定粉量 随后让冲煮准备指示为真
    if (rawWeightUpper > 0)
    {
      GRIND_COFFEE_WEIGHT = rawWeightUpper;
      scaleUpper.tareNoDelay();
      readyToBrew = true;
      t_ReadyToBrew = millis();
    }
  }
  else {
    //在计时中 按了没反应
  }
}

void b2() {
  if (stopWatch.isRunning() == false) {
    //不在计时中 也就是说没有冲煮 因此可以重量归零 时间归零
    scaleUpper.tareNoDelay();
    scaleLower.tareNoDelay();
    GRIND_COFFEE_WEIGHT = 0;
    stopWatch.reset();
  }
  else {
    //在计时中 按一下则结束计时 停止冲煮 （固定参数回头再说）
    stopWatch.stop();
  }
}

void checkBrew() {
  //下液超过1g自动开始计时 && 准备冲煮的标志为真 则开始计时
  //因为已经开冲了 所以准备标志应当复位为假 这样秒表开始也只按一次

  if (rawWeightUpper > 1 && readyToBrew == true && scaleUpper.getTareStatus() == true && (millis() - t_ReadyToBrew > 3000)) {
    stopWatch.start();
    readyToBrew = false;
  }
}

void setup() {
  delay(1000); //有些单片机会重启两次
  //Wire.setClock(4000000);//高速iic模式 加速oled数据发送
  Serial.begin(115200);
  while (!Serial); //等待串口就绪
  Serial.println(F("Sofronio Dual Pour Over Scale"));

  button_init();
  u8g2.begin();
  u8g2.setFont(FONT_M);
  FONT_M_HEIGHT = u8g2.getMaxCharHeight();
  u8g2.setFont(FONT_S);
  FONT_S_HEIGHT = u8g2.getMaxCharHeight();


  unsigned long stabilizingtime = 500;  //去皮时间(毫秒)，增加可以提高去皮精确度
  boolean _tare = true;                  //电子秤初始化去皮，如果不想去皮则设为false

  scaleUpper.begin();
  scaleUpper.start(stabilizingtime, _tare);
  scaleUpper.setCalFactor(calibrationValueUpper);  //设定校准值
  scaleUpper.setSamplesInUse(4);               //设定采样窗口长度
  Serial.print(F("scaleUpper calibrated, sps="));
  Serial.println(scaleUpper.getSPS());

  scaleLower.begin();
  scaleLower.start(stabilizingtime, _tare);
  scaleLower.setCalFactor(calibrationValueLower);  //设定校准值
  scaleLower.setSamplesInUse(4);               //设定采样窗口长度
  Serial.print(F("scaleLower calibrated, sps="));
  Serial.println(scaleLower.getSPS());

  button1.check();
  button2.check();
  button3.check();
  button4.check();

  stopWatch.setResolution(StopWatch::SECONDS);
  //给stopWatch.elapsed()赋值
  stopWatch.start();
  stopWatch.reset();

  Serial.println(F("setup done"));
}

void pourOverScale() {
  checkBrew();
  static boolean newDataReadyUpper = 0;
  static boolean scaleStableUpper = 0;

  static boolean newDataReadyLower = 0;
  static boolean scaleStableLower = 0;

  if (scaleUpper.update()) newDataReadyUpper = true;
  if (scaleLower.update()) newDataReadyLower = true;

  if (newDataReadyUpper) {
    rawWeightUpper = scaleUpper.getData();
    newDataReadyUpper = 0;
    Serial.print("上称：");
    if (rawWeightUpper >= -0.15 && rawWeightUpper <= 0)
      rawWeightUpper = 0.0;
    dtostrf(rawWeightUpper, 7, 1, wu);
    Serial.print(trim(wu));
  }

  if (newDataReadyLower) {
    rawWeightLower = scaleLower.getData();
    newDataReadyLower = 0;
    Serial.print("  下称：");
    if (rawWeightLower >= -0.15 && rawWeightLower <= 0)
      rawWeightLower = 0.0;
    dtostrf(rawWeightLower, 7, 1, wl);
    Serial.print(trim(wl));
  }
  dtostrf(rawWeightUpper + rawWeightLower, 7, 1, wuwl);
  //float c = 16.0;

  float ratio_temp = (rawWeightUpper + rawWeightLower) / GRIND_COFFEE_WEIGHT;
  if (ratio_temp < 0)
    ratio_temp = 0.0;
  if (GRIND_COFFEE_WEIGHT < 0.1)
    ratio_temp = 0.0;
  dtostrf(ratio_temp, 7, 1, r);

  Serial.println("");
}

void loop() {
  // Allow wake up pin to trigger interrupt on low.
  button1.check();
  button2.check();

  pourOverScale();

  if (millis() > oledRefreshMarker + oledPrintInterval)
  {
    //达到设定的oled刷新频率后进行刷新
    oledRefreshMarker = millis();
    int x = 0;
    int y = 0;
    char ratio[30];
    sprintf(ratio, "1:%s", trim(r));

    char coffeepowder[30];
    //sprintf(coffeepowder, "%s", GRIND_COFFEE_WEIGHT);
    dtostrf(GRIND_COFFEE_WEIGHT, 7, 1, coffeepowder);

    u8g2.firstPage();
    do {
      u8g2.setFont(FONT_M);
      //      x = Margin_Left;
      //      y =  FONT_S_HEIGHT + FONT_M_HEIGHT + Margin_Top;
      //      u8g2.drawStr(x, y, trim(wu));
      //      u8g2.drawStr(AR(trim(wuwl)), y, trim(wuwl));
      //
      //      y = LCDHeight - FONT_S_HEIGHT - Margin_Bottom;
      //      u8g2.drawStr(x, y, trim(wl));
      //      u8g2.drawStr(AR(trim(ratio)), y, trim(ratio));
      //
      //      u8g2.setFont(FONT_S);
      //      y = FONT_S_HEIGHT + Margin_Top;
      //      u8g2.drawStr(x, y, "UPPER");
      //      u8g2.drawStr(AR("TOTAL"), y, "TOTAL");
      //
      //      u8g2.setFont(FONT_S);
      //      y = LCDHeight - Margin_Bottom;
      //      u8g2.drawStr(x, y, "LOWER");
      //      u8g2.drawStr(AR("RATIO"), y, "RATIO");
      int LINE_INTERVAL = 4;
      int WORD_INTERVAL = 5;
      x = Margin_Left;
      u8g2.setFont(FONT_S);
      y = FONT_M_HEIGHT + Margin_Top;
      u8g2.drawStr(x, y, "UPPER");

      y = FONT_M_HEIGHT * 2 + Margin_Top + LINE_INTERVAL;
      u8g2.drawStr(x, y, "LOWER");

      y = FONT_M_HEIGHT * 3 + Margin_Top + LINE_INTERVAL * 2;
      u8g2.drawStr(x, y, "TOTAL");

      x = AL("UPPER") + WORD_INTERVAL;
      u8g2.setFont(FONT_M);
      y = FONT_M_HEIGHT + Margin_Top;
      u8g2.drawStr(x, y, trim(wu));

      y = FONT_M_HEIGHT * 2 + Margin_Top + LINE_INTERVAL;
      u8g2.drawStr(x, y, trim(wl));

      y = FONT_M_HEIGHT * 3 + Margin_Top + LINE_INTERVAL * 2;
      u8g2.drawStr(x, y, trim(wuwl));


      u8g2.setFont(FONT_M);
      y = FONT_M_HEIGHT + Margin_Top;
      u8g2.drawStr(AR(trim(coffeepowder)), y, trim(coffeepowder));
      y = FONT_M_HEIGHT * 2 + Margin_Top + LINE_INTERVAL;
      u8g2.drawStr(AR(sec2minsec(stopWatch.elapsed())), y, sec2minsec(stopWatch.elapsed()));
      y = FONT_M_HEIGHT * 3 + Margin_Top + LINE_INTERVAL * 2;
      u8g2.drawStr(AR(trim(ratio)), y, trim(ratio));

    } while ( u8g2.nextPage() );
  }
}
