// ==================================================
// AerTiny
// ==================================================

// web sockets setup
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// ==================================================
// ==================================================

// chart values
const xValues = [];
for (let i = 0; i < 128; i++) {
  xValues.push(`${i}`);
}

Chart.defaults.global.animation.duration = 0;

// charting
const chart = new Chart('temp_chart', {
  type: 'line',
  animation: false,
  labels: ['0', '16', '32', '48', '64', '80', '96'],
  data: {
    datasets: [
      {
        label: 'Measure',
        data: [],
        borderColor: 'red',
        borderWidth: 1,
        fill: false,
        pointRadius: 1.2
      },
      {
        label: 'Average 1 Minute',
        data: [],
        borderColor: 'orange',
        borderWidth: 2,
        fill: false,
        pointRadius: 1
      },
      {
        label: 'Set Temperature',
        data: [],
        borderColor: 'yellow',
        borderWidth: 2,
        fill: false,
        pointRadius: 1
      },
      {
        label: 'Measure Trend',
        data: [],
        borderColor: '#ff4d0d',
        borderWidth: 1,
        fill: false,
        pointRadius: 0,
        hidden: true
      }
    ]
  },
  options: {
    scales: {
      xAxes: [
        {
          gridLines: {
            color: 'rgba(255, 255, 255, 0.1)' // Set the x-axis grid color to a light color
          }
        }
      ],
      yAxes: [
        {
          gridLines: {
            color: 'rgba(255, 255, 255, 0.08)' // Set the y-axis grid color to a light color
          }
        }
      ]
    }
  }
});

function calculateTrendLine(temps) {
  var a,
    b,
    c,
    d,
    e,
    slope,
    yIntercept = 0;
  var xSum = 0;
  var ySum = 0;
  var xySum = 0;
  var xSquare = 0;
  var dpsLength = temps.length;
  for (var i = 0; i < dpsLength; i++) xySum += Number(temps[i].x) * Number(temps[i].y);
  a = xySum * dpsLength;

  for (var i = 0; i < dpsLength; i++) {
    xSum += Number(temps[i].x);
    ySum += Number(temps[i].y);
  }
  b = xSum * ySum;

  for (var i = 0; i < dpsLength; i++) xSquare += Math.pow(Number(temps[i].x), 2);
  c = dpsLength * xSquare;

  d = Math.pow(xSum, 2);
  slope = (a - b) / (c - d);
  e = slope * xSum;
  yIntercept = (ySum - e) / dpsLength;

  var dps = [];
  for (var i = 0; i < dpsLength; i++) {
    dps.push(getTrendLinePoint(temps[i].x, slope, yIntercept));
  }
  return dps;

  //var startPoint = getTrendLinePoint(temps[0].x, slope, yIntercept);
  //var endPoint = getTrendLinePoint(temps[dpsLength - 1].x, slope, yIntercept);
  //chart.data.datasets[3].data = [startPoint, endPoint];
}

function getTrendLinePoint(x, slope, intercept) {
  return { x: x, y: slope * x + intercept };
}

let zoomRefreshTick = 0;

const updateChart = (state, t, ta) => {
  if (state.TEMPS.length > 128) {
    state.TEMPS.shift();
  }
  state.TEMPS.push(t);

  if (state.TEMPS_AVG.length > 128) {
    state.TEMPS_AVG.shift();
  }
  state.TEMPS_AVG.push(ta);

  const labels = [];
  const temps = [];
  const set_temps = [];
  const avg_temps = [];
  for (let i = 0; i < state.TEMPS.length; i++) {
    temps.push({ x: i, y: state.TEMPS[i] });
    avg_temps.push({ x: i, y: state.TEMPS_AVG[i] });
    set_temps.push({
      x: i,
      y:
        state.UNIT == TemperatureUnit.FAHRENHEIT
          ? cToF(state.SET_TEMP)
          : state.UNIT == TemperatureUnit.KELVIN
          ? cToK(state.SET_TEMP)
          : cToC(state.SET_TEMP)
    });
    if (state.TEMPS.length <= 16) {
      if (i % 2 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    } else if (state.TEMPS.length <= 32) {
      if (i % 8 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    } else {
      if (i % 16 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    }
  }

  chart.data.datasets[0].data = temps.reverse(); // reverse order
  chart.data.datasets[1].data = avg_temps.reverse(); // reverse order
  chart.data.datasets[2].data = set_temps;
  chart.data.datasets[3].data = calculateTrendLine(temps);
  chart.data.labels = labels;
  if (zoomRefreshTick <= 0) {
    updateZoom(state.ZOOM_LEVEL);
    zoomRefreshTick = 3;
  } else {
    zoomRefreshTick--;
  }
  chart.update();
};

const palletChange = (obj) => {
  const value = obj.value;
  if (value == 'warm') {
    chart.data.datasets[0].borderColor = 'red';
    chart.data.datasets[1].borderColor = 'orange';
    chart.data.datasets[2].borderColor = 'yellow';
    chart.data.datasets[3].borderColor = '#81d41a';
  } else if (value == 'cool') {
    chart.data.datasets[0].borderColor = '#6A5ACD';
    chart.data.datasets[1].borderColor = '#87faf2';
    chart.data.datasets[2].borderColor = '#4169e1';
    chart.data.datasets[3].borderColor = '#dd4cf4';
  } else if (value == 'forest') {
    chart.data.datasets[0].borderColor = '#158466';
    chart.data.datasets[1].borderColor = '#40E0D0';
    chart.data.datasets[2].borderColor = '#81d41a';
    chart.data.datasets[3].borderColor = '#2a6099';
  } else if (value == 'traffic') {
    chart.data.datasets[0].borderColor = '#ff0000';
    chart.data.datasets[1].borderColor = '#ffbf00';
    chart.data.datasets[2].borderColor = '#00a933';
    chart.data.datasets[3].borderColor = '#87faf2';
  } else if (value == 'void') {
    chart.data.datasets[0].borderColor = '#ffffff';
    chart.data.datasets[1].borderColor = '#7a7b7f';
    chart.data.datasets[2].borderColor = '#2a6099';
    chart.data.datasets[3].borderColor = '#650953';
  }
  chart.update();
};

const updateZoom = (value) => {
  if (!state.TEMPS || state.TEMPS.length == 0) {
    return;
  }
  let low = 1000;
  let high = 0;
  for (const _t of state.TEMPS) {
    const t = Number(_t);
    if (t < low && t > 0) {
      low = t;
    }
    if (t > high) {
      high = t;
    }
  }
  const delta = high - low;
  const hDelta = delta * 0.5;
  const dDelta = delta * 2;
  if (value == 'auto') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart.config.options.scales.yAxes[0].ticks.min = undefined;
    chart.config.options.scales.yAxes[0].ticks.max = undefined;
  } else if (value == 'fine') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart.config.options.scales.yAxes[0].ticks.min = low - 0.1;
    chart.config.options.scales.yAxes[0].ticks.max = high + 0.1;
  } else if (value == 'tiny') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart.config.options.scales.yAxes[0].ticks.min = low - 1;
    chart.config.options.scales.yAxes[0].ticks.max = high + 1;
  } else if (value == 'near') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart.config.options.scales.yAxes[0].ticks.min = low - hDelta - 1;
    chart.config.options.scales.yAxes[0].ticks.max = high + hDelta + 1;
  } else if (value == 'avg') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart.config.options.scales.yAxes[0].ticks.min = low - delta - 3;
    chart.config.options.scales.yAxes[0].ticks.max = high + delta + 3;
  } else if (value == 'far') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart.config.options.scales.yAxes[0].ticks.min = low - dDelta - 7;
    chart.config.options.scales.yAxes[0].ticks.max = high + dDelta + 7;
  } else if (value == 'dist') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart.config.options.scales.yAxes[0].ticks.min = low - dDelta - delta - 10;
    chart.config.options.scales.yAxes[0].ticks.max = high + dDelta + delta + 10;
  } else if (value == 'max') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = true;
    chart.config.options.scales.yAxes[0].ticks.min = low - delta - 25;
    chart.config.options.scales.yAxes[0].ticks.max = high + delta + 25;
  } else if (value == 'all') {
    chart.config.options.scales.yAxes[0].ticks.beginAtZero = true;
    chart.config.options.scales.yAxes[0].ticks.min = 0;
    chart.config.options.scales.yAxes[0].ticks.max = high + 25;
  }
};

