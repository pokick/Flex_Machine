#include <Servo.h>

#define PIN_ENDSTOP_1 PB3
#define PIN_ENDSTOP_2 PB4
#define PIN_ENDSTOP_3 PB5
#define PIN_ENDSTOP_4 PB6

#define PIN_MEASURE PB7
#define PIN_SERVO_1 PA7
#define PIN_SERVO_2 PA6
#define PIN_EN_SERVO PA5
#define PIN_STEP_1 PA4
#define PIN_DIR_1 PA3
#define PIN_EN_STEP PA2
#define PIN_STEP_2 PA1
#define PIN_DIR_2 PA0

const int STEPS_PER_MM = 250; // for full rotaton
// compensate Servo 1 deg
const int C_SERVO1 = -3;

// compensate Servo 2 deg
const int C_SERVO2 = 0;

// compensate size 
const int C_SIZE = -5;

static bool DIR_STEP_1 = 0;
static bool DIR_STEP_2 = 0;

bool homed = false;
static uint16_t stepper_position = 0;

uint32_t angle_array[90] = {
  0
};

// serwo 50HZ, czas impuslu od 800us do 2200us

Servo servo_1;
Servo servo_2;

void setup() {

  Serial.begin(115200);
  Serial.println("BOOTING ...");
  //define all pins 
  pinMode(PIN_EN_STEP, OUTPUT);
  digitalWrite(PIN_EN_STEP, HIGH);
  pinMode(PIN_EN_SERVO, OUTPUT);
  digitalWrite(PIN_EN_SERVO, LOW);

  servo_1.attach(PIN_SERVO_1);
  servo_2.attach(PIN_SERVO_2);
  servo_1.write(90 + C_SERVO1);
  servo_2.write(90 + C_SERVO2);

  pinMode(PIN_ENDSTOP_1, INPUT);
  pinMode(PIN_ENDSTOP_2, INPUT);
  pinMode(PIN_ENDSTOP_3, INPUT);
  pinMode(PIN_ENDSTOP_4, INPUT);

  pinMode(PIN_MEASURE, OUTPUT);
  digitalWrite(PIN_MEASURE, LOW);
  // pinMode(PIN_SERVO_1, OUTPUT);
  // digitalWrite(PIN_SERVO_1,LOW);
  //  pinMode(PIN_SERVO_2, OUTPUT);
  //digitalWrite(PIN_SERVO_2,LOW);
  pinMode(PIN_STEP_1, OUTPUT);
  digitalWrite(PIN_STEP_1, LOW);
  pinMode(PIN_DIR_1, OUTPUT);
  digitalWrite(PIN_DIR_1, LOW);
  pinMode(PIN_STEP_2, OUTPUT);
  digitalWrite(PIN_STEP_2, LOW);
  pinMode(PIN_DIR_2, OUTPUT);
  digitalWrite(PIN_DIR_2, LOW);

}

void loop() {

  Read_Serial();

}

