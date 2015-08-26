// Load modules and use them
define([], function() {

    var Identity = function (str) {
        this.nickname   = "";
        this.avatar     = "";
        this.gender     = 0; ///< uint8_t
        this.username   = "";
    };

    return Identity;
});