// ==================================================
// enums
// ==================================================

const emptyColor = [0, 0, 0];

const LedMode = {
  AMBIENT: 0,
  BLINK: 1,
  PULSE: 2,
  PRESET: 3,
  RAINBOW: 4,
  RAINBOW_PULSE: 5,
  RAINBOW_WAVE: 6,
  SHIFT: 7,
  PREPULSE: 8,
  STATIC: 9,
  STATUS: 10
};

const Led = {
  ENABLE: 0,
  MODE: 1,
  COLOR: 2,
  BRIGHTNESS: 3,
  STAT: 4
};

const ParamPid = {
  PID_P: 0,
  PID_I: 1,
  PID_D: 2,
  PARAM_PID: 3
};

const SerialCommand = {
  INIT: 0x20,
  STATUS: 0x21,
  AUTO_OFF_LENGTH: 0x35,
  AUTO_OFF_TOGGLE: 0x32,
  ADJUST_AMOUNT: 0x24,
  BLE: 0x50,
  BUMP_UPDATE: 0x43,
  BUMP_AMOUNT: 0x44,
  BUMP_LENGTH: 0x45,
  BUMP_TOGGLE: 0x42,
  COIL_TOGGLE: 0x52,
  ESP: 0x00,
  FAV_1: 0x86,
  FAV_2: 0x87,
  FAV_3: 0x88,
  FAV_4: 0x89,
  LED: 0x70,
  PID: 0x90,
  TEMP: 0x10,
  UNIT: 0x11,
  WIFI: 0x60
};

const Operation = {
  GET: 0x00,
  SET: 0x01
};

const WiFi = {
  ENABLE: 0,
  JOIN: 1,
  RESET: 2,
  SCAN: 3
};

const Favorite = {
  NAME: 0,
  TEMP: 1,
  BUMP: 2
};

const ESP = {
  RESET: 0,
  DEVICE_RESET: 1
};

const TemperatureUnit = {
  CELSIUS: 0,
  FAHRENHEIT: 1,
  KELVIN: 2
};

// state storage object
let state = {
  INITIALIZED: false,
  VERSION: '-',
  NET_VERSION: '-',
  HOSTNAME: '-',
  UPTIME: 0,
  UNIT: TemperatureUnit.CELSIUS,
  TEMP: 0,
  SET_TEMP: 0,
  AVG_TEMP: 0,
  TEMPS: [],
  TEMPS_AVG: [],
  BUMP: {
    enabled: false,
    length: 0,
    amount: 0
  },
  COIL: {
    enabled: false,
    P: 0.0,
    I: 0.0,
    D: 0.0
  },
  LED: {
    enabled: true,
    mode: LedMode.PULSE,
    color: emptyColor,
    brightness: 0,
    status: false
  },
  FAV_1: {
    name: 'Fav 1',
    temp: 0
  },
  FAV_2: {
    name: 'Fav 2',
    temp: 0
  },
  FAV_3: {
    name: 'Fav 3',
    temp: 0
  },
  FAV_4: {
    name: 'Fav 4',
    temp: 0
  },
  AUTO_OFF: {
    enabled: false,
    length: 0
  },
  ZOOM_LEVEL: 'auto'
};

// ==================================================
// ==================================================

const Constants = {
  MAX_MESSAGE_SIZE: 16384,
  START_BYTE: 0x02,
  END_BYTE: 0x03,
  ESCAPE_BYTE: 0x10
};

/**
 * Convert celsius to celsius
 * @param {number} celsius the integer temperature in celsius multiplied by 10
 * @returns the temperature in fahrenheit
 */
const cToC = (celsius) => celsius / 10;

/**
 * Convert celsius to fahrenheit
 * @param {number} celsius the integer temperature in celsius multiplied by 10
 * @returns the temperature in fahrenheit
 */
const cToF = (celsius) => ((celsius / 10) * 9) / 5 + 32;

/**
 * Convert fahrenheit to celsius
 * @param {number} fahrenheit the integer temperature in fahrenheit multiplied by 10
 * @returns the temperature in celsius
 */
const fToC = (fahrenheit) => ((fahrenheit - 32) * 5) / 9;

/**
 * Convert celsius to kelvin
 * @param {number} celsius the integer temperature in celsius multiplied by 10
 * @returns the temperature in kelvin
 */
const cToK = (celsius) => celsius / 10 + 273.15;

/**
 * Convert kelvin to celsius
 * @param {number} kelvin the integer temperature in kelvin multiplied by 10
 * @returns the temperature in kelvin
 */
const kToC = (kelvin) => kelvin / 10 - 273.15;

/**
 * Convert a buffer into a hex string
 * @param {ArrayBuffer | Uint8Array | Array<number>} buffer
 * @returns a hex string or empty string if the buffer is empty
 */
function buf2hex(buffer) {
  return [...new Uint8Array(buffer)].map((x) => x.toString(16).padStart(2, '0')).join('');
}

/**
 * Get a number from a byte array
 * @param {Uint8Array | ArrayBuffer | Array<number>} data an array of bytes
 * @returns
 */
const getNumber = (data) => {
  const buffer = new Uint8Array(data);
  return Number(
    `0x${Array.from(buffer)
      .map((i) => i.toString(16).padStart(2, '0'))
      .join('')}`
  );
};

/**
 * Convert a javascript number to a 64 bit double
 * @param {number} inputNumber a javascript number
 * @returns an 8 byte (64bit) double
 */
const doubleToBytes = (inputNumber) => {
  const buffer = new ArrayBuffer(8); // 8 bytes for a double
  const view = new DataView(buffer);
  view.setFloat64(0, inputNumber, true); // 'true' for little-endian
  return new Uint8Array(buffer);
};

/**
 * Get a javascript number from an 8 byte array (64 bit double)
 * @param {Uint8Array | Array<number>} bytes a 64 bit double
 * @returns a javascript number
 */
const bytesToDouble = (bytes) => {
  if (bytes.length !== 8) {
    throw new Error('Input byte array must be 8 bytes in length to represent a double.');
  }

  const buffer = new ArrayBuffer(8);
  const view = new DataView(buffer);

  for (let i = 0; i < 8; i++) {
    view.setUint8(i, bytes[i]);
  }

  return parseFloat(view.getFloat64(0, true).toFixed(3));
};

// ==================================================
// ==================================================

/**
 * Encode a Uint8Array, ArrayBuffer or number array.
 *
 * @param {Uint8Array | ArrayBuffer | Array<number>} data
 * @returns an encoded Uint8Array of the data
 */
const encodeBuffer = (data) => {
  let arr = [Constants.START_BYTE];
  let dlength = 0;
  let d = new Uint8Array(data);
  for (let byte of d) {
    if (dlength > Constants.MAX_MESSAGE_SIZE) throw new Error('Message exceeds MAX_MESSAGE_SIZE');

    switch (byte) {
      case Constants.START_BYTE:
      case Constants.END_BYTE:
      case Constants.ESCAPE_BYTE:
        arr.push(Constants.ESCAPE_BYTE);
        arr.push(byte ^ Constants.ESCAPE_BYTE);
        dlength += 2;
        break;
      default:
        arr.push(byte);
        dlength++;
        break;
    }
  }
  arr.push(Constants.END_BYTE);
  return new Uint8Array(arr);
};

/**
 * Decode a Uint8Array, ArrayBuffer or number array.
 *
 * @param {Uint8Array | ArrayBuffer | Array<number>} data
 * @returns a decoded Uint8Array from the data
 */
const decodeBuffer = (data) => {
  if (data[0] !== Constants.START_BYTE) throw new Error('Invalid data');
  if (data[data.length - 1] !== Constants.END_BYTE) throw new Error('Invalid data');

  let arr = [];
  let endOfMessage = false;
  for (let i = 0; i < data.length; i++) {
    if (endOfMessage) break;
    switch (data[i]) {
      case Constants.START_BYTE:
        break;
      case Constants.END_BYTE:
        endOfMessage = true;
        break;
      case Constants.ESCAPE_BYTE:
        ++i;
        arr.push(data[i] ^ Constants.ESCAPE_BYTE);
        break;
      default:
        arr.push(data[i]);
        break;
    }
  }
  return new Uint8Array(arr);
};

