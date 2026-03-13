// Network.h
// COM3505 assignment network module declarations
//
// This module is responsible for:
//  - station-mode Wi-Fi connection management
//  - reconnect logic
//  - future Flask API synchronisation

#ifndef NETWORK_H
#define NETWORK_H

#include "Thing.h"

void setupNetwork(DeviceState&);
void updateNetwork(DeviceState&, unsigned long);
void printWifiStatus();

#endif
