void cloudUpdate() {
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  Serial.println(tempInt);
  Serial.println(humInt);
  Serial.println(tempExt);
  Serial.println(humExt);
  //ThingSpeak.setField(1, 1);
  //ThingSpeak.setField(2, humInt);
  //ThingSpeak.setField(3, tempExt);
  //ThingSpeak.setField(4, humExt);
  //ThingSpeak.setStatus("myStatus");
  //int x = ThingSpeak.writeFields(channelNumber, writeAPIKey);
  int x = ThingSpeak.writeField(myChannelNumber, 1, 1, myWriteAPIKey);
  Serial.println("Test2");
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}