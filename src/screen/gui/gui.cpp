#include "gui.h"

// Builds the menu defintion mapping
bool AerGUI::buildMenuDefinitions()
{
    std::vector<uint16_t> menus_root = {
        MENU_ROOT_HOME,
        MENU_ROOT_MAIN,
    };
    std::vector<uint16_t> menus_main = {
        MENU_MAIN_PID,
        MENU_MAIN_FAVS,
        MENU_MAIN_BUMP,
        MENU_MAIN_GRAPH,
        MENU_MAIN_DISPLAY,
        MENU_MAIN_SYSTEM,
        MENU_MAIN_LED,
        MENU_MAIN_WIFI,
        // MENU_MAIN_BLE, // not used for now...
        MENU_MAIN_USB,
        MENU_MAIN_POWER,
        MENU_MAIN_CLOCK,
    };
    std::vector<uint16_t> menus_pid = {
        MENU_PID_P,
        MENU_PID_I,
        MENU_PID_D,
        MENU_PID_TUNING,
        // MENU_PID_AUTO,
        // MENU_PID_PWM_FACTOR,
        // MENU_PID_PWM_CYCLE,
        // MENU_PID_PWM_FREQ,
    };
    std::vector<uint16_t> menus_pid_tuning = {
        // MENU_PID_AUTO,
        MENU_PID_PWM_BIAS,
        MENU_PID_PWM_FACTOR,
        MENU_PID_WINDUP_LIMIT,
        MENU_PID_PWM_CYCLE,
        MENU_PID_PWM_FREQ,
    };
    std::vector<uint16_t> menus_favs = {
        MENU_FAV_1,
        MENU_FAV_2,
        MENU_FAV_3,
        MENU_FAV_4,
    };
    std::vector<uint16_t> menus_fav1 = {
        MENU_FAV_1N,
        MENU_FAV_1T,
    };
    std::vector<uint16_t> menus_fav2 = {
        MENU_FAV_2N,
        MENU_FAV_2T,
    };
    std::vector<uint16_t> menus_fav3 = {
        MENU_FAV_3N,
        MENU_FAV_3T,
    };
    std::vector<uint16_t> menus_fav4 = {
        MENU_FAV_4N,
        MENU_FAV_4T,
    };
    std::vector<uint16_t> menus_bump = {
        MENU_BUMP_AMOUNT,
        MENU_BUMP_TIME,
    };
    std::vector<uint16_t> menus_fav_names = {
        MENU_FAV_NAME_FAVORITE1,
        MENU_FAV_NAME_FAVORITE2,
        MENU_FAV_NAME_FAVORITE3,
        MENU_FAV_NAME_FAVORITE4,
        MENU_FAV_NAME_HIGH,
        MENU_FAV_NAME_MEDIUM,
        MENU_FAV_NAME_LOW,
        MENU_FAV_NAME_STAYWARM,
        MENU_FAV_NAME_MAIN,
        MENU_FAV_NAME_HOME,
        MENU_FAV_NAME_CLEAN,
        MENU_FAV_NAME_PREHEAT,
        MENU_FAV_NAME_STAGE1,
        MENU_FAV_NAME_STAGE2,
        MENU_FAV_NAME_STAGE3,
        MENU_FAV_NAME_STAGE4,
        MENU_FAV_NAME_ONE,
        MENU_FAV_NAME_TWO,
        MENU_FAV_NAME_THREE,
        MENU_FAV_NAME_FOUR,
        MENU_FAV_NAME_COOLDOWN,
        MENU_FAV_NAME_PURGE,
        MENU_FAV_NAME_PRESS,
        MENU_FAV_NAME_SOUSVIDE,
        MENU_FAV_NAME_SMOKE,
        MENU_FAV_NAME_TOAST,
        MENU_FAV_NAME_DRY,
        MENU_FAV_NAME_DEHUMIDIFY,
        MENU_FAV_NAME_TEMPER,
        MENU_FAV_NAME_BAKE,
        MENU_FAV_NAME_SOAK,
        MENU_FAV_NAME_REFLOW,
        MENU_FAV_NAME_RED,
        MENU_FAV_NAME_GREEN,
        MENU_FAV_NAME_BLUE,
    };
    std::vector<uint16_t> menus_graphs = {
#if AERPID_COUNT == 2
        MENU_GRAPH_TEMPERATURE,
        MENU_GRAPH_TEMPERATURE_ZOOM,
        MENU_GRAPH_TEMPERATURE_LONG,
        MENU_GRAPH_TEMPERATURE_0,
        MENU_GRAPH_TEMPERATURE_ZOOM_0,
        MENU_GRAPH_TEMPERATURE_LONG_0,
        MENU_GRAPH_TEMPERATURE_1,
        MENU_GRAPH_TEMPERATURE_ZOOM_1,
        MENU_GRAPH_TEMPERATURE_LONG_1,
#endif
#if AERPID_COUNT == 1
        MENU_GRAPH_TEMPERATURE_0,
        MENU_GRAPH_TEMPERATURE_ZOOM_0,
        MENU_GRAPH_TEMPERATURE_LONG_0,
#endif
    };
    std::vector<uint16_t> menus_system = {
        MENU_SYSTEM_LOCAL_TEMPERATURE,
        MENU_SYSTEM_KNOB_ADJUST,
        MENU_SYSTEM_TOGGLE_IDLE,
        MENU_SYSTEM_LENGTH_IDLE,
        MENU_SYSTEM_FACTORY_RESET,
        // TODO: setup update check on local device...
        // MENU_SYSTEM_UPDATE_CHECK,
        MENU_SYSTEM_DISPLAY_SETTINGS,
        // TODO: setup themes??
        // MENU_SYSTEM_THEME,
    };
    std::vector<uint16_t> menus_display_backlight = {
        MENU_DISPLAY_BACKLIGHT_VAR,
        MENU_DISPLAY_BACKLIGHT_MIN_VAR,
    };
    std::vector<uint16_t> menus_knob_adjust = {
        MENU_KNOB_ADJUST_AMOUNT,
        MENU_KNOB_ADJUST_INVERT,
        // MENU_KNOB_DIAGRAM_SHOWN,
    };
    std::vector<uint16_t> menus_local_temp = {
        MENU_LOCAL_CELSIUS_TOGGLE,
        MENU_LOCAL_TEMPERATURE_1,
        MENU_LOCAL_TEMPERATURE_2,
#if AERPID_COUNT == 2
        MENU_LOCAL_TEMPERATURE_3,
        MENU_LOCAL_FAN_CONTROL,
#endif
    };
    std::vector<uint16_t> menus_system_display = {
        MENU_DISPLAY_BACKLIGHT,
        MENU_DISPLAY_AUTO_DIM,
        MENU_DISPLAY_TIMEOUT_DIM,
        MENU_DISPLAY_SCREENSAVER,
        MENU_DISPLAY_SYS_TRAY,
    };
    std::vector<uint16_t> menus_screen_saver = {
        MENU_SCREENSAVER_ENABLED,
        MENU_SCREENSAVER_TIME,
        MENU_SCREENSAVER_STARS,
        MENU_SCREENSAVER_TOGGLE,
    };
    std::vector<uint16_t> menus_led = {
        MENU_LED_MAIN_TOGGLE,
        MENU_LED_SETUP,
        MENU_LED_STATUS,
        // MENU_LED_AMBIENT,
        MENU_LED_PRESET,
    };
    std::vector<uint16_t> menus_led_color = {
        MENU_LED_BRIGHTNESS,
        MENU_LED_COLOR,
    };
    std::vector<uint16_t> menus_led_colorlist = {
        MENU_COLOR_MAROON,
        MENU_COLOR_DARK_RED,
        MENU_COLOR_BROWN,
        MENU_COLOR_FIREBRICK,
        MENU_COLOR_CRIMSON,
        MENU_COLOR_RED,
        MENU_COLOR_TOMATO,
        MENU_COLOR_CORAL,
        MENU_COLOR_INDIAN_RED,
        MENU_COLOR_LIGHT_CORAL,
        MENU_COLOR_DARK_SALMON,
        MENU_COLOR_SALMON,
        MENU_COLOR_LIGHT_SALMON,
        MENU_COLOR_ORANGE_RED,
        MENU_COLOR_DARK_ORANGE,
        MENU_COLOR_ORANGE,
        MENU_COLOR_GOLD,
        MENU_COLOR_DARK_GOLDEN_ROD,
        MENU_COLOR_GOLDEN_ROD,
        MENU_COLOR_PALE_GOLDEN_ROD,
        MENU_COLOR_DARK_KHAKI,
        MENU_COLOR_KHAKI,
        MENU_COLOR_OLIVE,
        MENU_COLOR_YELLOW,
        MENU_COLOR_YELLOW_GREEN,
        MENU_COLOR_DARK_OLIVE_GREEN,
        MENU_COLOR_OLIVE_DRAB,
        MENU_COLOR_LAWN_GREEN,
        MENU_COLOR_CHARTREUSE,
        MENU_COLOR_GREEN_YELLOW,
        MENU_COLOR_DARK_GREEN,
        MENU_COLOR_GREEN,
        MENU_COLOR_FOREST_GREEN,
        MENU_COLOR_LIME,
        MENU_COLOR_LIME_GREEN,
        MENU_COLOR_LIGHT_GREEN,
        MENU_COLOR_PALE_GREEN,
        MENU_COLOR_DARK_SEA_GREEN,
        MENU_COLOR_MEDIUM_SPRING_GREEN,
        MENU_COLOR_SPRING_GREEN,
        MENU_COLOR_SEA_GREEN,
        MENU_COLOR_MEDIUM_AQUA_MARINE,
        MENU_COLOR_MEDIUM_SEA_GREEN,
        MENU_COLOR_LIGHT_SEA_GREEN,
        MENU_COLOR_DARK_SLATE_GRAY,
        MENU_COLOR_TEAL,
        MENU_COLOR_DARK_CYAN,
        MENU_COLOR_AQUA,
        MENU_COLOR_CYAN,
        MENU_COLOR_LIGHT_CYAN,
        MENU_COLOR_DARK_TURQUOISE,
        MENU_COLOR_TURQUOISE,
        MENU_COLOR_MEDIUM_TURQUOISE,
        MENU_COLOR_PALE_TURQUOISE,
        MENU_COLOR_AQUA_MARINE,
        MENU_COLOR_POWDER_BLUE,
        MENU_COLOR_CADET_BLUE,
        MENU_COLOR_STEEL_BLUE,
        MENU_COLOR_CORN_FLOWER_BLUE,
        MENU_COLOR_DEEP_SKY_BLUE,
        MENU_COLOR_DODGER_BLUE,
        MENU_COLOR_LIGHT_BLUE,
        MENU_COLOR_SKY_BLUE,
        MENU_COLOR_LIGHT_SKY_BLUE,
        MENU_COLOR_MIDNIGHT_BLUE,
        MENU_COLOR_NAVY,
        MENU_COLOR_DARK_BLUE,
        MENU_COLOR_MEDIUM_BLUE,
        MENU_COLOR_BLUE,
        MENU_COLOR_ROYAL_BLUE,
        MENU_COLOR_BLUE_VIOLET,
        MENU_COLOR_INDIGO,
        MENU_COLOR_DARK_SLATE_BLUE,
        MENU_COLOR_SLATE_BLUE,
        MENU_COLOR_MEDIUM_SLATE_BLUE,
        MENU_COLOR_MEDIUM_PURPLE,
        MENU_COLOR_DARK_MAGENTA,
        MENU_COLOR_DARK_VIOLET,
        MENU_COLOR_DARK_ORCHID,
        MENU_COLOR_MEDIUM_ORCHID,
        MENU_COLOR_PURPLE,
        MENU_COLOR_THISTLE,
        MENU_COLOR_PLUM,
        MENU_COLOR_VIOLET,
        MENU_COLOR_MAGENTA,
        MENU_COLOR_ORCHID,
        MENU_COLOR_MEDIUM_VIOLET_RED,
        MENU_COLOR_PALE_VIOLET_RED,
        MENU_COLOR_DEEP_PINK,
        MENU_COLOR_HOT_PINK,
        MENU_COLOR_LIGHT_PINK,
        MENU_COLOR_PINK,
        MENU_COLOR_ANTIQUE_WHITE,
        MENU_COLOR_BEIGE,
        MENU_COLOR_BISQUE,
        MENU_COLOR_BLANCHED_ALMOND,
        MENU_COLOR_WHEAT,
        MENU_COLOR_CORN_SILK,
        MENU_COLOR_LEMON_CHIFFON,
        MENU_COLOR_LIGHT_GOLDEN_ROD_YELLOW,
        MENU_COLOR_LIGHT_YELLOW,
        MENU_COLOR_SADDLE_BROWN,
        MENU_COLOR_SIENNA,
        MENU_COLOR_CHOCOLATE,
        MENU_COLOR_PERU,
        MENU_COLOR_SANDY_BROWN,
        MENU_COLOR_BURLY_WOOD,
        MENU_COLOR_TAN,
        MENU_COLOR_ROSY_BROWN,
        MENU_COLOR_MOCCASIN,
        MENU_COLOR_NAVAJO_WHITE,
        MENU_COLOR_PEACH_PUFF,
        MENU_COLOR_MISTY_ROSE,
        MENU_COLOR_LAVENDER_BLUSH,
        MENU_COLOR_LINEN,
        MENU_COLOR_OLD_LACE,
        MENU_COLOR_PAPAYA_WHIP,
        MENU_COLOR_SEA_SHELL,
        MENU_COLOR_MINT_CREAM,
        MENU_COLOR_SLATE_GRAY,
        MENU_COLOR_LIGHT_SLATE_GRAY,
        MENU_COLOR_LIGHT_STEEL_BLUE,
        MENU_COLOR_LAVENDER,
        MENU_COLOR_FLORAL_WHITE,
        MENU_COLOR_ALICE_BLUE,
        MENU_COLOR_GHOST_WHITE,
        MENU_COLOR_HONEY_DEW,
        MENU_COLOR_IVORY,
        MENU_COLOR_AZURE,
        MENU_COLOR_SNOW,
        MENU_COLOR_BLACK,
        MENU_COLOR_DIM_GRAY,
        MENU_COLOR_GRAY,
        MENU_COLOR_DARK_GRAY,
        MENU_COLOR_SILVER,
        MENU_COLOR_LIGHT_GRAY,
        MENU_COLOR_GAINSBORO,
        MENU_COLOR_WHITE_SMOKE,
        MENU_COLOR_WHITE};
    std::vector<uint16_t> menus_led_presets = {
        // MENU_LED_PRESET_TOGGLE,
        MENU_LED_RWAVE,
        MENU_LED_RAINBOW,
        MENU_LED_RPULSE,
        MENU_LED_CSHIFT,
        MENU_LED_CPULSE,
        MENU_LED_ASTATIC,
        MENU_LED_ABLINK,
        MENU_LED_APULSE,
    };
    std::vector<uint16_t> menus_led_ambients = {
        MENU_LED_AMBIENT_TOGGLE,
        MENU_LED_ABLINK,
        MENU_LED_APULSE,
    };
    std::vector<uint16_t> menus_led_stat = {
        MENU_LED_STATUS_TOGGLE,
    };
    std::vector<uint16_t> menus_wifi = {
        MENU_WIFI_NETWORK_ENABLED,
        MENU_WIFI_NETWORK_SCAN,
        MENU_WIFI_NETWORKS_NEARBY,
        MENU_WIFI_PASSWORD,
        MENU_WIFI_PASSWORD_NETWORK_JOIN,
        MENU_WIFI_PASSWORD_DELETE_STORED,
        MENU_WIFI_HOSTNAME,
        MENU_WIFI_IP_ADDR,
        MENU_WIFI_MAC_ADDR,
        MENU_WIFI_STATIC_ADDR,
    };
    std::vector<uint16_t> menus_static_addr = {
        MENU_WIFI_STATIC_ENABLED,
        MENU_WIFI_STATIC_IP,
        MENU_WIFI_STATIC_GATEWAY,
        MENU_WIFI_STATIC_NETMASK,
        MENU_WIFI_STATIC_DNS1,
        MENU_WIFI_STATIC_DNS2,
    };
    std::vector<uint16_t> menus_wifi_password = {
        MENU_WIFI_PASSWORD_EDIT,
        MENU_WIFI_PASSWORD_SAVE,
    };
    std::vector<uint16_t> menus_wifi_hostname = {
        MENU_WIFI_HOSTNAME_EDIT,
        MENU_WIFI_HOSTNAME_SAVE,
    };
    std::vector<uint16_t> menus_ble = {
        MENU_BLE_ENABLE,
        // MENU_BLE_MONITOR,  // what this do?
    };
    std::vector<uint16_t> menus_usb = {
        MENU_USB_ENABLE,
        // MENU_USB_MONITOR,  // we don't need this for usb?
        // MENU_USB_DISCOVER, // we don't need this for usb?
    };
    std::vector<uint16_t> menus_power = {
        MENU_SYS_RESET,
        // MENU_SYS_POWER_OFF, // this option is not avaiable...
    };
    std::vector<uint16_t> menus_clock = {
        MENU_RTC_CURRENT,
        MENU_RTC_DATE_SET,
        MENU_RTC_TIME_SET,
        MENU_RTC_ALARM,
        MENU_RTC_TIMER,
    };
    std::vector<uint16_t> menus_rtc_date = {
        MENU_RTC_SET_DATE_YEAR,
        MENU_RTC_SET_DATE_MONTH,
        MENU_RTC_SET_DATE_DAY,
    };
    std::vector<uint16_t> menus_rtc_time = {
        MENU_RTC_SET_TIME_HOUR,
        MENU_RTC_SET_TIME_MINUTE,
        MENU_RTC_SET_TIME_SECOND,
    };

    std::vector<uint16_t> menus_date_years = {};
    std::vector<uint16_t> menus_date_months = {};
    std::vector<uint16_t> menus_date_days = {};

    int j = 0;
    for (int t = MENU_SPAN_75_START; t < MENU_SPAN_75_END; t++)
    {
        menus_date_years.push_back(t);

        char b[6];
        snprintf(b, 6, "%d", j++);
        menuNames.insert(std::pair<int, String>(t, b));
    }
    for (int t = MENU_SPAN_12_START; t < MENU_SPAN_12_END; t++)
    {
        menus_date_months.push_back(t);
    }
    for (int t = MENU_SPAN_31_START; t < MENU_SPAN_31_END; t++)
    {
        menus_date_days.push_back(t);
    }

    std::vector<uint16_t> menus_time_hours = {};
    std::vector<uint16_t> menus_time_minutes = {};
    std::vector<uint16_t> menus_time_seconds = {};

    for (int t = MENU_SPAN_24_START; t < MENU_SPAN_24_END; t++)
    {
        menus_time_hours.push_back(t);
    }
    for (int t = MENU_SPAN_60_START; t < MENU_SPAN_60_END; t++)
    {
        menus_time_minutes.push_back(t);
    }
    for (int t = MENU_SPAN_60_START; t < MENU_SPAN_60_END; t++)
    {
        menus_time_seconds.push_back(t);
    }

    std::vector<uint16_t> menus_wifi_scanned = {};
    for (int t = 1000; t < 1025; t++)
    {
        menus_wifi_scanned.push_back(t);
    }

    // Root
    menus.push_back(AerMenu(MENU_ROOT_HOME, 0, menus_root));
    menus.push_back(AerMenu(MENU_ROOT_MAIN, MENU_ROOT_HOME, menus_main));
    // PID
    menus.push_back(AerMenu(MENU_MAIN_PID, MENU_ROOT_MAIN, menus_pid));
    menus.push_back(AerMenu(MENU_PID_P, MENU_MAIN_PID, {}));
    menus.push_back(AerMenu(MENU_PID_I, MENU_MAIN_PID, {}));
    menus.push_back(AerMenu(MENU_PID_D, MENU_MAIN_PID, {}));
    // PID Tuning
    menus.push_back(AerMenu(MENU_PID_TUNING, MENU_MAIN_PID, menus_pid_tuning));
    menus.push_back(AerMenu(MENU_PID_AUTO, MENU_PID_TUNING, {MENU_PID_AUTO_TOGGLE}));
    menus.push_back(AerMenu(MENU_PID_PWM_BIAS, MENU_PID_TUNING, {MENU_PID_PWM_BIAS_VAR}));
    menus.push_back(AerMenu(MENU_PID_PWM_FACTOR, MENU_PID_TUNING, {MENU_PID_PWM_FACTOR_VAR}));
    menus.push_back(AerMenu(MENU_PID_WINDUP_LIMIT, MENU_PID_TUNING, {MENU_PID_WINDUP_LIMIT_VAR}));
    menus.push_back(AerMenu(MENU_PID_PWM_CYCLE, MENU_PID_TUNING, {MENU_PID_PWM_CYCLE_VAR}));
    menus.push_back(AerMenu(MENU_PID_PWM_FREQ, MENU_PID_TUNING, {MENU_PID_PWM_FREQ_VAR}));
    // Favs
    menus.push_back(AerMenu(MENU_MAIN_FAVS, MENU_ROOT_MAIN, menus_favs));
    menus.push_back(AerMenu(MENU_FAV_1, MENU_MAIN_FAVS, menus_fav1));
    menus.push_back(AerMenu(MENU_FAV_2, MENU_MAIN_FAVS, menus_fav2));
    menus.push_back(AerMenu(MENU_FAV_3, MENU_MAIN_FAVS, menus_fav3));
    menus.push_back(AerMenu(MENU_FAV_4, MENU_MAIN_FAVS, menus_fav4));
    menus.push_back(AerMenu(MENU_FAV_1N, MENU_FAV_1, menus_fav_names));
    menus.push_back(AerMenu(MENU_FAV_2N, MENU_FAV_2, menus_fav_names));
    menus.push_back(AerMenu(MENU_FAV_3N, MENU_FAV_3, menus_fav_names));
    menus.push_back(AerMenu(MENU_FAV_4N, MENU_FAV_4, menus_fav_names));
    menus.push_back(AerMenu(MENU_FAV_1T, MENU_FAV_1, {}));
    menus.push_back(AerMenu(MENU_FAV_2T, MENU_FAV_2, {}));
    menus.push_back(AerMenu(MENU_FAV_3T, MENU_FAV_3, {}));
    menus.push_back(AerMenu(MENU_FAV_4T, MENU_FAV_4, {}));
    // Bump
    menus.push_back(AerMenu(MENU_MAIN_BUMP, MENU_ROOT_MAIN, menus_bump));
    menus.push_back(AerMenu(MENU_BUMP_AMOUNT, MENU_MAIN_BUMP, {}));
    menus.push_back(AerMenu(MENU_BUMP_TIME, MENU_MAIN_BUMP, {}));
    // Graphs
    menus.push_back(AerMenu(MENU_MAIN_GRAPH, MENU_ROOT_MAIN, menus_graphs));
#if AERPID_COUNT == 2
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE, MENU_MAIN_GRAPH, {}));
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE_ZOOM, MENU_MAIN_GRAPH, {}));
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE_LONG, MENU_MAIN_GRAPH, {}));
#endif
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE_0, MENU_MAIN_GRAPH, {}));
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE_ZOOM_0, MENU_MAIN_GRAPH, {}));
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE_LONG_0, MENU_MAIN_GRAPH, {}));
#if AERPID_COUNT == 2
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE_1, MENU_MAIN_GRAPH, {}));
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE_ZOOM_1, MENU_MAIN_GRAPH, {}));
    menus.push_back(AerMenu(MENU_GRAPH_TEMPERATURE_LONG_1, MENU_MAIN_GRAPH, {}));
