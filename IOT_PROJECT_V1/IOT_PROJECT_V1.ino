#include <Wire.h>
#include <DHT.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include <MQUnifiedsensor.h>

/* analog pin */
#define rain A8   // 수분 감지 센서
#define wla A9   // Window Linear Actuator 전압센서
#define room_lux1 A10   // 실내 조도 감지 1
#define room_lux2 A11   // 실내 조도 감지 2
MQUnifiedsensor gas5("Arduino UNO", 5, 10, A12, "MQ-5");   // lpg 가스 센서 가스측 mq5 - 초흰 - 12
MQUnifiedsensor gas7("Arduino UNO", 5, 10, A13, "MQ-7");   // co 가스 센서 가스측 mq7 - 초 - 13

/* digital pin */
#define detect1 28     // 감지1
#define detect2 29     // 감지2
#define detect3 30     // 감지3
#define heatrelay 31  // 매트 쇼트 릴레이
#define boilerrelay 32  // 보일러 쇼트 릴레이 // 파
#define windowclose 33 // 윈도우 클로즈
#define windowopen 34 // 윈도우 오픈
#define window0 35 // 윈도우 닫힘
#define window2 36 // 윈도우 열림 - 2단계
#define window1 37 // 윈도우 열림 - 1단계
#define window4 38 // 윈도우 완전 열림
#define window3 39 // 윈도우 열림 - 3단계

//DHT room_dht1(41, DHT22); // 방 1 온습도센서
DHT room_dht2(42, DHT22); // 방 2 온습도센서
//DHT room_dht3(43, DHT22); // 방 3 온습도센서
DHT room_dht4(44, DHT22); // 방 4 온습도센서
DHT pad_dht1(46, DHT22); // 매트 온습도센서
DHT outdoor_dht1(47, DHT22); // 실외 1 온습도센서
DHT outdoor_dht2(48, DHT22); // 실외 2 온습도센서

EthernetServer es(21263);

byte seconds, minutes, hours, day, date, month, year,
     roomtemp, roomhumi, roomhic, roomindex, roomlux,
     outdoortemp, outdoorhumi, outdoorhic, outdoorindex,
     padtemp, padhumi, padhic, padindex,
     f_overload,
     mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
char domain[] = "seo.chaegeon.com";
float room_t, room_h, room_hic, room_index, room_lux,
      room_t_previous_value, room_h_previous_value, room_hic_previous_value, room_index_previous_value, room_lux_previous_value,
      outdoor_t, outdoor_h, outdoor_hic, outdoor_index, outdoor_moisture,
      outdoor_t_previous_value, outdoor_h_previous_value, outdoor_hic_previous_value, outdoor_index_previous_value,
      pad_t, pad_h, pad_hic, pad_index,
      pad_t_previous_value, pad_h_previous_value, pad_hic_previous_value, pad_index_previous_value,
      window_v;
String identifier, command, ipaddress, useragent;
boolean f_lastboot, detect, serialflag = 0;
unsigned long sensing, overloaddetectdelay;

void setup() {
  Serial.begin(9600);

  Wire.begin();
  get3231Date();

  //  room_dht1.begin();
  room_dht2.begin();
  //  room_dht3.begin();
  room_dht4.begin();
  pad_dht1.begin();
  outdoor_dht1.begin();
  outdoor_dht2.begin();

  gas5.setRegressionMethod(1); gas5.setA(80.897); gas5.setB(-2.431); gas5.init(); gas5.setR0(12.24);
  gas7.setRegressionMethod(1); gas7.setA(99.042); gas7.setB(-1.518); gas7.init(); gas7.setR0(21.78);

  pinMode(heatrelay, OUTPUT); digitalWrite(heatrelay, 1);
  pinMode(boilerrelay, OUTPUT); digitalWrite(boilerrelay, 1);
  pinMode(windowopen, OUTPUT); digitalWrite(windowopen, 1);
  pinMode(windowclose, OUTPUT); digitalWrite(windowclose, 1);
  pinMode(wla, INPUT);
  pinMode(rain, INPUT);
  pinMode(detect1, INPUT);
  pinMode(detect2, INPUT);
  pinMode(detect3, INPUT);
  pinMode(window0, INPUT_PULLUP);
  pinMode(window1, INPUT_PULLUP);
  pinMode(window2, INPUT_PULLUP);
  pinMode(window3, INPUT_PULLUP);
  pinMode(window4, INPUT_PULLUP);

  sensor();

  Ethernet.begin(mac);
  es.begin();
}