/**
 * @param {Uint8Array | Array<number>} data
 * @returns initData object
 */
const parseInitMessage = (data) => {
  const ver = data.slice(0, 3);
  const verNet = data.slice(3, 6);
  const ipAddr = data.slice(6, 10);
  const ssid = new TextDecoder()
    .decode(new Uint8Array(data.slice(10, 42)))
    .replace(/\u0000/g, '')
    .trim();
  const hostname = new TextDecoder()
    .decode(new Uint8Array(data.slice(42, 295)))
    .replace(/\u0000/g, '')
    .trim();
  const uptime = getNumber(data.slice(295, 299));
  const bumpTemp = getNumber(data.slice(299, 301));
  const bumpTime = getNumber(data.slice(301, 303));
  const autoOffTime = getNumber(data.slice(303, 305));
  const ledMode = data.slice(305, 306)[0];
  const ledStatus = data.slice(306, 307)[0];
  const ledColor = data.slice(307, 310);
  const ledBrightness = data.slice(310, 311)[0];
  const fav1Name = new TextDecoder()
    .decode(new Uint8Array(data.slice(311, 375)))
    .replace(/\u0000/g, '')
    .trim();
  const fav1Temp = getNumber(data.slice(375, 377));
  const fav2Name = new TextDecoder()
    .decode(new Uint8Array(data.slice(377, 441)))
    .replace(/\u0000/g, '')
    .trim();
  const fav2Temp = getNumber(data.slice(441, 443));
  const fav3Name = new TextDecoder()
    .decode(new Uint8Array(data.slice(443, 507)))
    .replace(/\u0000/g, '')
    .trim();
  const fav3Temp = getNumber(data.slice(507, 509));
  const fav4Name = new TextDecoder()
    .decode(new Uint8Array(data.slice(509, 573)))
    .replace(/\u0000/g, '')
    .trim();
  const fav4Temp = getNumber(data.slice(573, 575));
  const tempAdjustAmt = getNumber(data.slice(575, 577));
  const temp = getNumber(data.slice(577, 579));
  const setTemp = getNumber(data.slice(579, 581));
  const avgTemp = getNumber(data.slice(581, 583));
  const P = bytesToDouble(Uint8Array.from(data.slice(583, 591)));
  const I = bytesToDouble(Uint8Array.from(data.slice(591, 599)));
  const D = bytesToDouble(Uint8Array.from(data.slice(599, 607)));
  const unitType = data.slice(607, 608)[0];
  const booleanMap = [...Array(8)].map((_, i) => Boolean(data.slice(608)[0] & (1 << (7 - i))));
  const [
    AUTO_OFF_ENABLED,
    COIL_ENABLED,
    LIGHTS_ENABLED,
    BUMP_ENABLED,
    WIFI_ENABLED,
    SSID_SET,
    BT_ENABLED,
    _UNUSED_4
  ] = booleanMap;

  return {
    ver: `${ver[0]}.${ver[1]}.${ver[2]}`,
    verNet: `${verNet[0]}.${verNet[1]}.${verNet[2]}`,
    ipAddr: `${ipAddr[0]}.${ipAddr[1]}.${ipAddr[2]}.${ipAddr[3]}`,
    ssid,
    hostname,
    uptime,
    unitType,
    bumpTemp,
    bumpTime,
    autoOffTime,
    ledMode,
    ledStatus,
    ledColor,
    ledBrightness,
    fav1Name,
    fav1Temp,
    fav2Name,
    fav2Temp,
    fav3Name,
    fav3Temp,
    fav4Name,
    fav4Temp,
    tempAdjustAmt,
    temp,
    setTemp,
    avgTemp,
    P,
    I,
    D,
    AUTO_OFF_ENABLED,
    COIL_ENABLED,
    BUMP_ENABLED,
    SSID_SET,
    LIGHTS_ENABLED,
    WIFI_ENABLED,
    BT_ENABLED
  };
};

// ==================================================

