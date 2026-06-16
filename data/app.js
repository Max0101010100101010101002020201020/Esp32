const $ = s => document.querySelector(s);
const token = localStorage.getItem('token') || '';

function login() {
  const u = $('#user').value, p = $('#pass').value;
  fetch('/api/login', {method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:`username=${encodeURIComponent(u)}&password=${encodeURIComponent(p)}`})
  .then(r=>r.json()).then(d=>{
    if(d.token) { localStorage.setItem('token',d.token); location.reload(); }
    else alert('Invalid');
  });
}
function logout() { localStorage.removeItem('token'); location.reload(); }
function api(url, opts={}) {
  opts.headers = opts.headers || {};
  if(token) opts.headers['Authorization'] = `Bearer ${token}`;
  return fetch(url,opts).then(r=>r.json());
}
function render() {
  if(!token) {
    $('#app').innerHTML = `<div class="card"><input id="user" placeholder="User"><input id="pass" type="password" placeholder="Password"><button onclick="login()">Login</button></div>`;
    return;
  }
  $('#app').innerHTML = `<div class="card"><h1>ESP32 Dashboard</h1><button onclick="logout()">Logout</button><div id="status"></div></div>`;
  setInterval(()=>{
    api('/api/status').then(d=>{
      $('#status').innerHTML = `Heap: ${d.heap} | RSSI: ${d.wifi_rssi} | Uptime: ${d.uptime}s`;
    });
  },2000);
}
render();
