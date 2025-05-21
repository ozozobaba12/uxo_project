const ws = new WebSocket(`ws://${location.host}`);
ws.onmessage = event => {
  const msg = JSON.parse(event.data);
  if (msg.type === 'state') {
    render(msg.state);
  }
};

function render(state) {
  const container = document.getElementById('grid');
  container.innerHTML = '';
  state.forEach((row, i) => {
    const rowEl = document.createElement('div');
    row.forEach((cell, j) => {
      const btn = document.createElement('button');
      btn.textContent = cell === -2 ? '·' : (cell === -1 ? '💣' : cell);
      btn.onclick = () => ws.send(JSON.stringify({ type: 'reveal', r: i, c: j }));
      rowEl.appendChild(btn);
    });
    container.appendChild(rowEl);
  });
}

document.getElementById('start').onclick = () => {
  const n = parseInt(document.getElementById('rows').value, 10);
  const m = parseInt(document.getElementById('cols').value, 10);
  const B = parseInt(document.getElementById('bombs').value, 10);
  ws.send(JSON.stringify({ type: 'init', n, m, B }));
};