#endif
    // System
    menus.push_back(AerMenu(MENU_MAIN_SYSTEM, MENU_ROOT_MAIN, menus_system));
    menus.push_back(AerMenu(MENU_SYSTEM_LOCAL_TEMPERATURE, MENU_MAIN_SYSTEM, menus_local_temp));
    menus.push_back(AerMenu(MENU_SYSTEM_KNOB_ADJUST, MENU_MAIN_SYSTEM, menus_knob_adjust));
    menus.push_back(AerMenu(MENU_SYSTEM_TOGGLE_IDLE, MENU_MAIN_SYSTEM, {MENU_SYSTEM_TOGGLE_IDLE_VAR}));
    menus.push_back(AerMenu(MENU_SYSTEM_LENGTH_IDLE, MENU_MAIN_SYSTEM, {MENU_SYSTEM_LENGTH_IDLE_VAR}));
    menus.push_back(AerMenu(MENU_SYSTEM_FACTORY_RESET, MENU_MAIN_SYSTEM, {MENU_SYSTEM_FACTORY_RESET_CONFIRM}));
    menus.push_back(AerMenu(MENU_SYSTEM_UPDATE_CHECK, MENU_MAIN_SYSTEM, {}));
    menus.push_back(AerMenu(MENU_SYSTEM_THEME, MENU_MAIN_SYSTEM, {}));
    // reset confirm
    // menus.push_back(AerMenu(MENU_SYSTEM_FACTORY_RESET_CONFIRM, MENU_SYSTEM_FACTORY_RESET, {}));
    // > temperature
    menus.push_back(AerMenu(MENU_LOCAL_CELSIUS_TOGGLE, MENU_SYSTEM_LOCAL_TEMPERATURE, {}));
    menus.push_back(AerMenu(MENU_LOCAL_TEMPERATURE_1, MENU_SYSTEM_LOCAL_TEMPERATURE, {}));
    menus.push_back(AerMenu(MENU_LOCAL_TEMPERATURE_2, MENU_SYSTEM_LOCAL_TEMPERATURE, {}));
