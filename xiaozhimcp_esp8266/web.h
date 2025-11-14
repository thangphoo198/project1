#include <pgmspace.h>

const char top_head[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no"/>
  <meta charset="utf-8">
  <title>Cài đặt thiết bị</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; min-height: 100vh; background: #f3f4f6; }
    .container { max-width: 480px; width: 100%; padding: 15px; background: white; border-radius: 8px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); text-align: center; }
    h2 { color: #333; font-size: 1.4em; margin-bottom: 15px; }
    input, button { width: 100%; padding: 12px; border: 1px solid #ccc; border-radius: 4px; font-size: 1em; margin: 8px 0; }
    button { background: #1fa3ec; color: white; border: none; cursor: pointer; transition: 0.3s; }
    button:hover { background: #188ec1; }
    .wifi-list { list-style: none; padding: 0; }
    .wifi-list li { padding: 10px; background: #f9f9f9; margin: 5px 0; border-radius: 4px; border: 1px solid #ddd; font-size: 0.95em; }
    .wifi-list a { color: #333; text-decoration: none; }
    .wifi-list a:hover { color: #007bff; }
  </style>
</head>
<body>
  <div class="container">
)rawliteral";

const char bot_head[] PROGMEM = R"rawliteral(
    <div style="text-align:right;font-size:14px;">
      <hr><a href="Wifi setup" target="_blank" style="color:#66B032;">Công Ty TNHH IoT Solutions Việt Nam</a>
    </div>
  </div>
</body>
</html>
)rawliteral";