void Read_Serial(void) {

  String data = "";
  if (Serial.available()) {
    data = Serial.readStringUntil('\n');
    Serial.println(data);
  }

  String dataArgs[11] = {
    ""
  };
  uint8_t argCount = 0;
  uint8_t data_arg_len = 0;
  for (int x = 0; x <= data.length(); x++) {
    if (data.charAt(x) == ' ' || x == data.length()) {
      dataArgs[argCount] = data.substring(data_arg_len, x);
      data_arg_len += dataArgs[argCount].length() + 1;
      argCount++;
    }
  }

  if (dataArgs[0] == "set_servo") {
    Serial.println("USTAWIAM SERWO NR " + dataArgs[1] + " NA POZYCJE " + dataArgs[2]);
    set_servo(dataArgs[1].toInt(), dataArgs[2].toInt());

  } else if (dataArgs[0] == "step") {
    Serial.println("RUSZAM SILNIKIEM NR " + dataArgs[1] + " W KIERUNKU  " + dataArgs[2] + " KROKOW :  " + dataArgs[3]);
    _step(dataArgs[1].toInt(), dataArgs[2].toInt(), dataArgs[3].toInt());

  } else if (dataArgs[0] == "test_sample") {
    Serial.println("ZGINAM PROBKE O DLUGOSCI : " + dataArgs[1] + " PROMIEN ZGINANIA :  " + dataArgs[2]);
    Serial.println("PREDKOSC ZGINANIA : " + dataArgs[3] + " ILOSC CYKLI : " + dataArgs[4] + " Typ zginania : " + dataArgs[5]);

    test_sample(dataArgs[1].toInt(), dataArgs[2].toInt(), dataArgs[3].toInt(), dataArgs[4].toInt(), dataArgs[5].toInt());

  } else if (dataArgs[0] == "get_max_radius") {
    Serial.println("MAKSYMALNY PROMIEN ZGINANIA DLA PROBKI O DLUGOSCI " + dataArgs[1] + " TO " + String(get_Max_Bending_Radius(dataArgs[1].toInt())));

  } else if (dataArgs[0] == "home_step") {
    Serial.println("USTALAM POZYCJE 0");
    home_step();

  } else if (dataArgs[0] == "arm") {
    arm();
    Serial.println("MASZYNA UZBROJONA");

  } else if (dataArgs[0] == "disarm") {
    disarm();
    Serial.println("MASZYNA ROZBROJONA");

  } else if (dataArgs[0] == "sample") {
    Serial.println("ROZMIESZCZENIE DLA  PROBKI " + dataArgs[1]);
    sample_in(dataArgs[1].toInt());
  } else if (dataArgs[0] == "step_dual") {
    Serial.println("ruszam dwoma silnikami w kierunku " + dataArgs[1] + " krokow : " + dataArgs[2]);
    _step_dual(dataArgs[1].toInt(), dataArgs[2].toInt());
  } else {
    // Serial.print("NIEZNANA FUNKCJA : ");
    // Serial.println(dataArgs[0]);
  }

}

void home_step(void) {

  while (digitalRead(PIN_ENDSTOP_1) != 0) {
    _step(1, 1, 1);
  }
  while (digitalRead(PIN_ENDSTOP_2) != 0) {
    _step(2, 1, 1);
  }

  _step(1, 0, 250);
  _step(2, 0, 250);

  while (digitalRead(PIN_ENDSTOP_1) != 0) {
    _step(1, 1, 1);
    delay(15);
  }
  while (digitalRead(PIN_ENDSTOP_2) != 0) {
    _step(2, 1, 1);
    delay(15);
  }

  stepper_position = 0;
  homed = true;
  Serial.println("SILNIKI KROKOWE NA POZYCJI 0 ");
}

void set_servo(int servo_number, int deg) {
  if (servo_number == 1) {
    servo_1.write(deg + C_SERVO1);
  } else if (servo_number == 2) {
    servo_2.write(deg + C_SERVO2);
  } else {
    Serial.println("BLEDNY NUMER SERWA ! wybierz 1 lub 2 ");
  }
}

// kroki bez zabezpieczenia ! uzywac rozwaznie
void _step(int motor, int dir, int steps) {

  int delay_time = 500;

  while (steps >= 0) {

    if (motor == 1) {
      if (dir == 0) {
        digitalWrite(PIN_DIR_1, LOW);
      } else {
        digitalWrite(PIN_DIR_1, HIGH);
      }

      digitalWrite(PIN_STEP_1, HIGH);
      delayMicroseconds(delay_time);
      digitalWrite(PIN_STEP_1, LOW);
      delayMicroseconds(delay_time);

    } else if (motor == 2) {
      if (dir == 0) {
        digitalWrite(PIN_DIR_2, LOW);
      } else {
        digitalWrite(PIN_DIR_2, HIGH);
      }

      digitalWrite(PIN_STEP_2, HIGH);
      delayMicroseconds(delay_time);
      digitalWrite(PIN_STEP_2, LOW);
      delayMicroseconds(delay_time);
    }
    steps--;
  }
}