#if AERPID_COUNT == 2
    menus.push_back(AerMenu(MENU_LOCAL_TEMPERATURE_3, MENU_SYSTEM_LOCAL_TEMPERATURE, {}));
    menus.push_back(AerMenu(MENU_LOCAL_FAN_CONTROL, MENU_SYSTEM_LOCAL_TEMPERATURE, {MENU_FAN_CONTROL_ENABLED, MENU_FAN_CONTROL_MODE, MENU_FAN_CONTROL_SPEED}));
#endif
    // > knob adjustment
    menus.push_back(AerMenu(MENU_KNOB_ADJUST_AMOUNT, MENU_SYSTEM_KNOB_ADJUST, {}));
    menus.push_back(AerMenu(MENU_KNOB_ADJUST_INVERT, MENU_SYSTEM_KNOB_ADJUST, {}));
    menus.push_back(AerMenu(MENU_KNOB_DIAGRAM_SHOWN, MENU_SYSTEM_KNOB_ADJUST, {}));
    // > display
    menus.push_back(AerMenu(MENU_MAIN_DISPLAY, MENU_ROOT_MAIN, menus_system_display));
    menus.push_back(AerMenu(MENU_DISPLAY_BACKLIGHT, MENU_MAIN_DISPLAY, menus_display_backlight));
    menus.push_back(AerMenu(MENU_DISPLAY_AUTO_DIM, MENU_MAIN_DISPLAY, {MENU_DISPLAY_AUTO_DIM_VAR}));
    menus.push_back(AerMenu(MENU_DISPLAY_TIMEOUT_DIM, MENU_MAIN_DISPLAY, {MENU_DISPLAY_TIMEOUT_DIM_VAR}));
    menus.push_back(AerMenu(MENU_DISPLAY_BACKLIGHT_VAR, MENU_DISPLAY_BACKLIGHT, {}));
    menus.push_back(AerMenu(MENU_DISPLAY_BACKLIGHT_MIN_VAR, MENU_DISPLAY_BACKLIGHT, {}));
    // menus.push_back(AerMenu(MENU_DISPLAY_SCREENSAVER, MENU_SYSTEM_DISPLAY_SETTINGS, {}));
    // menus.push_back(AerMenu(MENU_DISPLAY_SCREENTIME, MENU_SYSTEM_DISPLAY_SETTINGS, {}));
    // >> Screensaver
    menus.push_back(AerMenu(MENU_DISPLAY_SCREENSAVER, MENU_MAIN_DISPLAY, menus_screen_saver));
    menus.push_back(AerMenu(MENU_SCREENSAVER_TIME, MENU_DISPLAY_SCREENSAVER, {}));
    menus.push_back(AerMenu(MENU_SCREENSAVER_ENABLED, MENU_DISPLAY_SCREENSAVER, {}));
    menus.push_back(AerMenu(MENU_SCREENSAVER_TOGGLE, MENU_DISPLAY_SCREENSAVER, {}));
    menus.push_back(AerMenu(MENU_SCREENSAVER_STARS, MENU_DISPLAY_SCREENSAVER, {}));
    // LED
    menus.push_back(AerMenu(MENU_MAIN_LED, MENU_ROOT_MAIN, menus_led));
    menus.push_back(AerMenu(MENU_LED_SETUP, MENU_MAIN_LED, menus_led_color));
    menus.push_back(AerMenu(MENU_LED_COLOR, MENU_LED_SETUP, menus_led_colorlist));
    menus.push_back(AerMenu(MENU_LED_STATUS, MENU_MAIN_LED, menus_led_stat));
    menus.push_back(AerMenu(MENU_LED_PRESET, MENU_MAIN_LED, menus_led_presets));
    menus.push_back(AerMenu(MENU_LED_BRIGHTNESS, MENU_LED_SETUP, {}));
    // menus.push_back(AerMenu(MENU_LED_AMBIENT, MENU_MAIN_LED, menus_led_ambients));
    //  Wifi
    menus.push_back(AerMenu(MENU_MAIN_WIFI, MENU_ROOT_MAIN, menus_wifi));
    menus.push_back(AerMenu(MENU_WIFI_NETWORK_ENABLED, MENU_MAIN_WIFI, {}));
    menus.push_back(AerMenu(MENU_WIFI_NETWORK_SCAN, MENU_MAIN_WIFI, {}));
    menus.push_back(AerMenu(MENU_WIFI_NETWORKS_NEARBY, MENU_MAIN_WIFI, menus_wifi_scanned));
    menus.push_back(AerMenu(MENU_WIFI_PASSWORD, MENU_MAIN_WIFI, menus_wifi_password));
    menus.push_back(AerMenu(MENU_WIFI_PASSWORD_NETWORK_JOIN, MENU_MAIN_WIFI, {}));
    menus.push_back(AerMenu(MENU_WIFI_PASSWORD_NETWORK_JOIN_ERROR, MENU_MAIN_WIFI, {}));
    menus.push_back(AerMenu(MENU_WIFI_PASSWORD_DELETE_STORED_ACK, MENU_MAIN_WIFI, {}));
    menus.push_back(AerMenu(MENU_WIFI_PASSWORD_DELETE_STORED, MENU_MAIN_WIFI, {}));
    menus.push_back(AerMenu(MENU_WIFI_HOSTNAME, MENU_MAIN_WIFI, menus_wifi_hostname));
    menus.push_back(AerMenu(MENU_WIFI_IP_ADDR, MENU_MAIN_WIFI, {}));
    menus.push_back(AerMenu(MENU_WIFI_MAC_ADDR, MENU_MAIN_WIFI, {}));
    menus.push_back(AerMenu(MENU_WIFI_STATIC_ADDR, MENU_MAIN_WIFI, menus_static_addr));
    // Static IPAddress
    menus.push_back(AerMenu(MENU_WIFI_STATIC_ENABLED, MENU_WIFI_STATIC_ADDR, {MENU_WIFI_STATIC_ENABLED_VAR}));
    menus.push_back(AerMenu(MENU_WIFI_STATIC_IP, MENU_WIFI_STATIC_ADDR, {MENU_WIFI_STATIC_IP_EDIT, MENU_WIFI_STATIC_IP_SAVE}));
    menus.push_back(AerMenu(MENU_WIFI_STATIC_GATEWAY, MENU_WIFI_STATIC_ADDR, {MENU_WIFI_STATIC_GATEWAY_EDIT, MENU_WIFI_STATIC_GATEWAY_SAVE}));
    menus.push_back(AerMenu(MENU_WIFI_STATIC_NETMASK, MENU_WIFI_STATIC_ADDR, {MENU_WIFI_STATIC_NETMASK_EDIT, MENU_WIFI_STATIC_NETMASK_SAVE}));
    menus.push_back(AerMenu(MENU_WIFI_STATIC_DNS1, MENU_WIFI_STATIC_ADDR, {MENU_WIFI_STATIC_DNS1_EDIT, MENU_WIFI_STATIC_DNS1_SAVE}));
    menus.push_back(AerMenu(MENU_WIFI_STATIC_DNS2, MENU_WIFI_STATIC_ADDR, {MENU_WIFI_STATIC_DNS1_EDIT, MENU_WIFI_STATIC_DNS2_SAVE}));
    // BLE
    menus.push_back(AerMenu(MENU_MAIN_BLE, MENU_ROOT_MAIN, menus_ble));
    menus.push_back(AerMenu(MENU_BLE_ENABLE, MENU_MAIN_BLE, {}));
    menus.push_back(AerMenu(MENU_BLE_MONITOR, MENU_MAIN_BLE, {}));
    // USB
    menus.push_back(AerMenu(MENU_MAIN_USB, MENU_ROOT_MAIN, menus_usb));
    menus.push_back(AerMenu(MENU_USB_ENABLE, MENU_MAIN_USB, {}));
    menus.push_back(AerMenu(MENU_USB_MONITOR, MENU_MAIN_USB, {}));
    menus.push_back(AerMenu(MENU_USB_DISCOVER, MENU_MAIN_USB, {}));
    // Power
    menus.push_back(AerMenu(MENU_MAIN_POWER, MENU_ROOT_MAIN, menus_power));
    menus.push_back(AerMenu(MENU_SYS_RESET, MENU_MAIN_POWER, {}));
    menus.push_back(AerMenu(MENU_SYS_POWER_OFF, MENU_MAIN_POWER, {}));
    // Clock
    menus.push_back(AerMenu(MENU_MAIN_CLOCK, MENU_ROOT_MAIN, menus_clock));
    menus.push_back(AerMenu(MENU_RTC_CURRENT, MENU_MAIN_CLOCK, {}));
    menus.push_back(AerMenu(MENU_RTC_DATE_SET, MENU_MAIN_CLOCK, menus_rtc_date));
    menus.push_back(AerMenu(MENU_RTC_TIME_SET, MENU_MAIN_CLOCK, menus_rtc_time));
    menus.push_back(AerMenu(MENU_RTC_ALARM, MENU_MAIN_CLOCK, {}));
    menus.push_back(AerMenu(MENU_RTC_TIMER, MENU_MAIN_CLOCK, {}));
    // Clock set Date
    menus.push_back(AerMenu(MENU_RTC_SET_DATE_YEAR, MENU_RTC_DATE_SET, menus_date_years));
    menus.push_back(AerMenu(MENU_RTC_SET_DATE_MONTH, MENU_RTC_DATE_SET, menus_date_months));
    menus.push_back(AerMenu(MENU_RTC_SET_DATE_DAY, MENU_RTC_DATE_SET, menus_date_days));
    // Clock set Time
    menus.push_back(AerMenu(MENU_RTC_SET_TIME_HOUR, MENU_RTC_TIME_SET, menus_time_hours));
    menus.push_back(AerMenu(MENU_RTC_SET_TIME_MINUTE, MENU_RTC_TIME_SET, menus_time_minutes));
    menus.push_back(AerMenu(MENU_RTC_SET_TIME_SECOND, MENU_RTC_TIME_SET, menus_time_seconds));

    // Menu ID's to String Names
    menuNames.insert(std::pair<int, String>(MENU_ROOT, "Root"));
    menuNames.insert(std::pair<int, String>(MENU_ROOT_HOME, "Home Screen"));
    menuNames.insert(std::pair<int, String>(MENU_ROOT_MAIN, "Main Menu"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_PID, "PID Setup"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_FAVS, "Favorites"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_BUMP, "Bump Setup"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_SYSTEM, "System Setup"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_LED, "LED Menu"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_WIFI, "WiFi Setup"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_BLE, "BLE Config"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_USB, "USB Control"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_POWER, "Sys Power"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_CLOCK, "Clock Setup"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_GRAPH, "Graphing"));
    menuNames.insert(std::pair<int, String>(MENU_PID_P, "Proportional"));
    menuNames.insert(std::pair<int, String>(MENU_PID_I, "Integral"));
    menuNames.insert(std::pair<int, String>(MENU_PID_D, "Derivative"));
    menuNames.insert(std::pair<int, String>(MENU_PID_TUNING, "PID Tuning"));
    menuNames.insert(std::pair<int, String>(MENU_PID_AUTO, "Auto Tune"));
    menuNames.insert(std::pair<int, String>(MENU_PID_PWM_FACTOR, "PWM Factor"));
    menuNames.insert(std::pair<int, String>(MENU_PID_PWM_BIAS, "Output Bias"));
    menuNames.insert(std::pair<int, String>(MENU_PID_WINDUP_LIMIT, "WindUp Limit"));
    menuNames.insert(std::pair<int, String>(MENU_PID_PWM_CYCLE, "PWM Cycle"));
    menuNames.insert(std::pair<int, String>(MENU_PID_PWM_FREQ, "PWM Freq"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_1, "Favorite 1"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_1N, "Fav 1: Name"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_1T, "Fav 1: Temp"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_2, "Favorite 2"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_2N, "Fav 2: Name"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_2T, "Fav 2: Temp"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_3, "Favorite 3"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_3N, "Fav 3: Name"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_3T, "Fav 3: Temp"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_4, "Favorite 4"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_4N, "Fav 4: Name"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_4T, "Fav 4: Temp"));
    menuNames.insert(std::pair<int, String>(MENU_BUMP_AMOUNT, "Bump Amount"));
    menuNames.insert(std::pair<int, String>(MENU_BUMP_TIME, "Bump Time"));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_KNOB_ADJUST, "Knob Control"));
    menuNames.insert(std::pair<int, String>(MENU_KNOB_ADJUST_AMOUNT, "Speed  "));
    menuNames.insert(std::pair<int, String>(MENU_KNOB_DIAGRAM_SHOWN, "Diagram"));
    menuNames.insert(std::pair<int, String>(MENU_KNOB_ADJUST_INVERT, "Invert "));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_TOGGLE_IDLE, "Toggle Idle"));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_LENGTH_IDLE, "Idle Length"));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_FACTORY_RESET, "Reset Flash"));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_FACTORY_RESET_CONFIRM, "Factory Reset"));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_UPDATE_CHECK, "Update Check"));
    menuNames.insert(std::pair<int, String>(MENU_MAIN_DISPLAY, "Display Setup"));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_DISPLAY_SETTINGS, "Display Setup"));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_THEME, "Local Theme"));
    menuNames.insert(std::pair<int, String>(MENU_SYSTEM_LOCAL_TEMPERATURE, "Local Thermal"));
    menuNames.insert(std::pair<int, String>(MENU_LOCAL_CELSIUS_TOGGLE, "C/F: "));
    menuNames.insert(std::pair<int, String>(MENU_LOCAL_TEMPERATURE_1, "TEMP 1"));
    menuNames.insert(std::pair<int, String>(MENU_LOCAL_TEMPERATURE_2, "TEMP 2"));
    menuNames.insert(std::pair<int, String>(MENU_LOCAL_TEMPERATURE_3, "TEMP 3"));
    menuNames.insert(std::pair<int, String>(MENU_LOCAL_FAN_CONTROL, "Fan Control"));
    menuNames.insert(std::pair<int, String>(MENU_FAN_CONTROL_ENABLED, "Fan Enable"));
    menuNames.insert(std::pair<int, String>(MENU_FAN_CONTROL_MODE, "Fan Mode"));
    menuNames.insert(std::pair<int, String>(MENU_FAN_CONTROL_SPEED, "Fan Speed"));
    menuNames.insert(std::pair<int, String>(MENU_DISPLAY_BACKLIGHT, "Backlight Set"));
    menuNames.insert(std::pair<int, String>(MENU_DISPLAY_AUTO_DIM, "Dim Toggle"));
    menuNames.insert(std::pair<int, String>(MENU_DISPLAY_TIMEOUT_DIM, "Dim Timeout"));
    menuNames.insert(std::pair<int, String>(MENU_DISPLAY_SCREENSAVER, "Disp Idle"));
    menuNames.insert(std::pair<int, String>(MENU_DISPLAY_SCREENTIME, "LED Timeout")); // ? no longer used...
    menuNames.insert(std::pair<int, String>(MENU_DISPLAY_SYS_TRAY, "SysTray"));
    menuNames.insert(std::pair<int, String>(MENU_SCREENSAVER_TIME, "Time"));
    menuNames.insert(std::pair<int, String>(MENU_SCREENSAVER_ENABLED, "Enabled"));
    menuNames.insert(std::pair<int, String>(MENU_SCREENSAVER_TOGGLE, "Enable Now?"));
    menuNames.insert(std::pair<int, String>(MENU_SCREENSAVER_STARS, "Stars"));