function initWebSocket() {
  console.log('Attempting to open WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}
function onOpen(event) {
  console.log('Connection to host opened!');
}
function onClose(event) {
  console.log('Connection to host closed!');
  setTimeout(initWebSocket, 3000);
}

const initPageData = async (initData) => {
  //console.log(JSON.stringify(initData, null, 2));
  state.UNIT = initData.unitType;
  if (document.getElementById('wifi_en')) {
    document.getElementById('wifi_en').checked = initData.WIFI_ENABLED;
  }
  if (document.getElementById('SSID')) {
    document.getElementById('SSID').value = `${initData.ssid}`;
  }
  if (document.getElementById('IPADD')) {
    document.getElementById('IPADD').value = `${initData.ipAddr}`;
  }
  if (document.getElementById('NVER')) {
    document.getElementById('NVER').value = `${initData.verNet}`;
  }

  state.LED.enabled = initData.LIGHTS_ENABLED;
  if (document.getElementById('toggle_lights') && document.getElementById('toggle_lights2')) {
    const element = document.getElementById('toggle_lights');
    const element2 = document.getElementById('toggle_lights2');
    if (state.LED.enabled) {
      element.innerHTML = '<i class="fas fa-stopwatch" style="color:Tomato;"></i> Disable LEDs';
      element2.innerHTML = '<i class="fas fa-stopwatch" style="color:Tomato;"></i> Disable LEDs';
    } else {
      element.innerHTML =
        '<i class="fas fa-stopwatch" style="color:MediumSeaGreen;"></i> Enable LEDs';
      element2.innerHTML =
        '<i class="fas fa-stopwatch" style="color:MediumSeaGreen;"></i> Enable LEDs';
    }
  }
  state.LED.mode = initData.ledMode;
  initLED_toggles(state.LED.mode);
  state.LED.color = initData.ledColor;
  state.LED.status = initData.ledStatus;
  if (state.LED.status) {
    document.getElementById('statusbut_span').innerText = 'Disable';
  } else {
    document.getElementById('statusbut_span').innerText = 'Enable';
  }
  state.LED.brightness = initData.ledBrightness;
  if (document.getElementById('bright_bar')) {
    document.getElementById('bright_bar').value = `${state.LED.brightness}`;
  }
  if (document.getElementById('r_set')) {
    document.getElementById('r_set').value = `${state.LED.color[0]}`;
  }
  if (document.getElementById('r_qset')) {
    document.getElementById('r_qset').value = `${state.LED.color[0]}`;
  }
  if (document.getElementById('g_set')) {
    document.getElementById('g_set').value = `${state.LED.color[1]}`;
  }
  if (document.getElementById('g_qset')) {
    document.getElementById('g_qset').value = `${state.LED.color[1]}`;
  }
  if (document.getElementById('b_set')) {
    document.getElementById('b_set').value = `${state.LED.color[2]}`;
  }
  if (document.getElementById('b_qset')) {
    document.getElementById('b_qset').value = `${state.LED.color[2]}`;
  }
  if (document.getElementById('br_set')) {
    document.getElementById('br_set').value = `${state.LED.brightness}`;
  }
  if (document.getElementById('br_qset')) {
    document.getElementById('br_qset').value = `${state.LED.brightness}`;
  }
  state.COIL.enabled = initData.COIL_ENABLED;
  state.COIL.P = initData.P;
  if (document.getElementById('p_set')) {
    document.getElementById('p_set').value = `${state.COIL.P}`;
  }
  if (document.getElementById('p_qset')) {
    document.getElementById('p_qset').value = `${state.COIL.P}`;
  }
  state.COIL.I = initData.I;
  if (document.getElementById('i_set')) {
    document.getElementById('i_set').value = `${state.COIL.I}`;
  }
  if (document.getElementById('i_qset')) {
    document.getElementById('i_qset').value = `${state.COIL.I}`;
  }
  state.COIL.D = initData.D;
  if (document.getElementById('d_set')) {
    document.getElementById('d_set').value = `${state.COIL.D}`;
  }
  if (document.getElementById('d_qset')) {
    document.getElementById('d_qset').value = `${state.COIL.D}`;
  }
  if (document.getElementById('toggle_heat')) {
    const element = document.getElementById('toggle_heat');
    if (state.COIL.enabled) {
      element.style.setProperty('background-color', '#faa843', 'important');
      element.style.setProperty('color', 'whitesmoke', 'important');
    } else {
      element.style.removeProperty('background-color');
      element.style.removeProperty('color');
    }
  }
  state.FAV_1 = { name: initData.fav1Name, temp: initData.fav1Temp };

  if (document.getElementById('fav1_n')) {
    document.getElementById('fav1_n').value =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.FAV_1.temp)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.FAV_1.temp)
        : cToC(state.FAV_1.temp);
  }
  if (document.getElementById('fav1q_n')) {
    document.getElementById('fav1q_n').innerHTML =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.FAV_1.temp)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.FAV_1.temp)
        : cToC(state.FAV_1.temp) + '&deg;';
  }
  if (document.getElementById('fav1_t')) {
    document.getElementById('fav1_t').value = `${state.FAV_1.name}`;
  }
  if (document.getElementById('fav1q_t')) {
    document.getElementById('fav1q_t').innerHTML = `${state.FAV_1.name}`;
  }

  state.FAV_2 = { name: initData.fav2Name, temp: initData.fav2Temp };

  if (document.getElementById('fav2_n')) {
    document.getElementById('fav2_n').value =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.FAV_2.temp)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.FAV_2.temp)
        : cToC(state.FAV_2.temp);
  }
  if (document.getElementById('fav2q_n')) {
    document.getElementById('fav2q_n').innerHTML =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.FAV_2.temp)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.FAV_2.temp)
        : cToC(state.FAV_2.temp) + '&deg;';
  }
  if (document.getElementById('fav2_t')) {
    document.getElementById('fav2_t').value = `${state.FAV_2.name}`;
  }
  if (document.getElementById('fav2q_t')) {
    document.getElementById('fav2q_t').innerHTML = `${state.FAV_2.name}`;
  }

  state.FAV_3 = { name: initData.fav3Name, temp: initData.fav3Temp };

  if (document.getElementById('fav3_n')) {
    document.getElementById('fav3_n').value =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.FAV_3.temp)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.FAV_3.temp)
        : cToC(state.FAV_3.temp);
  }
  if (document.getElementById('fav3q_n')) {
    document.getElementById('fav3q_n').innerHTML =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.FAV_3.temp)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.FAV_3.temp)
        : cToC(state.FAV_3.temp) + '&deg;';
  }
  if (document.getElementById('fav3_t')) {
    document.getElementById('fav3_t').value = `${state.FAV_3.name}`;
  }
  if (document.getElementById('fav3q_t')) {
    document.getElementById('fav3q_t').innerHTML = `${state.FAV_3.name}`;
  }

  state.FAV_4 = { name: initData.fav4Name, temp: initData.fav4Temp };

  if (document.getElementById('fav4_n')) {
    document.getElementById('fav4_n').value =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.FAV_4.temp)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.FAV_4.temp)
        : cToC(state.FAV_4.temp);
  }
  if (document.getElementById('fav4q_n')) {
    document.getElementById('fav4q_n').innerHTML =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.FAV_4.temp)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.FAV_4.temp)
        : cToC(state.FAV_4.temp) + '&deg;';
  }
  if (document.getElementById('fav4_t')) {
    document.getElementById('fav4_t').value = `${state.FAV_4.name}`;
  }
  if (document.getElementById('fav4q_t')) {
    document.getElementById('fav4q_t').innerHTML = `${state.FAV_4.name}`;
  }

  state.VERSION = initData.ver;
  if (document.getElementById('s_version'))
    document.getElementById('s_version').value = state.VERSION;
  state.NET_VERSION = initData.verNet;
  state.HOSTNAME = initData.hostname;
  if (document.getElementById('hostname'))
    document.getElementById('hostname').value = state.HOSTNAME;
  state.UPTIME = initData.uptime;
  if (document.getElementById('uptime'))
    document.getElementById('uptime').value = `${state.UPTIME}`;
  state.TEMP = initData.temp;
  if (document.getElementById('meas_temp'))
    document.getElementById('meas_temp').innerHTML =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.TEMP).toFixed(1)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.TEMP).toFixed(1)
        : cToC(state.TEMP).toFixed(1);
  updateTempMeter();
  state.SET_TEMP = initData.setTemp;
  if (document.getElementById('temp_setting'))
    document.getElementById('temp_setting').value =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.SET_TEMP).toFixed(1)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.SET_TEMP).toFixed(1)
        : cToC(state.SET_TEMP).toFixed(1);
  if (document.getElementById('temp_slider'))
    document.getElementById('temp_slider').value =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.SET_TEMP).toFixed(1)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.SET_TEMP).toFixed(1)
        : cToC(state.SET_TEMP).toFixed(1);

  state.AVG_TEMP = initData.avgTemp;
  state.AUTO_OFF.enabled = initData.AUTO_OFF_ENABLED;
  if (document.getElementById('toggle_aoff') && document.getElementById('aoff_en')) {
    const element = document.getElementById('toggle_aoff');
    const element2 = document.getElementById('aoff_en');
    if (state.AUTO_OFF.enabled) {
      element.innerHTML = '<i class="fas fa-stopwatch" style="color:Tomato;"></i> Disable Auto-Off';
      element2.checked = true;
    } else {
      element.innerHTML =
        '<i class="fas fa-stopwatch" style="color:MediumSeaGreen;"></i> Enable Auto-Off';
      element2.checked = false;
    }
  }
  state.AUTO_OFF.length = initData.autoOffTime;
  if (document.getElementById('aoff_time'))
    document.getElementById('aoff_time').value = `${state.AUTO_OFF.length}`;
};

const updateTempMeter = () => {
  const bar = document.getElementById('bar');
  const temp = parseInt(document.getElementById('meas_temp').innerHTML);
  const temp_setting = parseInt(document.getElementById('temp_setting').value);
  bar.style.transform = 'rotate(' + (45 + temp * 0.15) + 'deg)';
  var color = 'black';
  if (temp >= temp_setting - 7 && temp <= temp_setting + 7) {
    color = 'green';
  } else if (temp > temp_setting + 7 && temp <= temp_setting + 15) {
    color = 'rgba(255, 165, 0, 1)';
  } else {
    if (temp > temp_setting + 15) {
      color = 'red';
    } else {
      color = 'rgba(' + temp * 0.12 + ', ' + temp * 0.17 + ', ' + temp * 0.12 + ', 1)';
    }
  }
  bar.style.borderBottomColor = color;
  if (temp > 600) {
    bar.style.borderRightColor = color;
  } else {
    bar.style.borderRightColor = '#eee';
  }
};

function updateTempSlider() {
  document.getElementById('temp_slider').value = document.getElementById('temp_setting').value;
}

const initPageMessage = async (initData) => {
  const wifiSet = initData.SSID_SET;
  if (!wifiSet) {
    openWifiWarningModal();
  } else {
    if (document.getElementById('wifi_setup_alert')) {
      document.getElementById('wifi_setup_alert').style.display = 'none';
    }
  }
};

/**
 * @param {Uint8Array | Array<number>} dat the array of bytes
 */
