#include "Settings.h"
#include "Cloud.h"

Cloud::Cloud() : client(base_client, TAs, (size_t)TAs_NUM, SSL_RAND)
{
}

void Cloud::begin(Preferences *prefs)
{
  channelNumber = prefs->getULong("channelNumber", 1903776);
  prefs->getString("writeAPIKey", writeAPIKey, sizeof(writeAPIKey));

  ThingSpeak.begin(client);
}

byte Cloud::update(byte *status, float tempInt, float tempExt, float tempFloor, float tempCeiling,
                   float humInt, float humExt, float illum, bool heating, bool circulating)
{
  byte actuatorsState;

  if (millis() - lastUpdate >= DELAY_UPDATE)
  {
    lastUpdate = millis();

    if (Ethernet.linkStatus() == LinkON)
    {
      if (heating && circulating)
      {
        ThingSpeak.setStatus("Vytápění a cirkulace");
        actuatorsState = 3;
      }
      else if (circulating)
      {
        ThingSpeak.setStatus("Cirkulace");
        actuatorsState = 2;
      }
      else if (heating)
      {
        ThingSpeak.setStatus("Vytápění");
        actuatorsState = 1;
      }
      else
        actuatorsState = 0;

      ThingSpeak.setField(1, tempInt);
      ThingSpeak.setField(2, humInt);
      ThingSpeak.setField(3, tempExt);
      ThingSpeak.setField(4, humExt);
      ThingSpeak.setField(5, tempFloor);
      ThingSpeak.setField(6, tempCeiling);
      ThingSpeak.setField(7, illum);
      ThingSpeak.setField(8, actuatorsState);

      *status = ThingSpeak.writeFields(channelNumber, writeAPIKey);
      if (*status == 200)
      {
        log_i("Channel update successful.");
      }
      else
      {
        log_e("Problem updating channel. HTTP error code: %d", *status);
      }
    }
    else
    {
      *status = 0;
      log_w("Unable to update cloud, eth status: %d", Ethernet.linkStatus());
    }
    return true;
  }
  return false;
}