#if AERPID_COUNT == 1
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_0, "Temperature"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_ZOOM_0, "Temp Zoomed"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_LONG_0, "Temp Long"));
#endif
#if AERPID_COUNT == 2
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE, "Temperatures"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_ZOOM, "Temps Zoomed"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_LONG, "Temps Long"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_0, "Temperature 1"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_ZOOM_0, "Temp Zoomed 1"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_LONG_0, "Temp Long 1"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_1, "Temperature 2"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_ZOOM_1, "Temp Zoomed 2"));
    menuNames.insert(std::pair<int, String>(MENU_GRAPH_TEMPERATURE_LONG_1, "Temp Long 2"));
#endif

    menuNames.insert(std::pair<int, String>(MENU_LED_MAIN_TOGGLE, "LEDS:"));
    menuNames.insert(std::pair<int, String>(MENU_LED_SETUP, "Colors"));
    menuNames.insert(std::pair<int, String>(MENU_LED_BRIGHTNESS, "Brightness"));
    menuNames.insert(std::pair<int, String>(MENU_LED_COLOR, "Select Color"));
    // menuNames.insert(std::pair<int, String>(MENU_LED_AMBIENT, "Ambient"));
    menuNames.insert(std::pair<int, String>(MENU_LED_AMBIENT_TOGGLE, "Ambient:"));
    menuNames.insert(std::pair<int, String>(MENU_LED_ASTATIC, "Static Color"));
    menuNames.insert(std::pair<int, String>(MENU_LED_ABLINK, "Ambient Blink"));
    menuNames.insert(std::pair<int, String>(MENU_LED_APULSE, "Ambient Pulse"));
    menuNames.insert(std::pair<int, String>(MENU_LED_PRESET, "Presets"));
    // menuNames.insert(std::pair<int, String>(MENU_LED_PRESET_TOGGLE, "Preset:"));
    menuNames.insert(std::pair<int, String>(MENU_LED_RWAVE, "Rainbow Wave"));
    menuNames.insert(std::pair<int, String>(MENU_LED_RAINBOW, "Rainbow"));
    menuNames.insert(std::pair<int, String>(MENU_LED_RPULSE, "Rainbow Pulse"));
    menuNames.insert(std::pair<int, String>(MENU_LED_CSHIFT, "Color Shift"));
    menuNames.insert(std::pair<int, String>(MENU_LED_CPULSE, "Color Pulse"));
    menuNames.insert(std::pair<int, String>(MENU_LED_STATUS, "Status LED"));
    menuNames.insert(std::pair<int, String>(MENU_LED_STATUS_TOGGLE, "Status:"));

    menuNames.insert(std::pair<int, String>(MENU_COLOR_ALICE_BLUE, "Alice Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_ANTIQUE_WHITE, "Antique White"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_AQUA, "Aqua"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_AQUA_MARINE, "Aqua Marine"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_AZURE, "Azure"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_BEIGE, "Beige"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_BISQUE, "Bisque"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_BLACK, "Black"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_BLANCHED_ALMOND, "Blanched Almond"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_BLUE, "Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_BLUE_VIOLET, "Blue Violet"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_BROWN, "Brown"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_BURLY_WOOD, "Burly Wood"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_CADET_BLUE, "Cade Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_CHARTREUSE, "Chatreuse"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_CHOCOLATE, "Chocolate"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_CORAL, "Coral"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_CORN_FLOWER_BLUE, "Corn Flower Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_CORN_SILK, "Corn Silk"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_CRIMSON, "Crimson"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_CYAN, "Cyan"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_BLUE, "Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_CYAN, "Dark Cyan"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_GOLDEN_ROD, "Dark Golden Rod"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_GRAY, "Dark Gray"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_GREEN, "Dark Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_KHAKI, "Dark Khaki"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_MAGENTA, "Dark Magenta"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_OLIVE_GREEN, "Dark Olive Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_ORANGE, "Dark Orange"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_ORCHID, "Dark Orchid"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_RED, "Dark Red"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_SALMON, "Dark Salmon"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_SEA_GREEN, "Dark Sea Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_SLATE_BLUE, "Dark Slate Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_SLATE_GRAY, "Dark Slate Gray"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_TURQUOISE, "Dark Turquoise"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DARK_VIOLET, "Dark Violet"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DEEP_PINK, "Deep Pink"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DEEP_SKY_BLUE, "Deep Sky Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DIM_GRAY, "Dim Gray"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_DODGER_BLUE, "Dodger Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_FIREBRICK, "Firebrick"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_FLORAL_WHITE, "Floral White"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_FOREST_GREEN, "Forest Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_GAINSBORO, "Gainsboro"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_GHOST_WHITE, "Ghost White"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_GOLD, "Gold"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_GOLDEN_ROD, "Golden Rod"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_GRAY, "Gray"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_GREEN, "Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_GREEN_YELLOW, "Green Yellow"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_HONEY_DEW, "Honey Dew"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_HOT_PINK, "Hot Pink"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_INDIAN_RED, "Indian Red"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_INDIGO, "Indigo"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_IVORY, "Ivory"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_KHAKI, "Khaki"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LAVENDER, "Lavender"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LAVENDER_BLUSH, "Lavender Blush"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LAWN_GREEN, "Lawn Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LEMON_CHIFFON, "Lemon Chiffon"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_BLUE, "Light Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_CORAL, "Light Coral"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_CYAN, "Light Cyan"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_GOLDEN_ROD_YELLOW, "Light Golden Rod Yellow"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_GRAY, "Light Gray"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_GREEN, "Light Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_PINK, "Light Pink"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_SALMON, "Light Salmon"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_SEA_GREEN, "Light Sea Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_SKY_BLUE, "Light Sky Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_SLATE_GRAY, "Light Slate Gray"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_STEEL_BLUE, "Light Steel Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIGHT_YELLOW, "Light Yellow"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIME, "Lime"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LIME_GREEN, "Lime Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_LINEN, "Linen"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MOCCASIN, "Maccasin"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MAGENTA, "Magenta"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MAROON, "Maroon"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_AQUA_MARINE, "Med. Aqua Marine"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_BLUE, "Medium Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_ORCHID, "Medium Orchid"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_PURPLE, "Medium Purple"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_SEA_GREEN, "Medium Sea Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_SLATE_BLUE, "Medium Slate Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_SPRING_GREEN, "Med. Spring Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_TURQUOISE, "Medium Turquoise"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MEDIUM_VIOLET_RED, "Medium Violet Red"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MIDNIGHT_BLUE, "Midnight Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MINT_CREAM, "Mint Cream"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_MISTY_ROSE, "Misty Rose"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_NAVAJO_WHITE, "Navajo White"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_NAVY, "Navy"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_OLD_LACE, "Old Lace"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_OLIVE, "Olive"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_OLIVE_DRAB, "Olive Drab"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_ORANGE, "Orange"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_ORANGE_RED, "Orange Red"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_ORCHID, "Orchid"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PALE_GOLDEN_ROD, "Pale Golden Rod"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PALE_GREEN, "Pale Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PALE_TURQUOISE, "Pale Turquoise"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PALE_VIOLET_RED, "Pale Violet Red"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PAPAYA_WHIP, "Papaya Whip"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PEACH_PUFF, "Peach Puff"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PERU, "Peru"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PINK, "Pink"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PLUM, "Plum"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_POWDER_BLUE, "Powder Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_PURPLE, "Purple"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_RED, "Red"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_ROSY_BROWN, "Rosy Brown"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_ROYAL_BLUE, "Royal Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SADDLE_BROWN, "Saddle Brown"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SALMON, "Salmon"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SANDY_BROWN, "Sandy Brown"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SEA_GREEN, "Sea Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SEA_SHELL, "Sea Shell"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SIENNA, "Sienna"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SILVER, "Silver"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SKY_BLUE, "Sky Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SLATE_BLUE, "Slate Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SLATE_GRAY, "Slate Gray"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SNOW, "Snow"));
    ;
    menuNames.insert(std::pair<int, String>(MENU_COLOR_SPRING_GREEN, "Spring Green"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_STEEL_BLUE, "Steel Blue"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_TAN, "Tan"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_TEAL, "Teal"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_THISTLE, "Thistle"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_TOMATO, "Tomato"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_TURQUOISE, "Turquoise"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_VIOLET, "Violet"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_WHEAT, "Wheat"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_WHITE, "White"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_WHITE_SMOKE, "White Smoke"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_YELLOW, "Yellow"));
    menuNames.insert(std::pair<int, String>(MENU_COLOR_YELLOW_GREEN, "Yellow Green"));
    // menuNames.insert(std::pair<int, String>(MENU_COLOR, ""));

    menuNames.insert(std::pair<int, String>(MENU_WIFI_NETWORK_SCAN, "Scan Network"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_NETWORKS_NEARBY, "Find Nearby"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_NETWORK_ENABLED, "Radio:"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_PASSWORD, "Password"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_HOSTNAME, "Hostname"));
    // menuNames.insert(std::pair<int, String>(MENU_WIFI_HOSTNAME_EDIT, "Edit"));
    // menuNames.insert(std::pair<int, String>(MENU_WIFI_HOSTNAME_SAVE, "Save"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_PASSWORD_NETWORK_JOIN, "Join Network"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_PASSWORD_DELETE_STORED, "Delete Net"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_PASSWORD_NETWORK_JOIN_ERROR, "Net Error"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_PASSWORD_EDIT, "Edit"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_PASSWORD_SAVE, "Save"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_STATIC_ADDR, "Static IP"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_STATIC_ENABLED, "Static Enb"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_STATIC_IP, "IP Addr"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_STATIC_GATEWAY, "Gateway"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_STATIC_NETMASK, "NetMask"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_STATIC_DNS1, "DNS 1"));
    menuNames.insert(std::pair<int, String>(MENU_WIFI_STATIC_DNS2, "DNS 2"));
    menuNames.insert(std::pair<int, String>(MENU_BLE_ENABLE, "Enabled"));
    menuNames.insert(std::pair<int, String>(MENU_BLE_MONITOR, "Monitor"));
    menuNames.insert(std::pair<int, String>(MENU_USB_ENABLE, "Enabled"));
    menuNames.insert(std::pair<int, String>(MENU_USB_MONITOR, "Monitor"));
    menuNames.insert(std::pair<int, String>(MENU_USB_DISCOVER, "Discover"));
    menuNames.insert(std::pair<int, String>(MENU_SYS_RESET, "Reset"));
    menuNames.insert(std::pair<int, String>(MENU_SYS_POWER_OFF, "Power Off"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_CURRENT, "DateTime"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_DATE_SET, "Date Set"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_TIME_SET, "Time Set"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_ALARM, "Alarm Setup"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_TIMER, "Timer Setup"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_SET_DATE_YEAR, "Year"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_SET_DATE_MONTH, "Month"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_SET_DATE_DAY, "Day"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_SET_TIME_HOUR, "Hour"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_SET_TIME_MINUTE, "Minute"));
    menuNames.insert(std::pair<int, String>(MENU_RTC_SET_TIME_SECOND, "Second"));

    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_FAVORITE1, "Favorite 1"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_FAVORITE2, "Favorite 2"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_FAVORITE3, "Favorite 3"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_FAVORITE4, "Favorite 4"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_HIGH, "High"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_MEDIUM, "Medium"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_LOW, "Low"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_STAYWARM, "Stay Warm"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_MAIN, "Main"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_HOME, "Home"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_CLEAN, "Clean"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_PREHEAT, "Preheat"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_STAGE1, "Stage I"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_STAGE2, "Stage II"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_STAGE3, "Stage III"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_STAGE4, "Stage IV"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_ONE, "One"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_TWO, "Two"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_THREE, "Three"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_FOUR, "Four"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_COOLDOWN, "Cooldown"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_PURGE, "Purge"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_PRESS, "Press"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_SOUSVIDE, "Sous Vide"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_SMOKE, "Smoke"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_TOAST, "Toast"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_DRY, "Dry"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_DEHUMIDIFY, "Dehumidify"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_TEMPER, "Temper"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_BAKE, "Bake"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_SOAK, "Soak"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_REFLOW, "Reflow"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_RED, "Red"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_GREEN, "Green"));
    menuNames.insert(std::pair<int, String>(MENU_FAV_NAME_BLUE, "Blue"));

    return true;
}

