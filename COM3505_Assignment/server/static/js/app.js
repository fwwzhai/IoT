const POLL_INTERVAL_MS = 2000;
const HEALTH_LABELS = {
  live: "Live",
  stale: "Stale",
  offline: "Offline",
};
const CHART_COLOURS = {
  grid: "rgba(21, 35, 33, 0.08)",
  axis: "rgba(21, 35, 33, 0.18)",
  label: "rgba(21, 35, 33, 0.62)",
  temperature: "#df5d2e",
  light: "#0e8b7f",
  hover: "rgba(21, 35, 33, 0.2)",
};

const stateElements = {
  temperature: document.getElementById("temperature"),
  temperatureNote: document.getElementById("temperature-note"),
  light: document.getElementById("light"),
  motion: document.getElementById("motion"),
  mode: document.getElementById("mode"),
  pattern: document.getElementById("pattern-name"),
  healthPill: document.getElementById("health-pill"),
  lastSeen: document.getElementById("last-seen"),
  lastSeenDetail: document.getElementById("last-seen-detail"),
  wifiState: document.getElementById("wifi-state"),
  backendState: document.getElementById("backend-state"),
};

const chartCanvas = document.getElementById("history-chart");
const chartContext = chartCanvas.getContext("2d");
const chartFrame = document.getElementById("chart-frame");
const chartTooltip = document.getElementById("chart-tooltip");
const modeButtons = Array.from(document.querySelectorAll("[data-mode]"));
const patternButtons = Array.from(document.querySelectorAll("[data-pattern]"));

let latestState = null;
let latestHistory = [];
let latestChartLayout = null;
let hoveredChartIndex = null;

function safeMetric(value, suffix = "") {
  if (value === null || value === undefined || value === "") {
    return "--";
  }

  return `${value}${suffix}`;
}

function formatMotion(value) {
  if (value === true) {
    return "Detected";
  }
  if (value === false) {
    return "Clear";
  }
  return "--";
}

function formatRelativeTime(isoString) {
  if (!isoString) {
    return "Waiting for first packet";
  }

  const timestamp = new Date(isoString);
  if (Number.isNaN(timestamp.getTime())) {
    return "Unknown";
  }

  const diffSeconds = Math.max(0, Math.round((Date.now() - timestamp.getTime()) / 1000));

  if (diffSeconds < 5) {
    return "Just now";
  }
  if (diffSeconds < 60) {
    return `${diffSeconds}s ago`;
  }

  const diffMinutes = Math.round(diffSeconds / 60);
  if (diffMinutes < 60) {
    return `${diffMinutes}m ago`;
  }

  const diffHours = Math.round(diffMinutes / 60);
  return `${diffHours}h ago`;
}

function formatTimestampLabel(isoString) {
  if (!isoString) {
    return "--";
  }

  const timestamp = new Date(isoString);
  if (Number.isNaN(timestamp.getTime())) {
    return "--";
  }

  return timestamp.toLocaleTimeString([], {
    hour: "2-digit",
    minute: "2-digit",
  });
}

function formatTooltipTime(isoString) {
  if (!isoString) {
    return "No timestamp";
  }

  const timestamp = new Date(isoString);
  if (Number.isNaN(timestamp.getTime())) {
    return "Unknown time";
  }

  return timestamp.toLocaleString([], {
    month: "short",
    day: "numeric",
    hour: "2-digit",
    minute: "2-digit",
    second: "2-digit",
  });
}

function formatSeriesValue(value, key) {
  if (value === null || value === undefined) {
    return "--";
  }

  if (key === "temperature_c") {
    return `${Number(value).toFixed(1)} C`;
  }

  return `${Math.round(Number(value))}`;
}

function updateButtonStates() {
  const mode = latestState?.device?.mode ?? "manual";
  const pattern = latestState?.device?.pattern ?? "blink";

  for (const button of modeButtons) {
    button.classList.toggle("is-active", button.dataset.mode === mode);
  }

  for (const button of patternButtons) {
    button.classList.toggle("is-active", button.dataset.pattern === pattern);
    button.disabled = mode === "auto";
  }
}

