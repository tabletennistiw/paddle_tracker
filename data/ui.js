// ui.js — DOM updates for angle cards, accel bars, and log

function setBar(barEl, valEl, value) {
    const pct = Math.max(0, Math.min(1, (value + 2) / 4));
    if (value >= 0) {
      barEl.style.left  = '50%';
      barEl.style.width = (pct - 0.5) * 100 + '%';
    } else {
      barEl.style.left  = pct * 100 + '%';
      barEl.style.width = (0.5 - pct) * 100 + '%';
    }
    valEl.textContent = (value >= 0 ? '+' : '') + value.toFixed(3);
  }
  
  function addLog(d) {
    const log = document.getElementById('log');
    const t   = new Date().toLocaleTimeString('en-US', { hour12: false });
    const el  = document.createElement('div');
    el.className   = 'log-entry';
    el.textContent = `[${t}] P=${d.pitch.toFixed(1)}° R=${d.roll.toFixed(1)}° Y=${d.yaw.toFixed(1)}°  |  ax=${d.ax.toFixed(2)} ay=${d.ay.toFixed(2)} az=${d.az.toFixed(2)}`;
    log.appendChild(el);
    if (log.children.length > 30) log.removeChild(log.firstChild);
    log.scrollTop = log.scrollHeight;
  }
  
  window.updateUI = function(d) {
    document.getElementById('pitch-val').innerHTML = d.pitch.toFixed(1) + '<span class="unit">°</span>';
    document.getElementById('roll-val').innerHTML  = d.roll.toFixed(1)  + '<span class="unit">°</span>';
    document.getElementById('yaw-val').innerHTML   = d.yaw.toFixed(1)   + '<span class="unit">°</span>';
  
    setBar(document.getElementById('ax-bar'), document.getElementById('ax-val'), d.ax);
    setBar(document.getElementById('ay-bar'), document.getElementById('ay-val'), d.ay);
    setBar(document.getElementById('az-bar'), document.getElementById('az-val'), d.az);
  
    addLog(d);
  };