// ===========================================================

PropsMenu propsMenu = {1, 0, 0, 0, true, true, "", 0, {0, 0, 0, 0}};

// Initializes the gui class object
void AerGUI::init()
{
    this->st7789 = new AerST7789();

    AerGUI::menuStor = &propsMenu;

    this->buildMenuDefinitions();

    this->lastMenu = new AerMenu();
    this->intialized = true;
}

// Gets the ST7789 Display driver
AerST7789 *AerGUI::getST7789()
{
    return AerGUI::st7789;
}

// Gets the TFT Display Interface
TFT_eSPI *AerGUI::getTFT()
{
    return AerGUI::st7789->getTFT();
}

// Gets the TFT Display Sprite Interface
TFT_eSprite *AerGUI::getSpriteBuffer(uint bufferNum)
{
    return AerGUI::st7789->getSpriteBuffer(bufferNum);
}

// Gets the Menu struct
PropsMenu *AerGUI::getMenuProps()
{
    return AerGUI::menuStor;
}

// ===========================================================

void AerGUI::gotoMenu(uint16_t menu_id, uint16_t menu_val, uint16_t id)
{
    this->lastMenu->select(0, false);
    AerMenu menu = getSelectedMenu(menu_id);
    this->menuStor->menuIndex = menu_id;
    if (menu_val == 0 && menu.getChildren().size() > 0)
    {
        this->menuStor->menuLevelVal = menu.getChildren()[0];
    }
    else
    {
        this->menuStor->menuLevelVal = menu_val;
    }
    this->menuStor->menuLevelId = id;
    this->menuStor->menuItemId = 0;
    this->menuStor->menuChange = true;
    menu.select(menu_id, true);
    this->lastMenu = &menu;
    this->updateMenu();
}

AerMenu AerGUI::getSelectedMenu(uint16_t index)
{
    AerMenu current; // AerMenu();
    for (int i = 0; i < menus.size(); i++)
    {
        if (menus[i].getIndex() == index)
        {
            current = menus[i];
            break;
        }
    }
    return current;
}

void AerGUI::setMenu(uint16_t index, AerMenu menu)
{
    for (int i = 0; i < menus.size(); i++)
    {
        if (menus[i].getIndex() == index)
        {
            menus[i] = menu;
            break;
        }
    }
}

AerMenu AerGUI::getSelectedMenu()
{
    AerMenu current; // AerMenu();
    for (int i = 0; i < menus.size(); i++)
    {
        if (menus[i].getIndex() == menuStor->menuIndex)
        {
            current = menus[i];
            break;
        }
    }
    return current;
}

AerMenu *AerGUI::getCurrentMenu()
{
    return lastMenu;
}

String AerGUI::getMenuName(int index)
{
    return this->menuNames[index];
}

void AerGUI::updateDisplayMenu(uint16_t index, uint16_t level)
{
    AerMenu current = getSelectedMenu(index);
    if (current.getIndex() > 0)
    {
        if (current.getChildren().size() == 0)
        {
            return;
        }
        menuStor->menuUpdate = true;
        menuStor->menuChange = true;
        menuStor->menuIndex = index;
        menuStor->menuLevelVal = level;
        if (level == 0 && current.getChildren().size() > 0)
        {
            menuStor->menuLevelVal = current.getChildren()[0];
            menuStor->menuLevelId = 0;
            menuStor->menuItemId = 0;
        }
        // select level index
        current.select(level, true);
        // deselect prior
        this->lastMenu->select(0, false);
        this->lastMenu = &current;

        // NOTE: debug outputs here may cause a core panic!!!
        // Serial.print("Update Selected to INDEX: ");
        // Serial.print(index, 10);
        // Serial.print("  LEVEL: ");
        // Serial.print(level, 10);
        // Serial.println(" ...");
    }
    else if (current.getIndex() == 0)
    {
        this->lastMenu = &current;
        menuStor->menuUpdate = true;
        menuStor->menuChange = true;
        menuStor->menuIndex = 1;
        menuStor->menuLevelVal = 0;
        menuStor->menuLevelId = 0;
        menuStor->menuItemId = 0;
    }
}

void AerGUI::updateDisplayMenuLevel(uint16_t level)
{
    AerMenu current = getSelectedMenu(menuStor->menuIndex);
    if (current.getIndex() > 0 && current.getParent() > 0)
    {
        menuStor->menuLevelVal = level;
        menuStor->menuUpdate = true;
        // select levle index
        current.select(level, true);

        // Serial.print("Update Selected INDEX: ");
        // Serial.print(menuStor->menuIndex, 10);
        // Serial.print("  to LEVEL: ");
        // Serial.print(menuStor->menuLevelVal, 10);
        // Serial.println(" ...");
    }
}

bool AerGUI::selectMenuLevel()
{
    AerMenu current = getSelectedMenu(menuStor->menuIndex);
    if (current.getIndex() > 0)
    {
        uint16_t selected = menuStor->menuLevelVal;
        if (selected < 1)
        {
            selected = (uint16_t)MENU_ROOT_MAIN;
        }
        updateDisplayMenu(selected, 0);
        return true;
    }
    return false;
}

void AerGUI::updateMenu(bool change)
{
    menuStor->menuUpdate = true;
    if (change)
    {
        menuStor->menuChange = true;
    }
}

bool AerGUI::incrDisplayMenuLevel()
{
    uint16_t sel = menuStor->menuLevelId;
    if (sel >= menus.size())
    {
        menuStor->menuIndex = 0;
    }
    AerMenu current = getSelectedMenu(menuStor->menuIndex);
    if (current.getIndex() > 0)
    {
        if (sel + 1 < current.getLevels())
        {
            menuStor->menuLevelId = sel + 1;
            uint16_t i = 0;
            if (current.getChildren().size() > 0)
            {
                i = current.getChildren()[menuStor->menuLevelId];
            }
            // Serial.print(" +++");
            // Serial.println(i);
            updateDisplayMenuLevel(i);
            return true;
        }
    }
    return false;
}

