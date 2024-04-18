#include "webServer.h"
#include ".conf_wifi.h"

// initialize web server and socket
AsyncWebServer server(HTTP_ADDRESS);
AsyncWebSocket ws("/ws");

AsyncWebSocket *WebServer::getWebSocket() { return &ws; };

// Set your Static IP address
// IPAddress local_IP_s(192, 168, 0, 192);
// Set your Gateway IP address
// IPAddress gateway(192, 168, 0, 1);

// IPAddress subnet(255, 255, 0, 0);
// IPAddress primaryDNS(192, 168, 0, 8); // optional
// IPAddress secondaryDNS(8, 8, 4, 4);   // optional

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

char *WebServer::getHost()
{
    return aerManager.getNet()->getHostname();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Setup the async web handler
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
        Serial.print(">No host saved!  Setting up default hostname... ");
        Serial.println(hostname);
    }

    // Scan network SSID's...
    scanNetworks(false, macShrt);
    delay(500);

    if (MDNS.begin(hostname))
    {
        Serial.print("mDNS responder started: ");
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
        char *set_ssid = commstor.getSSID();
        WiFi.mode(WIFI_MODE_STA);
        Serial.println(F("[WiFi] -> Connecting to WiFi network.."));
        WiFi.begin(commstor.getSSID(), commstor.getPSK());
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
        Serial.println(F("Setup 'HTML Interface'"));

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
                      { request->send(404, "text/html", "<p><b>ERROR 404:</b> Path not found: " + request->url() + "<br/>The content you are looking for at the requested location was not found.</p>"); });

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
}

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

    // TODO: Update clients with any changes made locally or via usb.

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
    case SerialCommand::CMD_ESP:
    {
        uint8_t op = data[2];
        uint8_t prm = data[1];
        if (prm == ESP::PARAM_RESET)
        {
            // reboot
            ESP.restart();
        }
        else if (prm == ESP::PARAM_DEVICE_RESET)
        {
            // facfory reset?
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
                Serial.println(">>New Name: " + String(val));
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
                Serial.println(">>New Name: " + String(val));
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
                Serial.println(">>New Name: " + String(val));
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
                Serial.println(">>New Name: " + String(val));
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
            Serial.println(">>> LED: Mode >> ");
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
            Serial.println(">>> LED: Brightness >> ");
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
            Serial.println(">>> LED: Color >>");
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
            xAerPID1.SET_TEMP = val; // val = xAerPID1.SET_TEMP = bd.value;
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
        reply->Val(val);
        reply->build();
        reply->emit(&ws);
        delete reply;
    }
    break;
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

void WebServer::processSocketCmd(SerialCommand cmd, AsyncWebSocketClient *client, char *data, size_t len)
{
}

void WebServer::notifyClients()
{
    // ws.textAll(String(ledState));
}

void WebServer::sendTempStr()
{
    // ws.textAll("temp:" + String(xAerPID1.AVG_TEMP));
}

void WebServer::sendCoilStatus()
{ // Update all clients on coil state
    if (xAerPID1.PID_ON)
    {
        /// ws.textAll("cstat:true");
    }
    else
    {
        /// ws.textAll("cstat:false");
    }
}

void WebServer::sendBumpStatus()
{ // Update all clients: EN_BUMP
    if (bumpStor.getEnabled(0))
    {
        /// ws.textAll("bstat:true");
    }
    else
    {
        /// ws.textAll("bstat:false");
    }
    // Serial.println("Set in: sendBumpStatus");
    sendSetTemp();
}

void WebServer::sendLightStatus()
{                              // Update all clients: LEDstate
    if (lightstor.isEnabled()) // (LEDstate)
    {
        /// ws.textAll("lstat:true");
    }
    else
    {
        /// ws.textAll("lstat:false");
    }
    updateLEDVars();
    /// ws.textAll("brightset:" + String(lightstor.getBright()));
}

void WebServer::sendSetTemp()
{ // Update all clients: SET_TEMP
  /// BleUP_SetTemp = true;
  /// ws.textAll("settemp:" + String(xAerPID1.SET_TEMP, 1));
}

void WebServer::sendPIDSet()
{ // Send PID settings to clients  String(kP,4)
  /// ws.textAll("setpid:" + String(xAerPID1.kP, 2) + ":" + String(xAerPID1.kI, 4) + ":" + String(xAerPID1.kD, 2));
}

void WebServer::sendInitString()
{ // Send dump of data to set up page on load.
    /*
        ws.textAll("init:" + aerManager.getVersion()->get() + ":" + aerManager.getVersionWeb()->get() + ":" + local_IP + ":" + ssid + ":" + String("AerTiny") + ":" + String(0) + ":" +
                   String(bumpStor.getTemp()) + ":" + String(bumpStor.getTime()) + ":" + String(xAerPID1.AUTO_OFF_ENB) + ":" + String(xAerPID1.AUTO_OFF_TIME) + ":" + String(lightstor.statusEnabled()) + ":" + String(lightstor.ambientEnabled()) + ":" +
                   String(lightstor.presetsEnabled()) + ":" + String((int)favstor.getTemp(1)) + ":" + String((int)favstor.getTemp(2)) + ":" + String((int)favstor.getTemp(3)) + ":" + String((int)favstor.getTemp(4)) + ":" + String(favstor.getName(1)) + ":" + String(favstor.getName(2)) + ":" + String(favstor.getName(3)) + ":" + String(favstor.getName(4)) + ":" +
                   String(xAerPID1.TEMP_ADJ_AMOUNT) + ":" + String(lightstor.amPulseEnabled()) + ":" + String(lightstor.amBlinkEnabled()) + ":" + String(commstor.getWifiEn()) + ":" + String(commstor.getBTEn()) + ":" + String(commstor.getBTDis()) + ":" +
                   String(commstor.getBTMon()) + ":" + String(commstor.getUSBEn()) + ":" + String(commstor.getUSBDis()) + ":" + String(commstor.getUSBMon()));
    */
}

void WebServer::toggleCoil()
{
    // Serial.printf(": "+ String(SET_TEMP_MAX) +"\n");
    Serial.print(F("[PID] SET_TEMP_MAX= "));
    Serial.print(xAerPID1.SET_TEMP_MAX);
    Serial.print(F("\n"));
    if (!xAerPID1.PID_ON)
    {
        xAerPID1.SET_TEMP_LAST = xAerPID1.SET_TEMP;
        xAerPID1.PID_ON = true;
        delay(420);
    }
    else
    {
        if (bumpStor.getEnabled(0))
        {
            bumpStor.setEnabled(0, false);
            xAerPID1.SET_TEMP = xAerPID1.SET_TEMP_LAST;
            sendBumpStatus();
        }
        xAerPID1.PID_ON = false;
        delay(350);
    }
    sendCoilStatus(); // Update Web-Clients
}

void WebServer::toggleBump()
{
    if (xAerPID1.PID_ON && bumpStor.getEnabled(0))
    {
        bumpStor.setEnabled(0, false);
        delay(350);
    }
    else if (xAerPID1.PID_ON)
    {
        bumpStor.setEnabled(0, true);
        delay(350);
    }
}

/*
void WebServer::handleWebSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        if (strcmp((char *)data, "coiltoggle") == 0)
        {
            Serial.printf("#%u Sent a coil-toggle request\n", client->id());
            // TODO: Figure out why it doesn't start heating the coil on the first time this way.
            // PID_ON = !PID_ON;  // Toggle PID Enable
            // sendCoilStatus(); // Update clients on coil state
            toggleCoil();
        }
        if (strcmp((char *)data, "bumptoggle") == 0)
        {
            Serial.printf("#%u Sent a bump-toggle request\n", client->id());
            toggleBump();
        }
        if (strcmp((char *)data, "ledtoggle") == 0)
        {
            FancyLED *xled = aerManager.getFancyLED();
            Serial.printf("#%u Sent an led-toggle request\n", client->id());
            // LEDstate = !LEDstate;
            lightstor.isEnabled(!lightstor.isEnabled());
            if (!lightstor.isEnabled())
            {
                if (xSemaphoreTake(sys1_mutex, 5) == pdTRUE)
                {
                    Serial.println("LEDS OFF!");
                    // brightenPixel(&xled_B, 0);
                    xled->brightenPixels(0);
                    xled->setPixel(xled->getPixels()[0], "OFF");
                    // setPixel(&xled_B, "OFF");
                    xSemaphoreGive(sys1_mutex);
                }
            }
            else
            {
                if (xSemaphoreTake(sys1_mutex, 5) == pdTRUE)
                {
                    Serial.println("LEDS ON!");
                    xled->brightenPixels(90);
                    xled->getPixels()->setRGB(lightstor.getRgbVal('r'), lightstor.getRgbVal('g'), lightstor.getRgbVal('b'));
                    FastLED.show();
                    xSemaphoreGive(sys1_mutex);
                }
            }

            sendLightStatus(); // Update clients on light state
        }
        if (strcmp((char *)data, "tUp") == 0)
        {
            Serial.printf("[WebApp] >> #%u Sent a tempUp request\n", client->id());
            // TODO: Add something other than a serial print out here....
        }
        if (strcmp((char *)data, "tDown") == 0)
        {
            Serial.printf("[WebApp] >> #%u Sent a tempDown request\n", client->id());
            // TODO: Add something other than a serial print out here....
        }
        if (strcmp((char *)data, "DERP") == 0)
        {
            Serial.printf("[WebApp] >> #%u Sent a DERP request\n", client->id());
            // TODO: Add something other than a serial print out here....
        }

        //=================================================
        // WiFi Settings sockets
        if (strcmp((char *)data, "wifi_scan") == 0)
        {
            Serial.printf("[WebApp] >> #%u Sent a WiFi_Scan request\n", client->id());
            // Start WiFi Network Scan and let loop know.
            WiFi.scanNetworks(true);
            sendNetworks = true;
        }

        if (strncmp((char *)data, "wifi_join:", 10) == 0)
        {
            Serial.printf("[WebApp] >> #%u Sent a Wifi_Join request\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *ssid = strtok(NULL, ":");
            char *pass = strtok(NULL, ":");

            Serial.println(F("[WiFi] -> Connecting to WiFi network.."));
            WiFi.begin(ssid, pass);
            commstor.setSSID(ssid);
            commstor.setPSK(pass);
            wifiStarting = true;
        }
        if (strcmp((char *)data, "wifi_reset") == 0)
        {
            Serial.printf("[WebApp] >> #%u Sent a WiFi_Reset request\n", client->id());
            // Reset CommStor values and save; will take effect on reboot.  TODO: Force reboot?
            commstor.setSSID("NULL");
            commstor.setPSK("NULL");
            commstor.setSSIDSet(false);
            commstor.saveWiFi();
            Serial.println("[WebApp] >> WiFi Values Reset. Please restart device.");
        }
        //=================================================

        //=================================================
        // ESP RESET Sockets
        if (strcmp((char *)data, "esp_reset") == 0)
        {
            Serial.printf("[WebApp] >> #%u Sent a Reboot request\n", client->id());
            delay(10);
            ESP.restart();
        }
        if (strcmp((char *)data, "esp_device_reset") == 0)
        {
            Serial.printf("[WebApp] >> #%u Sent a Factory-Reset request\n", client->id());
            // TODO: Update squid's factory reset function and call it here.
        }
        //=================================================

        //=================================================
        // Bluetooth Sockets
        if (strncmp((char *)data, "bt_en:", 5) == 0) // BT General
        {
            Serial.printf("[WebApp] >> #%u Sent a Bluetooth toggle\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            if (strcmp((char *)used, "true") == 0)
            {
                Serial.println("[WebApp] >> Turning on Bluetooth.");
                commstor.setBTEn(true);
            }
            else
            {
                Serial.println("[WebApp] >> Turning off Bluetooth.");
                commstor.setBTEn(false);
            }
            commstor.saveBT();
        }
        if (strncmp((char *)data, "bt_mon:", 6) == 0) // BT Monitor Only
        {
            Serial.printf("[WebApp] >> #%u Sent a Bluetooth Monitor toggle\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            if (strcmp((char *)used, "true") == 0)
            {
                Serial.println("[WebApp] >> Turning on Bt_Monitor.");
                commstor.setBTMon(true);
            }
            else
            {
                Serial.println("[WebApp] >> Turning off Bt_Monitor.");
                commstor.setBTMon(false);
            }
            commstor.saveBT();
        }
        if (strncmp((char *)data, "bt_disc:", 5) == 0) // BT Discoverable >> Currently hidden since BLE doesn't work this way.
        {
            Serial.printf("[WebApp] >> #%u Sent a Bluetooth Discoverable toggle\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            if (strcmp((char *)used, "true") == 0)
            {
                Serial.println("[WebApp] >> Turning on Bt_Discover.");
                commstor.setBTDis(true);
            }
            else
            {
                Serial.println("[WebApp] >> Turning off Bt_Discover.");
                commstor.setBTDis(false);
            }
            commstor.saveBT();
        }
        //=================================================

        //=================================================
        // Bump Sockets
        if (strncmp((char *)data, "bump_amt:", 11) == 0) // Change the bump amount
        {
            Serial.printf("[WebApp] >> #%u Sent a bump amount request\n", client->id());
            // string[] split = (char*)data.Split(':');
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            Serial.printf("[WebApp] >> %s > is the requested temp.\n", used);

            String myString = String(used);

            double t = myString.toDouble();
            if (t + xAerPID1.SET_TEMP < xAerPID1.SET_TEMP_MAX && t > 1)
            {
                bool turncoilbkon = false;
                bumpStor.setTemp(t);
                if (xAerPID1.PID_ON)
                {
                    toggleCoil();
                    turncoilbkon = true;
                }
                bumpStor.save();
                delay(50);
                if (turncoilbkon)
                {
                    toggleCoil();
                }
            }
        }

        if (strncmp((char *)data, "bumplength:", 11) == 0) // Change the bump amount
        {
            Serial.printf("[WebApp] >> #%u Sent a bump length request\n", client->id());
            // string[] split = (char*)data.Split(':');
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            Serial.printf("[WebApp] >> %s > is the requested length in seconds.\n", used);

            String myString = String(used);

            double t = myString.toDouble();
            if (t < 600 && t > 10)
            {
                bool turncoilbkon = false;
                bumpStor.setTime(t);
                if (xAerPID1.PID_ON)
                {
                    toggleCoil();
                    turncoilbkon = true;
                }
                bumpStor.save();
                delay(50);
                if (turncoilbkon)
                {
                    toggleCoil();
                }
            }
        }
        //=================================================

        if (strncmp((char *)data, "changetemp:", 11) == 0) // Change the temp
        {
            Serial.printf("[WebApp] >> #%u Sent a tempchange request\n", client->id());
            // string[] split = (char*)data.Split(':');
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            Serial.printf("[WebApp] >> %s > is the requested temp.\n", used);

            String myString = String(used);

            double t = myString.toDouble();
            if (xAerPID1.SET_TEMP_MAX < 100)
            {
                xAerPID1.SET_TEMP_MAX = 850;
                // eepromStor.save_set_temp_max(SET_TEMP_MAX);
                tempStor.setMaxTemp(xAerPID1.SET_TEMP_MAX);
            }
            if (t < xAerPID1.SET_TEMP_MAX && t > 100)
            {
                bool turncoilbkon = false;
                xAerPID1.SET_TEMP = t;
                if (xAerPID1.PID_ON)
                {
                    toggleCoil();
                    turncoilbkon = true;
                }
                tempStor.setTemp(xAerPID1.SET_TEMP);
                tempStor.save();
                delay(50);
                if (turncoilbkon)
                {
                    toggleCoil();
                }

                sendSetTemp();
            }
        }

        if (strncmp((char *)data, "changeadj:", 10) == 0) // Change Quick-Adjust amount
        {
            Serial.printf("#%u Sent a changeadj request\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            Serial.printf("%s > is the requested amt.\n", used);
            // String(TEMP_ADJ_AMOUNT)
            int t = String(used).toInt();
            xAerPID1.TEMP_ADJ_AMOUNT = t;
            // BleUP_AdjAmt = true;
            ws.textAll("setadj:" + String(used));
        }

        if (strncmp((char *)data, "fav_name:", 9) == 0) // Change NAME of selected fav
        {
            Serial.printf("#%u Sent a fav_name request\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *fav = strtok(NULL, ":");
            char *name = strtok(NULL, ":");
            Serial.printf("%s > change requested for fav.\n", fav);
            Serial.printf("%s > is the requested name.\n", name);
            int x = String(fav).toInt();
            switch (x)
            {
            case 1:
                // FAVS_A = String(name);
                favstor.setFavName(name, 1);
                break;
            case 2:
                // FAVS_B = String(name);
                favstor.setFavName(name, 2);
                break;
            case 3:
                // FAVS_C = String(name);
                favstor.setFavName(name, 3);
                break;
            case 4:
                // FAVS_D = String(name);
                favstor.setFavName(name, 4);
                break;

            default:
                break;
            }
        }
        if (strncmp((char *)data, "fav_temp:", 9) == 0) // Change TEMP of selected fav
        {
            Serial.printf("#%u Sent a fav_temp request\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *fav = strtok(NULL, ":");
            char *temp = strtok(NULL, ":");
            Serial.printf("%s > change requested for fav.\n", fav);
            Serial.printf("%s > is the requested name.\n", temp);
            int x = String(fav).toInt();
            switch (x)
            {
            case 1:
                favstor.setFavTemp(atof(temp), 1);
                break;
            case 2:
                favstor.setFavTemp(atof(temp), 2);
                break;
            case 3:
                favstor.setFavTemp(atof(temp), 3);
                break;
            case 4:
                favstor.setFavTemp(atof(temp), 4);
                break;

            default:
                break;
            }
        }

        if (strncmp((char *)data, "pidset{", 7) == 0) // Update PID settings
        {                                             // Changing PID Settings
            Serial.printf("#%u Sent a pidset request\n", client->id());
            // string[] split = (char*)data.Split(':');
            char *burn = strtok((char *)data, "{}");
            char *used = strtok(NULL, "{}");
            Serial.printf("%s > is the settings string.\n", used);
            char *p1 = strtok(used, "|");
            char *i1 = strtok(NULL, "|");
            char *d1 = strtok(NULL, "|");
            burn = strtok(p1, ":");
            char *p = strtok(NULL, ":");
            burn = strtok(i1, ":");
            char *i = strtok(NULL, ":");
            burn = strtok(d1, ":");
            char *d = strtok(NULL, ":");
            char *stopstring;
            Serial.printf("P: %s I: %s D: %s \n", p, i, d);
            xAerPID1.kP = atof(p);
            xAerPID1.kI = atof(i);
            xAerPID1.kD = atof(d);
            Serial.print("Raw KI as Double: ");
            Serial.println(xAerPID1.kI, 4);
            // FIXME: this still uses eeprom... lol
            // eepromStor.save_kP(xAerPID1.kP);
            delay(20);
            // eepromStor.save_kI(xAerPID1.kI);
            delay(20);
            // eepromStor.save_kD(xAerPID1.kD);
            delay(20);
            // FIXME: need setter for this somehow...
            // myPID.SetTunings(xAerPID1.kP, xAerPID1.kI, xAerPID1.kD);
            sendPIDSet();
        }

        //=================================================
        //           LED Control Sockets
        if (strncmp((char *)data, "color_set{", 10) == 0)
        { // Changing PID Settings

            FancyLED *xled = aerManager.getFancyLED(); //_am->getFancyLED();
            Serial.printf("#%u Sent a colorchange request\n", client->id());
            // string[] split = (char*)data.Split(':');
            char *burn = strtok((char *)data, "{}");
            char *used = strtok(NULL, "{}");
            Serial.printf("%s > is the colors string.\n", used);
            char *r1 = strtok(used, "|");
            char *g1 = strtok(NULL, "|");
            char *b1 = strtok(NULL, "|");
            burn = strtok(r1, ":");
            char *r = strtok(NULL, ":");
            burn = strtok(g1, ":");
            char *g = strtok(NULL, ":");
            burn = strtok(b1, ":");
            char *b = strtok(NULL, ":");
            printf("R: %s G: %s B: %s \n", r, g, b);

            lightstor.setRgbVal('r', atoi(r));
            lightstor.setRgbVal('g', atoi(g));
            lightstor.setRgbVal('b', atoi(b));
            lightstor.save();

            if (xSemaphoreTake(sys1_mutex, 5) == pdTRUE)
            {
                xled->colorPixels(xled->setRGB(lightstor.getRgbVal('r'), lightstor.getRgbVal('g'), lightstor.getRgbVal('b')));
                FastLED.show();
                xSemaphoreGive(sys1_mutex);
            }
        }

        if (strncmp((char *)data, "brightset:", 10) == 0) // Change the brightness
        {
            FancyLED *xled = aerManager.getFancyLED();
            Serial.printf("[WebApp] >> #%u Sent a Brightness request\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            Serial.printf("[WebApp] >> %s > is the requested brightness.\n", used);

            String myString = String(used);

            int t = myString.toInt();

            lightstor.setBright(t);
            lightstor.save();
            if (!lightstor.statusEnabled())
            {
                xled->brightenPixels(t);
            }
            ws.textAll('brightset:' + String(t));
        }

        if (strncmp((char *)data, "leden_stat:", 11) == 0) // Toggle LED Coil Status Pulse
        {
            Serial.printf("#%u Sent a leden_stat request\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            Serial.printf("%s > is the requested state.\n", used);
            if (strcmp((char *)used, "true") == 0)
            {
                if (lightstor.ambientEnabled())
                {
                    lightstor.ambientEnabled(false);
                    ws.textAll("leden_ambient:0");
                }
                if (lightstor.presetsEnabled())
                {
                    lightstor.presetsEnabled(false);
                    ws.textAll("leden_preset:0");
                }
                lightstor.statusEnabled(true);
                ws.textAll("leden_stat:1");
            }
            else
            {
                lightstor.statusEnabled(false);
                ws.textAll("leden_stat:0");
            }
            lightstor.save();
        }

        if (strncmp((char *)data, "leden_preset:", 13) == 0) // Toggle LED Presets
        {
            Serial.printf("#%u Sent a leden_preset request\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            Serial.printf("%s > is the requested state.\n", used);
            if (strcmp((char *)used, "true") == 0)
            {
                if (lightstor.ambientEnabled())
                {
                    lightstor.ambientEnabled(false);
                    ws.textAll("leden_ambient:0");
                }
                if (lightstor.statusEnabled())
                {
                    lightstor.statusEnabled(false);
                    ws.textAll("leden_stat:0");
                }
                lightstor.presetsEnabled(true);
                ws.textAll("leden_preset:1");
            }
            else
            {
                lightstor.presetsEnabled(false);
                ws.textAll("leden_preset:0");
            }
            lightstor.save();
        }

        if (strncmp((char *)data, "leden_ambient:", 14) == 0) // Toggle Ambient LEDs
        {
            FancyLED *xled = aerManager.getFancyLED();
            Serial.printf("#%u Sent a leden_ambient request\n", client->id());
            char *burn = strtok((char *)data, ":");
            char *used = strtok(NULL, ":");
            Serial.printf("%s > is the requested state.\n", used);
            if (strcmp((char *)used, "true") == 0)
            {
                if (lightstor.statusEnabled())
                {
                    lightstor.statusEnabled(false);
                    ws.textAll("leden_stat:0");
                }
                if (lightstor.presetsEnabled())
                {
                    lightstor.presetsEnabled(false);
                    ws.textAll("leden_preset:0");
                }
                lightstor.ambientEnabled(true);
                ws.textAll("leden_ambient:1");
                if (xSemaphoreTake(sys1_mutex, 5) == pdTRUE)
                {
                    xled->setRGB(lightstor.getRgbVal('r'), lightstor.getRgbVal('g'), lightstor.getRgbVal('b'));
                    FastLED.show();
                    xSemaphoreGive(sys1_mutex);
                }
            }
            else
            {
                lightstor.ambientEnabled(false);
                ws.textAll("leden_ambient:0");
            }
            lightstor.save();
        }

        if (strncmp((char *)data, "leden_pulse:", 12) == 0) // Enable Ambient Pulse
        {
            Serial.printf("#%u Sent a Ambient-Pulse toggle\n", client->id());
            if (lightstor.amPulseEnabled())
            {
                lightstor.amPulseEnabled(false);
            }
            else
            {
                if (lightstor.amBlinkEnabled())
                {
                    lightstor.amBlinkEnabled(false);
                }
                lightstor.amPulseEnabled(true);
            }
            // TODO: Update BLE
            updateLEDVars();
            lightstor.save();
        }

        if (strncmp((char *)data, "leden_blink:", 12) == 0) // Enable Ambient Blink
        {
            Serial.printf("#%u Sent a Ambient-Blink toggle\n", client->id());
            if (lightstor.amBlinkEnabled())
            {
                lightstor.amBlinkEnabled(false);
            }
            else
            {
                if (lightstor.amPulseEnabled())
                {
                    lightstor.amPulseEnabled(false);
                }
                lightstor.amBlinkEnabled(true);
            }
            // TODO: Update BLE
            updateLEDVars();
            lightstor.save();
        }

        if (strcmp((char *)data, "toggle_led_rainbow") == 0) // Enable LED Rainbow
        {
            Serial.printf("#%u Sent a toggle_led_rainbow request\n", client->id());
            if (lightstor.preRainEnabled())
            {
                lightstor.preRainEnabled(false);
            }
            else
            {
                lightstor.prePulseEnabled(false);
                lightstor.preShiftEnabled(false);
                lightstor.preRain2Enabled(false);
                lightstor.preRain3Enabled(false);
                lightstor.preRainEnabled(true);
            }
            // TODO: Update BLE
            updateLEDVars();
            lightstor.save();
        }

        if (strcmp((char *)data, "toggle_led_rainbow2") == 0) // Enable LED Rainbow
        {
            Serial.printf("#%u Sent a toggle_led_rainbow2 request\n", client->id());
            if (lightstor.preRain2Enabled())
            {
                lightstor.preRain2Enabled(false);
            }
            else
            {
                lightstor.prePulseEnabled(false);
                lightstor.preShiftEnabled(false);
                lightstor.preRainEnabled(false);
                lightstor.preRain3Enabled(false);
                lightstor.preRain2Enabled(true);
            }
            // TODO: Update BLE
            updateLEDVars();
            lightstor.save();
        }

        if (strcmp((char *)data, "toggle_led_rainbow3") == 0) // Enable LED Rainbow
        {
            Serial.printf("#%u Sent a toggle_led_rainbow3 request\n", client->id());
            if (lightstor.preRain3Enabled())
            {
                lightstor.preRain3Enabled(false);
            }
            else
            {
                lightstor.prePulseEnabled(false);
                lightstor.preShiftEnabled(false);
                lightstor.preRainEnabled(false);
                lightstor.preRain2Enabled(false);
                lightstor.preRain3Enabled(true);
            }
            // TODO: Update BLE
            updateLEDVars();
            lightstor.save();
        }

        if (strcmp((char *)data, "toggle_led_shift") == 0) // Enable LED Shift
        {
            Serial.printf("#%u Sent a toggle_led_shift request\n", client->id());
            if (lightstor.preShiftEnabled())
            {
                lightstor.preShiftEnabled(false);
            }
            else
            {
                lightstor.prePulseEnabled(false);
                lightstor.preRainEnabled(false);
                lightstor.preRain2Enabled(false);
                lightstor.preRain3Enabled(false);
                lightstor.preShiftEnabled(true);
            }
            // TODO: Update BLE
            updateLEDVars();
            lightstor.save();
        }

        if (strcmp((char *)data, "toggle_led_pulse") == 0) // Toggle LED Color-Pulse
        {
            Serial.printf("#%u Sent a toggle_led_pulse request\n", client->id());
            if (lightstor.prePulseEnabled())
            {
                lightstor.prePulseEnabled(false);
            }
            else
            {
                lightstor.preShiftEnabled(false);
                lightstor.preRainEnabled(false);
                lightstor.preRain2Enabled(false);
                lightstor.preRain3Enabled(false);
                lightstor.prePulseEnabled(true);
            }
            // TODO: Update BLE
            updateLEDVars();
            lightstor.save();
        }

        // End of WebSocket msg handler
    }
}

void WebServer::updateLEDVars()
{
    ws.textAll("LEDVars:" + String(lightstor.preRainEnabled()) + ":" + String(lightstor.preRain2Enabled()) + ":" + String(lightstor.preRain3Enabled()) + ":" + String(lightstor.preShiftEnabled()) + ":" + String(lightstor.prePulseEnabled()) + ":" + String(lightstor.amBlinkEnabled()) + ":" + String(lightstor.amPulseEnabled()) + ":");
}*/

// ========================================================================
// Web Functions
// ========================================================================

String WebServer::processor(const String &var)
{
    if (var == "F_VERSION")
    {
        // return globals.version;
    }
    if (var == "N_VERSION")
    {
        // return globals.webui_version;
    }
    if (var == "CLIENT_IP")
    {
        return "TODO: Convert IP"; // WiFi.localIP();
    }
    if (var == "HOSTNAME")
    {
        return String("AerTiny");
    }
    if (var == "SSID")
    {
        return ssid;
    }
    if (var == "FOOTER_NAME")
    {
        return String("AerPID-tiny");
    }
    if (var == "FOOTER_COPYRIGHT_DATE")
    {
        return String("2020");
    }
    if (var == "FOOTER_COPYRIGHT")
    {
        return String("aerifydigital.com");
    }
    if (var == "F_UPTIME")
    {
        // return String(globals.uptime);
    }
    if (var == "FAV_1")
    {
        // return String((int)globals.FAV_A);
    }
    if (var == "FAV_2")
    {
        // return String((int)globals.FAV_B);
    }
    if (var == "FAV_3")
    {
        // return String((int)globals.FAV_C);
    }
    if (var == "FAV_4")
    {
        // return String((int)globals.FAV_D);
    }
    if (var == "NONCE_I")
    {
        return String(webServer.NONCE_current);
    }
    if (var == "NONCE")
    {
        return webServer.NONCE_A[webServer.NONCE_current];
    }
    // return String(String("%") + String(var.c_str()) + String("%"));

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