const handleMessage = (dat) => {
  //console.log('Received socket message.');
  const cmd = dat[0];
  let pid_enb;
  let param;
  switch (cmd) {
    case SerialCommand.INIT:
      const initMsg = parseInitMessage(dat.slice(1));
      //console.log(initMsg);
      initPageData(initMsg);
      initPageMessage(initMsg);
      break;
    case SerialCommand.STATUS:
      pid_enb = dat[2] > 0;
      var mes_temp = getNumber(dat.slice(3, 5));
      var avg_temp = getNumber(dat.slice(5, 7));
      var set_temp = getNumber(dat.slice(7));
      updateChart(
        state,
        state.UNIT == TemperatureUnit.FAHRENHEIT
          ? cToF(mes_temp).toFixed(1)
          : state.UNIT == TemperatureUnit.KELVIN
          ? cToK(mes_temp).toFixed(1)
          : cToC(mes_temp).toFixed(1),
        state.UNIT == TemperatureUnit.FAHRENHEIT
          ? cToF(avg_temp).toFixed(1)
          : state.UNIT == TemperatureUnit.KELVIN
          ? cToK(avg_temp).toFixed(1)
          : cToC(avg_temp).toFixed(1)
      );
      //console.log(pid_enb, mes_temp, avg_temp, set_temp);
      state.COIL.enabled = pid_enb;
      state.TEMP = mes_temp;
      state.SET_TEMP = set_temp;
      if (document.getElementById('toggle_heat')) {
        const element = document.getElementById('toggle_heat');
        if (state.COIL.enabled) {
          element.style.setProperty('background-color', '#faa843', 'important');
          element.style.setProperty('color', 'whitesmoke', 'important');
        } else {
          element.style.removeProperty('background-color');
          element.style.removeProperty('color');
        }
      }
      if (document.getElementById('meas_temp'))
        document.getElementById('meas_temp').innerHTML =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.TEMP).toFixed(1)
            : cToC(state.TEMP).toFixed(1);
      if (document.getElementById('temp_setting'))
        document.getElementById('temp_setting').value =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.SET_TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.SET_TEMP).toFixed(1)
            : cToC(state.SET_TEMP).toFixed(1);
      updateTempMeter();
      break;
    case SerialCommand.TEMP:
      var set_temp = getNumber(dat.slice(2, 4));
      state.SET_TEMP = set_temp;
      if (document.getElementById('temp_setting'))
        document.getElementById('temp_setting').value =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.SET_TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.SET_TEMP).toFixed(1)
            : cToC(state.SET_TEMP).toFixed(1);
      updateTempMeter();
      break;
    case SerialCommand.UNIT:
      const unitType = dat[1];
      state.UNIT = unitType;
      break;
    case SerialCommand.COIL_TOGGLE:
      pid_enb = dat[1] > 0;
      state.COIL.enabled = pid_enb;
      if (document.getElementById('toggle_heat')) {
        const element = document.getElementById('toggle_heat');
        if (state.COIL.enabled) {
          element.style.setProperty('background-color', '#faa843', 'important');
          element.style.setProperty('color', 'whitesmoke', 'important');
        } else {
          element.style.removeProperty('background-color');
          element.style.removeProperty('color');
        }
      }
      break;
    case SerialCommand.PID:
      param = dat[1];
      switch (param) {
        case ParamPid.PARAM_PID:
          const P = bytesToDouble(Uint8Array.from(dat.slice(2, 10)));
          const I = bytesToDouble(Uint8Array.from(dat.slice(10, 18)));
          const D = bytesToDouble(Uint8Array.from(dat.slice(18, 26)));
          //console.log(P, I, D);
          state.COIL.P = P;
          if (document.getElementById('p_set')) {
            document.getElementById('p_set').value = `${state.COIL.P}`;
          }
          if (document.getElementById('p_qset')) {
            document.getElementById('p_qset').value = `${state.COIL.P}`;
          }
          state.COIL.I = I;
          if (document.getElementById('i_set')) {
            document.getElementById('i_set').value = `${state.COIL.I}`;
          }
          if (document.getElementById('i_qset')) {
            document.getElementById('i_qset').value = `${state.COIL.I}`;
          }
          state.COIL.D = D;
          if (document.getElementById('d_set')) {
            document.getElementById('d_set').value = `${state.COIL.D}`;
          }
          if (document.getElementById('d_qset')) {
            document.getElementById('d_qset').value = `${state.COIL.D}`;
          }
          break;
        default:
          break;
      }
      break;
    case SerialCommand.WIFI:
      param = dat[1];
      switch (param) {
        case WiFi.ENABLE:
          break;
        case WiFi.JOIN:
          console.log('Setup WiFi');
          if (document.getElementById('join_wifi')) {
            document.getElementById('join_wifi').classList.remove('w3-pale-green');
            document.getElementById(
              'join_wifi'
            ).innerHTML = `<div><i class="fas fa-check"></i> Joining Network</div>`;
          }
          if (document.getElementById('save_wifi_alert')) {
            document.getElementById('save_wifi_alert').style.display = 'block';
          }
          if (document.getElementById('rst_wifi')) {
            document.getElementById('rst_wifi').classList.add('w3-pale-red');
          }
          break;
        case WiFi.RESET:
          console.log('Cleared WiFi');
          if (document.getElementById('rst_wifi')) {
            document.getElementById('rst_wifi').classList.remove('w3-pale-red');
          }
          if (document.getElementById('save_wifi_alert')) {
            document.getElementById('save_wifi_alert').style.display = 'block';
          }
          if (document.getElementById('join_wifi')) {
            document.getElementById(
              'join_wifi'
            ).innerHTML = `<div><i class="fas fa-link"></i> Join Network</div>`;
            document.getElementById('join_wifi').classList.add('w3-pale-green');
          }
          break;
        case WiFi.SCAN:
          break;
        default:
          break;
      }
      break;
    case SerialCommand.LED:
      console.log('LED > Updates received');
      param = dat[1];
      switch (param) {
        case Led.ENABLE:
          var led_enb = dat[3] > 0;
          state.LED.enabled = led_enb;
          if (
            document.getElementById('toggle_lights') &&
            document.getElementById('toggle_lights2')
          ) {
            const element = document.getElementById('toggle_lights');
            const element2 = document.getElementById('toggle_lights2');
            if (state.LED.enabled) {
              element.innerHTML =
                '<i class="fas fa-stopwatch" style="color:Tomato;"></i> Disable LEDs';
              element2.innerHTML =
                '<i class="fas fa-stopwatch" style="color:Tomato;"></i> Disable LEDs';
            } else {
              element.innerHTML =
                '<i class="fas fa-stopwatch" style="color:MediumSeaGreen;"></i> Enable LEDs';
              element2.innerHTML =
                '<i class="fas fa-stopwatch" style="color:MediumSeaGreen;"></i> Enable LEDs';
            }
          }
          break;
        case Led.MODE:
          console.log('LED MODE');
          let lmode = dat[3];
          //setLED_Toggles(lmode);
          switch (lmode) {
            case LedMode.AMBIENT:
              break;
            case LedMode.BLINK:
              if (setLED_Toggles(LedMode.BLINK)) {
                document.getElementById('leden_blink').disabled = true;
              }
              break;
            case LedMode.PULSE:
              if (setLED_Toggles(LedMode.PULSE)) {
                document.getElementById('leden_pulse').disabled = true;
              }
              break;
            case LedMode.PRESET:
              break;
            case LedMode.RAINBOW:
              if (setLED_Toggles(LedMode.RAINBOW)) {
                document.getElementById('led_rainbow2').disabled = true;
              }
              break;
            case LedMode.RAINBOW_PULSE:
              if (setLED_Toggles(LedMode.RAINBOW_PULSE)) {
                document.getElementById('led_rainbow3').disabled = true;
              }
              break;
            case LedMode.RAINBOW_WAVE:
              if (setLED_Toggles(LedMode.RAINBOW_WAVE)) {
                document.getElementById('led_rainbow').disabled = true;
              }
              break;
            case LedMode.SHIFT:
              if (setLED_Toggles(LedMode.SHIFT)) {
                document.getElementById('led_shift').disabled = true;
              }
              break;
            case LedMode.PREPULSE:
              if (setLED_Toggles(LedMode.PREPULSE)) {
                document.getElementById('led_pulse').disabled = true;
              }
              break;
            case LedMode.STATIC:
              if (setLED_Toggles(LedMode.STATIC)) {
                //document.getElementById('led_static').disabled = true;
                document.getElementById('leden_static').disabled = true;
              }
              break;
            case LedMode.STATUS: // Shouldn't turn off other ones.
              break;

            default:
              break;
          }
          break;
        case Led.COLOR:
          console.log('LED > Color');
          let hexcol = new TextDecoder()
            .decode(new Uint8Array(dat.slice(3)))
            .replace(/\u0000/g, '')
            .trim();
          console.log('LED > Color Hex: ' + hexcol);
          break;
        case Led.BRIGHTNESS:
          console.log('LED > Bright');
          document.getElementById('bright_bar').value = dat[3];
          document.getElementById('br_qset').value = dat[3];
          state.LED.brightness = this.value;
          break;
        case Led.STAT:
          console.log('LED > Status');
          var stat_enb = dat[3] > 0;
          if (stat_enb) {
            state.LED.status = true;
            document.getElementById('statusbut_span').innerText = 'Disable';
          } else {
            state.LED.status = false;
            document.getElementById('statusbut_span').innerText = 'Enable';
          }
        default:
          break;
      }
      break;
    case SerialCommand.AUTO_OFF_TOGGLE:
      console.log('Status update: Auto-Off State');
      var ao_enb = dat[1] > 0;
      if (document.getElementById('toggle_aoff') && document.getElementById('aoff_en')) {
        const element = document.getElementById('toggle_aoff');
        const element2 = document.getElementById('aoff_en');
        if (ao_enb) {
          element.innerHTML =
            '<i class="fas fa-stopwatch" style="color:Tomato;"></i> Disable Auto-Off';
          element2.checked = true;
        } else {
          element.innerHTML =
            '<i class="fas fa-stopwatch" style="color:MediumSeaGreen;"></i> Enable Auto-Off';
          element2.checked = false;
        }
      }
      break;
    case SerialCommand.AUTO_OFF_LENGTH:
      console.log('Status update: Auto-Off Length');
      var ao_len = dat[1];
      if (document.getElementById('aoff_time'))
        document.getElementById('aoff_time').value = `${ao_len}`;
      break;
    case SerialCommand.FAV_1:
      console.log('Cmd: FAV_1');
      param = dat[1];
      switch (param) {
        case Favorite.BUMP:
          // Not implemented yet.
          break;
        case Favorite.NAME:
          console.log('Fav 1 Name');
          state.FAV_1.name = new TextDecoder()
            .decode(new Uint8Array(dat.slice(3)))
            .replace(/\u0000/g, '')
            .trim();
          if (document.getElementById('fav1_t')) {
            document.getElementById('fav1_t').value = `${state.FAV_1.name}`;
          }
          if (document.getElementById('fav1q_t')) {
            document.getElementById('fav1q_t').innerHTML = `${state.FAV_1.name}`;
          }
          break;
        case Favorite.TEMP:
          state.FAV_1.temp = getNumber(dat.slice(3));
          console.log('Fav 1 Temp: ' + `${state.FAV_1.temp}`);
          if (document.getElementById('fav1_n')) {
            document.getElementById('fav1_n').value =
              state.UNIT == TemperatureUnit.FAHRENHEIT
                ? cToF(state.FAV_1.temp)
                : state.UNIT == TemperatureUnit.KELVIN
                ? cToK(state.FAV_1.temp)
                : cToC(state.FAV_1.temp);
          }
          if (document.getElementById('fav1q_n')) {
            document.getElementById('fav1q_n').innerHTML =
              state.UNIT == TemperatureUnit.FAHRENHEIT
                ? cToF(state.FAV_1.temp)
                : state.UNIT == TemperatureUnit.KELVIN
                ? cToK(state.FAV_1.temp)
                : cToC(state.FAV_1.temp) + '&deg;';
          }
          break;

        default:
          break;
      }
      break;
    case SerialCommand.FAV_2:
      console.log('Cmd: FAV_2');
      param = dat[1];
      switch (param) {
        case Favorite.BUMP:
          // Not implemented yet.
          break;
        case Favorite.NAME:
          console.log('Fav 2 Name');
          state.FAV_2.name = new TextDecoder()
            .decode(new Uint8Array(dat.slice(3)))
            .replace(/\u0000/g, '')
            .trim();
          if (document.getElementById('fav2_t')) {
            document.getElementById('fav2_t').value = `${state.FAV_2.name}`;
          }
          if (document.getElementById('fav2q_t')) {
            document.getElementById('fav2q_t').innerHTML = `${state.FAV_2.name}`;
          }
          break;
        case Favorite.TEMP:
          state.FAV_2.temp = getNumber(dat.slice(3));
          console.log('Fav 1 Temp: ' + `${state.FAV_2.temp}`);
          if (document.getElementById('fav2_n')) {
            document.getElementById('fav2_n').value =
              state.UNIT == TemperatureUnit.FAHRENHEIT
                ? cToF(state.FAV_2.temp)
                : state.UNIT == TemperatureUnit.KELVIN
                ? cToK(state.FAV_2.temp)
                : cToC(state.FAV_2.temp);
          }
          if (document.getElementById('fav2q_n')) {
            document.getElementById('fav2q_n').innerHTML =
              state.UNIT == TemperatureUnit.FAHRENHEIT
                ? cToF(state.FAV_2.temp)
                : state.UNIT == TemperatureUnit.KELVIN
                ? cToK(state.FAV_2.temp)
                : cToC(state.FAV_2.temp) + '&deg;';
          }
          break;

        default:
          break;
      }
      break;
    case SerialCommand.FAV_3:
      console.log('Cmd: FAV_3');
      param = dat[1];
      switch (param) {
        case Favorite.BUMP:
          // Not implemented yet.
          break;
        case Favorite.NAME:
          console.log('Fav 3 Name');
          state.FAV_3.name = new TextDecoder()
            .decode(new Uint8Array(dat.slice(3)))
            .replace(/\u0000/g, '')
            .trim();
          if (document.getElementById('fav3_t')) {
            document.getElementById('fav3_t').value = `${state.FAV_3.name}`;
          }
          if (document.getElementById('fav3q_t')) {
            document.getElementById('fav3q_t').innerHTML = `${state.FAV_3.name}`;
          }
          break;
        case Favorite.TEMP:
          state.FAV_3.temp = getNumber(dat.slice(3));
          console.log('Fav 3 Temp: ' + `${state.FAV_3.temp}`);
          if (document.getElementById('fav3_n')) {
            document.getElementById('fav3_n').value =
              state.UNIT == TemperatureUnit.FAHRENHEIT
                ? cToF(state.FAV_3.temp)
                : state.UNIT == TemperatureUnit.KELVIN
                ? cToK(state.FAV_3.temp)
                : cToC(state.FAV_3.temp);
          }
          if (document.getElementById('fav3q_n')) {
            document.getElementById('fav3q_n').innerHTML =
              state.UNIT == TemperatureUnit.FAHRENHEIT
                ? cToF(state.FAV_3.temp)
                : state.UNIT == TemperatureUnit.KELVIN
                ? cToK(state.FAV_3.temp)
                : cToC(state.FAV_3.temp) + '&deg;';
          }
          break;

        default:
          break;
      }
      break;
    case SerialCommand.FAV_4:
      console.log('Cmd: FAV_4');
      param = dat[1];
      switch (param) {
        case Favorite.BUMP:
          // Not implemented yet.
          break;
        case Favorite.NAME:
          console.log('Fav 4 Name');
          state.FAV_4.name = new TextDecoder()
            .decode(new Uint8Array(dat.slice(3)))
            .replace(/\u0000/g, '')
            .trim();
          if (document.getElementById('fav4_t')) {
            document.getElementById('fav4_t').value = `${state.FAV_4.name}`;
          }
          if (document.getElementById('fav4q_t')) {
            document.getElementById('fav4q_t').innerHTML = `${state.FAV_4.name}`;
          }
          break;
        case Favorite.TEMP:
          state.FAV_4.temp = getNumber(dat.slice(3));
          console.log('Fav 1 Temp: ' + `${state.FAV_4.temp}`);
          if (document.getElementById('fav4_n')) {
            document.getElementById('fav4_n').value =
              state.UNIT == TemperatureUnit.FAHRENHEIT
                ? cToF(state.FAV_4.temp)
                : state.UNIT == TemperatureUnit.KELVIN
                ? cToK(state.FAV_4.temp)
                : cToC(state.FAV_4.temp);
          }
          if (document.getElementById('fav4q_n')) {
            document.getElementById('fav4q_n').innerHTML =
              state.UNIT == TemperatureUnit.FAHRENHEIT
                ? cToF(state.FAV_4.temp)
                : state.UNIT == TemperatureUnit.KELVIN
                ? cToK(state.FAV_4.temp)
                : cToC(state.FAV_4.temp) + '&deg;';
          }
          break;

        default:
          break;
      }
      break;
    default:
      break;
  }
};

