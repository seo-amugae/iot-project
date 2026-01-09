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

void communication() {
  EthernetClient server = es.available(), client;

  //  if (millis() - senddelay >= 500) {
  if (client.connect(domain, 80)) {
    String sendvalue = "RT=";
    sendvalue += int(room_t); sendvalue += "."; sendvalue += int((room_t - int(room_t)) * 10);
    sendvalue += "&RH="; sendvalue += int(room_h); sendvalue += "."; sendvalue += int((room_h - int(room_h)) * 10);
    sendvalue += "&RC="; sendvalue += int(room_hic); sendvalue += "."; sendvalue += int((room_hic - int(room_hic)) * 10);
    sendvalue += "&RI="; sendvalue += int(room_index); sendvalue += "."; sendvalue += int((room_index - int(room_index)) * 10);
    sendvalue += "&RL="; sendvalue += int(room_lux); sendvalue += "."; sendvalue += int((room_lux - int(room_lux)) * 100);
    sendvalue += "&WV="; sendvalue += int(window_v); sendvalue += "."; sendvalue += int((window_v - int(window_v)) * 100);
    sendvalue += "&OT="; sendvalue += int(outdoor_t); sendvalue += ".";

    if (int((outdoor_t - int(outdoor_t)) * 10) < 0) {
      sendvalue += int((outdoor_t - int(outdoor_t)) * -10);
    } else if (int((outdoor_t - int(outdoor_t)) * 10) >= 0) {
      sendvalue += int((outdoor_t - int(outdoor_t)) * 10);
    }

    sendvalue += "&OH="; sendvalue += int(outdoor_h); sendvalue += "."; sendvalue += int((outdoor_h - int(outdoor_h)) * 10);
    sendvalue += "&OC="; sendvalue += int(outdoor_hic); sendvalue += ".";

    if (int((outdoor_hic - int(outdoor_hic)) * 10) < 0) {
      sendvalue += int((outdoor_t - int(outdoor_t)) * -10);
    } else if (int((outdoor_hic - int(outdoor_hic)) * 10) >= 0) {
      sendvalue += int((outdoor_hic - int(outdoor_hic)) * 10);
    }

    sendvalue += "&OI="; sendvalue += int(outdoor_index); sendvalue += "."; sendvalue += int((outdoor_index - int(outdoor_index)) * 10);
    sendvalue += "&OM="; sendvalue += int(outdoor_moisture); sendvalue += "."; sendvalue += int((outdoor_moisture - int(outdoor_moisture)) * 100);
    sendvalue += "&PT="; sendvalue += int(pad_t); sendvalue += "."; sendvalue += int((pad_t - int(pad_t)) * 10);
    sendvalue += "&PC="; sendvalue += int(pad_hic); sendvalue += "."; sendvalue += int((pad_hic - int(pad_hic)) * 10);
    sendvalue += "&g5="; sendvalue += int(gas5.readSensor()); sendvalue += "."; sendvalue += int((gas5.readSensor() - int(gas5.readSensor())) * 100);
    sendvalue += "&g7="; sendvalue += int(gas7.readSensor()); sendvalue += "."; sendvalue += int((gas7.readSensor() - int(gas7.readSensor())) * 100);

    if (detect != 0) {
      sendvalue += "&d=";
      sendvalue += detect;
    }

    if (EEPROM.read(6) != 0) {
      sendvalue += "&WS=";
      sendvalue += EEPROM.read(6);
    }

    if (EEPROM.read(10) != 0) {
      sendvalue += "&WM=";
      sendvalue += EEPROM.read(10);
    }

    if (EEPROM.read(7) != 0) {
      sendvalue += "&WE=";
      sendvalue += EEPROM.read(7);
    }

    if (EEPROM.read(2) != 0) {
      sendvalue += "&PS=";
      sendvalue += EEPROM.read(2);
    }

    if (digitalRead(heatrelay) != 0) {
      sendvalue += "&PR=";
      sendvalue += digitalRead(heatrelay);
    }

    if (EEPROM.read(1) != 0) {
      sendvalue += "&ps=";
      sendvalue += EEPROM.read(1);
    }

    if (EEPROM.read(5) != 0) {
      sendvalue += "&BS=";
      sendvalue += EEPROM.read(5);
    }

    if (digitalRead(boilerrelay) != 0) {
      sendvalue += "&BR=";
      sendvalue += digitalRead(boilerrelay);
    }

    if (EEPROM.read(4) != 0) {
      sendvalue += "&bs=";
      sendvalue += EEPROM.read(4);
    }

    if (roomtemp != 0) {
      sendvalue += "&rt=";
      sendvalue += roomtemp;
    }

    if (roomhumi != 0) {
      sendvalue += "&rh=";
      sendvalue += roomhumi;
    }

    if (roomhic != 0) {
      sendvalue += "&rc=";
      sendvalue += roomhic;
    }

    if (roomindex != 0) {
      sendvalue += "&ri=";
      sendvalue += roomindex;
    }

    if (outdoortemp != 0) {
      sendvalue += "&ot=";
      sendvalue += outdoortemp;
    }

    if (outdoorhumi != 0) {
      sendvalue += "&oh=";
      sendvalue += outdoorhumi;
    }

    if (outdoorhic != 0) {
      sendvalue += "&oc=";
      sendvalue += outdoorhic;
    }

    if (outdoorindex != 0) {
      sendvalue += "&oi=";
      sendvalue += outdoorindex;
    }

    if (padtemp != 0) {
      sendvalue += "&pt=";
      sendvalue += padtemp;
    }

    if (padhic != 0) {
      sendvalue += "&pc=";
      sendvalue += padhic;
    }

    if (f_lastboot == 0) {
      sendvalue += "&lb=20";
      if (year < 10) sendvalue += "0"; sendvalue += year;
      if (month < 10) sendvalue += "0"; sendvalue += month;
      if (date < 10) sendvalue += "0"; sendvalue += date;
      if (hours < 10) sendvalue += "0"; sendvalue += hours;
      if (minutes < 10) sendvalue += "0"; sendvalue += minutes;
      if (seconds < 10) sendvalue += "0"; sendvalue += seconds;

      sendvalue += "&dip=";
      for (byte thisByte = 0; thisByte < 4; thisByte++) {
        sendvalue += Ethernet.localIP()[thisByte];
        if (thisByte < 3) {
          sendvalue += ".";
        }
      }

      f_lastboot = 1;
    }

    if (f_overload == 0) {
      sendvalue += "&wd=20";
      if (year < 10) sendvalue += "0"; sendvalue += year;
      if (month < 10) sendvalue += "0"; sendvalue += month;
      if (date < 10) sendvalue += "0"; sendvalue += date;
      if (hours < 10) sendvalue += "0"; sendvalue += hours;
      if (minutes < 10) sendvalue += "0"; sendvalue += minutes;
      if (seconds < 10) sendvalue += "0"; sendvalue += seconds;
      sendvalue += "&wv=";
      sendvalue += int(window_v);
      sendvalue += ".";
      sendvalue += int((window_v - int(window_v)) * 100);

      f_overload = 1;
    }

    if (ipaddress != "") {
      sendvalue += "&ip=";
      sendvalue += ipaddress;
      ipaddress = "";
    }

    if (identifier != "") {
      sendvalue += "&id=";
      sendvalue += identifier;
      identifier = "";
    }

    if (command != "") {
      sendvalue += "&c=";
      sendvalue += command;
      command = "";
    }

    if (useragent != "") {
      sendvalue += "&ua=";
      sendvalue += useragent;
      useragent = "";
    }

    sendvalue += "&l=";
    int sendvaluelength = sendvalue.length();
    sendvaluelength = sendvaluelength + String(sendvaluelength).length();
    sendvalue += sendvaluelength;

    client.println("POST /communication.php HTTP/1.1");
    client.println("Host: seo.chaegeon.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(sendvalue.length());
    client.println();
    client.println(sendvalue);
    client.stop();
    //    senddelay = millis();
  }
  //  }

  if (server) {
    boolean currentLineIsBlank = 1;
    String finder = "", temp = "", identifier_ = "", command_ = "", ipaddress_ = "", useragent_ = "";
    int data_length = -1;
    while (server.connected()) {
      if (server.available()) {
        char c = server.read();
        finder += c;

        if (c == '\n' && currentLineIsBlank) {
          temp = finder.substring(finder.indexOf("Content-Length:") + 15); temp.trim(); data_length = temp.toInt();
          while (data_length-- > 0) {
            c = server.read();
            finder += c;
          }

          ipaddress_ = finder.substring(finder.indexOf("ip=") + 3, finder.indexOf("&", finder.indexOf("ip=") + 3)); ipaddress = ipaddress_;
          identifier_ = finder.substring(finder.indexOf("&") + 1, finder.indexOf("=", finder.indexOf("&") + 1)); identifier = identifier_;
          command_ = finder.substring(finder.indexOf(identifier + "=") + identifier.length() + 1, finder.indexOf("\n", finder.indexOf(identifier + "=") + identifier.length() + 1)); command = command_;
          useragent_ = finder.substring(finder.indexOf("User-Agent: ") + 12, finder.indexOf("\n" - 1, finder.indexOf("User-Agent: ") + 12)); useragent = useragent_;

          server.println("HTTP/1.1 301 Moved Permanently");
          server.println("Location: http://seo.chaegeon.com");
          server.println();
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = 1;
        } else if (c != '\r') {
          currentLineIsBlank = 0;
        }
      }
    }
    server.stop();
  }
}

void serial_print() {
  if (serialflag == 1) {
    //    String sendvalue = "datetime=20";
    //    sendvalue += int(year);
    //    sendvalue += ".";
    //    sendvalue += int(month);
    //    sendvalue += ".";
    //    sendvalue += int(date);
    //    sendvalue += ".%20";
    //    sendvalue += int(hours);
    //    sendvalue += ":";
    //    sendvalue += int(minutes);
    //    sendvalue += ":";
    //    sendvalue += int(seconds);
    //    sendvalue += "&room_t=";
    //    sendvalue += int(floor(room_t));
    //    sendvalue += ".";
    //    sendvalue += int((room_t - int(floor(room_t))) * 100);
    //    sendvalue += "&room_h=";
    //    sendvalue += int(floor(room_h));
    //    sendvalue += ".";
    //    sendvalue += int((room_h - int(floor(room_h))) * 100);
    //    sendvalue += "&room_hic=";
    //    sendvalue += int(floor(room_hic));
    //    sendvalue += ".";
    //    sendvalue += int((room_hic - int(floor(room_hic))) * 100);
    //    sendvalue += "&room_index=";
    //    sendvalue += int(floor(room_index));
    //    sendvalue += ".";
    //    sendvalue += int((room_index - int(floor(room_index))) * 100);
    //    sendvalue += "&room_lux=";
    //    sendvalue += int(floor(room_lux));
    //    sendvalue += ".";
    //    sendvalue += int((room_lux - int(floor(room_lux))) * 100);
    //    sendvalue += "&detect=";
    //    sendvalue += detect;
    //    sendvalue += "&window_s=";
    //    sendvalue += EEPROM.read(6);
    //    sendvalue += "&window_l=";
    //    sendvalue += window_l;
    //    sendvalue += "&window_e=";
    //    sendvalue += EEPROM.read(7);
    //    sendvalue += "&window_v=";
    //    sendvalue += int(floor(window_v));
    //    sendvalue += ".";
    //    sendvalue += int((window_v - int(floor(window_v))) * 100);
    //    sendvalue += "&window_ov=";
    //    sendvalue += EEPROM.read(8);
    //    sendvalue += ".";
    //    sendvalue += EEPROM.read(9);
    //    sendvalue += "&outdoor_t=";
    //    sendvalue += int(floor(outdoor_t));
    //    sendvalue += ".";
    //    sendvalue += int((outdoor_t - int(floor(outdoor_t))) * 100);
    //    sendvalue += "&outdoor_h=";
    //    sendvalue += int(floor(outdoor_h));
    //    sendvalue += ".";
    //    sendvalue += int((outdoor_h - int(floor(outdoor_h))) * 100);
    //    sendvalue += "&outdoor_hic=";
    //    sendvalue += int(floor(outdoor_hic));
    //    sendvalue += ".";
    //    sendvalue += int((outdoor_hic - int(floor(outdoor_hic))) * 100);
    //    sendvalue += "&outdoor_index=";
    //    sendvalue += int(floor(outdoor_index));
    //    sendvalue += ".";
    //    sendvalue += int((outdoor_index - int(floor(outdoor_index))) * 100);
    //    sendvalue += "&pad_s=";
    //    sendvalue += EEPROM.read(2);
    //    sendvalue += "&pad_r=";
    //    sendvalue += digitalRead(heatrelay);
    //    sendvalue += "&pad_t=";
    //    sendvalue += int(floor(pad_t));
    //    sendvalue += ".";
    //    sendvalue += int((pad_t - int(floor(pad_t))) * 100);
    //    sendvalue += "&pad_h=";
    //    sendvalue += int(floor(pad_h));
    //    sendvalue += ".";
    //    sendvalue += int((pad_h - int(floor(pad_h))) * 100);
    //    sendvalue += "&pad_hic=";
    //    sendvalue += int(floor(pad_hic));
    //    sendvalue += ".";
    //    sendvalue += int((pad_hic - int(floor(pad_hic))) * 100);
    //    sendvalue += "&pad_low=";
    //    sendvalue += EEPROM.read(0);
    //    sendvalue += "&pad_high=";
    //    sendvalue += EEPROM.read(1);
    //    sendvalue += "&boiler_s=";
    //    sendvalue += EEPROM.read(5);
    //    sendvalue += "&boiler_r=";
    //    sendvalue += digitalRead(boilerrelay);
    //    sendvalue += "&boiler_t=";
    //    sendvalue += int(floor(boiler_t));
    //    sendvalue += ".";
    //    sendvalue += int((boiler_t - int(floor(boiler_t))) * 100);
    //    sendvalue += "&boiler_h=";
    //    sendvalue += int(floor(boiler_h));
    //    sendvalue += ".";
    //    sendvalue += int((boiler_h - int(floor(boiler_h))) * 100);
    //    sendvalue += "&boiler_low=";
    //    sendvalue += EEPROM.read(3);
    //    sendvalue += "&boiler_high=";
    //    sendvalue += EEPROM.read(4);
    //    sendvalue += "&gas5=";
    //    sendvalue += int(floor(gas5.readSensor()));
    //    sendvalue += ".";
    //    sendvalue += int((gas5.readSensor() - int(floor(gas5.readSensor()))) * 100);
    //    sendvalue += "&boiler5=";
    //    sendvalue += int(floor(boiler5.readSensor()));
    //    sendvalue += ".";
    //    sendvalue += int((boiler5.readSensor() - int(floor(boiler5.readSensor()))) * 100);
    //    sendvalue += "&gas7=";
    //    sendvalue += int(floor(gas7.readSensor()));
    //    sendvalue += ".";
    //    sendvalue += int((gas7.readSensor() - int(floor(gas7.readSensor()))) * 100);
    //    sendvalue += "&boiler7=";
    //    sendvalue += int(floor(boiler7.readSensor()));
    //    sendvalue += ".";
    //    sendvalue += int((boiler7.readSensor() - int(floor(boiler7.readSensor()))) * 100);
    //    sendvalue += "&roomtemp=";
    //    sendvalue += roomtemp;
    //    sendvalue += "&roomhumi=";
    //    sendvalue += roomhumi;
    //    sendvalue += "&roomhic=";
    //    sendvalue += roomhic;
    //    sendvalue += "&roomindex=";
    //    sendvalue += roomindex;
    //    sendvalue += "&roomlux=";
    //    sendvalue += roomlux;
    //    sendvalue += "&outdoortemp=";
    //    sendvalue += outdoortemp;
    //    sendvalue += "&outdoorhumi=";
    //    sendvalue += outdoorhumi;
    //    sendvalue += "&outdoorhic=";
    //    sendvalue += outdoorhic;
    //    sendvalue += "&outdoorindex=";
    //    sendvalue += outdoorindex;
    //    sendvalue += "&padtemp=";
    //    sendvalue += padtemp;
    //    sendvalue += "&padhumi=";
    //    sendvalue += padhumi;
    //    sendvalue += "&padhic=";
    //    sendvalue += padhic;
    //    sendvalue += "&boilertemp=";
    //    sendvalue += boilertemp;
    //    sendvalue += "&boilerhumi=";
    //    sendvalue += boilerhumi;
    //    sendvalue += " HTTP/1.1";
    //    Serial.println(sendvalue);
    //    Serial.print("Content-Length: ");
    //    Serial.println(sendvalue.length());

    //    Serial.print("20");
    //    if (year < 10) {
    //      Serial.print("0");
    //    }
    //    Serial.print(year, DEC);
    //    Serial.print(" / ");
    //    if (month < 10) {
    //      Serial.print("0");
    //    }
    //    Serial.print(month, DEC);
    //    Serial.print(" / ");
    //    if (date < 10) {
    //      Serial.print("0");
    //    }
    //    Serial.print(date, DEC);
    //    Serial.print("\t");
    //    if (hours < 10) {
    //      Serial.print("0");
    //    }
    //    Serial.print(hours, DEC);
    //    Serial.print(" : ");
    //    if (minutes < 10) {
    //      Serial.print("0");
    //    }
    //    Serial.print(minutes, DEC);
    //    Serial.print(" : ");
    //    if (seconds < 10) {
    //      Serial.print("0");
    //    }
    //    Serial.print(seconds, DEC);
    //    Serial.println();
    //    Serial.print("RoomDHT(41): ");
    //    Serial.print(room_dht1.readTemperature(), 1);
    //    Serial.print(" c");
    //    Serial.print("\tRoomDHT(42): ");
    //    Serial.print(room_dht2.readTemperature(), 1);
    //    Serial.print(" c");
    //    Serial.print("\tRoomDHT(43): ");
    //    Serial.print(room_dht3.readTemperature(), 1);
    //    Serial.print(" c");
    //    Serial.print("\tRoomDHT(44): ");
    //    Serial.print(room_dht4.readTemperature(), 1);
    //    Serial.print(" c");
    //    Serial.print("\tPadDHT(46): ");
    //    Serial.print(pad_dht1.readTemperature(), 1);
    //    Serial.print(" c");
    //    Serial.print("\tOutdoorDHT(47): ");
    //    Serial.print(outdoor_dht1.readTemperature(), 1);
    //    Serial.print(" c");
    //    Serial.print("\tSENDVALUE: ");
    //    Serial.print(int(outdoor_t));
    //    Serial.print(".");
    //    if (int((outdoor_t - int(outdoor_t)) * 10) < 0)
    //      Serial.print(int((outdoor_t - int(outdoor_t)) * -10));
    //    else
    //      Serial.print(int((outdoor_t - int(outdoor_t)) * 10));
    //    Serial.print(" c");
    //        Serial.print("\tOutdoorDHT(48): ");
    //        Serial.print(outdoor_dht2.readTemperature(), 1);
    //        Serial.print(" c");
    //    Serial.println();
    //    Serial.print("BoilerDHT(40): ");
    //    Serial.print(boiler_dht1.readHumidity(), 1);
    //    Serial.print(" %");
    //    Serial.print("\tRoomDHT(41): ");
    //    Serial.print(room_dht1.readHumidity(), 1);
    //    Serial.print(" %");
    //    Serial.print("\tRoomDHT(42): ");
    //    Serial.print(room_dht2.readHumidity(), 1);
    //    Serial.print(" %");
    //    Serial.print("\tRoomDHT(43): ");
    //    Serial.print(room_dht3.readHumidity(), 1);
    //    Serial.print(" %");
    //    Serial.print("\tRoomDHT(44): ");
    //    Serial.print(room_dht4.readHumidity(), 1);
    //    Serial.print(" %");
    //    Serial.print("\tPadDHT(46): ");
    //    Serial.print(pad_dht1.readHumidity(), 1);
    //    Serial.print(" %");
    //    Serial.print("\tOutdoorDHT(47): ");
    //    Serial.print(outdoor_dht1.readHumidity(), 1);
    //    Serial.print(" %");
    //    Serial.print("\tOutdoorDHT(48): ");
    //    Serial.print(outdoor_dht2.readHumidity(), 1);
    //    Serial.print(" %");
    //    Serial.println();
    //    Serial.println();
    //    Serial.print("Window Location: ");
    //    Serial.print(window_l);
    //    Serial.println();
    //    Serial.print("room_lux: ");
    //    Serial.print(room_lux, 1);
    //    Serial.print(" %");
    //    Serial.print("\tDETECT: ");
    //    if (detect == 1) {
    //      Serial.print("used");
    //    } else if (detect == 0) {
    //      Serial.print("not used");
    //    }
    //    Serial.println();
  }
}

void value_change_check() {
  //  if (room_t != room_t_previous_value || room_h != room_h_previous_value || room_hic != room_hic_previous_value || room_index != room_index_previous_value || room_lux != room_lux_previous_value ||
  //      pad_t != pad_t_previous_value || pad_h != pad_h_previous_value || pad_hic != pad_hic_previous_value || pad_index != pad_index_previous_value) {
  //    Serial.print("20");
  //    Serial.print(year);
  //    Serial.print(".");
  //    Serial.print(month);
  //    Serial.print(".");
  //    Serial.print(date);
  //    Serial.print(". ");
  //    Serial.print(hours);
  //    Serial.print("_");
  //    Serial.print(minutes);
  //    Serial.print("_");
  //    Serial.print(seconds);
  //    Serial.print("_\trTemp : ");
  //    Serial.print(room_t);
  //    Serial.print("\trHumi : ");
  //    Serial.print(room_h);
  //    Serial.print("\trHic : ");
  //    Serial.print(room_hic);
  //    Serial.print("\trIndex : ");
  //    Serial.print(room_index);
  //    Serial.print("\trLux : ");
  //    Serial.print(room_lux);
  //    Serial.print("\tpTemp : ");
  //    Serial.print(pad_t);
  //    Serial.print("\tpHumi : ");
  //    Serial.print(pad_h);
  //    Serial.print("\tpHic : ");
  //    Serial.print(pad_hic);
  //    Serial.print("\tpIndex : ");
  //    Serial.println(pad_index);
  //  }








  if (room_t != room_t_previous_value) {
    if (room_t > room_t_previous_value) {
      roomtemp = 1;
    } else if (room_t < room_t_previous_value) {
      roomtemp = 2;
    }
    room_t_previous_value = room_t;
  } else if (room_t == room_t_previous_value) {
    roomtemp = 0;
  }
  if (room_h != room_h_previous_value) {
    if (room_h > room_h_previous_value) {
      roomhumi = 1;
    } else if (room_h < room_h_previous_value) {
      roomhumi = 2;
    }
    room_h_previous_value = room_h;
  } else if (room_h == room_h_previous_value) {
    roomhumi = 0;
  }
  if (room_hic != room_hic_previous_value) {
    if (room_hic > room_hic_previous_value) {
      roomhic = 1;
    } else if (room_hic < room_hic_previous_value) {
      roomhic = 2;
    }
    room_hic_previous_value = room_hic;
  } else if (room_hic == room_hic_previous_value) {
    roomhic = 0;
  }
  if (room_index != room_index_previous_value) {
    if (room_index > room_index_previous_value) {
      roomindex = 1;
    } else if (room_index < room_index_previous_value) {
      roomindex = 2;
    }
    room_index_previous_value = room_index;
  } else if (room_index == room_index_previous_value) {
    roomindex = 0;
  }
  if (room_lux != room_lux_previous_value) {
    if (room_lux > room_lux_previous_value) {
      roomlux = 1;
    } else if (room_lux < room_lux_previous_value) {
      roomlux = 2;
    }
    room_lux_previous_value = room_lux;
  } else if (room_lux == room_lux_previous_value) {
    roomlux = 0;
  }








  if (outdoor_t != outdoor_t_previous_value) {
    if (outdoor_t > outdoor_t_previous_value) {
      outdoortemp = 1;
    } else if (outdoor_t < outdoor_t_previous_value) {
      outdoortemp = 2;
    }
    outdoor_t_previous_value = outdoor_t;
  } else if (outdoor_t == outdoor_t_previous_value) {
    outdoortemp = 0;
  }
  if (outdoor_h != outdoor_h_previous_value) {
    if (outdoor_h > outdoor_h_previous_value) {
      outdoorhumi = 1;
    } else if (outdoor_h < outdoor_h_previous_value) {
      outdoorhumi = 2;
    }
    outdoor_h_previous_value = outdoor_h;
  } else if (outdoor_h == outdoor_h_previous_value) {
    outdoorhumi = 0;
  }
  if (outdoor_hic != outdoor_hic_previous_value) {
    if (outdoor_hic > outdoor_hic_previous_value) {
      outdoorhic = 1;
    } else if (outdoor_hic < outdoor_hic_previous_value) {
      outdoorhic = 2;
    }
    outdoor_hic_previous_value = outdoor_hic;
  } else if (outdoor_hic == outdoor_hic_previous_value) {
    outdoorhic = 0;
  }
  if (outdoor_index != outdoor_index_previous_value) {
    if (outdoor_index > outdoor_index_previous_value) {
      outdoorindex = 1;
    } else if (outdoor_index < outdoor_index_previous_value) {
      outdoorindex = 2;
    }
    outdoor_index_previous_value = outdoor_index;
  } else if (outdoor_index == outdoor_index_previous_value) {
    outdoorindex = 0;
  }








  if (pad_t != pad_t_previous_value) {
    if (pad_t > pad_t_previous_value) {
      padtemp = 1;
    } else if (pad_t < pad_t_previous_value) {
      padtemp = 2;
    }
    pad_t_previous_value = pad_t;
  } else if (pad_t == pad_t_previous_value) {
    padtemp = 0;
  }
  if (pad_h != pad_h_previous_value) {
    if (pad_h > pad_h_previous_value) {
      padhumi = 1;
    } else if (pad_h < pad_h_previous_value) {
      padhumi = 2;
    }
    pad_h_previous_value = pad_h;
  } else if (pad_h == pad_h_previous_value) {
    padhumi = 0;
  }
  if (pad_hic != pad_hic_previous_value) {
    if (pad_hic > pad_hic_previous_value) {
      padhic = 1;
    } else if (pad_hic < pad_hic_previous_value) {
      padhic = 2;
    }
    pad_hic_previous_value = pad_hic;
  } else if (pad_hic == pad_hic_previous_value) {
    padhic = 0;
  }
  if (pad_index != pad_index_previous_value) {
    if (pad_index > pad_index_previous_value) {
      padindex = 1;
    } else if (pad_index < pad_index_previous_value) {
      padindex = 2;
    }
    pad_index_previous_value = pad_index;
  } else if (pad_index == pad_index_previous_value) {
    padindex = 0;
  }

  if (digitalRead(detect1) == 1 && digitalRead(detect2) == 1 && digitalRead(detect3) == 1) {
    detect = 1;
  } else if (digitalRead(detect1) == 0 || digitalRead(detect2) == 0 || digitalRead(detect3) == 0) {
    detect = 0;
  }
}

void set3231Date() {
  year    = (byte) ((Serial.read() - 48) * 10 +  (Serial.read() - 48));
  month   = (byte) ((Serial.read() - 48) * 10 +  (Serial.read() - 48));
  date    = (byte) ((Serial.read() - 48) * 10 +  (Serial.read() - 48));
  hours   = (byte) ((Serial.read() - 48) * 10 +  (Serial.read() - 48));
  minutes = (byte) ((Serial.read() - 48) * 10 +  (Serial.read() - 48));
  seconds = (byte) ((Serial.read() - 48) * 10 + (Serial.read() - 48));
  day     = (byte) (Serial.read() - 48);

  Wire.beginTransmission(104);
  Wire.write(0x00);
  Wire.write((seconds / 10 * 16) + (seconds % 10));
  Wire.write((minutes / 10 * 16) + (minutes % 10));
  Wire.write((hours / 10 * 16) + (hours % 10));
  Wire.write((day / 10 * 16) + (day % 10));
  Wire.write((date / 10 * 16) + (date % 10));
  Wire.write((month / 10 * 16) + (month % 10));
  Wire.write((year / 10 * 16) + (year % 10));
  Wire.endTransmission();
}

void get3231Date() {
  Wire.beginTransmission(104);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(104, 7);

  if (Wire.available()) {
    seconds = Wire.read();
    minutes = Wire.read();
    hours   = Wire.read();
    day     = Wire.read();
    date    = Wire.read();
    month   = Wire.read();
    year    = Wire.read();

    seconds = (((seconds & B11110000) >> 4) * 10 + (seconds & B00001111));
    minutes = (((minutes & B11110000) >> 4) * 10 + (minutes & B00001111));
    hours   = (((hours & B00110000) >> 4) * 10 + (hours & B00001111));
    day     = (day & B00000111);
    date    = (((date & B00110000) >> 4) * 10 + (date & B00001111));
    month   = (((month & B00010000) >> 4) * 10 + (month & B00001111));
    year    = (((year & B11110000) >> 4) * 10 + (year & B00001111));
  }

  //  switch (day) {
  //    case 1:
  //      strcpy(weekDay, "Sun");
  //      break;
  //    case 2:
  //      strcpy(weekDay, "Mon");
  //      break;
  //    case 3:
  //      strcpy(weekDay, "Tue");
  //      break;
  //    case 4:
  //      strcpy(weekDay, "Wed");
  //      break;
  //    case 5:
  //      strcpy(weekDay, "Thu");
  //      break;
  //    case 6:
  //      strcpy(weekDay, "Fri");
  //      break;
  //    case 7:
  //      strcpy(weekDay, "Sat");
  //      break;
  //  }
}

void window_control() {
  window_v = ((analogRead(wla) * 5.0) / 1024.0) / (7500.0 / 37500.0);

  if (window_v <= 9 && EEPROM.read(7) == 0 && EEPROM.read(10) >= 1) { // 부하 정지
    if (millis() - overloaddetectdelay >= 50) {
      EEPROM.update(7, 1);
      f_overload = 0;
    }
  } else {
    overloaddetectdelay = millis();
  }

  switch (EEPROM.read(6)) {
    case 0: // AUTO
      if (day == 1 || day == 7) {
      } else if (day >= 2 && day <= 6) {
      }
      break;
    case 1: // 닫기
      window_close();
      break;
    case 2: // 1 위치로
      window_open1();
      break;
    case 3: // 2 위치로
      window_open2();
      break;
    case 4: // 3 위치로
      window_open3();
      break;
    case 5: // 4 위치로
      window_open();
      break;
  }
}

void w_open() {
  digitalWrite(windowclose, 1);
  digitalWrite(windowopen, 0);
  EEPROM.update(10, 1);
}

void w_close() {
  digitalWrite(windowclose, 0);
  digitalWrite(windowopen, 1);
  EEPROM.update(10, 2);
}

void w_stop() {
  digitalWrite(windowclose, 1);
  digitalWrite(windowopen, 1);
  EEPROM.update(8, 0);
  EEPROM.update(10, 0);
}

void window_close() {
  if (EEPROM.read(8) == 1) {
    if (EEPROM.read(7) == 1) {
      w_open();
      if (digitalRead(window0) == 0) {
        w_stop();
        EEPROM.update(7, 0);
      } else if (digitalRead(window1) == 0 || digitalRead(window2) == 0 || digitalRead(window3) == 0 || digitalRead(window4) == 0) {
        w_close();
        EEPROM.update(7, 0);
      }
    } else if (digitalRead(window0) == 1) {
      w_close();
    } else if (digitalRead(window0) == 0) {
      w_stop();
    }
  }
}

void window_open1() {
  if (EEPROM.read(8) == 1) {
    if (EEPROM.read(7) == 1) {
      w_close();
      if (digitalRead(window1) == 0) {
        w_stop();
        EEPROM.update(7, 0);
      } else if (digitalRead(window0) == 0) {
        w_open();
        EEPROM.update(7, 0);
      }
    } else if (digitalRead(window1) == 1) {
      w_open();
    } else if (digitalRead(window1) == 0) {
      w_stop();
    }
  }
}

void window_open2() {
  if (EEPROM.read(8) == 1) {
    if (EEPROM.read(7) == 1) {
      w_close();
      if (digitalRead(window2) == 0) {
        w_stop();
        EEPROM.update(7, 0);
      } else if (digitalRead(window1) == 0 || digitalRead(window0) == 0) {
        w_open();
        EEPROM.update(7, 0);
      }
    } else if (digitalRead(window2) == 1) {
      w_open();
    } else if (digitalRead(window2) == 0) {
      w_stop();
    }
  }
}

void window_open3() {
  if (EEPROM.read(8) == 1) {
    if (EEPROM.read(7) == 1) {
      w_close();
      if (digitalRead(window3) == 0) {
        w_stop();
        EEPROM.update(7, 0);
      } else if (digitalRead(window2) == 0 || digitalRead(window1) == 0 || digitalRead(window0) == 0) {
        w_open();
        EEPROM.update(7, 0);
      }
    } else if (digitalRead(window3) == 1) {
      w_open();
    } else if (digitalRead(window3) == 0) {
      w_stop();
    }
  }
}

void window_open() {
  if (EEPROM.read(8) == 1) {
    if (EEPROM.read(7) == 1) {
      w_close();
      if (digitalRead(window4) == 0) {
        w_stop();
        EEPROM.update(7, 0);
      } else if (digitalRead(window3) == 0 || digitalRead(window2) == 0 || digitalRead(window1) == 0 || digitalRead(window0) == 0) {
        w_open();
        EEPROM.update(7, 0);
      }
    } else if (digitalRead(window4) == 1) {
      w_open();
    } else if (digitalRead(window4) == 0) {
      w_stop();
    }
  }
}