bool AerGUI::decrDisplayMenuLevel()
{
    uint16_t sel = menuStor->menuLevelId;
    if (sel >= menus.size())
    {
        menuStor->menuIndex = 0;
    }
    AerMenu current = getSelectedMenu(menuStor->menuIndex);
    if (current.getIndex() > 0)
    {
        if (sel - 1 >= 0)
        {
            menuStor->menuLevelId = sel - 1;
            uint16_t i = 1;
            if (current.getChildren().size() > 0)
            {
                i = current.getChildren()[menuStor->menuLevelId];
            }
            // Serial.print(" ---");
            // Serial.println(i);
            updateDisplayMenuLevel(i);
            return true;
        }
    }
    return false;
}

// ====================================================

void AerGUI::setCursorModify(uint16_t cursor)
{
    cursorModify = cursor;
}

void AerGUI::clearCursorModify()
{
    cursorModify = 0;
}

uint16_t AerGUI::getCursorModify()
{
    return cursorModify;
}

bool AerGUI::isCursorModify()
{
    return cursorModify > 0;
}

// ====================================================

void AerGUI::setElementIndex(uint8_t ei)
{
    elementIndex = ei;
}

uint8_t AerGUI::getElementIndex()
{
    return elementIndex;
}

// ====================================================

void AerGUI::printIcon(uint x, uint y, uint16_t menuIndex, bool selected)
{
    TFT_eSPI *tft = this->getTFT();

    if (menuIndex >= MENU_SPAN_75_START && menuIndex <= MENU_SPAN_75_END)
    {
        tft->fillCircle(x + 14, y + 14, 6, TFT_GOLD);
        return;
    }

    switch (menuIndex)
    {
    case MENU_ROOT:
    case MENU_ROOT_HOME:
        tft->pushImage(x, y, 28, 28, image_data_home, 0xffff);
        break;
    case MENU_ROOT_MAIN:
        tft->pushImage(x, y, 28, 28, image_data_home_sel, 0xffff);
        break;
    case MENU_MAIN_PID:
    case MENU_PID_TUNING:
        tft->pushImage(x, y, 28, 28, image_data_pid, 0xffff);
        break;
    case MENU_PID_P:
    case MENU_PID_I:
    case MENU_PID_D:
        tft->fillCircle(x + 14, y + 14, 6, TFT_BLUE);
        break;
    case MENU_MAIN_FAVS:
        tft->pushImage(x, y, 28, 28, image_data_favs_sel, 0xffff);
        break;
    case MENU_LED_BRIGHTNESS:
        tft->pushImage(x, y, 28, 28, image_data_bulb_glow, 0xffff);
        break;
    case MENU_MAIN_BUMP:
        tft->pushImage(x, y, 28, 28, image_data_bump, 0xffff);
        break;
    case MENU_BUMP_AMOUNT:
        tft->pushImage(x, y, 28, 28, image_data_bump_num, 0xffff);
        break;
    case MENU_BUMP_TIME:
        tft->pushImage(x, y, 28, 28, image_data_bump_time, 0xffff);
        break;
    case MENU_LED_COLOR:
        tft->pushImage(x, y, 28, 28, image_data_theme, 0xffff);
        break;
    case MENU_MAIN_GRAPH:
    case MENU_GRAPH_TEMPERATURE:
    case MENU_GRAPH_TEMPERATURE_ZOOM:
    case MENU_GRAPH_TEMPERATURE_LONG:
    case MENU_GRAPH_TEMPERATURE_0:
    case MENU_GRAPH_TEMPERATURE_ZOOM_0:
    case MENU_GRAPH_TEMPERATURE_LONG_0:
    case MENU_GRAPH_TEMPERATURE_1:
    case MENU_GRAPH_TEMPERATURE_ZOOM_1:
    case MENU_GRAPH_TEMPERATURE_LONG_1:
        tft->pushImage(x, y, 28, 28, image_data_graphs, 0xffff);
        break;
    case MENU_LED_SETUP:
        tft->pushImage(x, y, 28, 28, image_data_theme, 0xffff);
        break;
    case MENU_LED_STATUS:
    case MENU_MAIN_SYSTEM:
        tft->pushImage(x, y, 28, 28, image_data_sys, 0xffff);
        break;
    // case MENU_LED_AMBIENT:
    case MENU_LED_PRESET:
    case MENU_LED_RWAVE:
    case MENU_LED_RAINBOW:
    case MENU_LED_RPULSE:
    case MENU_LED_CSHIFT:
    case MENU_LED_CPULSE:
    case MENU_LED_ASTATIC:
    case MENU_LED_ABLINK:
    case MENU_LED_APULSE:
    case MENU_MAIN_LED:
        tft->pushImage(x, y, 28, 28, image_data_leds, 0xffff);
        break;
    case MENU_MAIN_WIFI:
        tft->pushImage(x, y, 28, 28, image_data_wifi, 0xffff);
        break;
    case MENU_MAIN_BLE:
    case MENU_BLE_ENABLE:
        tft->pushImage(x, y, 28, 28, image_data_ble, 0xffff);
        break;
    case MENU_MAIN_USB:
    case MENU_USB_ENABLE:
        tft->pushImage(x, y, 28, 28, image_data_usb, 0xffff);
        break;
    case MENU_LED_STATUS_TOGGLE:
    case MENU_LED_AMBIENT_TOGGLE:
    // case MENU_LED_PRESET_TOGGLE:
    case MENU_LED_MAIN_TOGGLE:
    case MENU_MAIN_POWER:
        tft->pushImage(x, y, 28, 28, image_data_power, 0xffff);
        break;
    case MENU_MAIN_CLOCK:
        tft->pushImage(x, y, 28, 28, image_data_clock, 0xffff);
        break;
    case MENU_RTC_DATE_SET:
    case MENU_RTC_TIME_SET:
        tft->pushImage(x, y, 28, 28, image_data_clock2, 0xffff);
        break;
    case MENU_RTC_ALARM:
        tft->pushImage(x, y, 28, 28, image_data_alarm, 0xffff);
        break;
    case MENU_RTC_TIMER:
        tft->pushImage(x, y, 28, 28, image_data_timer, 0xffff);
        break;
    case MENU_RTC_SET_DATE_YEAR:
    case MENU_RTC_SET_DATE_MONTH:
    case MENU_RTC_SET_DATE_DAY:
    case MENU_RTC_SET_TIME_HOUR:
    case MENU_RTC_SET_TIME_MINUTE:
    case MENU_RTC_SET_TIME_SECOND:
        tft->fillCircle(x + 14, y + 14, 6, TFT_BLACK);
        break;
    case MENU_SYSTEM_FACTORY_RESET:
    case MENU_SYSTEM_FACTORY_RESET_CONFIRM:
        tft->pushImage(x, y, 28, 28, image_data_reset, 0xffff);
        break;
    case MENU_SYSTEM_LOCAL_TEMPERATURE:
        tft->pushImage(x, y, 28, 28, image_data_thermal, 0xffff);
        break;
    case MENU_LOCAL_CELSIUS_TOGGLE:
        tft->pushImage(x, y, 28, 28, image_data_thermal2, 0xffff);
        break;
    case MENU_LOCAL_TEMPERATURE_1:
        tft->pushImage(x, y, 28, 28, image_data_thermal_cpu, 0xffff);
        break;
    case MENU_LOCAL_TEMPERATURE_2:
    case MENU_LOCAL_TEMPERATURE_3:
        tft->pushImage(x, y, 28, 28, image_data_thermal_ssr, 0xffff);
        break;
    case MENU_SYSTEM_UPDATE_CHECK:
        tft->pushImage(x, y, 28, 28, image_data_update, 0xffff);
        break;
    case MENU_SYSTEM_DISPLAY_SETTINGS:
        tft->pushImage(x, y, 28, 28, image_data_display, 0xffff);
        break;
    case MENU_WIFI_NETWORK_SCAN:
        tft->pushImage(x, y, 28, 28, image_data_wifi_scn, 0xffff);
        break;
    case MENU_WIFI_NETWORKS_NEARBY:
        tft->pushImage(x, y, 28, 28, image_data_wifi, 0xffff);
        break;
    case MENU_WIFI_PASSWORD:
        tft->pushImage(x, y, 28, 28, image_data_net_pass, 0xffff);
        break;
    case MENU_WIFI_HOSTNAME:
        tft->pushImage(x, y, 28, 28, image_data_net_host, 0xffff);
        break;
    case MENU_WIFI_IP_ADDR:
        tft->pushImage(x, y, 28, 28, image_data_net_ip, 0xffff);
        break;
    case MENU_WIFI_MAC_ADDR:
        tft->pushImage(x, y, 28, 28, image_data_net_mac, 0xffff);
        break;
    case MENU_SYS_RESET:
        tft->pushImage(x, y, 28, 28, image_data_power_off, 0xffff);
        break;
    case MENU_SYSTEM_KNOB_ADJUST:
    case MENU_KNOB_ADJUST_AMOUNT:
    case MENU_KNOB_ADJUST_INVERT:
        tft->pushImage(x, y, 28, 28, image_data_knob, 0xffff);
        break;
    case MENU_SYSTEM_TOGGLE_IDLE:
        tft->pushImage(x, y, 28, 28, image_data_idle_tog, 0xffff);
        break;
    case MENU_SYSTEM_LENGTH_IDLE:
        tft->pushImage(x, y, 28, 28, image_data_idle_len, 0xffff);
        break;
    case MENU_DISPLAY_SCREENSAVER:
    case MENU_SCREENSAVER_STARS:
        tft->pushImage(x, y, 28, 28, image_data_stars, 0xffff);
        break;
    case MENU_SCREENSAVER_TIME:
        tft->pushImage(x, y, 28, 28, image_data_stars_time, 0xffff);
        break;
    case MENU_SCREENSAVER_TOGGLE:
        tft->pushImage(x, y, 28, 28, image_data_stars_bl, 0xffff);
        break;
    case MENU_SCREENSAVER_ENABLED:
        tft->pushImage(x, y, 28, 28, image_data_stars_enb, 0xffff);
        break;
    case MENU_DISPLAY_SYS_TRAY:
        tft->pushImage(x, y, 28, 28, image_data_tray, 0xffff);
        break;
    case MENU_DISPLAY_BACKLIGHT:
    case MENU_DISPLAY_BACKLIGHT_VAR:
        tft->pushImage(x, y, 28, 28, image_data_bulb_blue, 0xffff);
        break;
    case MENU_DISPLAY_BACKLIGHT_MIN_VAR:
        tft->pushImage(x, y, 28, 28, image_data_bulb_red, 0xffff);
        break;
    case MENU_SYSTEM_THEME:
        tft->pushImage(x, y, 28, 28, image_data_theme, 0xffff);
        break;
    case MENU_DISPLAY_AUTO_DIM:
    case MENU_DISPLAY_AUTO_DIM_VAR:
        tft->pushImage(x, y, 28, 28, image_data_bulb_green, 0xffff);
        break;
    case MENU_DISPLAY_TIMEOUT_DIM:
    case MENU_DISPLAY_TIMEOUT_DIM_VAR:
        tft->pushImage(x, y, 28, 28, image_data_bulb_glow, 0xffff);
        break;
    case MENU_PID_AUTO:
    case MENU_PID_AUTO_TOGGLE:
        tft->pushImage(x, y, 28, 28, image_data_pwm_auto, 0xffff);
        break;
    case MENU_PID_PWM_BIAS:
    case MENU_PID_PWM_BIAS_VAR:
    case MENU_PID_PWM_FACTOR:
    case MENU_PID_PWM_FACTOR_VAR:
        tft->pushImage(x, y, 28, 28, image_data_pwm_factor, 0xffff);
        break;
    case MENU_PID_WINDUP_LIMIT:
    case MENU_PID_WINDUP_LIMIT_VAR:
        tft->pushImage(x, y, 28, 28, image_data_pwm_factor, 0xffff);
        break;
    case MENU_PID_PWM_CYCLE:
    case MENU_PID_PWM_CYCLE_VAR:
        tft->pushImage(x, y, 28, 28, image_data_pwm_cycle, 0xffff);
        break;
    case MENU_PID_PWM_FREQ:
    case MENU_PID_PWM_FREQ_VAR:
        tft->pushImage(x, y, 28, 28, image_data_pwm_freq, 0xffff);
        break;
    case MENU_LOCAL_FAN_CONTROL:
        tft->pushImage(x, y, 28, 28, image_data_fan_1, 0xffff);
        break;

    // colors
    case MENU_COLOR_ALICE_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF7DF);
        break;
    case MENU_COLOR_ANTIQUE_WHITE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFF5A);
        break;
    case MENU_COLOR_AQUA:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x07FF);
        break;
    case MENU_COLOR_AQUA_MARINE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x7FFA);
        break;
    case MENU_COLOR_AZURE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF7FF);
        break;
    case MENU_COLOR_BEIGE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF7BB);
        break;
    case MENU_COLOR_BISQUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFF38);
        break;
    case MENU_COLOR_BLACK:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x0000);
        break;
    case MENU_COLOR_BLANCHED_ALMOND:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFF59);
        break;
    case MENU_COLOR_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x001F);
        break;
    case MENU_COLOR_BLUE_VIOLET:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x895C);
        break;
    case MENU_COLOR_BROWN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xA145);
        break;
    case MENU_COLOR_BURLY_WOOD:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xDDD0);
        break;
    case MENU_COLOR_CADET_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x5CF4);
        break;
    case MENU_COLOR_CHARTREUSE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x7FE0);
        break;
    case MENU_COLOR_CHOCOLATE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xD343);
        break;
    case MENU_COLOR_CORAL:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFBEA);
        break;
    case MENU_COLOR_CORN_FLOWER_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x64BD);
        break;
    case MENU_COLOR_CORN_SILK:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFDB);
        break;
    case MENU_COLOR_CRIMSON:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xD8A7);
        break;
    case MENU_COLOR_CYAN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x07FF);
        break;
    case MENU_COLOR_DARK_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x0011);
        break;
    case MENU_COLOR_DARK_CYAN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x0451);
        break;
    case MENU_COLOR_DARK_GOLDEN_ROD:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xBC21);
        break;
    case MENU_COLOR_DARK_GRAY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8410);
        break;
    case MENU_COLOR_DARK_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x0320);
        break;
    case MENU_COLOR_DARK_KHAKI:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xBDAD);
        break;
    case MENU_COLOR_DARK_MAGENTA:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8811);
        break;
    case MENU_COLOR_DARK_OLIVE_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x5345);
        break;
    case MENU_COLOR_DARK_ORANGE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFC60);
        break;
    case MENU_COLOR_DARK_ORCHID:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x9999);
        break;
    case MENU_COLOR_DARK_RED:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8800);
        break;
    case MENU_COLOR_DARK_SALMON:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xECAF);
        break;
    case MENU_COLOR_DARK_SEA_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8DF1);
        break;
    case MENU_COLOR_DARK_SLATE_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x49F1);
        break;
    case MENU_COLOR_DARK_SLATE_GRAY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x2A69);
        break;
    case MENU_COLOR_DARK_TURQUOISE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x067A);
        break;
    case MENU_COLOR_DARK_VIOLET:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x901A);
        break;
    case MENU_COLOR_DEEP_PINK:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF8B2);
        break;
    case MENU_COLOR_DEEP_SKY_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x05FF);
        break;
    case MENU_COLOR_DIM_GRAY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x6B4D);
        break;
    case MENU_COLOR_DODGER_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x1C9F);
        break;
    case MENU_COLOR_FIREBRICK:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xB104);
        break;
    case MENU_COLOR_FLORAL_WHITE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFDE);
        break;
    case MENU_COLOR_FOREST_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x2444);
        break;
    case MENU_COLOR_GAINSBORO:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xDEFB);
        break;
    case MENU_COLOR_GHOST_WHITE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFDF);
        break;
    case MENU_COLOR_GOLD:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFEA0);
        break;
    case MENU_COLOR_GOLDEN_ROD:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xDD24);
        break;
    case MENU_COLOR_GRAY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8410);
        break;
    case MENU_COLOR_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x0400);
        break;
    case MENU_COLOR_GREEN_YELLOW:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xAFE5);
        break;
    case MENU_COLOR_HONEY_DEW:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF7FE);
        break;
    case MENU_COLOR_HOT_PINK:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFB56);
        break;
    case MENU_COLOR_INDIAN_RED:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xCAEB);
        break;
    case MENU_COLOR_INDIGO:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x4810);
        break;
    case MENU_COLOR_IVORY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFFE);
        break;
    case MENU_COLOR_KHAKI:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF731);
        break;
    case MENU_COLOR_LAVENDER:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xE73F);
        break;
    case MENU_COLOR_LAVENDER_BLUSH:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFF9E);
        break;
    case MENU_COLOR_LAWN_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x7FE0);
        break;
    case MENU_COLOR_LEMON_CHIFFON:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFD9);
        break;
    case MENU_COLOR_LIGHT_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xAEDC);
        break;
    case MENU_COLOR_LIGHT_CORAL:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF410);
        break;
    case MENU_COLOR_LIGHT_CYAN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xE7FF);
        break;
    case MENU_COLOR_LIGHT_GOLDEN_ROD_YELLOW:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFDA);
        break;
    case MENU_COLOR_LIGHT_GRAY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xD69A);
        break;
    case MENU_COLOR_LIGHT_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x9772);
        break;
    case MENU_COLOR_LIGHT_PINK:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFDB8);
        break;
    case MENU_COLOR_LIGHT_SALMON:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFD0F);
        break;
    case MENU_COLOR_LIGHT_SEA_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x2595);
        break;
    case MENU_COLOR_LIGHT_SKY_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x867F);
        break;
    case MENU_COLOR_LIGHT_SLATE_GRAY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x7453);
        break;
    case MENU_COLOR_LIGHT_STEEL_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xB63B);
        break;
    case MENU_COLOR_LIGHT_YELLOW:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFFC);
        break;
    case MENU_COLOR_LIME:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x07E0);
        break;
    case MENU_COLOR_LIME_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x3666);
        break;
    case MENU_COLOR_LINEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFF9C);
        break;
    case MENU_COLOR_MOCCASIN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFF36);
        break;
    case MENU_COLOR_MAGENTA:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF81F);
        break;
    case MENU_COLOR_MAROON:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8000);
        break;
    case MENU_COLOR_MEDIUM_AQUA_MARINE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x6675);
        break;
    case MENU_COLOR_MEDIUM_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x0019);
        break;
    case MENU_COLOR_MEDIUM_ORCHID:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xBABA);
        break;
    case MENU_COLOR_MEDIUM_PURPLE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x939B);
        break;
    case MENU_COLOR_MEDIUM_SEA_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x3D8E);
        break;
    case MENU_COLOR_MEDIUM_SLATE_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x7B5D);
        break;
    case MENU_COLOR_MEDIUM_SPRING_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x07D3);
        break;
    case MENU_COLOR_MEDIUM_TURQUOISE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x4E99);
        break;
    case MENU_COLOR_MEDIUM_VIOLET_RED:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xC0B0);
        break;
    case MENU_COLOR_MIDNIGHT_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x18CE);
        break;
    case MENU_COLOR_MINT_CREAM:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF7FF);
        break;
    case MENU_COLOR_MISTY_ROSE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFF3C);
        break;
    case MENU_COLOR_NAVAJO_WHITE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFEF5);
        break;
    case MENU_COLOR_NAVY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x0010);
        break;
    case MENU_COLOR_OLD_LACE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFBC);
        break;
    case MENU_COLOR_OLIVE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8400);
        break;
    case MENU_COLOR_OLIVE_DRAB:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x6C64);
        break;
    case MENU_COLOR_ORANGE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFD20);
        break;
    case MENU_COLOR_ORANGE_RED:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFA20);
        break;
    case MENU_COLOR_ORCHID:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xDB9A);
        break;
    case MENU_COLOR_PALE_GOLDEN_ROD:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xEF55);
        break;
    case MENU_COLOR_PALE_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x9FD3);
        break;
    case MENU_COLOR_PALE_TURQUOISE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xAF7D);
        break;
    case MENU_COLOR_PALE_VIOLET_RED:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xDB92);
        break;
    case MENU_COLOR_PAPAYA_WHIP:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFF7A);
        break;
    case MENU_COLOR_PEACH_PUFF:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFED7);
        break;
    case MENU_COLOR_PERU:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xCC27);
        break;
    case MENU_COLOR_PINK:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFE19);
        break;
    case MENU_COLOR_PLUM:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xDD1B);
        break;
    case MENU_COLOR_POWDER_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xB71C);
        break;
    case MENU_COLOR_PURPLE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8010);
        break;
    case MENU_COLOR_RED:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF800);
        break;
    case MENU_COLOR_ROSY_BROWN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xBC71);
        break;
    case MENU_COLOR_ROYAL_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x435C);
        break;
    case MENU_COLOR_SADDLE_BROWN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x8A22);
        break;
    case MENU_COLOR_SALMON:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFC0E);
        break;
    case MENU_COLOR_SANDY_BROWN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF52C);
        break;
    case MENU_COLOR_SEA_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x2C4A);
        break;
    case MENU_COLOR_SEA_SHELL:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFBD);
        break;
    case MENU_COLOR_SIENNA:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xA285);
        break;
    case MENU_COLOR_SILVER:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xC618);
        break;
    case MENU_COLOR_SKY_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x867D);
        break;
    case MENU_COLOR_SLATE_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x6AD9);
        break;
    case MENU_COLOR_SLATE_GRAY:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x7412);
        break;
    case MENU_COLOR_SNOW:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFDF);
        break;
    case MENU_COLOR_SPRING_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x07EF);
        break;
    case MENU_COLOR_STEEL_BLUE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x4416);
        break;
    case MENU_COLOR_TAN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xD5B1);
        break;
    case MENU_COLOR_TEAL:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x0410);
        break;
    case MENU_COLOR_THISTLE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xDDFB);
        break;
    case MENU_COLOR_TOMATO:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFB08);
        break;
    case MENU_COLOR_TURQUOISE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x471A);
        break;
    case MENU_COLOR_VIOLET:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x895C);
        break;
    case MENU_COLOR_WHEAT:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF6F6);
        break;
    case MENU_COLOR_WHITE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFFF);
        break;
    case MENU_COLOR_WHITE_SMOKE:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xF7BE);
        break;
    case MENU_COLOR_YELLOW:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0xFFE0);
        break;
    case MENU_COLOR_YELLOW_GREEN:
        tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
        tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, 0x9E66);
        break;

    default:
        if (selected)
        {
            tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_GREEN);
            tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, TFT_YELLOW);
        }
        else
        {
            tft->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
            tft->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, TFT_GREEN);
        }
        break;
    }
}