function updateStateCards(state) {
  latestState = state;

  stateElements.temperature.textContent = safeMetric(state.sensors.temperature_c, " C");
  stateElements.temperatureNote.textContent =
    state.sensors.temperature_c === null
      ? "Awaiting sensor data"
      : "Sampled by ESP32 and relayed through Flask";

  stateElements.light.textContent = safeMetric(state.sensors.light_level);
  stateElements.motion.textContent = formatMotion(state.sensors.motion_detected);
  stateElements.mode.textContent = state.device.mode ?? "manual";
  stateElements.pattern.textContent = `Pattern: ${state.device.pattern ?? "blink"}`;

  const health = state.device.health ?? "offline";
  stateElements.healthPill.dataset.health = health;
  stateElements.healthPill.textContent = HEALTH_LABELS[health] ?? "Offline";

  const relativeTime = formatRelativeTime(state.device.last_seen);
  stateElements.lastSeen.textContent = relativeTime;
  stateElements.lastSeenDetail.textContent = relativeTime;
  stateElements.wifiState.textContent = state.device.wifi_connected ? "connected" : "offline";
  stateElements.backendState.textContent =
    health === "offline" ? "awaiting device" : "receiving data";

  updateButtonStates();
}

function buildSeriesMeta(history, key) {
  const validValues = history
    .map((point) => point[key])
    .filter((value) => value !== null && value !== undefined);

  if (validValues.length === 0) {
    return {
      min: 0,
      max: 1,
      spread: 1,
    };
  }

  let min = Math.min(...validValues);
  let max = Math.max(...validValues);

  if (min === max) {
    const baseValue = validValues[0];
    const padding = key === "temperature_c"
      ? 2
      : Math.max(20, Math.round(Math.abs(baseValue) * 0.08));

    min -= padding;
    max += padding;
  }

  return {
    min,
    max,
    spread: Math.max(1, max - min),
  };
}

function makePlot(width, height) {
  return {
    left: 62,
    right: width - 58,
    top: 24,
    bottom: height - 34,
    width: width - 120,
    height: height - 58,
  };
}

function valueToY(value, meta, plot) {
  if (value === null || value === undefined) {
    return null;
  }

  const ratio = (value - meta.min) / meta.spread;
  return plot.bottom - ratio * plot.height;
}

function drawGrid(plot) {
  chartContext.strokeStyle = CHART_COLOURS.grid;
  chartContext.lineWidth = 1;

  for (let i = 0; i <= 4; i += 1) {
    const y = plot.top + (plot.height / 4) * i;
    chartContext.beginPath();
    chartContext.moveTo(plot.left, y);
    chartContext.lineTo(plot.right, y);
    chartContext.stroke();
  }

  for (let i = 0; i <= 4; i += 1) {
    const x = plot.left + (plot.width / 4) * i;
    chartContext.beginPath();
    chartContext.moveTo(x, plot.top);
    chartContext.lineTo(x, plot.bottom);
    chartContext.stroke();
  }
}

function drawAxes(plot) {
  chartContext.strokeStyle = CHART_COLOURS.axis;
  chartContext.lineWidth = 1.2;

  chartContext.beginPath();
  chartContext.moveTo(plot.left, plot.top);
  chartContext.lineTo(plot.left, plot.bottom);
  chartContext.lineTo(plot.right, plot.bottom);
  chartContext.stroke();

  chartContext.beginPath();
  chartContext.moveTo(plot.right, plot.top);
  chartContext.lineTo(plot.right, plot.bottom);
  chartContext.stroke();
}

function drawAxisLabels(plot, history, temperatureMeta, lightMeta) {
  chartContext.fillStyle = CHART_COLOURS.label;
  chartContext.font = "12px Segoe UI";
  chartContext.textBaseline = "middle";

  for (let i = 0; i <= 4; i += 1) {
    const ratio = i / 4;
    const y = plot.top + plot.height * ratio;
    const temperatureValue = temperatureMeta.max - temperatureMeta.spread * ratio;
    const lightValue = lightMeta.max - lightMeta.spread * ratio;

    chartContext.textAlign = "left";
    chartContext.fillText(formatSeriesValue(temperatureValue, "temperature_c"), 8, y);

    chartContext.textAlign = "right";
    chartContext.fillText(formatSeriesValue(lightValue, "light_level"), chartCanvas.width - 8, y);
  }

  chartContext.textBaseline = "alphabetic";
  chartContext.textAlign = "left";
  chartContext.fillText("Temp", plot.left, 14);

  chartContext.textAlign = "right";
  chartContext.fillText("Light", plot.right, 14);

  if (history.length === 0) {
    return;
  }

  const firstIndex = 0;
  const middleIndex = Math.floor((history.length - 1) / 2);
  const lastIndex = history.length - 1;
  const xLabels = [
    { x: plot.left, text: formatTimestampLabel(history[firstIndex]?.timestamp), align: "left" },
    { x: plot.left + plot.width / 2, text: formatTimestampLabel(history[middleIndex]?.timestamp), align: "center" },
    { x: plot.right, text: formatTimestampLabel(history[lastIndex]?.timestamp), align: "right" },
  ];

  chartContext.textBaseline = "top";
  for (const label of xLabels) {
    chartContext.textAlign = label.align;
    chartContext.fillText(label.text, label.x, plot.bottom + 10);
  }
}

