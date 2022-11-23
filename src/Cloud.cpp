#include "Settings.h"
#include "Cloud.h"

Cloud::Cloud(Preferences *prefs)
{
  prefs->begin("greenhouse", true);
  channelNumber = prefs->getULong("channelNumber", 1903776);
  prefs->getString("writeAPIKey", writeAPIKey, sizeof(writeAPIKey));
  prefs->end();
  ThingSpeak.begin(client); // initialize ThingSpeak
}

byte Cloud::update(byte *status, float tempInt, float tempExt, float tempFloor, float tempCeiling,
                   float humInt, float humExt, float illum, bool heating, bool circulating)
{
  if (millis() - lastUpdate < DELAY_UPDATE)
    return false;

  lastUpdate = millis();

  ThingSpeak.setField(1, tempInt);
  ThingSpeak.setField(2, humInt);
  ThingSpeak.setField(3, tempExt);
  ThingSpeak.setField(4, humExt);
  ThingSpeak.setField(5, tempFloor);
  ThingSpeak.setField(6, tempCeiling);
  ThingSpeak.setField(7, illum);
  if (heating && circulating)
    ThingSpeak.setStatus("Je aktivní vytápění i cirkulace.");
  else if (heating)
    ThingSpeak.setStatus("Je aktivní vytápění.");
  else if (circulating)
    ThingSpeak.setStatus("Je aktivní cirkulace.");
  *status = ThingSpeak.writeFields(channelNumber, writeAPIKey);
  if (*status == 200)
  {
    log_i("Channel update successful.");
  }
  else
  {
    log_e("Problem updating channel. HTTP error code: %d", *status);
  }
  return true;
}