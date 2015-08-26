
// Load modules and use them
define(['identity.js'], function(Indentity) {

    var PROTOCOL_VERSION    = 2;
    var LOBBY_UID           = 1;

/**
Format of the packet header:

(std::uint16_t)(0U)  // actually, the size of the packet, including this field
        << (std::uint8_t)Protocol::VERSION  // Version of the protocol
        << src_uuid     // Packet source uuid
        << dest_uuid    // packet destination uuid
        << (std::uint8_t)cmd; // command
*/
    var COMMAND_OFFSET   = 11;
    var HEADER_LENGTH    = 12;

    /** Constructor */
    var Protocol = function (data) {
        if (data === undefined) {
            this.packet = new ArrayBuffer(1000); // FIXME find the biggest packet!
            this.offset = 0;
        } else {
            // assume it is a packet reader
            this.packet = data;
            this.offset = HEADER_LENGTH; // skip protocol header    
        }
    };

    Protocol.prototype.GetCommand = function()
    {
        return this.GetUint8(COMMAND_OFFSET);
    };


    Protocol.prototype.GetUint8 = function (index)
    {
        return new Uint8Array(this.packet, index, 1)[0];
    };

    // Get a 32 bits value from the packet
    Protocol.prototype.GetUint32 = function (index)
    {
        return new Uint32Array(this.packet, index, 1)[0];
    };

    // Stream read of a 32 bits value
    Protocol.prototype.ReadUint32 = function()
    {
        var value = this.GetUint32(this.offset);
        this.offset += 4;
        return value;
    };

    Protocol.prototype.AppendUint8 = function(value)
    {
        var view = new DataView(this.packet);
        view.setUint8(this.offset, value, true);
        this.offset += 1;
    };

    Protocol.prototype.AppendUint16 = function(value)
    {
        var view = new DataView(this.packet);
        view.setUint16(this.offset, value, true);
        this.offset += 2;
    };

    Protocol.prototype.AppendUint32 = function(value)
    {
        var view = new DataView(this.packet);
        view.setUint32(this.offset, value, true);
        this.offset += 4;
    };

    Protocol.prototype.AppendString = function(value)
    {
        var strArray = new TextEncoder().encode(value);
        this.AppendUint32(strArray.byteLength);
        this.AppendBuffer(strArray);
    };

    // argument is an array buffer
    Protocol.prototype.AppendBuffer = function(buffer)
    {
        var view = new Uint8Array(this.packet, this.offset);
        view.set(new Uint8Array(buffer));
        this.offset += buffer.byteLength;
    };

    // Argument 'protocol' is an Protocol class: it contains the used size
    Protocol.prototype.BuildPacket = function(src, dest, cmd, protocol)
    {
        var newSize = HEADER_LENGTH + protocol.offset;
        this.packet = new ArrayBuffer(newSize);
        this.offset = 0;
        
        this.AppendUint16(newSize);
        this.AppendUint8(PROTOCOL_VERSION);
        this.AppendUint32(src);
        this.AppendUint32(dest);
        this.AppendUint8(cmd);
        this.AppendBuffer(protocol.packet.slice(0, protocol.offset));
        return this.packet;
    };

    Protocol.prototype.ReplyLogin = function(identity)
    {
        this.AppendString(identity.nickname);
        this.AppendString(identity.avatar);
        this.AppendUint8(identity.gender);
        this.AppendString(identity.username);
    };




/*
    Protocol.prototype.Clear = function()
    {
        this.packet.length = 0;   
    };

    Protocol.prototype.Size = function()
    {
        return this.packet.length;   
    };

    Protocol.prototype.Unpack = function(str)
    {
        for(var i = 0, n = str.length; i < n; i++) {
            var char = str.charCodeAt(i);
            this.packet.push(char & 0xFF);
        }
    };

    Protocol.prototype.GetData = function()
    {
        return this.packet;
    };    


    Protocol.prototype.GetValue8 = function()
    {
        var value = this.packet.GetUint8(this.offset);
        this.offset += 1;
        return value;
    };

    Protocol.prototype.GetValue16 = function()
    {
        var value = this.packet.GetUint16(this.offset);
        this.offset += 2;
        return value;
    };

    ByteBuffer.prototype.ToHex = function ()
    {
        var str = "";
        for(var i = 0; i < this.chars.length; i++) {
            str += this.chars[i].toString(16) + ", ";
        }
        return str;
    };

    // value is a ByteBuffer
    ByteBuffer.prototype.AppendBuffer = function(value)
    {
        for (var i = 0; i < value.Size(); i++) {
            this.chars.push(value.chars[i]);    
        }
    };    

    

    ByteBuffer.prototype.WriteUint32 = function(index, value)
    {
        this.chars[index++] = (value & 0xFF);
        this.chars[index++] = ((value>>8) & 0xFF);
        this.chars[index++] = ((value>>16) & 0xFF);
        this.chars[index] = ((value>>24) & 0xFF);
    };

    ByteBuffer.prototype.AppendString = function(value)
    {
        AppendUint32(value.length);
        Unpack(value);
    };

    

    ByteBuffer.prototype.GetUint16 = function (index)
    {
        var value = 0;
        if (index === undefined) {
            index = 0;
        }
        if (index < (this.chars.length - 1) ) {
            value = this.chars[index] + ((this.chars[index+1]<<8) &0xFF);
        }
        return value;
    };

    */

    return Protocol;
});
