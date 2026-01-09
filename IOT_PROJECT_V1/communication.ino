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
