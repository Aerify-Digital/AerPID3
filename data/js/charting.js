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
          label: 'Sigma+',
          data: [],
          borderColor: 'blue',
          borderWidth: 0.4,
          fill: false,
          pointRadius: 0,
          hidden: true,
          yAxisID: 'y'
        },
        {
          label: 'Sigma-',
          data: [],
          borderColor: 'blue',
          borderWidth: 0.4,
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
  const sigmas1 = [];
  const sigmas2 = [];
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
    sigmas1.push(Number(sTempsAvg[i]) + Number(sSigma[i]));
    sigmas2.push(sTempsAvg[i] - sSigma[i]);
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
  chart.data.datasets[5].data = sigmas1;
  chart.data.datasets[6].data = sigmas2;
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

  if (state.OUTPUT2.length > maxChartTime) {
    state.OUTPUT2.shift();
  }
  state.OUTPUT2.push(o);

  if (state.SIGMA2.length > maxChartTime) {
    state.SIGMA2.shift();
  }
  state.SIGMA2.push(s);

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
  for (let i = 0; i < state.OUTPUT2.length; i++) {
    sOutput.push(state.OUTPUT2[i]);
  }
  sOutput = sOutput.reverse();
  let sSigma = [];
  for (let i = 0; i < state.SIGMA2.length; i++) {
    sSigma.push(state.SIGMA2[i]);
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
