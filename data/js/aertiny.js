// ==================================================
// AerTiny
// ==================================================

// web sockets setup
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// ==================================================
// ==================================================

Chart.defaults.global.animation.duration = 0;

const createChart = (name) => {
  // charting
  const chart = new Chart(name, {
    type: 'line',
    animation: false,
    labels: ['0', '16', '32', '48', '64', '80', '96'],
    data: {
      datasets: [
        {
          label: 'Measure',
          data: [],
          borderColor: 'red',
          borderWidth: 0.9,
          fill: false,
          pointRadius: 1.2,
          yAxisID: 'y'
        },
        {
          label: 'Average 1 Minute',
          data: [],
          borderColor: 'orange',
          borderWidth: 2.2,
          fill: false,
          pointRadius: 0.8,
          yAxisID: 'y'
        },
        {
          label: 'Set Temperature',
          data: [],
          borderColor: 'yellow',
          borderWidth: 1.8,
          fill: false,
          pointRadius: 0.0,
          yAxisID: 'y'
        },
        {
          label: 'Measure Trend',
          data: [],
          borderColor: '#81d41a',
          borderWidth: 1,
          fill: false,
          pointRadius: 0,
          hidden: true,
          yAxisID: 'y'
        },
        {
          label: 'Output',
          data: [],
          borderColor: 'magenta',
          borderWidth: 1,
          fill: false,
          pointRadius: 1,
          hidden: true,
          yAxisID: 'y1'
        },
        {
          label: 'Sigma',
          data: [],
          borderColor: 'purple',
          borderWidth: 1,
          fill: false,
          pointRadius: 0,
          hidden: true,
          yAxisID: 'y'
        }
      ]
    },
    options: {
      scales: {
        xAxes: [
          {
            gridLines: {
              display: true,
              color: 'rgba(255, 255, 255, 0.1)' // Set the x-axis grid color to a light color
            }
          }
        ],
        yAxes: [
          {
            id: 'y',
            type: 'linear',
            display: true,
            position: 'left',
            gridLines: {
              display: true,
              color: 'rgba(255, 255, 255, 0.1)' // Set the x-axis grid color to a light color
            }
          },
          {
            id: 'y1',
            type: 'linear',
            display: true,
            position: 'right',
            color: 'blue',
            title: {
              display: true,
              color: 'blue'
            },
            ticks: {
              beginAtZero: true,
              color: 'blue'
            },

            // grid line settings
            grid: {
              drawOnChartArea: false, // only want the grid lines for one axis to show up
              backdropColor: 'blue',
              color: 'blue'
            }
          }
          /*{
            id: 'y2',
            type: 'linear',
            display: true,
            position: 'right',
            color: 'purple',
            title: {
              display: true,
              color: 'purple'
            },
            ticks: {
              beginAtZero: true,
              color: 'purple'
            },

            // grid line settings
            grid: {
              drawOnChartArea: false, // only want the grid lines for one axis to show up
              backdropColor: 'blue',
              color: 'purple'
            }
          }*/
        ]
      }
    }
  });
  return chart;
};

const chart = createChart('temp_chart');
const chart2 = createChart('temp_chart2');

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

const maxChartTime = 60 * 15;