// kroki zsuwajace lub rozsuwajace 
// zabezpieczone 

void _step_dual(int dir, int steps) {

  int delay_time = 500;

  if ((stepper_position >= 0) and(stepper_position < 100 * STEPS_PER_MM)) {

    while (steps > 0) {
      if (dir == 0) {
        digitalWrite(PIN_DIR_1, LOW);
        digitalWrite(PIN_DIR_2, LOW);
        stepper_position++;
      } else {
        digitalWrite(PIN_DIR_1, HIGH);
        digitalWrite(PIN_DIR_2, HIGH);
        stepper_position--;
      }

      digitalWrite(PIN_STEP_1, HIGH);
      digitalWrite(PIN_STEP_2, HIGH);
      delayMicroseconds(delay_time);
      digitalWrite(PIN_STEP_1, LOW);
      digitalWrite(PIN_STEP_2, LOW);
      delayMicroseconds(delay_time);

      // Serial.println(stepper_position);
      steps--;
    }
  }
}

void test_sample(uint16_t sample_length, float radius, int delay_time, int cycles, int type) {

  //sample_length+=24;

  float max_radius = get_Max_Bending_Radius(sample_length);

  float circle_angle = calc_Circle_Angle(sample_length, max_radius);
  float displacement = calc_Displacement(sample_length, circle_angle, max_radius);
  int servo_angle = (int) calc_servo_angle(sample_length, max_radius, circle_angle, displacement);

  if (radius < max_radius) {
    Serial.println("promień gięcia zbyt mały, ustawiam maksimum : " + String(max_radius));
    radius = max_radius;
  }

  float add_radius = 0.0001;
  float step_count = displacement * STEPS_PER_MM;
  int repeat_count = 0;
  int angle_temp = 0;
  int max_angle = 90 ;

  Serial.println("Obliczam pozycję silników oraz katy serwomechanizmow");

  while (step_count > 0) {

    radius += add_radius;
    circle_angle = calc_Circle_Angle(sample_length, radius);
    displacement = calc_Displacement(sample_length, circle_angle, radius);
    servo_angle = calc_servo_angle(sample_length, radius, circle_angle, displacement);
    step_count = displacement * STEPS_PER_MM;
    //Serial.println(String(radius,6) +" " + String(displacement,5) +" "+ String(displacement,5));
    //Serial.println( " step_count " + String(step_count) + " servo_angle " + String(abs(servo_angle)));

    if (servo_angle == angle_temp) {
      repeat_count++;
    } else {
      repeat_count = 0;
      angle_temp = servo_angle;
  //   Serial.println( " step_count " + String(step_count) + " servo_angle " + String(abs(servo_angle)));
      angle_array[abs(servo_angle)] = round(step_count);
      if( abs(servo_angle) < max_angle ){
        max_angle = abs(servo_angle);
      }
    }

    if (repeat_count > 9) {
      add_radius *= 2;

    }
  }
// wszystko poza 90st to smieci, trzeba wytrzucic, obliczenie dla kata 0 tez 

  //for(int x = servo_angle; x<90; x++){
 //  angle_array[x] = angle_array[servo_angle];
  //  }

  for(int x = 0; x<90; x++){
    // Serial.println( " step_count " + String(angle_array[x]) + " servo_angle " + String(x));
     
  }
 // Serial.println( " max_angle " + String(max_angle) );
     



  Serial.println("Macierz ruchow policzona, przechodze do giecia ");

  uint16_t sample_pos = sample_max_pos(sample_length);

  for (int c = cycles; c > 0; c--) {

    uint16_t pos = sample_pos;
    set_step_pos(pos - 25);
    


  if(type == 2 || type ==3){
    for (int angle = 89; angle > max_angle; angle--) {
      //set servos here 
      // Serial.println("kat ruchu " + String(180-angle));
    
      pos = sample_pos + (angle_array[angle] * 2);
     // Serial.println("pozycja silnika " + String(pos)+" ang aray " + String(angle_array[angle])) ;

      set_servo(1, 180 - angle);
      set_servo(2, angle);
      delay((int)(delay_time / 2));
      set_step_pos(pos);

    }
    
    for (int angle = max_angle ; angle < 89; angle++) {
      ///set servos here 
      //   Serial.println("kat ruchu " + String(180-angle));
    
      pos = sample_pos + (angle_array[angle] * 2);
      //   Serial.println("pozycja silnika " + String(pos));

      set_servo(1, 180 - angle);
      set_servo(2, angle);
      delay((int)(delay_time / 2));
      set_step_pos(pos);

    }
  }
    
    delay((int)(delay_time / 2));
    set_step_pos(pos - 125);
 if(type == 1 || type ==3){
    for (int angle = 89; angle > max_angle; angle--) {
      //set servos here 
         //Serial.println("kat ruchu " + String(angle));
     
      pos = sample_pos + (angle_array[angle] * 2);
       // Serial.println("pozycja silnika " + String(pos)+" ang aray " + String(angle_array[angle])) ;

      set_servo(1, angle);
      set_servo(2, 180 - angle);
      delay((int)(delay_time / 2));
      set_step_pos(pos);

    }
    for (int angle = max_angle; angle < 89; angle++) {
      ///set servos here 
     // Serial.println("kat ruchu " + String(angle));
      
      pos = sample_pos + (angle_array[angle] * 2);
      //  Serial.println("pozycja silnika " + String(pos));

      set_servo(1, angle);
      set_servo(2, 180 - angle);
      delay((int)(delay_time / 2));
      set_step_pos(pos);

    }
 }

    Serial.println(String(cycles - c) + " / " + String(cycles));
  } 
  Serial.println("gięcie zakonczone powodzeniem ");
  Serial.println("Wykonano cykli :  " + String(cycles));
}