// ====================================================

void AerGUI::printIcon(TFT_eSprite *spr, uint x, uint y, uint16_t menuIndex, bool selected)
{
    if (menuIndex >= MENU_SPAN_75_START && menuIndex <= MENU_SPAN_75_END)
    {
        spr->fillCircle(x + 14, y + 14, 6, TFT_GOLD);
        return;
    }

    switch (menuIndex)
    {
    case MENU_ROOT:
    case MENU_ROOT_HOME:
        spr->pushImage(x, y, 28, 28, image_data_home);
        break;
    case MENU_ROOT_MAIN:
        spr->pushImage(x, y, 28, 28, image_data_home_sel);
        break;
    case MENU_MAIN_PID:
    case MENU_PID_TUNING:
        spr->pushImage(x, y, 28, 28, image_data_pid);
        break;
    case MENU_PID_P:
    case MENU_PID_I:
    case MENU_PID_D:
        spr->fillCircle(x + 14, y + 14, 6, TFT_BLUE);
        break;
    case MENU_MAIN_FAVS:
        spr->pushImage(x, y, 28, 28, image_data_favs_sel);
        break;
    case MENU_LED_BRIGHTNESS:
        spr->pushImage(x, y, 28, 28, image_data_bulb_glow);
        break;
    case MENU_MAIN_BUMP:
        spr->pushImage(x, y, 28, 28, image_data_bump);
        break;
    case MENU_BUMP_AMOUNT:
        spr->pushImage(x, y, 28, 28, image_data_bump_num);
        break;
    case MENU_BUMP_TIME:
        spr->pushImage(x, y, 28, 28, image_data_bump_time);
        break;
    case MENU_LED_COLOR:
        spr->pushImage(x, y, 28, 28, image_data_theme);
        break;
    case MENU_MAIN_GRAPH:
    case MENU_GRAPH_TEMPERATURE:
    case MENU_GRAPH_TEMPERATURE_ZOOM:
    case MENU_GRAPH_TEMPERATURE_LONG:
    case MENU_GRAPH_TEMPERATURE_0:
    case MENU_GRAPH_TEMPERATURE_ZOOM_0:
    case MENU_GRAPH_TEMPERATURE_LONG_0:
    case MENU_GRAPH_TEMPERATURE_1:
    case MENU_GRAPH_TEMPERATURE_ZOOM_1:
    case MENU_GRAPH_TEMPERATURE_LONG_1:
        spr->pushImage(x, y, 28, 28, image_data_graphs);
        break;
    case MENU_LED_SETUP:
        spr->pushImage(x, y, 28, 28, image_data_theme);
        break;
    case MENU_LED_STATUS:
    case MENU_MAIN_SYSTEM:
        spr->pushImage(x, y, 28, 28, image_data_sys);
        break;
    // case MENU_LED_AMBIENT:
    case MENU_LED_PRESET:
    case MENU_LED_RWAVE:
    case MENU_LED_RAINBOW:
    case MENU_LED_RPULSE:
    case MENU_LED_CSHIFT:
    case MENU_LED_CPULSE:
    case MENU_LED_ASTATIC:
    case MENU_LED_ABLINK:
    case MENU_LED_APULSE:
    case MENU_MAIN_LED:
        spr->pushImage(x, y, 28, 28, image_data_leds);
        break;
    case MENU_MAIN_WIFI:
        spr->pushImage(x, y, 28, 28, image_data_wifi);
        break;
    case MENU_MAIN_BLE:
    case MENU_BLE_ENABLE:
        spr->pushImage(x, y, 28, 28, image_data_ble);
        break;
    case MENU_MAIN_USB:
    case MENU_USB_ENABLE:
        spr->pushImage(x, y, 28, 28, image_data_usb);
        break;
    case MENU_LED_STATUS_TOGGLE:
    // case MENU_LED_AMBIENT_TOGGLE:
    // case MENU_LED_PRESET_TOGGLE:
    case MENU_LED_MAIN_TOGGLE:
    case MENU_MAIN_POWER:
        spr->pushImage(x, y, 28, 28, image_data_power);
        break;
    case MENU_MAIN_CLOCK:
        spr->pushImage(x, y, 28, 28, image_data_clock);
        break;
    case MENU_RTC_DATE_SET:
    case MENU_RTC_TIME_SET:
        spr->pushImage(x, y, 28, 28, image_data_clock2);
        break;
    case MENU_RTC_ALARM:
        spr->pushImage(x, y, 28, 28, image_data_alarm);
        break;
    case MENU_RTC_TIMER:
        spr->pushImage(x, y, 28, 28, image_data_timer);
        break;
    case MENU_RTC_SET_DATE_YEAR:
    case MENU_RTC_SET_DATE_MONTH:
    case MENU_RTC_SET_DATE_DAY:
    case MENU_RTC_SET_TIME_HOUR:
    case MENU_RTC_SET_TIME_MINUTE:
    case MENU_RTC_SET_TIME_SECOND:
        spr->fillCircle(x + 14, y + 14, 6, TFT_BLACK);
        break;
    case MENU_SYSTEM_FACTORY_RESET:
    case MENU_SYSTEM_FACTORY_RESET_CONFIRM:
        spr->pushImage(x, y, 28, 28, image_data_reset);
        break;
    case MENU_SYSTEM_LOCAL_TEMPERATURE:
        spr->pushImage(x, y, 28, 28, image_data_thermal);
        break;
    case MENU_LOCAL_CELSIUS_TOGGLE:
        spr->pushImage(x, y, 28, 28, image_data_thermal2);
        break;
    case MENU_LOCAL_TEMPERATURE_1:
        spr->pushImage(x, y, 28, 28, image_data_thermal_cpu);
        break;
    case MENU_LOCAL_TEMPERATURE_2:
    case MENU_LOCAL_TEMPERATURE_3:
        spr->pushImage(x, y, 28, 28, image_data_thermal_ssr);
        break;
    case MENU_SYSTEM_UPDATE_CHECK:
        spr->pushImage(x, y, 28, 28, image_data_update);
        break;
    case MENU_SYSTEM_DISPLAY_SETTINGS:
        spr->pushImage(x, y, 28, 28, image_data_display);
        break;
    case MENU_WIFI_PASSWORD_NETWORK_JOIN:
    case MENU_WIFI_NETWORK_SCAN:
        spr->pushImage(x, y, 28, 28, image_data_wifi_scn);
        break;
    case MENU_WIFI_NETWORKS_NEARBY:
        spr->pushImage(x, y, 28, 28, image_data_wifi);
        break;
    case MENU_WIFI_PASSWORD_DELETE_STORED:
    case MENU_WIFI_NETWORK_ENABLED:
        spr->pushImage(x, y, 28, 28, image_data_wifi_rad);
        break;
    case MENU_WIFI_PASSWORD:
        spr->pushImage(x, y, 28, 28, image_data_net_pass);
        break;
    case MENU_WIFI_HOSTNAME:
        spr->pushImage(x, y, 28, 28, image_data_net_host);
        break;
    case MENU_WIFI_IP_ADDR:
        spr->pushImage(x, y, 28, 28, image_data_net_ip);
        break;
    case MENU_WIFI_MAC_ADDR:
        spr->pushImage(x, y, 28, 28, image_data_net_mac);
        break;
    case MENU_SYS_RESET:
        spr->pushImage(x, y, 28, 28, image_data_power_off);
        break;
    case MENU_SYSTEM_KNOB_ADJUST:
    case MENU_KNOB_ADJUST_AMOUNT:
    case MENU_KNOB_ADJUST_INVERT:
        spr->pushImage(x, y, 28, 28, image_data_knob);
        break;
    case MENU_SYSTEM_TOGGLE_IDLE:
    case MENU_SYSTEM_TOGGLE_IDLE_VAR:
        spr->pushImage(x, y, 28, 28, image_data_idle_tog);
        break;
    case MENU_SYSTEM_LENGTH_IDLE:
    case MENU_SYSTEM_LENGTH_IDLE_VAR:
        spr->pushImage(x, y, 28, 28, image_data_idle_len);
        break;
    case MENU_DISPLAY_SCREENSAVER:
    case MENU_SCREENSAVER_STARS:
        spr->pushImage(x, y, 28, 28, image_data_stars);
        break;
    case MENU_SCREENSAVER_TIME:
        spr->pushImage(x, y, 28, 28, image_data_stars_time);
        break;
    case MENU_SCREENSAVER_TOGGLE:
        spr->pushImage(x, y, 28, 28, image_data_stars_bl);
        break;
    case MENU_SCREENSAVER_ENABLED:
        spr->pushImage(x, y, 28, 28, image_data_stars_enb);
        break;
    case MENU_DISPLAY_SYS_TRAY:
        spr->pushImage(x, y, 28, 28, image_data_tray);
        break;
    case MENU_DISPLAY_BACKLIGHT:
    case MENU_DISPLAY_BACKLIGHT_VAR:
        spr->pushImage(x, y, 28, 28, image_data_bulb_blue);
        break;
    case MENU_DISPLAY_BACKLIGHT_MIN_VAR:
        spr->pushImage(x, y, 28, 28, image_data_bulb_red);
        break;
    case MENU_SYSTEM_THEME:
        spr->pushImage(x, y, 28, 28, image_data_theme);
        break;
    case MENU_DISPLAY_AUTO_DIM:
    case MENU_DISPLAY_AUTO_DIM_VAR:
        spr->pushImage(x, y, 28, 28, image_data_bulb_green);
        break;
    case MENU_DISPLAY_TIMEOUT_DIM:
    case MENU_DISPLAY_TIMEOUT_DIM_VAR:
        spr->pushImage(x, y, 28, 28, image_data_bulb_glow);
        break;
    case MENU_PID_AUTO:
    case MENU_PID_AUTO_TOGGLE:
        spr->pushImage(x, y, 28, 28, image_data_pwm_auto);
        break;
    case MENU_PID_PWM_BIAS:
    case MENU_PID_PWM_BIAS_VAR:
    case MENU_PID_PWM_FACTOR:
    case MENU_PID_PWM_FACTOR_VAR:
        spr->pushImage(x, y, 28, 28, image_data_pwm_factor);
        break;
    case MENU_PID_WINDUP_LIMIT:
    case MENU_PID_WINDUP_LIMIT_VAR:
        spr->pushImage(x, y, 28, 28, image_data_pwm_factor);
        break;
    case MENU_PID_PWM_CYCLE:
    case MENU_PID_PWM_CYCLE_VAR:
        spr->pushImage(x, y, 28, 28, image_data_pwm_cycle);
        break;
    case MENU_PID_PWM_FREQ:
    case MENU_PID_PWM_FREQ_VAR:
        spr->pushImage(x, y, 28, 28, image_data_pwm_freq);
        break;
    case MENU_LOCAL_FAN_CONTROL:
        spr->pushImage(x, y, 28, 28, image_data_fan_1);
        break;

    default:
        if (selected)
        {
            spr->drawRect(x, y, 32 - 4, 32 - 4, TFT_GREEN);
            spr->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, TFT_YELLOW);
        }
        else
        {
            spr->drawRect(x, y, 32 - 4, 32 - 4, TFT_LIGHTGREY);
            spr->fillRect(x + 1, y + 1, 32 - 6, 32 - 6, TFT_GREEN);
        }
        break;
    }
}

