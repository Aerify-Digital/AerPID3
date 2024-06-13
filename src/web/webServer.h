/* =============================================================================
 * Copyright (c) 2015-2024 AerifyDigital LLC.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * =============================================================================
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>

#include "SocketCmdOp.h"
#include "common/enum/serial_commands.h"
#include "common/datatypes/mutex.h"

#include "serial/messagePack.h"

#include "storage/networkStor.h"
#include "storage/wifiStor.h"
#include "core/controllers/AerManager.h"
#include "core/controllers/AerPID.h"

#include "util/uuid.h"
#include "util/splitter.h"
#include "util/thermals.hpp"

#include "page_404.h"

// ======================
// ======================
// HTTP Port Address
#define HTTP_ADDRESS 80

// Setup flags
static bool sendNetworks;
static bool wifiStarting;

// HTTP Web Server
class WebServer final
{
public:
    WebServer(boolean verbose_debug, AerManager *am)
    {
        verbose_d = verbose_debug;
        this->_am = am;

        local_IP = "";
        sendNetworks = false;
        wifiStarting = false;
    };

    // Setup Web
    bool setup();

    bool isRunning() { return srvRunning; }

    // Scan radio network for nearby SSID
    bool scanNetworks(bool restart, bool checkForRunning = true);

    char *getHost();

    // Web Update Function Tick
    void tick();

    // Local device IP Address
    String local_IP;

    // Get web socket object
    AsyncWebSocket *getWebSocket();

protected:
private:
    // Verbose debug flag
    boolean verbose_d;

    // AerManager instance pointer
    AerManager *_am;

    bool srvRunning = false;
    bool SpiffsLoaded = true;

    // Wifi AP vars
    const char *sap_ssid = "AerTiny-Setup";
    const char *sap_pass = "AerTiny123";

    int wificnt = 0;

    // tick counter
    int _tick = 20;

    // tick wifi network operations
    uint tickNetworkWifiScan();
    void tickNetworkWifiConnect();

    // update connect websocket clients
    void updateClients();

    // scan wifi networks
    bool scanNetworks(bool restart, String macShrt);

    // Web template processors
    static String processor(const String &var);
    static String process_temp_a();
    static String process_temp_b();
    static String process_temp_e();
    static String process_temp_s();

    // Nonce for simple get challenge from web
    static const uint8_t NONCE_MAX = 32;
    String NONCE_A[NONCE_MAX];
    uint8_t NONCE_current;
    // Nonce Functions..
    String gen_nonce();
    void gen_nonce_a(uint8_t ind);
    void reset_nonce(uint8_t index);
    bool match_nonce(uint8_t index, String nonce);
    bool match_nonce(String nonce);

    // local ssids list
    static std::vector<String> _local_ssids;

    static void enPackFill(MessagePack *messagePack);

    // Web Sockets
    void initWebSocket();
    static void _onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    static void handleSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client);
    static void processSocketData(char *data, size_t len, AsyncWebSocketClient *client);

    static void sendInitMessage(uint32_t client);

    static char *configureFavTempName(uint8_t index, char *data);
    static void configureFavTemp(uint8_t index, double temp);
};

extern WebServer webServer;

#endif
