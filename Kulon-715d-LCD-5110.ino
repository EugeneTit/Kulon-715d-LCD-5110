/*********************************************************************
Kulon-715d-LCD-5110

https://github.com/EugeneTit/Kulon-715d-LCD-5110

проект по замене LCD экрана зарядного устройства Кулон 715D на модуль 5110 + arduino

с управлением светом и бегущей строкой + допиливаю таймер



*********************************************************************/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "ACS712.h"


ACS712 sensor(ACS712_20A, A2);  // марка сенсора тока и его пин подключения


//     ПОДКЛЮЧЕНИЕ ДИСПЛЕЯ    Software SPI (slower updates, more flexible pin options):

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
//#define lightpin 9
/*
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000001, B00000000,
  B00000101, B01000000,
  B00000011, B10000000,
  B00100001, B00001000,
  B11000001, B00000110,
  B00110001, B00011000,
  B01001101, B01100100,
  B00000011, B10000000,
  B00000011, B10000000,
  B01001101, B01100100,
  B00110001, B00011000,
  B11000001, B00000110,
  B00100001, B00001000,
  B00000011, B10000000,
  B00000101, B01000000,
  B00000001, B00000000 };
*/


int analogVInput = 0;   // пин напряжения
int lightpin = 9;   // пин подсветки дисплея
float vout = 0.0;   //промежуточное напряжения для расчета      
float vin = 0.0;   //готовое значение напряжения для вывада
float R1 = 100000.0; // resistance of R1 (100K) -see text!  резисторы делителя
float R2 = 10000.0; // resistance of R2 (10K) - see text!
int vvalue = 0;   //  значение напряжения считанное
String text;    // текст бегущй строки
int textWidth;
float count = 1.0;
boolean toggle = false;
unsigned long prevparametrs = 0;        // время прошлого измерения и индикации


// constants won't change :
const long interval = 600;           // интервал измерений и индикации параметров (milliseconds)



void setup()   {
  Serial.begin(9600);
  
  pinMode(analogVInput, INPUT);
  pinMode(lightpin, OUTPUT);
  pinMode(9, OUTPUT);

  char DiodRight[] = "-\x91-";
  char DiodLeft[] = "-\x92-";
  char Capacitor[] = "-\x93-";
  char Resistor[] = "-\x94\x95
  char Omega[] = "\x90";
  char Micro[] = "\xB5";



  digitalWrite(lightpin, LOW);  //включить подсветку
    display.setTextWrap(false); // this is very important!  для бегущей строки
  display.begin();
 // display.cp437(true);
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);

 // display.display(); // show splashscreen
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer



  display.setCursor(0,0);
  display.setTextSize(1);
  display.println(utf8rus( "   ВНИМАНИЕ!!"));
   display.println(utf8rus("    ОТКЛЮЧИ "));
   display.println(utf8rus("   НАГРУЗКУ"));
   
   display.println();
   display.println(utf8rus("  калибровка"));
   display.println(utf8rus("   датчика"));
//display.setTextSize(2);
 // display.println(utf8rus("Внимание!"));
 // display.setTextSize(1);
 // display.println(utf8rus("калибровка датчик!"));
  display.display();
  delay(2000);
  sensor.calibrate(); // калибровка сенсора
}
  
void loop() {






///     вывод бегущей строки внизу ***********
//text = utf8rus("Время работы прибора составило приблизительно");
unsigned long mint =  millis()/60000;
unsigned long hor = mint/60;
text = utf8rus("Время работы прибора составило приблизительно ");
text = text  +  hor;
text =  text + utf8rus( " часов, ");
text = text  +  mint;
text =  text + utf8rus( " минут");

 textWidth = text.length()*6+85; // get the width of the text
 for(int i=0; i<textWidth; i+=1) {
   // display.clearDisplay();
   display.setContrast(55);
 // display.setTextSize(1);
    display.setTextSize(2);
    display.setCursor(0, 0);
   // display.print("Test..."); // just a title
   //display.drawLine(0, 15, display.width(), 15, BLACK);
   count += 0.2;

    // routine to draw the actual wobbly text character by character, each with a sine wave.
    for(int j=0; j<text.length(); j++) {
      display.drawChar((j*6)+85-i, 40, text[j], BLACK, WHITE, 1);
    }
display.display();
    delay(50);





 unsigned long currentMillis = millis(); // чтение текущего времени
   if (currentMillis - prevparametrs >= interval) {    // условие проверка интервала индикации и иизмерения
    prevparametrs = currentMillis;  // отметка времени
   
   digitalWrite(lightpin, LOW);  //включить подсветку
   digitalWrite(9, LOW);  //включить подсветку
   
// чтение напряги
// read the value at analog input
vvalue = analogRead(analogVInput);
vout = (vvalue * 4.70) / 1024.0; // see text/ здесь нужно измеренное значение VCC  самой дуины поставить...
vin = vout / (R2/(R1+R2));
if (vin<0.09) {
vin=0.0;//statement to quash undesired reading !

}


//  пошла сила тока 
  
  float I = sensor.getCurrentDC();   //чтение тока
 // if (I<0.07) {I=0;}  // успокоение нуля от случайных значений
 //if (I<0.07) {I=0.0;}  //statement to quash undesired reading !
  display.clearDisplay();
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.setTextSize(1);
  display.println(utf8rus("Напр-е и Ток"));
  display.setTextSize(2);
  display.print(vin);
  //display.println(utf8rus(" V")); 
  display.println(); 
    display.setTextSize(2);
 // display.println(utf8rus("11 A"));
//  Serial.println(String("I = ") + I + " A");

display.print(abs(I));
display.println();

  display.setCursor(61,8);
  display.setTextSize(2);
  display.println("V"); 
  display.setCursor(61,24);
  display.println("A");
display.display();
 
// конец индикации и измерений
}
}




    
 // }







 // delay(500);
}













/*преобразование русских букв************ Recode russian fonts from UTF-8 to Windows-1251 ****************************************/
String utf8rus(String source)
{
  int i,k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i]; i++;
          if (n == 0x81) { n = 0xA8; break; }
          if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
          break;
        }
        case 0xD1: {
          n = source[i]; i++;
          if (n == 0x91) { n = 0xB8; break; }
          if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
return target;
}