/**
 * @param {MessageEvent} event a MessageEvent
 */
function onMessage(event) {
  if (event.data instanceof Blob) {
    const reader = new FileReader();
    reader.onload = () => {
      //console.log(`result: ${buf2hex(reader.result)}`);
      handleMessage(decodeBuffer(new Uint8Array(reader.result)));
    };
    reader.readAsArrayBuffer(event.data);
  } else {
    console.log(event.data);
  }
}

/**
 *  Send raw data over websockets
 * @param {Uint8Array | ArrayBuffer | Array<number>} data
 */
const emit_websocket = (data) => {
  const buffer = new ArrayBuffer(data.length);
  const view = new DataView(buffer);
  for (var i = 0; i < data.length; i++) {
    view.setUint8(i, data[i]);
  }
  websocket.send(encodeBuffer(view.buffer));
};

var brightslide = document.getElementById('bright_bar');
brightslide.onchange = function () {
  emit_websocket([SerialCommand.LED, Operation.SET, Led.BRIGHTNESS, this.value]);
  state.LED.brightness = this.value;
};

function hexTorgb(hex) {
  return [('0x' + hex[1] + hex[2]) | 0, ('0x' + hex[3] + hex[4]) | 0, ('0x' + hex[5] + hex[6]) | 0];
}

