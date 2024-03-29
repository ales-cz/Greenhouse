#ifndef MY_TIME_SYNC_NTP_H
#define MY_TIME_SYNC_NTP_H
#include <Arduino.h>
#include <Preferences.h>
#include <TimeLib.h>
#include <EthernetUdp.h>

#define LOCAL_PORT 8888     // local port to listen for UDP packets
#define NTP_PACKET_SIZE 48  // NTP time stamp is in the first 48 bytes of the message
#define DELAY_SYNC 86400000 // Interval of NTP update 24 h

class TimeSyncNTP
{

private:
    Preferences *prefs;
    DS3232RTC *myRTC;
    EthernetUDP udp;
    String timeServer;
    byte timeZone;
    byte timeDLS;
    byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
    int lastLinkStatus = Unknown;
    u_long lastUpdate;

    void sendNTPpacket();
    time_t getNtpTime();

public:
    TimeSyncNTP();
    void begin(Preferences *prefs, DS3232RTC *myRTC);
    bool update(bool force = false);
    byte getTimeZone();
    void setTimeZone(byte tz);
    byte getTimeDLS();
    void setTimeDLS(byte tDLS);
};
#endif