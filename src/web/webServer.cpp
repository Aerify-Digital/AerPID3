#include "webServer.h"

// initialize web server and socket
AsyncWebServer server(HTTP_ADDRESS);
AsyncWebSocket ws("/ws");

// web socket accessor
AsyncWebSocket *WebServer::getWebSocket() { return &ws; };

// local ssid listing
std::vector<String> WebServer::_local_ssids = {};

/**
 * @brief Ticks network wifi connection process
 *
 */
void WebServer::tickNetworkWifiConnect()
{
    if (wifiStarting)
    {
        // Serial.print("[WiFi]");

        if (WiFi.status() != WL_CONNECTED)
        {
            // delay(600);
            if ((wificnt % 10) == 0)
            {
                Serial.println(F("[WiFi] -> Joining..."));
            }
            wificnt++;
            if (wificnt > 500)
            {
                ws.textAll("wifi_failed");
                Serial.println(F("[WiFi] -> Connection Failed!"));
                wificnt = 0;
                wifiStarting = false;
            }
        }
        else
        {
            wificnt = 0;
            ws.textAll("wifi_connected.");
            Serial.println(F("[WiFi] -> WiFi Connected!"));
            Serial.print(F("[WiFi] -> WiFi IP: "));
            Serial.println(WiFi.localIP());

            Serial.println(F("[WiFi] -> Saving Network!"));
            commstor.setSSIDSet(true);
            commstor.saveWiFi();
            Serial.println(F("[WiFi] -> Network Saved!"));
            wifiStarting = false;
            // WiFi.softAPdisconnect (true);
            //  TODO: Try setting wifi mode to WIFI_MODE_STA instead of rebooting.
            ESP.restart();
        }
    }

    /*
        if (sendNetworks)
        {
            String json = "json:[";
            int n = WiFi.scanComplete();

            if (n == -1)
            {
                Serial.println("[WiFi] >> Scan in progress...");
            }
            if (n == 0)
            {
                Serial.println("[WiFi] >> No networks found...");
            }
            if (n > 0)
            {
                Serial.print(n);
                Serial.println(" networks found");
                for (int i = 0; i < n; ++i)
                {
                    // Print SSID and RSSI for each network found
                    if (i)
                        json += ", ";
                    json += " {";
                    json += "\"rssi\":" + String(WiFi.RSSI(i));
                    json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
                    json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
                    json += ",\"channel\":" + String(WiFi.channel(i));
                    json += ",\"secure\":" + String(WiFi.encryptionType(i));
                    //                json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
                    json += "}";
                    if (i == (n - 1))
                    {
                        json += "]";
                    }
                }
                // delay(10);
                Serial.println(json);
                ws.textAll(json);
                sendNetworks = false;
            }
        }
    */
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Ticks web server processes
 *
 */
void WebServer::tick()
{
    // Serial.println(">>WebServer>> Tick: " + String(_tick));
    if (!isRunning())
    {
        // Serial.println(">>Webserver>> is not running, return.");
        return;
    }

    if (tickNetworkWifiScan())
    {
        // Serial.println(">>Webserver>> ticknetworkwifiscan, return.");
        return;
    }

    tickNetworkWifiConnect();

    if (WiFi.status() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1)
    {
        // Serial.println(">>Webserver>> status says not connected, return.");
        //  do not send --> ERROR: Too many messages queued
        aerManager.setActiveWifi(false);
        return;
    }

    ws.cleanupClients();

    if (!ws.availableForWriteAll())
    {
        // Serial.println(">>Webserver>> not available for writeall, return.");
        aerManager.setActiveWifi(false);
        return;
    }

    // Tick process
    if (_tick == 0)
    {
        updateClients();
        aerManager.setActiveWifi(true);
    }
    if (_tick == 3)
    {
        aerManager.setActiveWifi(false);
    }
    // Decrement tick counter
    if (_tick <= 0)
    {
        // Reset tick counter
        _tick = 10;
    }
    _tick--;
}

/// @brief ticks wifi network scan operation
/// @return scan tick result code
uint WebServer::tickNetworkWifiScan()
{
    NetworkVars *net = aerManager.getNetVars();
    if (net->doscan)
    {
        if (_tick > 0)
        {
            _tick--;
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        if (!net->scanned)
        {
            if (_tick < 10)
            {
                _tick = 30;
                net->scanning = true;
                aerManager.getGUI()->getMenuProps()->menuUpdate = true;
                vTaskDelay(3200 / portTICK_PERIOD_MS);
                scanNetworks(true, false);
                vTaskDelay(1250 / portTICK_PERIOD_MS);
                net->scanning = false;
                aerManager.getGUI()->getMenuProps()->menuUpdate = true;
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                net->scanned = true;
                aerManager.getGUI()->getMenuProps()->menuUpdate = true;
            }
            return 2;
        }
        if (_tick <= 0)
        {
            net->doscan = false;
            aerManager.getGUI()->getMenuProps()->menuUpdate = true;
            vTaskDelay(1250 / portTICK_PERIOD_MS);
            aerManager.getGUI()->gotoMenu(MENU_MAIN_WIFI, MENU_WIFI_NETWORKS_NEARBY, 3);
            return 3;
        }
        return 1;
    }
    return 0;
}

bool WebServer::scanNetworks(bool restart, bool checkForRunning)
{
    if (checkForRunning && isRunning())
    {
        return false;
    }
    uint8_t mac[6];
    WiFi.macAddress(mac);
    String macShrt = String(mac[3], 16) + String(mac[4], 16) + String(mac[5], 16);
    // Scan network SSID's...
    return scanNetworks(restart, macShrt);
}

bool WebServer::scanNetworks(bool restart, String macShrt)
{
    if (_local_ssids.size() > 0)
    {
        _local_ssids.clear();
        _local_ssids.shrink_to_fit();
    }

    NetworkVars *net = aerManager.getNetVars();
    if (net->scanResults.size() > 0)
    {
        net->scanResults.clear();
        net->scanResults.shrink_to_fit();
    }

    vTaskDelay(300 / portTICK_PERIOD_MS);
    // change to station mode and disconnect..
    WiFi.mode(WIFI_STA);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    WiFi.disconnect();
    vTaskDelay(500);

    int n = WiFi.scanNetworks();
    while (n == -1)
    {
        n = WiFi.scanComplete();
        vTaskDelay(500);
    }

    vTaskDelay(500);
    if (n != 0)
    {
        for (int i = 0; i < n; i++)
        {
            String ssid = WiFi.SSID(i);
            _local_ssids.push_back(ssid);
            net->scanResults.push_back(ssid);
        }
    }
    WiFi.scanDelete();

    // change back to access point mode...
    WiFi.mode(WIFI_MODE_APSTA);

    if (restart)
    {
        vTaskDelay(200);
        const char *_sap_ssid = (String(sap_ssid) + "-" + macShrt).c_str();
        // restart the AP
        return WiFi.softAP(_sap_ssid, sap_pass);
    }
    else
    {
        return true;
    }
}

/// @brief Get Hostname
/// @return net hostname
char *WebServer::getHost()
{
    return aerManager.getNet()->getHostname();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/**
 * @brief Setup the async web handler
 *
 * @return boolean success
 */
boolean WebServer::setup()
{
    Serial.println(F(" "));
    Serial.println(F("==============================="));
    Serial.println(F("Setting up WiFi web handler.."));

    // Initialize SPIFFS
    if (!SPIFFS.begin(/*true*/))
    {
        SpiffsLoaded = false;
        Serial.println(F("An Error has occurred while mounting SPIFFS!"));
        Serial.println(F(" "));
        // return false;
    }

    // Did we Initialize SPIFFS?
    if (!SpiffsLoaded)
    {
        Serial.println(F("An Error occurred while mounting SPIFFS, can not start web."));
        Serial.println(F(" "));
        return false;
    }

    uint8_t mac[6];
    WiFi.macAddress(mac);                 // get mac address
    aerManager.getNetVars()->setMAC(mac); // set the mac
    // build the short mac
    String macShrt = String(mac[3], 16) + String(mac[4], 16) + String(mac[5], 16);

    char *hostname = aerManager.getNet()->getHostname();
    if (!networkingStorage.isHostnameSetup())
    {
        // get or load the hostname
        asprintf(&hostname, "%s-%02X%02X%02X", aerManager.getNetVars()->hostname, mac[3], mac[4], mac[5]);
        aerManager.getNet()->setHostname(hostname);
        Serial.print(F(">No host saved!  Setting up default hostname... "));
        Serial.println(hostname);
    }

    // Scan network SSID's...
    scanNetworks(false, macShrt);
    delay(500);

    if (MDNS.begin(hostname))
    {
        Serial.print(F("mDNS responder started: "));
        Serial.println(hostname);
    }

    // should be called before setting WiFi.mode
    WiFi.setHostname(hostname);
    // Configure WiFi settings
    WiFi.mode(WIFI_MODE_APSTA);

    // Connect to WiFi
    int cnt = 0;
    bool connected = false;
    if (commstor.getSSIDSet())
    { // SSID set; connecting to preconfigured network
        if (wifiStorage.isUsingStaticIP())
        {
            IPAddress localIP = *wifiStorage.getLocalIP();
            IPAddress gatewayIP = *wifiStorage.getGatewayIP();
            IPAddress subnet = *wifiStorage.getNetMask();
            IPAddress dns1 = *wifiStorage.getDNS1();
            IPAddress dns2 = *wifiStorage.getDNS2();
            WiFi.config(localIP, gatewayIP, subnet, dns1, dns2);
            Serial.println(F("[WiFi] -> Using static network..."));
        }
        WiFi.mode(WIFI_MODE_STA);
        Serial.println(F("[WiFi] -> Connecting to WiFi network..."));
        char *set_ssid = commstor.getSSID();
        WiFi.begin(set_ssid, commstor.getPSK());
        Serial.print("[WiFi]");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(F("."));
            cnt++;
            if (cnt > 20)
            {
                Serial.printf("STA: Failed!\n");
                break;
            }
        }
        Serial.println(F(" "));
        connected = WiFi.status() == WL_CONNECTED;
        if (connected)
        {
            Serial.println(F("[WiFi] -> WiFi Connected!"));
            Serial.print(F("[WiFi] -> WiFi IP: "));
            Serial.println(WiFi.localIP());

            local_IP = WiFi.localIP().toString();
            aerManager.getNetVars()->ip = WiFi.localIP();
            aerManager.getNetVars()->hostname = (char *)WiFi.getHostname();
            aerManager.getNetVars()->ssid = (char *)set_ssid;
        }
        else
        {
            Serial.println(F("[WiFi] -> WiFi Connect Failed!"));
            Serial.println(F("[WiFi] -> Falling back to AP Mode..."));
            WiFi.mode(WIFI_MODE_APSTA);
            WiFi.disconnect();
            delay(400);
        }
    }
    if (!connected)
    { // No SSID; starting access point
        Serial.println(F("[WiFi] -> Starting WiFi AP.."));
        const char *_sap_ssid = (String(sap_ssid) + "-" + macShrt).c_str();
        WiFi.softAPsetHostname(hostname);
        WiFi.mode(WIFI_MODE_APSTA);
        WiFi.softAP(_sap_ssid, sap_pass);
        delay(500);
        /*while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(F("."));
            cnt++;
            if (cnt > 30)
            {
                Serial.printf("APSTA: Failed!\n");
                WiFi.disconnect(false);
                return false;
            }
        }*/
        Serial.println(F(" "));
        connected = WiFi.status() == WL_CONNECTED;
        Serial.println(F("[WiFi] -> WiFi AP Started!"));
        Serial.print(F("[WiFi] -> WiFi AP IP: "));
        Serial.println(WiFi.softAPIP().toString()); // 192.168.4.1 << Default SoftAP IP

        local_IP = WiFi.softAPIP().toString();
        aerManager.getNetVars()->ip = WiFi.softAPIP();
        aerManager.getNetVars()->hostname = (char *)WiFi.softAPgetHostname();
        aerManager.getNetVars()->ssid = (char *)_sap_ssid;
    }

    srvRunning = true;
    // -----------------------
    Serial.println(F("Setting up Request Handlers.."));
    delay(300);

    // Setup websocket connections
    initWebSocket();

    // disable wifi sleeping
    WiFi.setSleep(false);
    delay(10);

    // ======================================
    // Routes for New Web-UI
    server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    server.on("/index", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    server.on("/home", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    server.on("/home.html", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });

    if (verbose_d)
    {
        Serial.println(F("Setup 'HTML Interface'"));
    }

    Serial.println("");

    // ======================================
    // Static routes for folders
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/favicon.ico", "image/x-icon"); }); // Route to load favicon.ico file
    server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/favicon.png", "image/png"); });
    if (verbose_d)
        Serial.println(F("Setup 'favicon' file routes"));

    server.serveStatic("/js", SPIFFS, "/js");   // Route to javascript folder
    server.serveStatic("/css", SPIFFS, "/css"); // Route to styles folder
    if (verbose_d)
        Serial.println(F("Setup '/js' and '/css' directory routes"));

    // File: fa-brands-400.ttf
    server.on("/webfonts/fa-brands-400.ttf", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wf/a9545819", "application/x-font-ttf"); });
    Serial.println("Setup SPIFFS File Mapping: '/webfonts/fa-brands-400.ttf' -> '/wf/a9545819'");
    // File: fa-brands-400.woff2
    server.on("/webfonts/fa-brands-400.woff2", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wf/84a1fc9a", "application/x-font-woff2"); });
    Serial.println("Setup SPIFFS File Mapping: '/webfonts/fa-brands-400.woff2' -> '/wf/84a1fc9a'");
    // File: fa-regular-400.ttf
    server.on("/webfonts/fa-regular-400.ttf", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wf/283fce66", "application/x-font-ttf"); });
    Serial.println("Setup SPIFFS File Mapping: '/webfonts/fa-regular-400.ttf' -> '/wf/283fce66'");
    // File: fa-regular-400.woff2
    server.on("/webfonts/fa-regular-400.woff2", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wf/356a2eb9", "application/x-font-woff2"); });
    Serial.println("Setup SPIFFS File Mapping: '/webfonts/fa-regular-400.woff2' -> '/wf/356a2eb9'");
    // File: fa-solid-900.ttf
    server.on("/webfonts/fa-solid-900.ttf", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wf/5f0a1e22", "application/x-font-ttf"); });
    Serial.println("Setup SPIFFS File Mapping: '/webfonts/fa-solid-900.ttf' -> '/wf/5f0a1e22'");
    // File: fa-solid-900.woff2
    server.on("/webfonts/fa-solid-900.woff2", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wf/27ac0159", "application/x-font-woff2"); });
    Serial.println("Setup SPIFFS File Mapping: '/webfonts/fa-solid-900.woff2' -> '/wf/27ac0159'");
    // File: fa-v4compatibility.ttf
    server.on("/webfonts/fa-v4compatibility.ttf", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wf/732ebf9a", "application/x-font-ttf"); });
    Serial.println("Setup SPIFFS File Mapping: '/webfonts/fa-v4compatibility.ttf' -> '/wf/732ebf9a'");
    // File: fa-v4compatibility.woff2
    server.on("/webfonts/fa-v4compatibility.woff2", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/wf/aa619ab3", "application/x-font-woff2"); });
    Serial.println("Setup SPIFFS File Mapping: '/webfonts/fa-v4compatibility.woff2' -> '/wf/aa619ab3'");

    // Heap Size
    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", String(ESP.getFreeHeap())); });

    // 404 page
    server.onNotFound([](AsyncWebServerRequest *request)
                      { request->send(404, "text/html", get404Page(request->url())); });

    // Functional Endpoints
    server.on("/scan_wifi", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
                int n = _local_ssids.size();
                if (n != 0) {
                    String json = String("");
                    json += "[";
                    for (int i = 0; i < n; i++ ) {
                        String ssid = _local_ssids.at(i);
                        json += "\"" + ssid + "\"";
                        if (i < n - 1) {
                            json += ",";
                        }
                    }
                    json += "]";
                    request->send(200, "text/json", json);
                } else {
                    String json = String("[]");
                    request->send(200, "text/json", json);
                } });

    Serial.println(F("> Setup All General and Special Request Events."));

    if (verbose_d)
        Serial.println(F("Server Setup Complete!"));

    delayMicroseconds(3000);
    // Start server
    server.begin();

    MDNS.setInstanceName("AerTiny PID-Suite");
    // Setup MDNS service
    MDNS.addService("_http", "_tcp", 80);
    MDNS.addServiceTxt("_http", "_tcp", "board", "ESP32");

    aerManager.setConnectWifi(true);

    Serial.println(F("Starting WebServer..."));
    Serial.println(F(" "));
    return true;
}

// ========================================================================
// Web Sockets
// ========================================================================

/**
 * @brief Setup web socket event listener
 *
 */
void WebServer::initWebSocket()
{
    Serial.println(F("Setting up WebSocket Events.."));
    ws.onEvent(_onEvent);
    server.addHandler(&ws);
}

void WebServer::_onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("[WebSocket] client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        client->keepAlivePeriod(60); // keepalive setting
        sendInitMessage(client->id());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("[WebSocket] client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleSocketMessage(arg, data, len, client);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    default:
        break;
    }
}

/**
 * @brief Packs init message data for web socket
 *
 * @param messagePack
 */
void WebServer::enPackFill(MessagePack *messagePack)
{
    NetworkVars *net = aerManager.getNetVars();
    messagePack->setIP(net->ip);
    messagePack->setHostname(net->hostname);
    messagePack->setSSID(net->ssid);

    AerPID *as = aerManager.getAerPID(0);
    messagePack->setUpTime(millis());
    messagePack->setBumpTemp(aerManager.getBump()->getTemp());
    messagePack->setBumpTime(aerManager.getBump()->getTime());
    messagePack->setAutoOffTime(as->AUTO_OFF_TIME);
    messagePack->setLedMode(lightstor.getMode());
    messagePack->setLedSatus(lightstor.statusEnabled());
    messagePack->setLedColor(lightstor.getRgbVal('r'), lightstor.getRgbVal('g'), lightstor.getRgbVal('b'));
    messagePack->setLedBrightness(lightstor.getBright());

    FavsStor *favs = aerManager.getFavs();
    messagePack->setFavName(1, favs->getName(1));
    messagePack->setFavTemp(1, favs->getTemp(1));
    messagePack->setFavName(2, favs->getName(2));
    messagePack->setFavTemp(2, favs->getTemp(2));
    messagePack->setFavName(3, favs->getName(3));
    messagePack->setFavTemp(3, favs->getTemp(3));
    messagePack->setFavName(4, favs->getName(4));
    messagePack->setFavTemp(4, favs->getTemp(4));

    messagePack->setTempAdjAmt(as->TEMP_ADJ_AMOUNT);
    messagePack->setTemp(static_cast<uint16_t>(10.0 * as->MES_TEMP));
    messagePack->setAvgTemp(static_cast<uint16_t>(10.0 * as->AVG_TEMP));
    messagePack->setSetTemp(static_cast<uint16_t>(10.0 * as->SET_TEMP));
    messagePack->setPID(as->kP, as->kI, as->kD);

    SettingsStorage *sets = aerManager.getSettings();
    messagePack->setUnitType(sets->getThermalUnit()); // temp unit type

    // auto_off_enb, coil_enb, lights_enb, unused, wifi_enb, unused, bt_enb, unused
    byte bitmap = 0b00000000;
    bitWrite(bitmap, 7, as->AUTO_OFF_ENB);
    bitWrite(bitmap, 6, as->PID_ON);
    bitWrite(bitmap, 5, aerManager.getLights()->isEnabled());
    bitWrite(bitmap, 4, aerManager.getBump()->getEnabled(0)); // get bump enabled status
    bitWrite(bitmap, 3, aerManager.getComms()->getWifiEn());  // get status of wifi
    bitWrite(bitmap, 2, aerManager.getComms()->getSSIDSet()); // get ssid setup
    bitWrite(bitmap, 1, aerManager.getComms()->getBTEn());    // get status of bluetooth
    bitWrite(bitmap, 0, 0);                                   // unused
    messagePack->setBoleanBitmap(bitmap);
#if AERPID_COUNT == 2
    messagePack->setModel(2);
#else
    messagePack->setModel(1);
#endif
}

/**
 * @brief Send init message to socket
 *
 * @param client
 */
void WebServer::sendInitMessage(uint32_t client)
{
    const xVersion v = aerManager.getVersion()->getVer();
    const xVersion vw = aerManager.getVersionWeb()->getVer();
    MessagePack messagePack = MessagePack(v.major, v.minor, v.build, vw.major, vw.minor, vw.build);
    enPackFill(&messagePack);

    uint len = messagePack.fetchInitMessage();
    if (len > 0 && messagePack.isPackComplete())
    {
        SocketCmdOp *cmd = new SocketCmdOp((char *)messagePack.getPack(), len);
        cmd->AddClient(client);
        cmd->build();
        cmd->emit(&ws);
        delete cmd;
    }
    messagePack.dispose();
}

void WebServer::updateClients()
{
    if (ws.count() == 0 || !ws.enabled())
    {
        // Serial.println("update clients returning early");
        return;
    }
    SocketCmdOp *cmd = new SocketCmdOp(SerialCommand::CMD_STATUS);
    cmd->Op(Operation::OP_GET);
    cmd->Val(xAerPID1.PID_ON);
    cmd->Val(static_cast<uint16_t>(10.0 * xAerPID1.MES_TEMP));
    cmd->Val(static_cast<uint16_t>(10.0 * xAerPID1.AVG_TEMP));
    cmd->Val(static_cast<uint16_t>(10.0 * xAerPID1.SET_TEMP));
    cmd->build();
    cmd->emitAll(&ws);
    delete cmd;

#if AERPID_COUNT == 2
    SocketCmdOp *cmd2 = new SocketCmdOp(SerialCommand::CMD_STATUS2);
    cmd2->Op(Operation::OP_GET);
    cmd2->Val(xAerPID2.PID_ON);
    cmd2->Val(static_cast<uint16_t>(10.0 * xAerPID2.MES_TEMP));
    cmd2->Val(static_cast<uint16_t>(10.0 * xAerPID2.AVG_TEMP));
    cmd2->Val(static_cast<uint16_t>(10.0 * xAerPID2.SET_TEMP));
    cmd2->build();
    cmd2->emitAll(&ws);
    delete cmd2;
#endif

    // TODO: Update clients with any changes made locally or via usb.

    if (aerManager.webUpdateDTR())
    {
        SocketCmdOp *cmd = new SocketCmdOp(SerialCommand::CMD_DEBUG);
        cmd->Op(Operation::OP_GET);
        cmd->Val(aerManager.isEnabledDTR());
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
        aerManager.webUpdateDTR(false);
    }
    if (aerManager.webUpdateBLE())
    {
        SocketCmdOp *cmd = new SocketCmdOp(SerialCommand::CMD_BLE);
        cmd->Op(Operation::OP_GET);
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
    }
    if (aerManager.webUpdateBump())
    {
        SocketCmdOp *cmd = new SocketCmdOp(SerialCommand::BUMP_UPDATE);
        cmd->Op(Operation::OP_GET);
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
    }
    if (aerManager.webUpdateFavs())
    {
        for (int i = 0; i < 4; ++i)
        {
            if (aerManager.getWebFnameVal(i) > 0)
            {
                Serial.println("Update F-Name# " + String(i + 1));
                SocketCmdOp *cmd = new SocketCmdOp();
                cmd->Op(Operation::OP_GET);
                switch (i)
                {
                case 0:
                    cmd->Cmd(SerialCommand::FAV_1);
                    cmd->Val(aerManager.getFavs()->getName(1));
                    break;
                case 1:
                    cmd->Cmd(SerialCommand::FAV_2);
                    cmd->Val(aerManager.getFavs()->getName(2));
                    break;
                case 2:
                    cmd->Cmd(SerialCommand::FAV_3);
                    cmd->Val(aerManager.getFavs()->getName(3));
                    break;
                case 3:
                    cmd->Cmd(SerialCommand::FAV_4);
                    cmd->Val(aerManager.getFavs()->getName(4));
                    break;
                default:
                    cmd->Cmd(SerialCommand::FAV_1);
                    cmd->Val(aerManager.getFavs()->getName(1));
                    break;
                }
                cmd->Param(Favorite::FAV_NAME);
                cmd->build();
                cmd->emitAll(&ws);
                delete cmd;
            }

            if (aerManager.getWebFtempVal(i) > 0)
            {
                Serial.println("Update F-Temp# " + String(i + 1));
                SocketCmdOp *cmd = new SocketCmdOp();
                cmd->Op(Operation::OP_GET);
                switch (i)
                {
                case 0:
                    cmd->Cmd(SerialCommand::FAV_1);
                    cmd->Val(static_cast<uint16_t>(10.0 * aerManager.getFavs()->getTemp(1)));
                    break;
                case 1:
                    cmd->Cmd(SerialCommand::FAV_2);
                    cmd->Val(static_cast<uint16_t>(10.0 * aerManager.getFavs()->getTemp(2)));
                    break;
                case 2:
                    cmd->Cmd(SerialCommand::FAV_3);
                    cmd->Val(static_cast<uint16_t>(10.0 * aerManager.getFavs()->getTemp(3)));
                    break;
                case 3:
                    cmd->Cmd(SerialCommand::FAV_4);
                    cmd->Val(static_cast<uint16_t>(10.0 * aerManager.getFavs()->getTemp(4)));
                    break;
                default:
                    cmd->Cmd(SerialCommand::FAV_1);
                    cmd->Val(static_cast<uint16_t>(10.0 * aerManager.getFavs()->getTemp(1)));
                    break;
                }
                cmd->Param(Favorite::FAV_TEMP);
                cmd->build();
                cmd->emitAll(&ws);
                delete cmd;
            }
        }
        aerManager.clearWebFNames();
        aerManager.clearWebFTemps();
        aerManager.webUpdateFavs(false);
    }
    if (aerManager.webUpdateLEDState())
    {
        SocketCmdOp *cmd = new SocketCmdOp();
        cmd->Op(Operation::OP_GET);
        cmd->Cmd(SerialCommand::CMD_LED);
        cmd->Param(LED::PARAM_ENABLE);
        // bool enb = lightstor.isEnabled();
        // uint8_t val = enb ? 0x01 : 0x00;
        cmd->Val(lightstor.isEnabled());
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
        aerManager.webUpdateLEDState(false);
    }
    if (aerManager.webUpdateLEDStatus())
    {
        SocketCmdOp *cmd = new SocketCmdOp();
        cmd->Op(Operation::OP_GET);
        cmd->Cmd(SerialCommand::CMD_LED);
        cmd->Param(LED::PARAM_STAT);
        cmd->Val(lightstor.statusEnabled());
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
        aerManager.webUpdateLEDStatus(false);
    }
    if (aerManager.webUpdateLEDMode())
    {
        SocketCmdOp *cmd = new SocketCmdOp();
        cmd->Op(Operation::OP_GET);
        cmd->Cmd(SerialCommand::CMD_LED);
        cmd->Param(LED::PARAM_MODE);
        cmd->Val(lightstor.getMode());
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
        aerManager.webUpdateLEDMode(false);
    }
    if (aerManager.webUpdateLEDBright())
    {
        SocketCmdOp *cmd = new SocketCmdOp();
        cmd->Op(Operation::OP_GET);
        cmd->Cmd(SerialCommand::CMD_LED);
        cmd->Param(LED::PARAM_BRIGHTNESS);
        cmd->Val(lightstor.getBright());
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
        aerManager.webUpdateLEDBright(false);
    }
    if (aerManager.webUpdateLEDColor())
    {
        SocketCmdOp *cmd = new SocketCmdOp();
        cmd->Op(Operation::OP_GET);
        cmd->Cmd(SerialCommand::CMD_LED);
        cmd->Param(LED::PARAM_COLOR);
        cmd->Val(lightstor.getRgbHex());
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
        aerManager.webUpdateLEDColor(false);
    }
    if (aerManager.webUpdatePID())
    {
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID);
        reply->Param(PARAM_PID::PARAM_PID);
        reply->Val(aerManager.getAerPID(0)->kP);
        reply->Val(aerManager.getAerPID(0)->kI);
        reply->Val(aerManager.getAerPID(0)->kD);
        reply->build();
        reply->emitAll(getWebSocket());
        delete reply;
        aerManager.webUpdatePID(false);
    }
    if (aerManager.webUpdateTemp())
    {
        SocketCmdOp *cmd = new SocketCmdOp();
        cmd->Op(Operation::OP_GET);
        cmd->Cmd(SerialCommand::CMD_TEMP);
        cmd->Val(static_cast<uint16_t>(10.0 * xAerPID1.SET_TEMP));
        cmd->build();
        cmd->emitAll(&ws);
        delete cmd;
        aerManager.webUpdateTemp(false);
    }
    if (aerManager.webUpdateUSB())
    {
        // code here
    }
    if (aerManager.webUpdateWIFI())
    {
        // code here
    }
}

void WebServer::handleSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_BINARY)
    {
        char *decoded = new char[1024];
        decodeData((char *)data, len, decoded);
        uint ln = 3; // what does this do???
        processSocketData(decoded, ln, client);
        delete[] decoded;
        return;
    }
    else if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        return;
    }
}

void WebServer::processSocketData(char *data, size_t len, AsyncWebSocketClient *client)
{
    SerialCommand cmd = (SerialCommand)data[0];

    Serial.printf("[WebSocket] client #%u sent cmd: %u\n", client->id(), cmd);
    union byteDouble
    {
        double value;
        char bytes[sizeof(double)];
    };

    switch (cmd)
    {
    case SerialCommand::CMD_INIT:
    {
        // SocketCmdOp op = new SocketCmdOp(CMD_INIT);
    }
    break;
    case SerialCommand::AUTO_OFF_LENGTH:
        break;
    case SerialCommand::AUTO_OFF_TOGGLE:
    {
        uint8_t op = data[1];
        uint8_t val;
        if (op == Operation::OP_GET)
        {
            bool enb = xAerPID1.AUTO_OFF_ENB;
            val = enb ? 0x01 : 0x00;
        }
        else
        {
            val = data[2];
            bool enb = val > 0 ? true : false;
            xAerPID1.AUTO_OFF_ENB = enb;
            aerManager.updateFlashStor(true);
            aerManager.setPressTick(250);
        }
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::AUTO_OFF_TOGGLE);
        reply->AddClient(client->id());
        reply->Val(val);
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
    case SerialCommand::ADJUST_AMOUNT:
        break;
    case SerialCommand::CMD_BLE:
        break;
    case SerialCommand::BUMP_AMOUNT:
    {
        double val;
        uint8_t op = data[1];
        if (op == Operation::OP_SET)
        {
            byteDouble bd;
            for (int i = 0; i < sizeof(double); i++)
            {
                bd.bytes[i] = data[2 + i];
            }
            if (aerManager.getReadingType() == ThermalUnitsType::FAHRENHEIT)
            { // convert to C
                Serial.printf(">> Bump-In(f): %f \n", bd.value);
                val = toCelsius(bd.value);
            }
            else if (aerManager.getReadingType() == ThermalUnitsType::KELVIN)
            { // convert to c
                Serial.printf(">> Bump-In(k): %f \n", bd.value);
                val = fromKelvin(bd.value);
            }
            else
            { // is c
                val = bd.value;
            }
            Serial.printf(">> Bump Amount(c): %f \n", val);
            // xAerPID1.BUMP_AMT = val;
            // aerManager.updateTempStor(true);
            // aerManager.setPressTick(250);
        }
        else
        {
            val = aerManager.getBump()->getTemp();
        }

        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_TEMP);
        reply->AddClient(client->id());
        reply->Op(op);
        reply->Val(val);
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
    case SerialCommand::BUMP_LENGTH:
    {
        int val;
        uint8_t op = data[1];
        if (op == Operation::OP_SET)
        {
            byteDouble bd;
            for (int i = 0; i < sizeof(double); i++)
            {
                bd.bytes[i] = data[2 + i];
            }
            val = int(bd.value);
            Serial.printf(">> Bump Length(s): %u \n", val);
            // xAerPID1.BUMP_TIME = val;
            // aerManager.updateTempStor(true);
            // aerManager.setPressTick(250);
        }
    }
    break;
    case SerialCommand::BUMP_TOGGLE:
    {
        uint8_t op = data[1];
        uint8_t val;
        if (op == Operation::OP_GET)
        {
            bool enb = aerManager.getBump()->getEnabled(0);
            val = enb ? 0x01 : 0x00;
        }
        else
        {
            val = data[2];
            bool enb = val > 0 ? true : false;
            aerManager.getBump()->setEnabled(0, enb);
        }
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::BUMP_TOGGLE);
        reply->AddClient(client->id());
        reply->Val(val);
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
    case SerialCommand::COIL_TOGGLE:
    {
        uint8_t op = data[1];
        uint8_t val;
        if (op == Operation::OP_GET)
        {
            bool enb = xAerPID1.PID_ON;
            val = enb ? 0x01 : 0x00;
        }
        else
        {
            val = data[2];
            bool enb = val > 0 ? true : false;
            xAerPID1.PID_ON = enb;
        }
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::COIL_TOGGLE);
        reply->AddClient(client->id());
        reply->Val(val);
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
#if AERPID_COUNT == 2
    case SerialCommand::COIL_TOGGLE2:
    {
        uint8_t op = data[1];
        uint8_t val;
        if (op == Operation::OP_GET)
        {
            bool enb = xAerPID2.PID_ON;
            val = enb ? 0x01 : 0x00;
        }
        else
        {
            val = data[2];
            bool enb = val > 0 ? true : false;
            xAerPID2.PID_ON = enb;
        }
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::COIL_TOGGLE2);
        reply->AddClient(client->id());
        reply->Val(val);
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
#endif
    case SerialCommand::CMD_ESP:
    {
        uint8_t op = data[2];
        uint8_t prm = data[1];
        if (prm == ESP::PARAM_RESET)
        {
            Serial.println(F(">>> SOFTWARE RESET TRIGGERED! <<<"));
            // reboot
            ESP.restart();
        }
        else if (prm == ESP::PARAM_DEVICE_RESET)
        {
            Serial.println(F(">>> FLASH RESET TRIGGERED! <<<"));
            // factory reset
            aerManager.getSettings()->resetFlash();
            vTaskDelay(3000);
            esp_restart();
        }
    }
    break;
    case SerialCommand::FAV_1:
    {
        double val;
        uint8_t op = data[1];
        uint8_t prm = data[2];
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::FAV_1);
        reply->AddClient(client->id());
        reply->Param(prm);
        reply->Op(op);
        if (prm == Favorite::FAV_NAME)
        {
            if (op == Operation::OP_SET)
            {
                char *val;
                val = configureFavTempName(1, data);
                Serial.print(F(">>New Name: "));
                Serial.println(String(val));
                reply->Val(val);
            }
            else
            {
                reply->Val(aerManager.getFavs()->getName(1));
            }
        }
        else if (prm == Favorite::FAV_TEMP)
        {
            if (op == Operation::OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = bd.value;
                configureFavTemp(1, val);
            }
            else
            {
                reply->Val(aerManager.getFavs()->getTemp(1));
            }
        }
        else // FAV_BUMP
        {
            // ToDo
        }

        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
    case SerialCommand::FAV_2:
    {
        double val;
        uint8_t op = data[1];
        uint8_t prm = data[2];
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::FAV_2);
        reply->AddClient(client->id());
        reply->Param(prm);
        reply->Op(op);
        if (prm == Favorite::FAV_NAME)
        {
            if (op == Operation::OP_SET)
            {
                char *val;
                val = configureFavTempName(2, data);
                Serial.print(F(">>New Name: "));
                Serial.println(String(val));
                reply->Val(val);
            }
            else
            {
                reply->Val(aerManager.getFavs()->getName(2));
            }
        }
        else if (prm == Favorite::FAV_TEMP)
        {
            if (op == Operation::OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = bd.value;
                configureFavTemp(2, val);
            }
            else
            {
                reply->Val(aerManager.getFavs()->getTemp(2));
            }
        }
        else // FAV_BUMP
        {
            // ToDo
        }

        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
    case SerialCommand::FAV_3:
    {
        double val;
        uint8_t op = data[1];
        uint8_t prm = data[2];
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::FAV_3);
        reply->AddClient(client->id());
        reply->Param(prm);
        reply->Op(op);
        if (prm == Favorite::FAV_NAME)
        {
            if (op == Operation::OP_SET)
            {
                char *val;
                val = configureFavTempName(3, data);
                Serial.print(F(">>New Name: "));
                Serial.println(String(val));
                reply->Val(val);
            }
            else
            {
                reply->Val(aerManager.getFavs()->getName(3));
            }
        }
        else if (prm == Favorite::FAV_TEMP)
        {
            if (op == Operation::OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = bd.value;
                configureFavTemp(3, val);
            }
            else
            {
                reply->Val(aerManager.getFavs()->getTemp(3));
            }
        }
        else // FAV_BUMP
        {
            // ToDo
        }

        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
    case SerialCommand::FAV_4:
    {
        double val;
        uint8_t op = data[1];
        uint8_t prm = data[2];
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::FAV_4);
        reply->AddClient(client->id());
        reply->Param(prm);
        reply->Op(op);
        if (prm == Favorite::FAV_NAME)
        {
            if (op == Operation::OP_SET)
            {
                char *val;
                val = configureFavTempName(4, data);
                Serial.print(F(">>New Name: "));
                Serial.println(String(val));
                reply->Val(val);
            }
            else
            {
                reply->Val(aerManager.getFavs()->getName(4));
            }
        }
        else if (prm == Favorite::FAV_TEMP)
        {
            if (op == Operation::OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = bd.value;
                configureFavTemp(4, val);
            }
            else
            {
                reply->Val(aerManager.getFavs()->getTemp(4));
            }
        }
        else // FAV_BUMP
        {
            // ToDo
        }

        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
    case SerialCommand::CMD_LED:
    {
        // Serial.println("CMD_LED");
        uint8_t op = data[1];
        uint8_t prm = data[2];
        uint8_t val;
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_LED);
        reply->AddClient(client->id());
        reply->Param(prm);
        reply->Op(op);
        if (prm == LED::PARAM_ENABLE)
        {
            if (op == Operation::OP_GET)
            {
                bool enb = lightstor.isEnabled();
                val = enb ? 0x01 : 0x00;
            }
            else
            {
                val = data[3];
                bool enb = val > 0 ? true : false;
                lightstor.isEnabled(enb);
                aerManager.updateLStor(true);
                aerManager.updateLightState(true);
            }
        }
        else if (prm == LED::PARAM_MODE)
        {
            // change modes.
            Serial.println(F(">>> LED: Mode >> "));
            if (op == Operation::OP_GET)
            {
                val = lightstor.getMode();
            }
            else
            {
                // setting new light mode
                val = data[3];
                lightstor.setMode(val);
                aerManager.updateLStor(true);
                aerManager.updateLightState(true);
            }
        }
        else if (prm == LED::PARAM_BRIGHTNESS)
        {
            // change brightness
            Serial.println(F(">>> LED: Brightness >> "));
            if (op == Operation::OP_GET)
            {
                val = lightstor.getBright();
            }
            else
            {
                val = data[3];
                lightstor.setBright(val);
                aerManager.updateLStor(true);
                aerManager.updateLightState(true);
            }
        }
        else if (prm == LED::PARAM_COLOR)
        {
            // change color
            Serial.println(F(">>> LED: Color >>"));
            if (op == Operation::OP_GET)
            {
                val = lightstor.getBright(); // just filler for now
                // ToDo: Squid how do I make val take a string?
            }
            else
            {

                char *rgb = new char[8];
                for (int i = 0; i < 8; i++)
                {
                    if (data[3 + i] == 0x00)
                    {
                        rgb[i] = '\0';
                        break;
                    }
                    rgb[i] = data[3 + i];
                }
                Serial.println(String(rgb));
                char *rgb2 = new char[6];
                strncpy(rgb2, rgb + 1, 6);
                int r, g, b;
                sscanf(rgb2, "%02x%02x%02x", &r, &g, &b);
                Serial.println("R: " + String(r) + " G: " + String(g) + " B: " + String(b));
                lightstor.setRgbVal('r', r);
                lightstor.setRgbVal('g', g);
                lightstor.setRgbVal('b', b);

                Serial.println(">>> rgb to hex test: " + lightstor.getRgbHex() + " <<<<");

                aerManager.updateLightState(true);
                aerManager.updateLStor(true);
            }
        }
        else if (prm == LED::PARAM_STAT)
        {
            if (op == Operation::OP_GET)
            {
                val = lightstor.statusEnabled() ? 0x01 : 0x00;
            }
            else
            {
                val = data[3];
                bool enb = val > 0 ? true : false;
                lightstor.statusEnabled(enb);
                aerManager.updateLStor(true);
                aerManager.updateLightState(true);
            }
            // reply->Op(op);
        }
        reply->Val(val);
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
    case SerialCommand::CMD_PID:
    {
        uint8_t op = data[1];
        uint8_t prm = data[2];
        switch (prm)
        {
        case PARAM_PID::PARAM_PID:
        {
            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Val(xAerPID1.kP);
            reply->Val(xAerPID1.kI);
            reply->Val(xAerPID1.kD);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        break;
        case PARAM_PID::PARAM_PID_P:
        {
            double val;
            if (op == OP_GET)
            {
                val = xAerPID1.kP;
            }
            else if (op == OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = xAerPID1.kP = bd.value;
                Serial.println(">>> P Val >> " + String(val));
                xAerPID1.setTunings();
                aerManager.setPressTick(600);
                xAerPID1.pid_saved = false;
            }

            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Val(val);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        break;
        case PARAM_PID::PARAM_PID_I:
        {
            double val;
            if (op == OP_GET)
            {
                val = xAerPID1.kI;
            }
            else if (op == OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = xAerPID1.kI = bd.value;
                Serial.println(">>> I Val >> " + String(val, 4));
                xAerPID1.setTunings();
                aerManager.setPressTick(600);
                xAerPID1.pid_saved = false;
            }
            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Val(val);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        break;
        case PARAM_PID::PARAM_PID_D:
        {
            double val;
            if (op == OP_GET)
            {
                val = xAerPID1.kD;
            }
            else if (op == OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = xAerPID1.kD = bd.value;
                Serial.println(">>> D Val >> " + String(val));
                xAerPID1.setTunings();
                aerManager.setPressTick(600);
                xAerPID1.pid_saved = false;
            }
            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Val(val);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        break;
        }
    }
    break;
#if AERPID_COUNT == 2
    case SerialCommand::CMD_PID2:
    {
        uint8_t op = data[1];
        uint8_t prm = data[2];
        switch (prm)
        {
        case PARAM_PID::PARAM_PID:
        {
            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID2);
            reply->AddClient(client->id());
            reply->Param(PARAM_PID::PARAM_PID);
            reply->Val(xAerPID2.kP);
            reply->Val(xAerPID2.kI);
            reply->Val(xAerPID2.kD);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        break;
        case PARAM_PID::PARAM_PID_P:
        {
            double val;
            if (op == OP_GET)
            {
                val = xAerPID2.kP;
            }
            else if (op == OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = xAerPID2.kP = bd.value;
                Serial.println(">>> P Val >> " + String(val));
                xAerPID2.setTunings();
                aerManager.setPressTick(600);
                xAerPID2.pid_saved = false;
            }

            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID2);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Val(val);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        break;
        case PARAM_PID::PARAM_PID_I:
        {
            double val;
            if (op == OP_GET)
            {
                val = xAerPID2.kI;
            }
            else if (op == OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = xAerPID2.kI = bd.value;
                Serial.println(">>> I Val >> " + String(val, 4));
                xAerPID2.setTunings();
                aerManager.setPressTick(600);
                xAerPID2.pid_saved = false;
            }
            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID2);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Val(val);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        break;
        case PARAM_PID::PARAM_PID_D:
        {
            double val;
            if (op == OP_GET)
            {
                val = xAerPID2.kD;
            }
            else if (op == OP_SET)
            {
                byteDouble bd;
                for (int i = 0; i < sizeof(double); i++)
                {
                    bd.bytes[i] = data[3 + i];
                }
                val = xAerPID2.kD = bd.value;
                Serial.println(">>> D Val >> " + String(val));
                xAerPID2.setTunings();
                aerManager.setPressTick(600);
                xAerPID2.pid_saved = false;
            }
            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_PID2);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Val(val);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        break;
        }
    }
    break;
#endif
    case SerialCommand::CMD_TEMP:
    {
        double val;
        uint8_t op = data[1];
        if (op == Operation::OP_SET)
        {
            byteDouble bd;
            for (int i = 0; i < sizeof(double); i++)
            {
                bd.bytes[i] = data[2 + i];
            }
            if (aerManager.getReadingType() == ThermalUnitsType::FAHRENHEIT)
            { // convert to C
                Serial.printf(">> Temp-In(f): %f \n", bd.value);
                val = toCelsius(bd.value);
            }
            else if (aerManager.getReadingType() == ThermalUnitsType::KELVIN)
            { // convert to c
                Serial.printf(">> Temp-In(k): %f \n", bd.value);
                val = fromKelvin(bd.value);
            }
            else
            { // is c
                val = bd.value;
            }
            Serial.printf(">> Set Temp(c): %f \n", val);
            xAerPID1.SET_TEMP = val;
            aerManager.updateTempStor(0, true);
            aerManager.setPressTick(250);
        }
        else
        {
            val = xAerPID1.SET_TEMP;
        }

        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_TEMP);
        reply->AddClient(client->id());
        reply->Op(op);
        reply->Val(static_cast<uint16_t>(10.0 * val));
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
#if AERPID_COUNT == 2
    case SerialCommand::CMD_TEMP2:
    {
        double val;
        uint8_t op = data[1];
        if (op == Operation::OP_SET)
        {
            byteDouble bd;
            for (int i = 0; i < sizeof(double); i++)
            {
                bd.bytes[i] = data[2 + i];
            }
            if (aerManager.getReadingType() == ThermalUnitsType::FAHRENHEIT)
            { // convert to C
                Serial.printf(">> Temp-In(f): %f \n", bd.value);
                val = toCelsius(bd.value);
            }
            else if (aerManager.getReadingType() == ThermalUnitsType::KELVIN)
            { // convert to c
                Serial.printf(">> Temp-In(k): %f \n", bd.value);
                val = fromKelvin(bd.value);
            }
            else
            { // is c
                val = bd.value;
            }
            Serial.printf(">> Set Temp(c): %f \n", val);
            xAerPID2.SET_TEMP = val;
            aerManager.updateTempStor(1, true);
            aerManager.setPressTick(250);
        }
        else
        {
            val = xAerPID2.SET_TEMP;
        }

        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_TEMP2);
        reply->AddClient(client->id());
        reply->Op(op);
        reply->Val(static_cast<uint16_t>(10.0 * val));
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
#endif
    case SerialCommand::CMD_WIFI:
    {
        uint8_t op = data[1];
        uint8_t prm = data[2];
        if (prm == WIFI::PARAM_JOIN_WIFI)
        {
            char val[64];
            for (int i = 0; i < 64; i++)
            {
                if (data[3 + i] == 0x00)
                {
                    val[i] = '\0';
                    break;
                }
                val[i] = data[3 + i];
            }
            String ssid_pw = String(val);
            SplitterClass splitter = SplitterClass();
            splitter.split(ssid_pw, ":");
            String ssid = splitter.contents()[0];
            String pass = splitter.contents()[1];
            splitter.dispose();
            commstor.setSSID((char *)ssid.c_str());
            commstor.setPSK((char *)pass.c_str());
            commstor.setSSIDSet(true);
            commstor.saveWiFi();

            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_WIFI);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Op(op);
            reply->Val((uint8_t)0x01);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
        else if (prm == WIFI::PARAM_RESET_WIFI)
        {
            commstor.setSSID("");
            commstor.setPSK("");
            commstor.setSSIDSet(false);
            commstor.saveWiFi();

            SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_WIFI);
            reply->AddClient(client->id());
            reply->Param(prm);
            reply->Op(op);
            reply->Val((uint8_t)0x01);
            reply->build();
            reply->emit(&ws);
            delete reply;
        }
    }
    break;
    case SerialCommand::CMD_DEBUG:
    {
        uint8_t val;
        uint8_t op = data[1];
        if (op == Operation::OP_SET)
        {
            val = data[2];
            if (val == 0)
            {
                aerManager.setEnabledDTR(false);
            }
            else
            {
                aerManager.setEnabledDTR(true);
                val = 1;
            }
        }
        else if (op == Operation::OP_GET)
        {
            val = aerManager.isEnabledDTR() ? 1 : 0;
        }
        else
        {
            break;
        }
        SocketCmdOp *reply = new SocketCmdOp(SerialCommand::CMD_DEBUG);
        reply->AddClient(client->id());
        reply->Op(op);
        reply->Val(val);
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;

    default:
        break;
    }
}

char *WebServer::configureFavTempName(uint8_t index, char *data)
{
    char *val = new char[64];
    for (int i = 0; i < 64; i++)
    {
        if (data[3 + i] == 0x00)
        {
            val[i] = '\0';
            break;
        }
        val[i] = data[3 + i];
    }
    Serial.printf(">> Fav Temp: %u | New Name: %s \n", index, String(val));
    favstor.setFavName(val, index);
    aerManager.setPressTick(250);
    aerManager.updateFStor(true);
    return val;
}

void WebServer::configureFavTemp(uint8_t index, double temp)
{
    if (aerManager.getReadingType() == ThermalUnitsType::FAHRENHEIT)
    { // convert to C and save
        Serial.printf(">> Fav Temp: %u | Temp-In(f): %f \n", index, temp);
        temp = toCelsius(temp);
    }
    else if (aerManager.getReadingType() == ThermalUnitsType::KELVIN)
    { // convert to c and save.
        Serial.printf(">> Fav Temp: %u | Temp-In(k): %f \n", index, temp);
        temp = fromKelvin(temp);
    }
    else
    { // is c already so save it as is
    }
    Serial.printf(">> Fav Temp: %u | Temp(c): %f \n", index, temp);
    favstor.setFavTemp(temp, index);
    aerManager.setPressTick(250);
    aerManager.updateFStor(true);
}

// ========================================================================
// Web Functions
// ========================================================================

String WebServer::processor(const String &var)
{
    return String(var.c_str());
}

// Process Temperature A
String WebServer::process_temp_a()
{
    char outstr[15];
    // dtostrf(globals.TMP_A, 7, 3, outstr);
    return String(outstr);
}

// Process Temperature B
String WebServer::process_temp_b()
{
    char outstr[15];
    // dtostrf(globals.TMP_B, 7, 3, outstr);
    return String(outstr);
}

// Process Temperature Element
String WebServer::process_temp_e()
{
    char outstr[15];
    dtostrf(xAerPID1.AVG_TEMP, 7, 3, outstr);
    return String(outstr);
}

// Process Set Temperature Element
String WebServer::process_temp_s()
{
    char outstr[15];
    dtostrf(xAerPID1.SET_TEMP, 7, 1, outstr);
    return String(outstr);
}

String WebServer::gen_nonce()
{
    uint8_t index = NONCE_current;
    index++;
    if (index >= sizeof(NONCE_A))
    {
        index = 0;
    }
    gen_nonce_a(index);
    NONCE_current = index;
    return NONCE_A[index];
}

// Generate a Nonce
void WebServer::gen_nonce_a(uint8_t ind)
{
    uint8_t r0 = ind & 255;
    uint8_t r1 = random(65, 90);
    uint8_t r2 = random(65, 90);
    uint8_t r3 = random(0, 255);
    uint8_t r4 = random(0, 255);
    uint8_t r5 = random(0, 255);
    uint8_t r6 = random(0, 255);
    NONCE_A[ind] = String(r0) + String("00") + String((char)r1) + String((char)r2) + String("00") + String(r2) + String(r3) + String("00") + String(r4) + String(r5);
    NONCE_current = ind;
}

// Match nonce
bool WebServer::match_nonce(uint8_t index, String nonce)
{
    return NONCE_A[index] == nonce;
}

// Match nonce
bool WebServer::match_nonce(String nonce)
{
    for (int i = 0; i < sizeof(NONCE_A); i++)
    {
        if (NONCE_A[i] == nonce)
        {
            return true;
        }
    }
    return false;
}

void WebServer::reset_nonce(uint8_t index)
{
    NONCE_A[index] = "";
}