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
