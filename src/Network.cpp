#include "Settings.h"
#include "Network.h"

Network::Network()
{
}

void Network::begin(Preferences *prefs)
{
    dhcp = prefs->getBool("dhcp", 1);
    IPAddress ip(
        prefs->getUChar("ip1", 192),
        prefs->getUChar("ip2", 168),
        prefs->getUChar("ip3", 2),
        prefs->getUChar("ip4", 177));
    IPAddress dns(
        prefs->getUChar("dns1", 192),
        prefs->getUChar("dns2", 168),
        prefs->getUChar("dns3", 2),
        prefs->getUChar("dns4", 1));

    pinMode(VSPI_SS, OUTPUT); // VSPI SS Ethernet
    Ethernet.init(VSPI_SS);
    if (dhcp)
    {
        if (Ethernet.begin(mac) != 0)
        {
            log_i("DHCP assigned IP");
        }
        else
        {
            log_i("Failed to configure Ethernet using DHCP");
            Ethernet.begin(mac, ip, dns);
        }
    }
    else
    {
        Ethernet.begin(mac, ip, dns);
    }
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
        log_e("Ethernet module was not found");
    }
    else if (Ethernet.linkStatus() == LinkOFF)
    {
        log_i("Ethernet cable is not connected");
    }
    else
    {
        log_i("IP address: %s", Ethernet.localIP().toString());
    }

    // give the Ethernet module a second to initialize:
    delay(1000);
}
