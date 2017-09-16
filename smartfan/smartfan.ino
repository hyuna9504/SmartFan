
#define TEMP_ADDR 72
//LCD라이브러리 삽입 및 객체 인스턴스 생성
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

//온도기호 정의
byte degree[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000,
};
//팬 작동중 기호 정의
byte fan_on[8] = {
  B00100,
  B10101,
  B01110,
  B11111,
  B01110,
  B10101,
  B00100,
  B00000,
};
//팬 정지 기호 정의
byte fan_off[8] = {
  B00100,
  B00100,
  B00100,
  B11111,
  B00100,
  B00100,
  B00100,
  B00000,
};

//회로연결 핀 상수 정의
const int DOWN_BUTTON=9;
const int UP_BUTTON=10;
const int DOWN_H_BUTTON=12;
const int UP_H_BUTTON=13;
const int FAN=11;
#define A0 14   //아날로그 핀을 디지털로 사용
#define A1 15  //아날로그 핀을 디지털로 사용
const int RED=14;
const int GREEN=15;

//스위치 디바운싱에 사용할 변수
boolean lastDownTempButton=LOW;
boolean currentDownTempButton=LOW;
boolean lastUpTempButton=LOW;
boolean currentUpTempButton=LOW;

boolean lastDownHumButton=LOW;
boolean currentDownHumButton=LOW;
boolean lastUpHumButton=LOW;
boolean currentUpHumButton=LOW;

int set_temp=29;  //희망 온도값 설정
int set_Hum=48;    //희망 습도값 설정
int DHpin = 8;
byte dat [5];
byte read_data () {
  byte data;
  for (int i = 0; i < 8; i ++) {
    if (digitalRead (DHpin) == LOW) {
      while (digitalRead (DHpin) == LOW); // wait for 50us
      delayMicroseconds (30); // determine the duration of the high level to determine the data is '0 'or '1'
      if (digitalRead (DHpin) == HIGH)
        data |= (1 << (7-i)); // high front and low in the post
      while (digitalRead (DHpin) == HIGH); // data '1 ', wait for the next one receiver
     }
  }
return data;
}
 
void start_test () {
  digitalWrite (DHpin, LOW); // bus down, send start signal
  delay (30); // delay greater than 18ms, so DHT11 start signal can be detected
 
  digitalWrite (DHpin, HIGH);
  delayMicroseconds (40); // Wait for DHT11 response
 
  pinMode (DHpin, INPUT);
  while (digitalRead (DHpin) == HIGH);
  delayMicroseconds (80); // DHT11 response, pulled the bus 80us
  if (digitalRead (DHpin) == LOW);
  delayMicroseconds (80); // DHT11 80us after the bus pulled to start sending data

  for (int i = 0; i < 4; i ++) // receive temperature and humidity data, the parity bit is not considered
    dat[i] = read_data ();
 
  pinMode (DHpin, OUTPUT);
  digitalWrite (DHpin, HIGH); // send data once after releasing the bus, wait for the host to open the next Start signal
}


void setup()
{
  pinMode(FAN, OUTPUT);
  pinMode (DHpin, OUTPUT);
  pinMode(RED, OUTPUT);//아날로그 핀을 디지털로 사용
  pinMode(GREEN, OUTPUT);//아날로그 핀을 디지털로 사용
 

  
  
  //LCD를 16행2열로 지정
  lcd.begin(16, 2);
  //사용자 정의문자 등록
  lcd.createChar(0, degree);
  lcd.createChar(1, fan_off);
  lcd.createChar(2, fan_on);

  //LCD에 고정 정보 출력
  lcd.setCursor(0,0);
  lcd.print("Cur:");
  lcd.setCursor(6,0);
  lcd.write((byte)0);
  lcd.setCursor(7,0);
  lcd.print("C"); 
  lcd.setCursor(11,0);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Set:");
  lcd.setCursor(6,1);
  lcd.write((byte)0);
  lcd.setCursor(7,1);
  lcd.print("C");
  lcd.setCursor(11,1);
  lcd.print("%");
  lcd.setCursor(15,1);
  lcd.write(1);
}

 //버튼 두개에서 사용하는 스위치 디바운싱 함수
 boolean debounce(boolean last, int pin)
 {
    boolean current=digitalRead(pin);
    if(last!=current)
    {
      delay(5);
      current=digitalRead(pin);
    }
    return current;
 }

void loop()
{
   //온도 확인
  // Wire.beginTransmission(TEMP_ADDR);
  // Wire.write(0); //쓰기모드로 레지스터 0에 읽기 요청
 //  Wire.endTransmission(); //전송종료
 //  Wire.requestFrom(TEMP_ADDR,1); //1바이트 읽기
  // int c = Wire.read(); //섭시온도 읽어 옴
   start_test ();
   lcd.setCursor(4,0); //1열 5행으로 커서 이동
   lcd.print(dat [2], DEC); //섭씨온도 LCD에 기록
   lcd.setCursor(9,0); //1열 9행으로 커서 이동
   lcd.print(dat [0], DEC); //습도 LCD에 기록
   delay(1000);

   //버튼 눌림 상태를 디바운싱으로 감지
   currentDownTempButton = debounce(lastDownTempButton, DOWN_BUTTON);
   currentUpTempButton = debounce(lastUpTempButton, UP_BUTTON);

   //희망 온도 내림
   if(lastDownTempButton==LOW&&currentDownTempButton==HIGH)
   {
      set_temp--;
   }
   //희망 온도 올림
   else if(lastUpTempButton==LOW&&currentUpTempButton==HIGH)
   {
      set_temp++;
   }
   //희망 온도 출력
   lcd.setCursor(4,1);
   lcd.print(set_temp);
   //현재 버튼 상태 업데이트
   lastDownTempButton=currentDownTempButton;
   lastUpTempButton=currentUpTempButton;

  ////////////////////////////////////////////
  //버튼 눌림 상태를 디바운싱으로 감지
   currentDownHumButton = debounce(lastDownHumButton, DOWN_H_BUTTON);
   currentUpHumButton = debounce(lastUpHumButton, UP_H_BUTTON);

   //희망 습도 내림
   if(lastDownHumButton==LOW&&currentDownHumButton==HIGH)
   {
      set_Hum--;
   }
   //희망 습도 올림
   else if(lastUpHumButton==LOW&&currentUpHumButton==HIGH)
   {
      set_Hum++;
   }
   //희망 온도 출력
   lcd.setCursor(9,1);
   lcd.print(set_Hum);
   //현재 버튼 상태 업데이트
   lastDownHumButton=currentDownHumButton;
   lastUpHumButton=currentUpHumButton;

     //습도와 온도가 높으면
     if(dat[0]>=set_Hum || dat[2]>=set_temp)
     {
     //팬 회전시키고, RED LED 점등, LCD화면에 회전표시
     digitalWrite(RED, HIGH);
     digitalWrite(GREEN, LOW);
     digitalWrite(FAN, HIGH);
     lcd.setCursor(15,1);
     lcd.write(2);
     } 
     //습도가 낮으면
     else
     {
     digitalWrite(FAN, LOW);
     digitalWrite(RED, LOW);
     digitalWrite(GREEN, HIGH);
     lcd.setCursor(15,1);
     lcd.write(1);
    }
  
    
  }

