#include "Cloud.h"

Cloud::Cloud() {
  // ThingSpeak channel configuration
  //channelNumber = preferences.getULong("channelNumber", 1903776);
  //preferences.getString("writeAPIKey", writeAPIKey, sizeof(writeAPIKey));
  //ThingSpeak.begin(client); // initialize ThingSpeak
}

void Cloud::update() {
  int status;
/*
  ThingSpeak.setField(1, tempInt);
  ThingSpeak.setField(2, humInt);
  ThingSpeak.setField(3, tempExt);
  ThingSpeak.setField(4, humExt);
  ThingSpeak.setField(5, tempFloor);
  ThingSpeak.setField(6, tempCeiling);
  ThingSpeak.setField(7, illum);
  if (heatOut && circulOut)
    ThingSpeak.setStatus("Je aktivní vytápění i cirkulace.");
  else if (heatOut)
    ThingSpeak.setStatus("Je aktivní vytápění.");
  else if (circulOut)
    ThingSpeak.setStatus("Je aktivní cirkulace.");*/
  status = ThingSpeak.writeFields(channelNumber, writeAPIKey);
  if (status == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.print("Problem updating channel. HTTP error code: ");
    Serial.println(status);
  }
  if (status != cloudLastUpdateStatus) {
    if (status == 200) {
      //imgReader.drawBMP((char*)"/cloud.bmp", tft, STAT_X, STAT_Y4);
    } else {
      //imgReader.drawBMP((char*)"/ycloud.bmp", tft, STAT_X, STAT_Y4);
    }
  }
}