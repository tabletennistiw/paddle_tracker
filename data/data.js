// data.js — fetch loop; bridges ESP32 JSON → viz.js + ui.js
// Toggle DEMO_MODE to false when running on real hardware

const DEMO_MODE = false;   // ← set false on ESP32
const POLL_MS   = 80;

// ── Live fetch ────────────────────────────────────────────────
async function fetchData() {
  try {
    const res = await fetch('/data');
    const d   = await res.json();
    window.updatePaddleOrientation(d.pitch, d.roll, d.yaw);
    window.updatePaddleAccel(d.ax, d.ay, d.az);
    window.updateUI(d);
    document.getElementById('status-txt').textContent = 'LIVE — ' + location.hostname;
  } catch (e) {
    document.getElementById('status-txt').textContent = 'RECONNECTING...';
  }
}

// ── Demo simulation ───────────────────────────────────────────
const demo = { p: 0, r: 0, y: 0, dp: 0.45, dr: 0.65, dy: 0.18 };
const DEG  = Math.PI / 180;

function demoTick() {
  demo.p += demo.dp; if (Math.abs(demo.p) > 40) demo.dp *= -1;
  demo.r += demo.dr; if (Math.abs(demo.r) > 55) demo.dr *= -1;
  demo.y += demo.dy; if (demo.y >  180) demo.y = -180;

  const ax = Math.sin(demo.p * DEG) * -1;
  const ay = Math.sin(demo.r * DEG);
  const az = Math.sqrt(Math.max(0, 1 - ax * ax - ay * ay));

  const d = { pitch: demo.p, roll: demo.r, yaw: demo.y, ax, ay, az };
  window.updatePaddleOrientation(d.pitch, d.roll, d.yaw);
  window.updatePaddleAccel(d.ax, d.ay, d.az);
  window.updateUI(d);
  document.getElementById('status-txt').textContent = 'DEMO MODE';
}

// ── Start ─────────────────────────────────────────────────────
setInterval(DEMO_MODE ? demoTick : fetchData, POLL_MS);