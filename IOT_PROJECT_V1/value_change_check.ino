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