var colorInp = document.getElementById('cp_input');
colorInp.onchange = function () {
  let val = this.value;
  val = `${val}\0`;
  let rgb = hexTorgb(val);
  console.log(val);
  console.log(rgb);
  emit_websocket([SerialCommand.LED, Operation.SET, Led.COLOR, ...new TextEncoder().encode(val)]);
};

// Setup Socket listeners.
var pChanged = false,
  iChanged = false,
  dChanged = false,
  qpChanged = false,
  qiChanged = false,
  qdChanged = false;
function pidButtonInit() {
  var pidQset = document.getElementById('pid_qset');
  var pidSet = document.getElementById('pid_set');
  pidQset.addEventListener('click', sendPidSettings);
  pidSet.addEventListener('click', sendPidSettings);
  document.getElementById('p_set').addEventListener('change', function () {
    pChanged = true;
  });
  document.getElementById('p_qset').addEventListener('change', function () {
    qpChanged = true;
  });
  document.getElementById('i_set').addEventListener('change', function () {
    iChanged = true;
  });
  document.getElementById('i_qset').addEventListener('change', function () {
    qiChanged = true;
  });
  document.getElementById('d_set').addEventListener('change', function () {
    dChanged = true;
  });
  document.getElementById('d_qset').addEventListener('change', function () {
    qdChanged = true;
  });
}

function sendPidSettings() {
  console.log('PID changes submitted via quick set.');
  if (pChanged) {
    let val = document.getElementById('p_set').value;
    document.getElementById('p_qset').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_P,
      ...doubleToBytes(Number(val))
    ]);
    pChanged = false;
  }
  if (iChanged) {
    let val = document.getElementById('i_set').value;
    document.getElementById('i_qset').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_I,
      ...doubleToBytes(Number(val))
    ]);
    iChanged = false;
  }
  if (dChanged) {
    let val = document.getElementById('d_set').value;
    document.getElementById('d_qset').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_D,
      ...doubleToBytes(Number(val))
    ]);
    dChanged = false;
  }
  if (qpChanged) {
    let val = document.getElementById('p_qset').value;
    document.getElementById('p_set').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_P,
      ...doubleToBytes(Number(val))
    ]);
    qpChanged = false;
  }
  if (qiChanged) {
    let val = document.getElementById('i_qset').value;
    document.getElementById('i_set').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_I,
      ...doubleToBytes(Number(val))
    ]);
    qiChanged = false;
  }
  if (qdChanged) {
    let val = document.getElementById('d_qset').value;
    document.getElementById('d_set').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_D,
      ...doubleToBytes(Number(val))
    ]);
    qdChanged = false;
  }
}

function favListenersInit() {
  document.getElementById('fav1_t').addEventListener('change', function () {
    changeFavName(1);
  });
  document.getElementById('fav1_n').addEventListener('change', function () {
    changeFavTemp(1);
  });
  document.getElementById('fav2_t').addEventListener('change', function () {
    changeFavName(2);
  });
  document.getElementById('fav2_n').addEventListener('change', function () {
    changeFavTemp(2);
  });
  document.getElementById('fav3_t').addEventListener('change', function () {
    changeFavName(3);
  });
  document.getElementById('fav3_n').addEventListener('change', function () {
    changeFavTemp(3);
  });
  document.getElementById('fav4_t').addEventListener('change', function () {
    changeFavName(4);
  });
  document.getElementById('fav4_n').addEventListener('change', function () {
    changeFavTemp(4);
  });
}

const changeFavTemp = (target) => {
  var selement = document.getElementById('fav1_t');
  var cmd = SerialCommand.FAV_1;
  switch (target) {
    case 1:
      selement = document.getElementById('fav1_n');
      cmd = SerialCommand.FAV_1;
      break;
    case 2:
      selement = document.getElementById('fav2_n');
      cmd = SerialCommand.FAV_2;
      break;
    case 3:
      selement = document.getElementById('fav3_n');
      cmd = SerialCommand.FAV_3;
      break;
    case 4:
      selement = document.getElementById('fav4_n');
      cmd = SerialCommand.FAV_4;
      break;
    default:
      selement = document.getElementById('fav1_n');
      cmd = SerialCommand.FAV_1;
      break;
  }
  var val = selement.value;
  console.log('new fav temp: ' + val);
  emit_websocket([cmd, Operation.SET, Favorite.TEMP, ...doubleToBytes(Number(val))]);
};

const changeFavName = (target) => {
  var selement = document.getElementById('fav1_t');
  var cmd = SerialCommand.FAV_1;
  switch (target) {
    case 1:
      selement = document.getElementById('fav1_t');
      cmd = SerialCommand.FAV_1;
      break;
    case 2:
      selement = document.getElementById('fav2_t');
      cmd = SerialCommand.FAV_2;
      break;
    case 3:
      selement = document.getElementById('fav3_t');
      cmd = SerialCommand.FAV_3;
      break;
    case 4:
      selement = document.getElementById('fav4_t');
      cmd = SerialCommand.FAV_4;
      break;
    default:
      selement = document.getElementById('fav1_t');
      cmd = SerialCommand.FAV_1;
      break;
  }
  var val = selement.value;
  console.log(val);
  val = `${val}\0`;
  emit_websocket([cmd, Operation.SET, Favorite.NAME, ...new TextEncoder().encode(val)]);
};

// Temp Funcs/Vars
document.getElementById('temp_setting').addEventListener('change', function () {
  updateTemp(0);
});
document.getElementById('temp_slider').addEventListener('change', function () {
  updateTemp(1);
});
const updateTemp = (target) => {
  var selement = document.getElementById('temp_setting');
  if (target == 1) {
    selement = document.getElementById('temp_slider');
  }

  var value =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? fToC(selement.value)
      : state.UNIT == TemperatureUnit.KELVIN
      ? kToC(selement.value)
      : selement.value;
  emit_websocket([SerialCommand.TEMP, Operation.SET, ...doubleToBytes(Number(selement.value))]);
};

// Bump Funcs/Vars
document.getElementById('bump_amt').addEventListener('change', function () {
  emit_websocket([SerialCommand.BUMP_AMOUNT, Operation.SET, ...doubleToBytes(Number(this.value))]);
});
document.getElementById('bump_l').addEventListener('change', function () {
  emit_websocket([SerialCommand.BUMP_LENGTH, Operation.SET, ...doubleToBytes(Number(this.value))]);
});
document.getElementById('toggle_bump').addEventListener('click', function () {
  toggleBump();
});
const toggleBump = () => {
  emit_websocket([SerialCommand.BUMP_TOGGLE, Operation.SET, state.BUMP.enabled ? 0 : 1]);
  state.BUMP.enabled = !state.BUMP.enabled;
};

