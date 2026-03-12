async function refreshState() {
  const response = await fetch("/api/state");
  const state = await response.json();

  document.getElementById("temperature").textContent =
    state.sensors.temperature_c ?? "--";
  document.getElementById("light").textContent =
    state.sensors.light_level ?? "--";
  document.getElementById("motion").textContent =
    state.sensors.motion_detected ?? "--";
  document.getElementById("mode").textContent =
    state.device.mode ?? "manual";
}

async function setPattern(pattern) {
  await fetch("/api/pattern", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ pattern }),
  });

  refreshState();
}

for (const button of document.querySelectorAll("[data-pattern]")) {
  button.addEventListener("click", () => {
    setPattern(button.dataset.pattern);
  });
}

refreshState();
setInterval(refreshState, 2000);