char AerGUI::getABC(uint i, bool lower)
{
    if (i >= 26)
    {
        i = 25;
    }
    if (lower)
    {
        return abcLower.c_str()[i];
    }
    else
    {
        return abcUpper.c_str()[i];
    }
}

char AerGUI::getNumber(uint i)
{
    if (i >= 11)
    {
        i = 10;
    }
    return nbrChars.c_str()[i];
}

char AerGUI::getSymbol(uint i, bool adv)
{
    if (i >= 2 && !adv)
    {
        i = 1;
    }
    else if (i >= (2 + 21) && adv)
    {
        i = 12;
    }
    return spcChars.c_str()[i];
}

// get utf8 character from index mappings
char AerGUI::getCharFromIndex(int i, uint type)
{
    if (i < 2 * 26)
    {
        return this->getABC(i >= 26 ? i - 26 : i, i < 26);
    }
    else if (i >= 2 * 26 && i < (2 * 26) + 10)
    {
        return this->getNumber(i - (2 * 26));
    }
    else if (i >= (2 * 26) + 10 && i < (2 * 26) + 10 + 2 + (type == 1 ? 21 : 0))
    {
        const int t = i - ((2 * 26) + 10);
        return this->getSymbol(t, type == 1);
    }
    else
    {
        return '*';
    }
}