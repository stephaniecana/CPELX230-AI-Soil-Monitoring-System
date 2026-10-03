const form = document.getElementById("predictionForm");

function pct(value) {
  if (value === undefined || value === null) return "—";
  return `${(value * 100).toFixed(1)}%`;
}

function showPrediction(data) {
  const prediction = document.getElementById("prediction");
  prediction.textContent = data.prediction || "—";
  document.getElementById("confidence").textContent =
    `Confidence: ${pct(data.confidence)}`;

  const probs = data.probabilities || {};
  document.getElementById("pNoWater").textContent = pct(probs.NO_WATER);
  document.getElementById("pWaterSoon").textContent = pct(probs.WATER_SOON);
  document.getElementById("pWaterNow").textContent = pct(probs.WATER_NOW);
}

form.addEventListener("submit", async (event) => {
  event.preventDefault();

  const payload = {
    soil_moisture: Number(document.getElementById("soil_moisture").value),
    temperature: Number(document.getElementById("temperature").value),
    humidity: Number(document.getElementById("humidity").value),
  };

  const response = await fetch("/api/predict", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(payload),
  });

  const data = await response.json();

  if (!response.ok) {
    alert(data.error || "Prediction failed.");
    return;
  }

  showPrediction(data);
});

async function refreshLatest() {
  try {
    const response = await fetch("/api/latest");
    const data = await response.json();

    if (data.soil_moisture === null || data.soil_moisture === undefined) return;

    document.getElementById("latestMoisture").textContent =
      `${Number(data.soil_moisture).toFixed(1)}%`;

    document.getElementById("latestTemperature").textContent =
      `${Number(data.temperature).toFixed(1)} °C`;

    document.getElementById("latestHumidity").textContent =
      `${Number(data.humidity).toFixed(1)}%`;

    document.getElementById("latestPrediction").textContent =
      data.prediction || "—";

    document.getElementById("latestTimestamp").textContent =
      `Last received: ${data.timestamp || "—"}`;
  } catch (error) {
    // Keep the dashboard usable even if polling temporarily fails.
  }
}

refreshLatest();
setInterval(refreshLatest, 3000);