function buildChartPoints(history, plot, temperatureMeta, lightMeta) {
  const stepX = history.length > 1 ? plot.width / (history.length - 1) : 0;

  return history.map((point, index) => ({
    x: plot.left + index * stepX,
    timestamp: point.timestamp,
    temperature_c: point.temperature_c,
    light_level: point.light_level,
    temperatureY: valueToY(point.temperature_c, temperatureMeta, plot),
    lightY: valueToY(point.light_level, lightMeta, plot),
  }));
}

function drawSeries(points, yKey, colour) {
  if (points.length < 2) {
    return;
  }

  chartContext.strokeStyle = colour;
  chartContext.lineWidth = 3;
  chartContext.lineJoin = "round";
  chartContext.lineCap = "round";
  chartContext.beginPath();

  let started = false;
  for (const point of points) {
    const y = point[yKey];
    if (y === null) {
      continue;
    }

    if (!started) {
      chartContext.moveTo(point.x, y);
      started = true;
    } else {
      chartContext.lineTo(point.x, y);
    }
  }

  if (started) {
    chartContext.stroke();
  }
}

function drawHoverState(layout, hoverIndex) {
  if (!layout || hoverIndex === null) {
    return;
  }

  const point = layout.points[hoverIndex];
  if (!point) {
    return;
  }

  chartContext.strokeStyle = CHART_COLOURS.hover;
  chartContext.lineWidth = 1.4;
  chartContext.beginPath();
  chartContext.moveTo(point.x, layout.plot.top);
  chartContext.lineTo(point.x, layout.plot.bottom);
  chartContext.stroke();

  const markers = [
    { y: point.temperatureY, colour: CHART_COLOURS.temperature },
    { y: point.lightY, colour: CHART_COLOURS.light },
  ];

  for (const marker of markers) {
    if (marker.y === null) {
      continue;
    }

    chartContext.fillStyle = "#fff";
    chartContext.strokeStyle = marker.colour;
    chartContext.lineWidth = 2.5;
    chartContext.beginPath();
    chartContext.arc(point.x, marker.y, 5, 0, Math.PI * 2);
    chartContext.fill();
    chartContext.stroke();
  }
}

function hideTooltip() {
  if (chartTooltip) {
    chartTooltip.hidden = true;
  }
}

function showTooltip(layout, hoverIndex) {
  if (!chartTooltip || !layout || hoverIndex === null) {
    hideTooltip();
    return;
  }

  const point = layout.points[hoverIndex];
  if (!point) {
    hideTooltip();
    return;
  }

  chartTooltip.innerHTML = `
    <p class="chart-tooltip-title">${formatTooltipTime(point.timestamp)}</p>
    <div class="chart-tooltip-row">
      <span class="chart-tooltip-key">
        <i class="chart-tooltip-dot temperature"></i>
        Temperature
      </span>
      <strong>${formatSeriesValue(point.temperature_c, "temperature_c")}</strong>
    </div>
    <div class="chart-tooltip-row">
      <span class="chart-tooltip-key">
        <i class="chart-tooltip-dot light"></i>
        Light
      </span>
      <strong>${formatSeriesValue(point.light_level, "light_level")}</strong>
    </div>
  `;

  chartTooltip.hidden = false;

  const pointY = point.temperatureY ?? point.lightY ?? layout.plot.top;
  const scaleX = chartFrame.clientWidth / chartCanvas.width;
  const scaleY = chartFrame.clientHeight / chartCanvas.height;
  const anchorX = point.x * scaleX;
  const anchorY = pointY * scaleY;

  let left = anchorX + 16;
  let top = anchorY - chartTooltip.offsetHeight - 16;

  if (left + chartTooltip.offsetWidth > chartFrame.clientWidth - 12) {
    left = anchorX - chartTooltip.offsetWidth - 16;
  }

  if (left < 12) {
    left = 12;
  }

  if (top < 12) {
    top = anchorY + 16;
  }

  if (top + chartTooltip.offsetHeight > chartFrame.clientHeight - 12) {
    top = chartFrame.clientHeight - chartTooltip.offsetHeight - 12;
  }

  chartTooltip.style.left = `${left}px`;
  chartTooltip.style.top = `${top}px`;
}

