// Load modules and use them
define(['protocol.js', 'identity.js'], function(Protocol, Identity) {

    var Client = function () {
        this.socket = null;
        this.On = new Array();
        this.uuid = 0;
        this.identity = new Identity();
        this.identity.nickname = 'Belegar';
    };

    var LOBBY_UID   =   1;

    var LOBBY_REQUEST_LOGIN = 0x41;
    var CLIENT_REPLY_LOGIN = 0x33;

    Client.prototype.Initialize = function ()
    {
        this.socket = new WebSocket('ws://localhost:4270', 'tarotclub');
        this.socket.binaryType = 'arraybuffer';

        this.Connect(this, function(ref, data) {
            var packet = new Protocol(data);
            var reply = new Protocol();
            var cmd = packet.GetCommand();
            
            if (cmd === LOBBY_REQUEST_LOGIN) {
                console.log("[-->] Lobby login request");
                ref.uuid = packet.ReadUint32();
                console.log("Assigned UUID: " + ref.uuid);

                // Reply with out identity
                reply.ReplyLogin(ref.identity);
                // src, dst, commad, packet
                ref.socket.send(packet.BuildPacket(ref.uuid, LOBBY_UID, CLIENT_REPLY_LOGIN, reply));

            } else {
                console.log("Command not supported: " + cmd);
            }
            
/**
            if (ref.On['chatMessage'] !== undefined)
            {
                ref.On['chatMessage']("coucou!");
            }
*/
         //   console.log("Size of buffer: " + buff.limit);
         //   console.log("Command: " + buff.readByte(11));
          //  console.log("Packet value: " + buff.toHex());
        });
    };

    Client.prototype.Connect = function(ref, callback)
    {
        console.log('WebSocket - status ' + this.socket.readyState);
        this.socket.onopen = function (msg)
        { 
            console.log("Connected - status " + this.readyState);
        };

        this.socket.onmessage = function (msg)
        {
            // Only accept binary protocol
            if(msg.data instanceof ArrayBuffer) {
                callback(ref, msg.data);
            }
        };

        this.socket.onclose = function (msg)
        {
            console.log("Disconnected - status " + this.readyState + ", code=" + msg.code 
                    + ", wasClean=" + msg.wasClean + ", Reason=" + msg.reason,  "Closed");
        };
    };

    return Client;
});