void loop() {
  get3231Date();
  window_control();
  communication();
  control();

  if (Serial.available()) {
    if (Serial.read() == 84) {
      set3231Date();
      get3231Date();
      Serial.print("20");
      Serial.print(year, DEC);
      Serial.print(".");
      Serial.print(month, DEC);
      Serial.print(".");
      Serial.print(date, DEC);
      Serial.print(".(");
      Serial.print(day, DEC);
      Serial.print(")\t");
      Serial.print(hours, DEC);
      Serial.print(":");
      Serial.print(minutes, DEC);
      Serial.print(":");
      Serial.println(seconds, DEC);
    }
  }

  if (millis() - sensing >= 2000) {
    sensor();
    serial_print();

    if (EEPROM.read(2) == 0) { // 자동 제어
      //      if (day == 1 || day == 7) {
      //        if (pad_hic <= EEPROM.read(1)) {
      //          digitalWrite(heatrelay, 0);
      //        } else if (pad_hic > EEPROM.read(1)) {
      //          digitalWrite(heatrelay, 1);
      //        }
      //      if (day >= 2 && day <= 6) { // } else
      if (hours >= 22 || hours <= 8) {
        if (pad_hic <= EEPROM.read(1)) {
          digitalWrite(heatrelay, 0);
        } else if (pad_hic > EEPROM.read(1)) {
          digitalWrite(heatrelay, 1);
        }
      } else if (hours > 8 && hours < 22) {
        digitalWrite(heatrelay, 1);
      }
      //      }
    } else if (EEPROM.read(2) == 1) { // 수동 제어 - 켜짐
      if ((hours == 0 || hours == 12) && minutes == 0 && seconds < 5) { // 수동 - 켜짐 상태에서 오전 12시 또는 오후 12시면 자동제어로 변경
        EEPROM.update(2, 0);
      }
      digitalWrite(heatrelay, 0);
    } else if (EEPROM.read(2) == 2) { // 수동 제어 - 꺼짐
      digitalWrite(heatrelay, 1);
    }

    if (EEPROM.read(5) == 0) { // 자동 제어
      if (day == 1 || day == 7) {
        if (room_hic <= EEPROM.read(4) - 0.5) {
          if (EEPROM.read(6) <= 1) {
            digitalWrite(boilerrelay, 0);
          } else if (EEPROM.read(6) > 1) {
            digitalWrite(boilerrelay, 1);
          }
        } else if (room_hic > EEPROM.read(4)) {
          digitalWrite(boilerrelay, 1);
        }
      } else if (day >= 2 && day <= 6) {
        if (hours >= 16 || hours <= 8) {
          if (room_hic <= EEPROM.read(4) - 0.5) {
            if (EEPROM.read(6) <= 1) {
              digitalWrite(boilerrelay, 0);
            } else if (EEPROM.read(6) > 1) {
              digitalWrite(boilerrelay, 1);
            }
          } else if (room_hic > EEPROM.read(4)) {
            digitalWrite(boilerrelay, 1);
          }
        } else if (hours > 8 && hours < 16) {
          digitalWrite(boilerrelay, 1);
        }
      }
    } else if (EEPROM.read(5) == 1) { // 수동 제어 - 켜짐
      if (room_hic > 25) {
        EEPROM.update(5, 0);
      }
      digitalWrite(boilerrelay, 0);
    } else if (EEPROM.read(5) == 2) { // 수동 제어 - 꺼짐
      digitalWrite(boilerrelay, 1);
    }
    sensing = millis();
  }
}

void sensor() {
  room_t = (room_dht2.readTemperature() + room_dht4.readTemperature()) / 2; // 센서 교체 후 원상 복구.room_dht1.readTemperature() + room_dht2.readTemperature() +
  room_h = (room_dht2.readHumidity() + room_dht4.readHumidity()) / 2; // 센서 교체 후 원상 복구.room_dht1.readHumidity() + room_dht2.readHumidity() +
  room_hic = room_dht2.computeHeatIndex(room_t, room_h, false);
  room_index = (1.8f * room_t) - (0.55 * (1 - room_h / 100.0f) * (1.8f * room_t - 26)) + 32;
  room_lux = (((1023 - analogRead(room_lux1)) + (1023 - analogRead(room_lux2))) / 2) / 1024.0f * 100;

  outdoor_t = outdoor_dht1.readTemperature(); // 센서 교체 후 원상 복구. + outdoor_dht2.readTemperature()
  outdoor_h = outdoor_dht1.readHumidity(); // 센서 교체 후 원상 복구. + outdoor_dht2.readHumidity()
  outdoor_hic = outdoor_dht1.computeHeatIndex(outdoor_t, outdoor_h, false);
  outdoor_index = (1.8f * outdoor_t) - (0.55 * (1 - outdoor_h / 100.0f) * (1.8f * outdoor_t - 26)) + 32;
  outdoor_moisture = (1023 - analogRead(rain)) / 1024.0f * 100;

  gas5.update();
  gas7.update();

  pad_t = pad_dht1.readTemperature();
  pad_h = pad_dht1.readHumidity();
  pad_hic = pad_dht1.computeHeatIndex(pad_t, pad_h, false);
  pad_index = (1.8f * pad_t) - (0.55 * (1 - pad_h / 100.0f) * (1.8f * pad_t - 26)) + 32;

  value_change_check();
}

void control() {
  if (ipaddress != "0.0.0.0" && ipaddress != "") {
    if (identifier == "window") {
      EEPROM.update(8, 1);
      if (command == "automatic") {
        EEPROM.update(6, 0); // 닫기
      } else if (command == "close") {
        EEPROM.update(6, 1); // 닫기
      } else if (command == "little") {
        EEPROM.update(6, 2);
      } else if (command == "1-3-open") {
        EEPROM.update(6, 3);
      } else if (command == "2-3-open") {
        EEPROM.update(6, 4);
      } else if (command == "fully-open") {
        EEPROM.update(6, 5);
      }
    } else if (identifier == "boiler") {
      if (command == "automatic") {
        EEPROM.update(5, 0);
      } else if (command == "manual-on") {
        EEPROM.update(5, 1);
      } else if (command == "manual-off") {
        EEPROM.update(5, 2);
      }
    } else if (identifier == "boiler-temp") {
      if (command.toInt() >= 17 && command.toInt() <= 25) {
        EEPROM.update(4, command.toInt());
      }
    } else if (identifier == "electric-pad") {
      if (command == "automatic") {
        EEPROM.update(2, 0);
      } else if (command == "manual-on") {
        EEPROM.update(2, 1);
      } else if (command == "manual-off") {
        EEPROM.update(2, 2);
      }
    } else if (identifier == "electric-pad-temp") {
      if (command.toInt() >= 29 && command.toInt() <= 90) {
        EEPROM.update(1, command.toInt());
      }
    }
  }
}