const updateChart = (state, t, ta, st, o, s) => {
  if (state.TEMPS.length > maxChartTime) {
    state.TEMPS.shift();
  }
  state.TEMPS.push(t);

  if (state.TEMPS_AVG.length > maxChartTime) {
    state.TEMPS_AVG.shift();
  }
  state.TEMPS_AVG.push(ta);

  if (state.OUTPUT.length > maxChartTime) {
    state.OUTPUT.shift();
  }
  state.OUTPUT.push(o);

  if (state.SIGMA.length > maxChartTime) {
    state.SIGMA.shift();
  }
  state.SIGMA.push(s);

  let maxTime = 32;
  if (state.TIME_SPAN1 == 'one') {
    maxTime = 64;
  } else if (state.TIME_SPAN1 == 'two') {
    maxTime = 128;
  } else if (state.TIME_SPAN1 == 'three') {
    maxTime = 3 * 60;
  } else if (state.TIME_SPAN1 == 'five') {
    maxTime = 5 * 60;
  } else if (state.TIME_SPAN1 == 'seven') {
    maxTime = 7 * 60;
  } else if (state.TIME_SPAN1 == 'ten') {
    maxTime = 10 * 60;
  } else if (state.TIME_SPAN1 == 'fifteen') {
    maxTime = 15 * 60;
  }

  const labels = [];
  const temps = [];
  const set_temps = [];
  const avg_temps = [];
  const outputs = [];
  const sigmas = [];
  let sTemps = [];
  for (let i = 0; i < state.TEMPS.length; i++) {
    sTemps.push(state.TEMPS[i]);
  }
  sTemps = sTemps.reverse();
  let sTempsAvg = [];
  for (let i = 0; i < state.TEMPS_AVG.length; i++) {
    sTempsAvg.push(state.TEMPS_AVG[i]);
  }
  sTempsAvg = sTempsAvg.reverse();
  let sOutput = [];
  for (let i = 0; i < state.OUTPUT.length; i++) {
    sOutput.push(state.OUTPUT[i]);
  }
  sOutput = sOutput.reverse();
  let sSigma = [];
  for (let i = 0; i < state.SIGMA.length; i++) {
    sSigma.push(state.SIGMA[i]);
  }
  sSigma = sSigma.reverse();
  for (let i = 0; i < sTemps.length; i++) {
    if (i >= maxTime) {
      break;
    }
    temps.push({ x: i, y: sTemps[i] });
    avg_temps.push({ x: i, y: sTempsAvg[i] });
    set_temps.push({
      x: i,
      y: st
    });
    //outputs.push({ x: i, y: sOutput[i] });
    //sigmas.push({ x: i, y: sSigma[i] });
    outputs.push(sOutput[i]);
    sigmas.push(sSigma[i]);
    if (sTemps.length <= 32) {
      if (i % 2 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    } else if (sTemps.length <= 64) {
      if (i % 8 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    } else if (sTemps.length <= 128) {
      if (i % 16 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    } else {
      if (i % 15 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    }
  }

  chart.data.datasets[0].data = temps;
  chart.data.datasets[1].data = avg_temps;
  chart.data.datasets[2].data = set_temps;
  chart.data.datasets[3].data = calculateTrendLine(temps);
  chart.data.datasets[4].data = outputs;
  chart.data.datasets[5].data = sigmas;
  chart.data.labels = labels;
  if (zoomRefreshTick <= 0) {
    updateZoom(state.ZOOM_LEVEL);
    zoomRefreshTick = 3;
  } else {
    zoomRefreshTick--;
  }
  chart.update();
};

let zoomRefreshTick2 = 0;

const updateChart2 = (state, t, ta, st, o, s) => {
  if (state.TEMPS2.length > maxChartTime) {
    state.TEMPS2.shift();
  }
  state.TEMPS2.push(t);

  if (state.TEMPS_AVG2.length > maxChartTime) {
    state.TEMPS_AVG2.shift();
  }
  state.TEMPS_AVG2.push(ta);

  if (state.OUTPUT.length > maxChartTime) {
    state.OUTPUT.shift();
  }
  state.OUTPUT.push(o);

  if (state.SIGMA.length > maxChartTime) {
    state.SIGMA.shift();
  }
  state.SIGMA.push(s);

  let maxTime = 32;
  if (state.TIME_SPAN2 == 'one') {
    maxTime = 64;
  } else if (state.TIME_SPAN2 == 'two') {
    maxTime = 128;
  } else if (state.TIME_SPAN2 == 'three') {
    maxTime = 3 * 60;
  } else if (state.TIME_SPAN2 == 'five') {
    maxTime = 5 * 60;
  } else if (state.TIME_SPAN2 == 'seven') {
    maxTime = 7 * 60;
  } else if (state.TIME_SPAN2 == 'ten') {
    maxTime = 10 * 60;
  } else if (state.TIME_SPAN2 == 'fifteen') {
    maxTime = 15 * 60;
  }

  const labels = [];
  const temps = [];
  const set_temps = [];
  const avg_temps = [];
  const outputs = [];
  const sigmas = [];
  let sTemps = [];
  for (let i = 0; i < state.TEMPS2.length; i++) {
    sTemps.push(state.TEMPS2[i]);
  }
  sTemps = sTemps.reverse();
  let sTempsAvg = [];
  for (let i = 0; i < state.TEMPS_AVG2.length; i++) {
    sTempsAvg.push(state.TEMPS_AVG2[i]);
  }
  sTempsAvg = sTempsAvg.reverse();
  let sOutput = [];
  for (let i = 0; i < state.OUTPUT.length; i++) {
    sOutput.push(state.OUTPUT[i]);
  }
  sOutput = sOutput.reverse();
  let sSigma = [];
  for (let i = 0; i < state.SIGMA.length; i++) {
    sSigma.push(state.SIGMA[i]);
  }
  sSigma = sSigma.reverse();
  for (let i = 0; i < sTemps.length; i++) {
    if (i >= maxTime) {
      break;
    }
    temps.push({ x: i, y: sTemps[i] });
    avg_temps.push({ x: i, y: sTempsAvg[i] });
    set_temps.push({
      x: i,
      y: st
    });
    outputs.push(sOutput[i]);
    sigmas.push(sSigma[i]);
    if (sTemps.length <= 32) {
      if (i % 2 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    } else if (sTemps.length <= 64) {
      if (i % 8 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    } else if (sTemps.length <= 128) {
      if (i % 16 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    } else {
      if (i % 15 == 0) {
        labels.push(`${i}`);
      } else {
        labels.push('');
      }
    }
  }

  chart2.data.datasets[0].data = temps;
  chart2.data.datasets[1].data = avg_temps;
  chart2.data.datasets[2].data = set_temps;
  chart2.data.datasets[3].data = calculateTrendLine(temps);
  chart2.data.datasets[4].data = outputs;
  chart2.data.datasets[5].data = sigmas;
  chart2.data.labels = labels;
  if (zoomRefreshTick2 <= 0) {
    updateZoom2(state.ZOOM_LEVEL2);
    zoomRefreshTick2 = 3;
  } else {
    zoomRefreshTick2--;
  }
  chart2.update();
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
    chart.data.datasets[1].borderColor = '#5adac2';
    chart.data.datasets[2].borderColor = '#2a6099';
    chart.data.datasets[3].borderColor = '#650953';
  }
  chart.update();
};

const palletChange2 = (obj) => {
  const value = obj.value;
  if (value == 'warm') {
    chart2.data.datasets[0].borderColor = 'red';
    chart2.data.datasets[1].borderColor = 'orange';
    chart2.data.datasets[2].borderColor = 'yellow';
    chart2.data.datasets[3].borderColor = '#81d41a';
  } else if (value == 'cool') {
    chart2.data.datasets[0].borderColor = '#6A5ACD';
    chart2.data.datasets[1].borderColor = '#87faf2';
    chart2.data.datasets[2].borderColor = '#4169e1';
    chart2.data.datasets[3].borderColor = '#dd4cf4';
  } else if (value == 'forest') {
    chart2.data.datasets[0].borderColor = '#158466';
    chart2.data.datasets[1].borderColor = '#40E0D0';
    chart2.data.datasets[2].borderColor = '#81d41a';
    chart2.data.datasets[3].borderColor = '#2a6099';
  } else if (value == 'traffic') {
    chart2.data.datasets[0].borderColor = '#ff0000';
    chart2.data.datasets[1].borderColor = '#ffbf00';
    chart2.data.datasets[2].borderColor = '#00a933';
    chart2.data.datasets[3].borderColor = '#87faf2';
  } else if (value == 'void') {
    chart2.data.datasets[0].borderColor = '#ffffff';
    chart2.data.datasets[1].borderColor = '#5adac2';
    chart2.data.datasets[2].borderColor = '#2a6099';
    chart2.data.datasets[3].borderColor = '#650953';
  }
  chart2.update();
};

const updateZoom = (value) => {
  if (!state.TEMPS || state.TEMPS.length == 0) {
    return;
  }
  let low = 1000;
  let high = 0;
  let sTemps = [];
  for (let i = 0; i < state.TEMPS.length; i++) {
    sTemps.push(state.TEMPS[i]);
  }
  sTemps = sTemps.reverse();
  let maxTime = 32;
  if (state.TIME_SPAN1 == 'one') {
    maxTime = 64;
  } else if (state.TIME_SPAN1 == 'two') {
    maxTime = 128;
  } else if (state.TIME_SPAN1 == 'three') {
    maxTime = 3 * 60;
  } else if (state.TIME_SPAN1 == 'five') {
    maxTime = 5 * 60;
  } else if (state.TIME_SPAN1 == 'seven') {
    maxTime = 7 * 60;
  } else if (state.TIME_SPAN1 == 'ten') {
    maxTime = 10 * 60;
  } else if (state.TIME_SPAN1 == 'fifteen') {
    maxTime = 15 * 60;
  }
  let i = 0;
  for (const _t of sTemps) {
    if (i++ > maxTime) {
      break;
    }
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

const updateZoom2 = (value) => {
  if (!state.TEMPS2 || state.TEMPS2.length == 0) {
    return;
  }
  let low = 1000;
  let high = 0;
  let sTemps = [];
  for (let i = 0; i < state.TEMPS2.length; i++) {
    sTemps.push(state.TEMPS2[i]);
  }
  sTemps = sTemps.reverse();
  let maxTime = 32;
  if (state.TIME_SPAN2 == 'one') {
    maxTime = 64;
  } else if (state.TIME_SPAN2 == 'two') {
    maxTime = 128;
  } else if (state.TIME_SPAN2 == 'three') {
    maxTime = 3 * 60;
  } else if (state.TIME_SPAN2 == 'five') {
    maxTime = 5 * 60;
  } else if (state.TIME_SPAN2 == 'seven') {
    maxTime = 7 * 60;
  } else if (state.TIME_SPAN2 == 'ten') {
    maxTime = 10 * 60;
  } else if (state.TIME_SPAN2 == 'fifteen') {
    maxTime = 15 * 60;
  }
  let i = 0;
  for (const _t of sTemps) {
    if (i++ > maxTime) {
      break;
    }
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
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart2.config.options.scales.yAxes[0].ticks.min = undefined;
    chart2.config.options.scales.yAxes[0].ticks.max = undefined;
  } else if (value == 'fine') {
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart2.config.options.scales.yAxes[0].ticks.min = low - 0.1;
    chart2.config.options.scales.yAxes[0].ticks.max = high + 0.1;
  } else if (value == 'tiny') {
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart2.config.options.scales.yAxes[0].ticks.min = low - 1;
    chart2.config.options.scales.yAxes[0].ticks.max = high + 1;
  } else if (value == 'near') {
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart2.config.options.scales.yAxes[0].ticks.min = low - hDelta - 1;
    chart2.config.options.scales.yAxes[0].ticks.max = high + hDelta + 1;
  } else if (value == 'avg') {
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart2.config.options.scales.yAxes[0].ticks.min = low - delta - 3;
    chart2.config.options.scales.yAxes[0].ticks.max = high + delta + 3;
  } else if (value == 'far') {
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart2.config.options.scales.yAxes[0].ticks.min = low - dDelta - 7;
    chart2.config.options.scales.yAxes[0].ticks.max = high + dDelta + 7;
  } else if (value == 'dist') {
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = false;
    chart2.config.options.scales.yAxes[0].ticks.min = low - dDelta - delta - 10;
    chart2.config.options.scales.yAxes[0].ticks.max = high + dDelta + delta + 10;
  } else if (value == 'max') {
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = true;
    chart2.config.options.scales.yAxes[0].ticks.min = low - delta - 25;
    chart2.config.options.scales.yAxes[0].ticks.max = high + delta + 25;
  } else if (value == 'all') {
    chart2.config.options.scales.yAxes[0].ticks.beginAtZero = true;
    chart2.config.options.scales.yAxes[0].ticks.min = 0;
    chart2.config.options.scales.yAxes[0].ticks.max = high + 25;
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
  STATUS2: 0x23,
  AUTO_OFF_LENGTH: 0x35,
  AUTO_OFF_TOGGLE: 0x32,
  ADJUST_AMOUNT: 0x24,
  BLE: 0x50,
  BUMP_UPDATE: 0x43,
  BUMP_AMOUNT: 0x44,
  BUMP_LENGTH: 0x45,
  BUMP_TOGGLE: 0x42,
  COIL_TOGGLE: 0x52,
  COIL_TOGGLE2: 0x53,
  ESP: 0x00,
  FAV_1: 0x86,
  FAV_2: 0x87,
  FAV_3: 0x88,
  FAV_4: 0x89,
  LED: 0x70,
  PID: 0x90,
  PID2: 0x91,
  TEMP: 0x10,
  TEMP2: 0x13,
  UNIT: 0x11,
  WIFI: 0x60,
  MODE_DEBUG: 0xff
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
  MODEL: 0,
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
  TEMP2: 0,
  SET_TEMP2: 0,
  AVG_TEMP2: 0,
  TEMPS2: [],
  TEMPS_AVG2: [],
  OUTPUT: [],
  SIGMA: [],
  OUTPUT2: [],
  SIGMA2: [],
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
  COIL2: {
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
  ZOOM_LEVEL: 'auto',
  ZOOM_LEVEL2: 'auto',
  TIME_SPAN1: 'two',
  TIME_SPAN2: 'two',
  DEV_MODE_DEBUG: false
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
  const booleanMap = [...Array(8)].map((_, i) => Boolean(data.slice(608, 609)[0] & (1 << (7 - i))));
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
  const model = data.slice(609, 610);

  return {
    model,
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

let connecting = false;
let connected = false;

function initWebSocket() {
  if (connecting) {
    return;
  }
  console.log('Attempting to open WebSocket connection...');
  connecting = true;
  document.getElementById('sockets-modal').style.display = 'block';
  document.getElementById('sockets-modal-text').innerHTML = '...Connecting...';
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}
function onOpen(event) {
  console.log('Connection to host opened!');
  connecting = false;
  connected = true;
  document.getElementById('sockets-modal-text').innerHTML = 'Connected!';
  document.getElementById('sockets-modal').style.display = 'none';
}
function onClose(event) {
  console.log('Connection to host closed!');
  connecting = false;
  connected = false;
  document.getElementById('sockets-modal-text').innerHTML = 'Connection Error!';
  document.getElementById('sockets-modal').style.display = 'block';
  //setTimeout(initWebSocket, 3000);
}

const initPageData = async (initData) => {
  state.MODEL = initData.model;
  if (state.MODEL == 2) {
    if (document.getElementById('element1')) {
      document.getElementById('element1').style.display = 'block';
    }
    if (document.getElementById('element2')) {
      document.getElementById('element2').style.display = 'block';
    }
  }
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
  if (document.getElementById('p_qset2')) {
    document.getElementById('p_qset2').value = `${state.COIL.P}`;
  }
  state.COIL.I = initData.I;
  if (document.getElementById('i_set')) {
    document.getElementById('i_set').value = `${state.COIL.I}`;
  }
  if (document.getElementById('i_qset')) {
    document.getElementById('i_qset').value = `${state.COIL.I}`;
  }
  if (document.getElementById('i_qset2')) {
    document.getElementById('i_qset2').value = `${state.COIL.I}`;
  }
  state.COIL.D = initData.D;
  if (document.getElementById('d_set')) {
    document.getElementById('d_set').value = `${state.COIL.D}`;
  }
  if (document.getElementById('d_qset')) {
    document.getElementById('d_qset').value = `${state.COIL.D}`;
  }
  if (document.getElementById('d_qset2')) {
    document.getElementById('d_qset2').value = `${state.COIL.D}`;
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
  state.SET_TEMP = initData.setTemp;
  if (document.getElementById('temp_setting'))
    document.getElementById('temp_setting').value =
      state.UNIT == TemperatureUnit.FAHRENHEIT
        ? cToF(state.SET_TEMP).toFixed(1)
        : state.UNIT == TemperatureUnit.KELVIN
        ? cToK(state.SET_TEMP).toFixed(1)
        : cToC(state.SET_TEMP).toFixed(1);

  updateTempMeter();
  updateTempSlider();

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

  emit_websocket([SerialCommand.PID2, Operation.GET, ParamPid.PARAM_PID]);
};

const updateTempMeter = () => {
  const bar = document.getElementById('bar');
  const temp =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.TEMP).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.TEMP).toFixed(1)
      : cToC(state.TEMP).toFixed(1);
  const temp_setting =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.SET_TEMP).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.SET_TEMP).toFixed(1)
      : cToC(state.SET_TEMP).toFixed(1);
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

const updateTempMeter_1 = () => {
  const bar = document.getElementById('bar1');
  const temp =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.TEMP).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.TEMP).toFixed(1)
      : cToC(state.TEMP).toFixed(1);
  const temp_setting =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.SET_TEMP).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.SET_TEMP).toFixed(1)
      : cToC(state.SET_TEMP).toFixed(1);
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

const updateTempMeter_2 = () => {
  const bar = document.getElementById('bar2');
  const temp =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.TEMP2).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.TEMP2).toFixed(1)
      : cToC(state.TEMP2).toFixed(1);
  const temp_setting =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.SET_TEMP2).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.SET_TEMP2).toFixed(1)
      : cToC(state.SET_TEMP2).toFixed(1);
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
  var elm_ts = document.getElementById('temp_slider');
  var isFocused = document.activeElement === elm_ts;
  if (isFocused) {
    return;
  }
  document.getElementById('temp_slider').value =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.SET_TEMP).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.SET_TEMP).toFixed(1)
      : cToC(state.SET_TEMP).toFixed(1);
}

function updateTempSlider_1() {
  var elm_ts = document.getElementById('temp_slider_1');
  var isFocused = document.activeElement === elm_ts;
  if (isFocused) {
    return;
  }
  document.getElementById('temp_slider_1').value =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.SET_TEMP).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.SET_TEMP).toFixed(1)
      : cToC(state.SET_TEMP).toFixed(1);
}

