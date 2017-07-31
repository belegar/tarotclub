(function () {
    'use strict';
    
    var game = new CHECKERS.Game({
        // The DOM element in which the drawing will happen.
        containerEl: document.getElementById('boardContainer'),
        
        // The base URL from where the BoardController will load its data.
        assetsUrl: '3d_assets/'
    });
    
})();