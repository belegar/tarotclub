// Load modules and use them
require(['client.js'], function(Client) {
    
    console.log("TarotClub WebSocket client started.");
    var client = new Client();

    client.On['chatMessage'] = function(message) {
        console.log(message);
    };

    client.Initialize();
});