function findHoverIndex(layout, pointerX) {
  if (!layout || layout.points.length === 0) {
    return null;
  }

  if (pointerX < layout.plot.left - 10 || pointerX > layout.plot.right + 10) {
    return null;
  }

  const stepX = layout.points.length > 1
    ? layout.plot.width / (layout.points.length - 1)
    : layout.plot.width;
  const rawIndex = Math.round((pointerX - layout.plot.left) / stepX);
  return Math.max(0, Math.min(layout.points.length - 1, rawIndex));
}

function getPointerPosition(event) {
  const rect = chartCanvas.getBoundingClientRect();
  const scaleX = chartCanvas.width / rect.width;
  const scaleY = chartCanvas.height / rect.height;

  return {
    x: (event.clientX - rect.left) * scaleX,
    y: (event.clientY - rect.top) * scaleY,
  };
}

function drawChart(history) {
  const width = chartCanvas.width;
  const height = chartCanvas.height;
  const plot = makePlot(width, height);

  chartContext.clearRect(0, 0, width, height);
  drawGrid(plot);
  drawAxes(plot);

  if (history.length === 0) {
    latestChartLayout = null;
    hideTooltip();
    chartContext.fillStyle = CHART_COLOURS.label;
    chartContext.font = "16px Segoe UI";
    chartContext.fillText("History will appear here when the ESP32 starts posting data.", 28, height / 2);
    return;
  }

  const temperatureMeta = buildSeriesMeta(history, "temperature_c");
  const lightMeta = buildSeriesMeta(history, "light_level");
  const points = buildChartPoints(history, plot, temperatureMeta, lightMeta);

  latestChartLayout = {
    plot,
    points,
  };

  drawAxisLabels(plot, history, temperatureMeta, lightMeta);
  drawSeries(points, "temperatureY", CHART_COLOURS.temperature);
  drawSeries(points, "lightY", CHART_COLOURS.light);
  drawHoverState(latestChartLayout, hoveredChartIndex);
}

async function setPattern(pattern) {
  await fetch("/api/pattern", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ pattern }),
  });

  await refreshDashboard();
}

async function setMode(mode) {
  await fetch("/api/mode", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ mode }),
  });

  await refreshDashboard();
}

async function refreshDashboard() {
  const [stateResponse, historyResponse] = await Promise.all([
    fetch("/api/state"),
    fetch("/api/history"),
  ]);

  const state = await stateResponse.json();
  const historyPayload = await historyResponse.json();

  latestHistory = historyPayload.history ?? [];
  updateStateCards(state);
  drawChart(latestHistory);
  showTooltip(latestChartLayout, hoveredChartIndex);
}

for (const button of patternButtons) {
  button.addEventListener("click", () => {
    setPattern(button.dataset.pattern);
  });
}

for (const button of modeButtons) {
  button.addEventListener("click", () => {
    setMode(button.dataset.mode);
  });
}

chartCanvas.addEventListener("pointermove", (event) => {
  if (!latestChartLayout) {
    return;
  }

  const pointer = getPointerPosition(event);
  hoveredChartIndex = findHoverIndex(latestChartLayout, pointer.x);
  drawChart(latestHistory);
  showTooltip(latestChartLayout, hoveredChartIndex);
});

chartCanvas.addEventListener("pointerleave", () => {
  hoveredChartIndex = null;
  hideTooltip();
  drawChart(latestHistory);
});

refreshDashboard();
setInterval(refreshDashboard, POLL_INTERVAL_MS);
