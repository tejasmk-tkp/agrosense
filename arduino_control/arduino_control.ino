#include <Servo.h>

int sensorThres = 500;
//DC Motor
const int motor_in1 = 9;
const int motor_in2 = 10;
//UltraSonic
const int echo = 0;
const int trig = 1;
//Gas Sensor
const int mq2 = A0;
//Buzzer
const int buzzer = 8;
//Rain Sensor
const int rain_sensor = 7;

Servo myServo;

int pos = 0; 

long duration;
float distance;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(motor_in1, OUTPUT);
  pinMode(motor_in2, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(mq2, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(rain_sensor, INPUT);

  myServo.attach(13);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead(mq2);
  //Serial.println(val);

  if (val-50 > sensorThres)
  {
    tone(buzzer, 1000, 200);
  }
  else
  {
    noTone(buzzer);
  }

  int rain_val = digitalRead(rain_sensor);
  //Serial.println(rain_val);

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  //Serial.println(distance);

  if (rain_val == 1) {
    digitalWrite(motor_in1, HIGH);
    digitalWrite(motor_in2, LOW);
    if (distance >= 15) {
      digitalWrite(motor_in1, LOW);
      digitalWrite(motor_in2, LOW);
      Serial.println("Opening");
    }
  }
  // if (distance > 5) {
  //   Serial.println("Far");
  // }
  // else {
  //   Serial.println("Near");
  // } 
  else {
    digitalWrite(motor_in1, LOW);
    digitalWrite(motor_in2, HIGH);
    if (distance <= 5) {
      digitalWrite(motor_in1, LOW);
      digitalWrite(motor_in2, LOW);
      Serial.print("Closing");
    }
  }
  for (pos = 0; pos <= 30; pos += 1) { // goes from 0 degrees to 180 degrees
    myServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(5000);
  for (pos = 30; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}