#include "Settings.h"
#include "TimeSyncNTP.h"

time_t TimeSyncNTP::getNtpTime()
{
    while (udp.parsePacket() > 0)
        ; // discard any previously received packets
    log_i("Transmit NTP Request");
    sendNTPpacket(timeServer);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500)
    {
        int size = udp.parsePacket();
        if (size >= NTP_PACKET_SIZE)
        {
            log_i("Receive NTP Response");
            udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
            unsigned long secsSince1900;
            // convert four bytes starting at location 40 to a long integer
            secsSince1900 = (unsigned long)packetBuffer[40] << 24;
            secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
            secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
            secsSince1900 |= (unsigned long)packetBuffer[43];
            return secsSince1900 - 2208988800UL + (timeZone + summerTime) * SECS_PER_HOUR;
        }
    }
    log_e("No NTP Response");
    return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void TimeSyncNTP::sendNTPpacket(const char *host)
{
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum, or type of clock
    packetBuffer[2] = 6;          // Polling Interval
    packetBuffer[3] = 0xEC;       // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(host, 123); // NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

TimeSyncNTP::TimeSyncNTP()
{
}

void TimeSyncNTP::begin(Preferences *prefs, DS3232RTC *myRTC)
{
    this->myRTC = myRTC;

    timeZone = prefs->getShort("timeZone", 1);
    summerTime = prefs->getBool("summerTime", 0);
    timeServer = prefs->getString("timeServer", "cz.pool.ntp.org").c_str();

    udp.begin(LOCAL_PORT);
    lastUpdate = millis() - DELAY_SYNC;
    update();
}

bool TimeSyncNTP::update()
{
    if (myRTC->oscStopped() || millis() - lastUpdate >= DELAY_SYNC)
    {
        sendNTPpacket(timeServer); // send an NTP packet to a time server
        if (myRTC->set(getNtpTime()) == 0)
            log_i("Successful synchronization of RTC time from NTP");
        else
            log_e("Error synchronizing RTC time from NTP");
        lastUpdate = millis();
        return true;
    }
    else
        return false;
}