void sample_in(int size_mm) {

  Serial.println("pozycja dla probki " + String(sample_max_pos(size_mm)));

  if (homed == true) {

    set_step_pos(sample_max_pos(size_mm));

  } else {
    Serial.println("MASZYNA NIE MA WYZNACZONYCH POZYCJI ZERO");
  }
}

uint16_t sample_max_pos(int size_mm) {
  size_mm += C_SIZE;
  int pos = 100 * STEPS_PER_MM - size_mm * STEPS_PER_MM / 2;
  return pos;
}

void set_step_pos(uint32_t pos) {

  while (stepper_position != pos) {
    // Serial.println(String(stepper_position) + " " + String(pos));
    if (stepper_position > pos) {
      _step_dual(1, 1);

    } else if (stepper_position < pos) {
      _step_dual(0, 1);
    }
  }

}

float calc_Circle_Angle(float sample_length, float radius) {
  return (sample_length * 360) / (2 * PI * radius);
}

float calc_Displacement(float sample_length, float circle_angle, float radius) {

  return (sample_length / 2) - radius * sin(toRad(circle_angle / 2));
}

float calc_servo_angle(float sample_length, float radius, float circle_angle, float displacement) {

  float h = radius * cos(toRad(circle_angle / 2));
  float servo_angle = toAngle(atan2((-1 * h), ((sample_length / 2) - displacement)));
  return servo_angle;

}

float toRad(float angle) {
  return (angle * 71 / 4068);
}
float toAngle(float radian) {
  return (radian * 4068 / 71);
}

float get_Max_Bending_Radius(float sample_length) {
  float max_radius = sample_length;

  while (calc_Circle_Angle(sample_length, max_radius) < 180) {
    max_radius -= 0.0005;
  }
  return max_radius;
}

void arm(void) {
  digitalWrite(PIN_EN_STEP, LOW);
  digitalWrite(PIN_EN_SERVO, HIGH);

}

void disarm(void) {
  digitalWrite(PIN_EN_STEP, HIGH);
  digitalWrite(PIN_EN_SERVO, LOW);

}