function updateTempSlider_2() {
  var elm_ts = document.getElementById('temp_slider_2');
  var isFocused = document.activeElement === elm_ts;
  if (isFocused) {
    return;
  }
  document.getElementById('temp_slider_2').value =
    state.UNIT == TemperatureUnit.FAHRENHEIT
      ? cToF(state.SET_TEMP2).toFixed(1)
      : state.UNIT == TemperatureUnit.KELVIN
      ? cToK(state.SET_TEMP2).toFixed(1)
      : cToC(state.SET_TEMP2).toFixed(1);
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
      var set_temp = getNumber(dat.slice(7, 9));
      var output = getNumber(dat.slice(9, 11));
      var sigma = getNumber(dat.slice(11, 13));
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
          : cToC(avg_temp).toFixed(1),
        state.UNIT == TemperatureUnit.FAHRENHEIT
          ? cToF(set_temp).toFixed(1)
          : state.UNIT == TemperatureUnit.KELVIN
          ? cToK(set_temp).toFixed(1)
          : cToC(set_temp).toFixed(1),
        output / 10,
        sigma / 10
      );
      //console.log('SerialCommand.STATUS: ', pid_enb, mes_temp, avg_temp, set_temp);
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
      if (document.getElementById('toggle_heat_1')) {
        const element = document.getElementById('toggle_heat_1');
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
      if (document.getElementById('meas_temp_1'))
        document.getElementById('meas_temp_1').innerHTML =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.TEMP).toFixed(1)
            : cToC(state.TEMP).toFixed(1);
      var elm_ts = document.getElementById('temp_setting');
      var isFocused = document.activeElement === elm_ts;
      if (!isFocused && document.getElementById('temp_setting'))
        document.getElementById('temp_setting').value =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.SET_TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.SET_TEMP).toFixed(1)
            : cToC(state.SET_TEMP).toFixed(1);
      var elm_ts2 = document.getElementById('temp_setting_1');
      var isFocused2 = document.activeElement === elm_ts2;
      if (!isFocused2 && document.getElementById('temp_setting_1'))
        document.getElementById('temp_setting_1').value =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.SET_TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.SET_TEMP).toFixed(1)
            : cToC(state.SET_TEMP).toFixed(1);
      updateTempMeter();
      updateTempMeter_1();
      updateTempSlider();
      updateTempSlider_1();
      break;
    case SerialCommand.STATUS2:
      pid_enb = dat[2] > 0;
      var mes_temp = getNumber(dat.slice(3, 5));
      var avg_temp = getNumber(dat.slice(5, 7));
      var set_temp = getNumber(dat.slice(7, 9));
      var output = getNumber(dat.slice(9, 11));
      var sigma = getNumber(dat.slice(11, 13));
      updateChart2(
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
          : cToC(avg_temp).toFixed(1),
        state.UNIT == TemperatureUnit.FAHRENHEIT
          ? cToF(set_temp).toFixed(1)
          : state.UNIT == TemperatureUnit.KELVIN
          ? cToK(set_temp).toFixed(1)
          : cToC(set_temp).toFixed(1),
        output / 10,
        sigma / 10
      );
      //console.log('SerialCommand.STATUS: ', pid_enb, mes_temp, avg_temp, set_temp);
      state.COIL2.enabled = pid_enb;
      state.TEMP2 = mes_temp;
      state.SET_TEMP2 = set_temp;
      if (document.getElementById('toggle_heat_2')) {
        const element = document.getElementById('toggle_heat_2');
        if (state.COIL2.enabled) {
          element.style.setProperty('background-color', '#faa843', 'important');
          element.style.setProperty('color', 'whitesmoke', 'important');
        } else {
          element.style.removeProperty('background-color');
          element.style.removeProperty('color');
        }
      }
      if (document.getElementById('meas_temp_2'))
        document.getElementById('meas_temp_2').innerHTML =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.TEMP2).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.TEMP2).toFixed(1)
            : cToC(state.TEMP2).toFixed(1);
      var elm_ts = document.getElementById('temp_setting_2');
      var isFocused = document.activeElement === elm_ts;
      if (!isFocused && document.getElementById('temp_setting_2'))
        document.getElementById('temp_setting_2').value =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.SET_TEMP2).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.SET_TEMP2).toFixed(1)
            : cToC(state.SET_TEMP2).toFixed(1);
      updateTempMeter_2();
      updateTempSlider_2();
      break;
    case SerialCommand.TEMP:
      var set_temp = getNumber(dat.slice(2));
      //console.log('SerialCommand.TEMP: ', set_temp);
      state.SET_TEMP = set_temp;
      var elm_ts = document.getElementById('temp_setting');
      var isFocused = document.activeElement === elm_ts;
      if (!isFocused && document.getElementById('temp_setting'))
        document.getElementById('temp_setting').value =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.SET_TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.SET_TEMP).toFixed(1)
            : cToC(state.SET_TEMP).toFixed(1);
      var elm_ts2 = document.getElementById('temp_setting_1');
      var isFocused2 = document.activeElement === elm_ts2;
      if (!isFocused2 && document.getElementById('temp_setting_1'))
        document.getElementById('temp_setting_1').value =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.SET_TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.SET_TEMP).toFixed(1)
            : cToC(state.SET_TEMP).toFixed(1);
      updateTempMeter();
      updateTempMeter_1();
      updateTempSlider();
      updateTempSlider_1();
      break;
    case SerialCommand.TEMP2:
      var set_temp = getNumber(dat.slice(2));
      //console.log('SerialCommand.TEMP: ', set_temp);
      state.SET_TEMP = set_temp;
      var elm_ts = document.getElementById('temp_setting_2');
      var isFocused = document.activeElement === elm_ts;
      if (!isFocused && document.getElementById('temp_setting_2'))
        document.getElementById('temp_setting_2').value =
          state.UNIT == TemperatureUnit.FAHRENHEIT
            ? cToF(state.SET_TEMP).toFixed(1)
            : state.UNIT == TemperatureUnit.KELVIN
            ? cToK(state.SET_TEMP).toFixed(1)
            : cToC(state.SET_TEMP).toFixed(1);
      updateTempMeter_2();
      updateTempSlider_2();
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
      if (document.getElementById('toggle_heat_1')) {
        const element = document.getElementById('toggle_heat_1');
        if (state.COIL.enabled) {
          element.style.setProperty('background-color', '#faa843', 'important');
          element.style.setProperty('color', 'whitesmoke', 'important');
        } else {
          element.style.removeProperty('background-color');
          element.style.removeProperty('color');
        }
      }
      break;
    case SerialCommand.COIL_TOGGLE2:
      pid_enb = dat[1] > 0;
      state.COIL2.enabled = pid_enb;
      if (document.getElementById('toggle_heat_2')) {
        const element = document.getElementById('toggle_heat_2');
        if (state.COIL2.enabled) {
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
          if (document.getElementById('p_qset2')) {
            document.getElementById('p_qset2').value = `${state.COIL.P}`;
          }
          state.COIL.I = I;
          if (document.getElementById('i_set')) {
            document.getElementById('i_set').value = `${state.COIL.I}`;
          }
          if (document.getElementById('i_qset')) {
            document.getElementById('i_qset').value = `${state.COIL.I}`;
          }
          if (document.getElementById('i_qset2')) {
            document.getElementById('i_qset2').value = `${state.COIL.I}`;
          }
          state.COIL.D = D;
          if (document.getElementById('d_set')) {
            document.getElementById('d_set').value = `${state.COIL.D}`;
          }
          if (document.getElementById('d_qset')) {
            document.getElementById('d_qset').value = `${state.COIL.D}`;
          }
          if (document.getElementById('d_qset2')) {
            document.getElementById('d_qset2').value = `${state.COIL.D}`;
          }
          break;
        default:
          break;
      }
      break;
    case SerialCommand.PID2:
      param = dat[1];
      switch (param) {
        case ParamPid.PARAM_PID:
          const P = bytesToDouble(Uint8Array.from(dat.slice(2, 10)));
          const I = bytesToDouble(Uint8Array.from(dat.slice(10, 18)));
          const D = bytesToDouble(Uint8Array.from(dat.slice(18, 26)));
          //console.log(P, I, D);
          state.COIL2.P = P;
          if (document.getElementById('p_qset2b')) {
            document.getElementById('p_qset2b').value = `${state.COIL2.P}`;
          }
          state.COIL2.I = I;
          if (document.getElementById('i_qset2b')) {
            document.getElementById('i_qset2b').value = `${state.COIL2.I}`;
          }
          state.COIL2.D = D;
          if (document.getElementById('d_qset2b')) {
            document.getElementById('d_qset2b').value = `${state.COIL2.D}`;
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
    case SerialCommand.MODE_DEBUG:
      {
        //const op = dat[1];
        const val = dat[2];
        state.DEV_MODE_DEBUG = val == 0 ? false : true;
        if (document.getElementById('esp_device_debug_checkbox')) {
          document.getElementById('esp_device_debug_checkbox').checked = state.DEV_MODE_DEBUG;
        }
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
  qdChanged = false,
  qp2Changed = false,
  qi2Changed = false,
  qd2Changed = false,
  qp2bChanged = false,
  qi2bChanged = false,
  qd2bChanged = false;
function pidButtonInit() {
  var pidQset2 = document.getElementById('pid_qset2');
  var pidQset2b = document.getElementById('pid_qset2b');
  var pidQset = document.getElementById('pid_qset');
  var pidSet = document.getElementById('pid_set');
  pidQset.addEventListener('click', sendPidSettings);
  pidQset2.addEventListener('click', sendPidSettings);
  pidQset2b.addEventListener('click', sendPid2Settings);
  pidSet.addEventListener('click', sendPidSettings);
  document.getElementById('p_set').addEventListener('change', function () {
    pChanged = true;
  });
  document.getElementById('p_qset').addEventListener('change', function () {
    qpChanged = true;
  });
  document.getElementById('p_qset2').addEventListener('change', function () {
    qp2Changed = true;
  });
  document.getElementById('i_set').addEventListener('change', function () {
    iChanged = true;
  });
  document.getElementById('i_qset').addEventListener('change', function () {
    qiChanged = true;
  });
  document.getElementById('i_qset2').addEventListener('change', function () {
    qi2Changed = true;
  });
  document.getElementById('d_set').addEventListener('change', function () {
    dChanged = true;
  });
  document.getElementById('d_qset').addEventListener('change', function () {
    qdChanged = true;
  });
  document.getElementById('d_qset2').addEventListener('change', function () {
    qd2Changed = true;
  });
  document.getElementById('p_qset2b').addEventListener('change', function () {
    qp2bChanged = true;
  });
  document.getElementById('i_qset2b').addEventListener('change', function () {
    qi2bChanged = true;
  });
  document.getElementById('d_qset2b').addEventListener('change', function () {
    qd2bChanged = true;
  });
}

function sendPidSettings() {
  console.log('PID changes submitted via quick set.');
  if (pChanged) {
    let val = document.getElementById('p_set').value;
    document.getElementById('p_qset').value = val;
    document.getElementById('p_qset2').value = val;
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
    document.getElementById('i_qset2').value = val;
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
    document.getElementById('d_qset2').value = val;
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
    document.getElementById('p_qset2').value = val;
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
    document.getElementById('i_qset2').value = val;
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
    document.getElementById('s_qset2').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_D,
      ...doubleToBytes(Number(val))
    ]);
    qdChanged = false;
  }
  if (qp2Changed) {
    let val = document.getElementById('p_qset2').value;
    document.getElementById('p_set').value = val;
    document.getElementById('d_qset').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_P,
      ...doubleToBytes(Number(val))
    ]);
    qp2Changed = false;
  }
  if (qi2Changed) {
    let val = document.getElementById('i_qset2').value;
    document.getElementById('i_set').value = val;
    document.getElementById('i_qset').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_I,
      ...doubleToBytes(Number(val))
    ]);
    qi2Changed = false;
  }
  if (qd2Changed) {
    let val = document.getElementById('d_qset2').value;
    document.getElementById('d_set').value = val;
    document.getElementById('d_qset').value = val;
    emit_websocket([
      SerialCommand.PID,
      Operation.SET,
      ParamPid.PID_D,
      ...doubleToBytes(Number(val))
    ]);
    qd2Changed = false;
  }
}

