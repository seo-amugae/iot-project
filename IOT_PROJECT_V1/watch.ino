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
