void cloudUpdate() {
  Serial.println(tempInt);
  Serial.println(humInt);
  Serial.println(tempExt);
  Serial.println(humExt);
  ThingSpeak.setField(1, tempInt);
  ThingSpeak.setField(2, humInt);
  ThingSpeak.setField(3, tempExt);
  ThingSpeak.setField(4, humExt);
  ThingSpeak.setField(5, tempFloor);
  ThingSpeak.setField(6, tempCeiling);
  ThingSpeak.setField(7, illumination);
  ThingSpeak.setStatus("myStatus");
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}