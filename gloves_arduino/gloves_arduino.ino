#include <LiquidCrystal.h>
#include <Wire.h>
#include <MPU6050.h>

#define thumb 6
#define index 7
#define middle 8
#define ring 9
#define pinky 10

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char ch_map[3][3][2][2][3] = {
                            '\0', ' ', '^', '\0', 'D', '3', '\0', 'C', '2', '\0', 'G', '6', // t = 0, i = 0
                            '\0', 'B', '1', '\0', 'J', '9', '\0', 'F', '5', '\0', 'L', '\0', // t = 0 ,i = 1
                            '\0', 'R', '\0', '\0', 'U', '\0', '\0', 'T', '\0', '\0', '\0', '\0', // t = 0, i = 2
                            '\0', 'A', '0', '\0', 'I', '8', '\0', 'H', '7', '\0', '\0', '\0', // t = 1, i = 0
                            '\0', 'E', '4', '\0', '\0', '\0', '\0', 'K', '\0', '\0', 'M', '\0', // t = 1, i = 1
                            '\0', 'S', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', // t = 1, i = 2
                            '\0', 'N', '<', '\0', 'Q', '\0', '\0', 'P', '\0', '\0', '\0', '\0', // t = 2, i = 0
                            '\0', 'O', '\0', '\0', 'W', '\0', '\0', 'V', '\0', '\0', 'X', '\0', // t = 2, i = 1
                            '\0', 'Y', '\0', '\0', 'Z', '\0', '\0', '\0', '\0', '\0', '\0', '\0'  // t = 2, i = 2
                          };

char mouse_click[2][2][2][2] = {
                        '\0', '\0', '\0', 'D', // t = 0, i = 0
                        '\0', 'R', '\0', '\0', // t = 0, i = 1
                        '\0', 'L', '\0', '\0', // t = 1, i = 0
                        '\0', '\0', '\0', '\0' //  t = 1, i = 1
                      };

bool thumb_press = false;
bool index_press = false;
bool middle_press = false;
bool ring_press = false;
bool pinky_press = false;

int thumb_count = 0;
int index_count = 0;
int middle_count = 0;
int ring_count = 0;
int pinky_count = 0;
long time = 0;

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
int vx, vy;
bool rotate = false;

void Reset(int x, int y, int z, int w, int b);
void desp(int x, int y, int w, int b, int z);
void clk(int x, int y, int w, int z);

void setup(){
  pinMode(thumb, INPUT_PULLUP);
  pinMode(index, INPUT_PULLUP);
  pinMode(middle, INPUT_PULLUP);
  pinMode(ring, INPUT_PULLUP);
  pinMode(pinky, INPUT_PULLUP);
  Serial.begin(9600);
  lcd.begin(16, 2);
  Wire.begin();
  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
  }
  mpu.CalibrateAccel();
  mpu.CalibrateGyro();
  Serial.println("Done cal");
}

void loop(){
  int x = digitalRead(thumb); // thumb
  int y = digitalRead(index); // index
  int w = digitalRead(middle); // middle
  int b = digitalRead(ring); // ring
  int z = digitalRead(pinky); // pinky
  
  while(!rotate){
    x = digitalRead(thumb); // thumb
    y = digitalRead(index); // index
    w = digitalRead(middle); // middle

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    vx = -(gx+260)/150;
    vy = (gz+100)/150;
    
    Serial.print("AccX:");
    Serial.print(vx+1);
    Serial.print(", AccY:");
    Serial.print(vy);

    if (x == 0 && !thumb_press){
      thumb_count++;
      thumb_press = true;

      Serial.print(", clk:");
      clk(thumb_count, index_count, middle_count, 1);
    }else{
      thumb_press = false;
    }
    
    if(y == 0 && !index_press){
      index_count++;
      index_press = true;
      
      Serial.print(", clk:");
      clk(thumb_count, index_count, middle_count, 1);
    }else{
        index_press = false;
    }

    if(w == 0 && !middle_press){
      middle_count++;
      middle_press = true;
      
      Serial.print(", clk:");
      clk(thumb_count, index_count, middle_count, 1);
      pinky_count = 1;
    }else{
        middle_press = false;
    }

    Serial.print(", ");
    Serial.print(thumb_count);
    Serial.print(index_count);
    Serial.println(middle_count);

    if(gy > 30000){
      rotate = true;
      Reset(thumb_count, index_count, middle_count, ring_count, pinky_count);
      break;
    }
    delay(200);
  }
  while(rotate){
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    lcd.blink();
    
    x = digitalRead(thumb); // thumb
    y = digitalRead(index); // index
    w = digitalRead(middle); // middle
    b = digitalRead(ring); // ring
    z = digitalRead(pinky); // pinky

    
    if (x == 0 && !thumb_press){
      if(pinky_count > 0){
          desp(thumb_count, index_count, middle_count, ring_count, pinky_count);
      }
      thumb_count++;
      thumb_press = true;
    }else{
      thumb_press = false;
    }
    
    if(y == 0 && !index_press){
      if(pinky_count > 0){
          desp(thumb_count, index_count, middle_count, ring_count, pinky_count);
      }
        index_count++;
        index_press = true;
    }else{
        index_press = false;
    }
    
    if(w == 0 && !middle_press){
      if(pinky_count > 0){
          desp(thumb_count, index_count, middle_count, ring_count, pinky_count);
      }
        middle_count++;
        middle_press = true;
    }else{
        middle_press = false;
    }
    
    if(b == 0 && !ring_press){
      if(pinky_count > 0){
          desp(thumb_count, index_count, middle_count, ring_count, pinky_count);
      }
        ring_count++;
        ring_press = true;
    }else{
        ring_press = false;
    }
    
    if(z == 0 && !pinky_press){
        pinky_count++;
        pinky_press = true;
        time = millis();
    }else{
        pinky_press = false;
    }
    
    Serial.print(thumb_count);
    Serial.print(index_count);
    Serial.print(middle_count);
    Serial.print(ring_count);
    Serial.print(pinky_count);
    Serial.print("\n");

    
    if(pinky_count > 0){
      if(millis() - time >= 5000){
          //Serial.print(pinky_count);
          desp(thumb_count, index_count, middle_count, ring_count, pinky_count);
      }
    }
    if(gy < -15000){
      rotate = false;
      Reset(thumb_count, index_count, middle_count, ring_count, pinky_count);
      break;
    }
    delay(1000);
  }
}

void Reset(int x, int y, int z, int w, int b){
  thumb_count = 0;
  index_count = 0;
  middle_count = 0;
  ring_count = 0;
  pinky_count = 0;
}

void desp(int x, int y, int w, int b, int z){
   char letter = ch_map[x][y][w][b][z];
      if(letter != '\0'){
        Serial.print(letter);
        lcd.print(letter);
      }
        // Reset counts
        Reset(thumb_count, index_count, middle_count, ring_count, pinky_count);
}

void clk(int x, int y, int w, int z){
   char mouse = mouse_click[x][y][w][z];
      if(mouse != '\0'){
        Serial.print(mouse);
        lcd.print(mouse);
      }
        // Reset counts
        Reset(thumb_count, index_count, middle_count, ring_count, pinky_count);
}