// LED Functions
document.getElementById('led_rainbow').addEventListener('click', function () {
  toggleLED_rainbowWave();
});
document.getElementById('led_rainbow2').addEventListener('click', function () {
  toggleLED_rainbow();
});
document.getElementById('led_rainbow3').addEventListener('click', function () {
  toggleLED_rainbowPulse();
});
document.getElementById('led_shift').addEventListener('click', function () {
  toggleLED_preShift();
});
document.getElementById('led_pulse').addEventListener('click', function () {
  toggleLED_prePulse();
});
document.getElementById('leden_blink').addEventListener('click', function () {
  toggleLED_amBlink();
});
document.getElementById('leden_pulse').addEventListener('click', function () {
  toggleLED_amPulse();
});
document.getElementById('leden_static').addEventListener('click', function () {
  toggleLED_static();
});
/*document.getElementById('led_static').addEventListener('click', function () {
  toggleLED_static();
});*/
document.getElementById('led_status').addEventListener('click', function () {
  toggleLED_status();
});

document.getElementById('toggle_lights').addEventListener('click', function () {
  toggleLights();
});
document.getElementById('toggle_lights2').addEventListener('click', function () {
  toggleLights();
});

const toggleLights = () => {
  emit_websocket([SerialCommand.LED, Operation.SET, Led.ENABLE, state.LED.enabled ? 0 : 1]);
  state.LED.enabled = !state.LED.enabled;
};
const setLED_Toggles = (mode) => {
  //TODO: Reset all LED Toggles to off then set mode.
  buttons = document.getElementsByClassName('ledbtn');
  for (var i = 0; i < buttons.length; i++) {
    buttons[i].disabled = false; //.innerText="Enable";
  }
  if (state.LED.mode != mode) {
    state.LED.mode = mode;
    return true;
  } else {
    return false;
  }
};
const initLED_toggles = (mode) => {
  buttons = document.getElementsByClassName('ledbtn');
  for (var i = 0; i < buttons.length; i++) {
    buttons[i].disabled = false; //.innerText="Enable";
  }
  switch (mode) {
    case LedMode.AMBIENT:
      break;
    case LedMode.BLINK:
      document.getElementById('leden_blink').disabled = true;
      break;
    case LedMode.PULSE:
      document.getElementById('leden_pulse').disabled = true;
      break;
    case LedMode.PRESET:
      break;
    case LedMode.RAINBOW:
      document.getElementById('led_rainbow2').disabled = true;
      break;
    case LedMode.RAINBOW_PULSE:
      document.getElementById('led_rainbow3').disabled = true;
      break;
    case LedMode.RAINBOW_WAVE:
      document.getElementById('led_rainbow').disabled = true;
      break;
    case LedMode.SHIFT:
      document.getElementById('led_shift').disabled = true;
      break;
    case LedMode.PREPULSE:
      document.getElementById('led_pulse').disabled = true;
      break;
    case LedMode.STATIC:
      //document.getElementById('led_static').disabled = true;
      document.getElementById('leden_static').disabled = true;
      break;
    case LedMode.STATUS: // Shouldn't turn off other ones.
      break;

    default:
      break;
  }
};
const toggleLED_status = () => {
  emit_websocket([SerialCommand.LED, Operation.SET, Led.STAT, state.LED.status ? 0 : 1]);
  if (!state.LED.status) {
    //(setLED_Toggles(LedMode.STATUS)){
    state.LED.status = true;
    document.getElementById('statusbut_span').innerText = 'Disable';
  } else {
    state.LED.status = false;
    document.getElementById('statusbut_span').innerText = 'Enable';
  }
};
const toggleLED_static = () => {
  emit_websocket([
    SerialCommand.LED,
    Operation.SET,
    Led.MODE,
    LedMode.STATIC,
    state.LED.mode == LedMode.STATIC ? 0 : 1
  ]);
  if (setLED_Toggles(LedMode.STATIC)) {
    //document.getElementById("staticbut_span").innerText = "Disable";
    //document.getElementById('led_static').disabled = true;
    document.getElementById('leden_static').disabled = true;
  }
};
const toggleLED_amPulse = () => {
  emit_websocket([
    SerialCommand.LED,
    Operation.SET,
    Led.MODE,
    LedMode.PULSE,
    state.LED.mode == LedMode.PULSE ? 0 : 1
  ]);
  if (setLED_Toggles(LedMode.PULSE)) {
    document.getElementById('leden_pulse').disabled = true;
  }
};
const toggleLED_amBlink = () => {
  emit_websocket([
    SerialCommand.LED,
    Operation.SET,
    Led.MODE,
    LedMode.BLINK,
    state.LED.mode == LedMode.BLINK ? 0 : 1
  ]);
  if (setLED_Toggles(LedMode.BLINK)) {
    document.getElementById('leden_blink').disabled = true;
  }
};
const toggleLED_rainbowWave = () => {
  emit_websocket([
    SerialCommand.LED,
    Operation.SET,
    Led.MODE,
    LedMode.RAINBOW_WAVE,
    state.LED.mode == LedMode.RAINBOW_WAVE ? 0 : 1
  ]);
  if (setLED_Toggles(LedMode.RAINBOW_WAVE)) {
    //document.getElementById("rainbowbut_span").disabled = true;
    document.getElementById('led_rainbow').disabled = true;
  }
};
const toggleLED_rainbow = () => {
  emit_websocket([
    SerialCommand.LED,
    Operation.SET,
    Led.MODE,
    LedMode.RAINBOW,
    state.LED.mode == LedMode.RAINBOW ? 0 : 1
  ]);
  if (setLED_Toggles(LedMode.RAINBOW)) {
    //document.getElementById("rainbowbut2_span").disabled = true;
    document.getElementById('led_rainbow2').disabled = true;
  }
};
const toggleLED_rainbowPulse = () => {
  emit_websocket([
    SerialCommand.LED,
    Operation.SET,
    Led.MODE,
    LedMode.RAINBOW_PULSE,
    state.LED.mode == LedMode.RAINBOW_PULSE ? 0 : 1
  ]);
  if (setLED_Toggles(LedMode.RAINBOW_PULSE)) {
    //document.getElementById("rainbowbut3_span").disabled = true;
    document.getElementById('led_rainbow3').disabled = true;
  }
};
const toggleLED_preShift = () => {
  emit_websocket([
    SerialCommand.LED,
    Operation.SET,
    Led.MODE,
    LedMode.SHIFT,
    state.LED.mode == LedMode.SHIFT ? 0 : 1
  ]);
  if (setLED_Toggles(LedMode.SHIFT)) {
    //document.getElementById("preshiftbut_span").disabled = true;
    document.getElementById('led_shift').disabled = true;
  }
};
const toggleLED_prePulse = () => {
  emit_websocket([
    SerialCommand.LED,
    Operation.SET,
    Led.MODE,
    LedMode.PREPULSE,
    state.LED.mode == LedMode.PREPULSE ? 0 : 1
  ]);
  if (setLED_Toggles(LedMode.PREPULSE)) {
    //document.getElementById("prepulsebut_span").disabled = true;
    document.getElementById('led_pulse').disabled = true;
  }
};
const updateBrightness = () => {
  emit_websocket([SerialCommand.LED, Operation.SET, Led.BRIGHTNESS, 0]);
};
// End LED Functions

// Coil Toggle Funcs/Vars
document.getElementById('toggle_heat').addEventListener('click', function () {
  toggleHeat();
});
const toggleHeat = () => {
  emit_websocket([SerialCommand.COIL_TOGGLE, Operation.SET, state.COIL.enabled ? 0 : 1]);
};

// Zoom change
const zoomChange = (obj) => {
  const value = obj.value;
  updateZoom(value);
  state.ZOOM_LEVEL = value;
  chart.update();
};

// ==================================================
// ==================================================

// init load
const init = () => {
  pidButtonInit();
  favListenersInit();
  window.addEventListener('load', onLoad);
  function onLoad(event) {
    initWebSocket();
  }
};

init();
