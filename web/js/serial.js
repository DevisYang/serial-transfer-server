function CreateSerialHandler(url, message_func, alert_func) {
    if ("WebSocket" in window)
    {
        var SerialHandler = {};
        SerialHandler.message_func = message_func;
        SerialHandler._ws = new WebSocket(url);
        SerialHandler._ws.onopen = function() {
            message_func("websocket已连接");
        }

        SerialHandler._ws.onmessage = function(evt) {
            var msg = evt.data;
            var obj = JSON.parse(msg);
            if(SerialHandler._localCallback) {
                SerialHandler._localCallback(obj);
                SerialHandler._localCallback = null;
            } 
            if(obj && obj.type==="data" && SerialHandler.message_func) {
                SerialHandler.message_func(obj.data);
            }
        }

        SerialHandler._ws.onclose = function() {
            alert_func("websocket已断开")
        }

        SerialHandler.EnumSerial = function(callback) {
            SerialHandler._localCallback = callback;
            var obj = {};
            obj.type = "enum";
            SerialHandler._sendObject(obj);
        }

        SerialHandler.Open = function(name, baud, callback) {
            SerialHandler._localCallback = callback;
            var obj = {};
            obj.type = "open";
            obj.serial_name = name;
            obj.serial_baud = baud;
            SerialHandler._sendObject(obj);
        }

        SerialHandler.Close = function(callback) {
            SerialHandler._localCallback = callback;
            var obj = {};
            obj.type = "close";
            SerialHandler._sendObject(obj);
        }
        SerialHandler.Send = function(data, callback) {
            SerialHandler._localCallback = callback;
            var obj = {};
            obj.type="send";
            obj.data = data;
            SerialHandler._sendObject(obj);
        }

        SerialHandler._sendObject = function(obj) {
            var str = JSON.stringify(obj);
            SerialHandler._ws.send(str);
        }
        return SerialHandler;
    }
    return null;
}