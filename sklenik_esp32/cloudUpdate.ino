void cloudUpdate() {
  String status = "";

  ThingSpeak.setField(1, tempInt);
  ThingSpeak.setField(2, humInt);
  ThingSpeak.setField(3, tempExt);
  ThingSpeak.setField(4, humExt);
  ThingSpeak.setField(5, tempFloor);
  ThingSpeak.setField(6, tempCeiling);
  ThingSpeak.setField(7, illum);
  if (heatOut && circulOut)
    status = "Je aktivní vytápění i cirkulace.";
  else if (heatOut)
    status = "Je aktivní vytápění.";
  else if (circulOut)
    status = "Je aktivní cirkulace.";
  ThingSpeak.setStatus(status);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}