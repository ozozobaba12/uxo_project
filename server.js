const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');

const app = express();
app.use(express.static(path.join(__dirname, './')));
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

const BombScannerModule = require('./bombscanner.js');

BombScannerModule().then(Module => {
  wss.on('connection', ws => {
    ws.on('message', message => {
      const msg = JSON.parse(message);
      let response;
      if (msg.type === 'init') {
        Module.initGrid(msg.n, msg.m, msg.B);
        const state = Module.getGridState();
        response = { type: 'state', state };
      } else if (msg.type === 'reveal') {
        Module.revealCell(msg.r, msg.c);
        const state = Module.getGridState();
        response = { type: 'state', state };
      }
      ws.send(JSON.stringify(response));
    });
  });

  server.listen(8080, () => console.log('Server listening on http://localhost:8080'));
});
