let port;
let reader;
let isCurrentlySitting = false;
let sittingStartTime = null;

// Predefined values for the bar chart
const sittingTimes = [10, 15, 20]; // Predefined durations (minutes)
const sittingTimeLabels = ["Session 1", "Session 2", "Session 3"]; // Predefined labels
const sittingTimeData = [...sittingTimes]; // Copy predefined values into the dataset

let chart = null;

// Initialize the chart
function initializeChart() {
  const ctx = document.getElementById("sitting-time-chart").getContext("2d");
  chart = new Chart(ctx, {
    type: "bar",
    data: {
      labels: sittingTimeLabels,
      datasets: [
        {
          label: "Focus Time (minutes)",
          data: sittingTimeData,
          backgroundColor: "rgba(54, 162, 235, 0.6)",
          borderColor: "rgba(54, 162, 235, 1)",
          borderWidth: 1,
        },
      ],
    },
    options: {
      responsive: true,
      scales: {
        y: {
          beginAtZero: true,
          title: {
            display: true,
            text: "Minutes",
          },
        },
        x: {
          title: {
            display: true,
            text: "Session",
          },
        },
      },
    },
  });
}

// Update the chart data dynamically
function updateChart() {
  chart.update();
}

// Function to handle new focus sessions
function trackSitting(isSitting) {
  const now = new Date();
  if (isSitting) {
    if (!isCurrentlySitting) {
      isCurrentlySitting = true;
      sittingStartTime = now;
    }
  } else {
    if (isCurrentlySitting) {
      const sittingEndTime = now;
      const durationMinutes = Math.round(
        (sittingEndTime - sittingStartTime) / 60000
      );
      if (durationMinutes > 0) {
        sittingTimes.push(durationMinutes);
        sittingTimeLabels.push(`Session ${sittingTimes.length}`);
        sittingTimeData.push(durationMinutes);

        updateChart();
      }
      isCurrentlySitting = false;
    }
  }
}

// Connect to the serial port
async function connectSerial() {
  try {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 9600 });
    const decoder = new TextDecoderStream();
    const readableStreamClosed = port.readable.pipeTo(decoder.writable);
    reader = decoder.readable.getReader();

    readSerialData();
  } catch (error) {
    console.error("Error connecting to serial port:", error);
  }
}

// Read data from the serial port
async function readSerialData() {
  const standingStatusElem = document.getElementById("standing-status");
  const snackAmountElem = document.getElementById("snack-amount");

  while (true) {
    try {
      const { value, done } = await reader.read();
      if (done) {
        console.log("Stream closed");
        break;
      }
      if (value) {
        // Parse JSON data from the serial port
        try {
          const data = JSON.parse(value);
          standingStatusElem.textContent = data.isStanding
            ? "Standing"
            : "Focusing (Sitting)";
          snackAmountElem.textContent = data.snackAmount;

          // Update focus (sitting) time tracking
          trackSitting(!data.isStanding);
        } catch (e) {
          console.error("Invalid JSON data:", value);
        }
      }
    } catch (error) {
      console.error("Error reading from serial port:", error);
      break;
    }
  }
}

initializeChart();

// Initialize chart on button click
document.getElementById("connect-button").addEventListener("click", () => {
  connectSerial();
});