function sendPid2Settings() {
  console.log('PID2 changes submitted via quick set.');
  if (qp2bChanged) {
    let val = document.getElementById('p_qset2b').value;
    emit_websocket([
      SerialCommand.PID2,
      Operation.SET,
      ParamPid.PID_P,
      ...doubleToBytes(Number(val))
    ]);
    qp2bChanged = false;
  }
  if (qi2bChanged) {
    let val = document.getElementById('i_qset2b').value;
    emit_websocket([
      SerialCommand.PID2,
      Operation.SET,
      ParamPid.PID_I,
      ...doubleToBytes(Number(val))
    ]);
    qi2bChanged = false;
  }
  if (qd2bChanged) {
    let val = document.getElementById('d_qset2b').value;
    emit_websocket([
      SerialCommand.PID2,
      Operation.SET,
      ParamPid.PID_D,
      ...doubleToBytes(Number(val))
    ]);
    qd2bChanged = false;
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

let timer_temp_setting = undefined;
let timer_temp_slider = undefined;

// Temp Funcs/Vars
document.getElementById('temp_setting').addEventListener('change', function () {
  if (timer_temp_setting) {
    clearTimeout(timer_temp_setting);
  }
  timer_temp_setting = setTimeout(() => {
    updateTemp(0);
    timer_temp_setting = undefined;
  }, 700);
});
document.getElementById('temp_slider').addEventListener('change', function () {
  if (timer_temp_slider) {
    clearTimeout(timer_temp_slider);
  }
  timer_temp_slider = setTimeout(() => {
    updateTemp(1);
    timer_temp_slider = undefined;
  }, 300);
});
const updateTemp = (target) => {
  let selement;
  if (target == 1) {
    selement = document.getElementById('temp_slider');
  } else if (target == 0) {
    selement = document.getElementById('temp_setting');
  } else {
    return;
  }
  emit_websocket([SerialCommand.TEMP, Operation.SET, ...doubleToBytes(Number(selement.value))]);
};

let timer_temp_setting_1 = undefined;
let timer_temp_slider_1 = undefined;
let timer_temp_setting_2 = undefined;
let timer_temp_slider_2 = undefined;

document.getElementById('temp_setting_1').addEventListener('change', function () {
  if (timer_temp_setting_1) {
    clearTimeout(timer_temp_setting_1);
  }
  timer_temp_setting_1 = setTimeout(() => {
    updateTemp_1(0);
    timer_temp_setting_1 = undefined;
  }, 700);
});
document.getElementById('temp_slider_1').addEventListener('change', function () {
  if (timer_temp_slider_1) {
    clearTimeout(timer_temp_slider_1);
  }
  timer_temp_slider_1 = setTimeout(() => {
    updateTemp_1(1);
    timer_temp_slider_1 = undefined;
  }, 300);
});
const updateTemp_1 = (target) => {
  let selement;
  if (target == 1) {
    selement = document.getElementById('temp_slider_1');
  } else if (target == 0) {
    selement = document.getElementById('temp_setting_1');
  } else {
    return;
  }
  emit_websocket([SerialCommand.TEMP, Operation.SET, ...doubleToBytes(Number(selement.value))]);
};

document.getElementById('temp_setting_2').addEventListener('change', function () {
  if (timer_temp_setting_2) {
    clearTimeout(timer_temp_setting_2);
  }
  timer_temp_setting_2 = setTimeout(() => {
    updateTemp_2(0);
    timer_temp_setting_2 = undefined;
  }, 700);
});
document.getElementById('temp_slider_2').addEventListener('change', function () {
  if (timer_temp_slider_2) {
    clearTimeout(timer_temp_slider_2);
  }
  timer_temp_slider_2 = setTimeout(() => {
    updateTemp_2(1);
    timer_temp_slider_2 = undefined;
  }, 300);
});
const updateTemp_2 = (target) => {
  let selement;
  if (target == 1) {
    selement = document.getElementById('temp_slider_2');
  } else if (target == 0) {
    selement = document.getElementById('temp_setting_2');
  } else {
    return;
  }
  emit_websocket([SerialCommand.TEMP2, Operation.SET, ...doubleToBytes(Number(selement.value))]);
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
document.getElementById('toggle_heat_1').addEventListener('click', function () {
  toggleHeat();
});
const toggleHeat = () => {
  emit_websocket([SerialCommand.COIL_TOGGLE, Operation.SET, state.COIL.enabled ? 0 : 1]);
};
document.getElementById('toggle_heat_2').addEventListener('click', function () {
  toggleHeat2();
});
const toggleHeat2 = () => {
  emit_websocket([SerialCommand.COIL_TOGGLE2, Operation.SET, state.COIL2.enabled ? 0 : 1]);
};

// Zoom change
const zoomChange = (obj) => {
  const value = obj.value;
  updateZoom(value);
  state.ZOOM_LEVEL = value;
  chart.update();
};

// Zoom change 2
const zoomChange2 = (obj) => {
  const value = obj.value;
  updateZoom2(value);
  state.ZOOM_LEVEL2 = value;
  chart2.update();
};

// Time Span change
const timeChange_1 = (obj) => {
  const value = obj.value;
  state.TIME_SPAN1 = value;
  chart.update();
};

const timeChange_2 = (obj) => {
  const value = obj.value;
  state.TIME_SPAN2 = value;
  chart2.update();
};

function openChartTab(evt, tabName) {
  // Declare all variables
  var i, tabcontent, tablinks;

  // Get all elements with class="tabcontent" and hide them
  tabcontent = document.getElementsByClassName('tabcontent');
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = 'none';
  }

  // Get all elements with class="tablinks" and remove the class "active"
  tablinks = document.getElementsByClassName('tablinks');
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(' active', '');
  }

  // Show the current tab, and add an "active" class to the button that opened the tab
  document.getElementById(tabName).style.display = 'block';
  evt.currentTarget.className += ' active';
}

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
  document.getElementById('element1').click();
};

init();
