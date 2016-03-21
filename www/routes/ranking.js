// user.js - User and Admin panel management

// ============================================================================
// LOADING OFFICIAL EXPRESS MIDDLEWARE
// ============================================================================
var express         = require('express');
var util            = require("util"); 
var fs              = require("fs");

// ============================================================================
// LOADING OUR OWN MODULES
// ============================================================================
var isLoggedIn      = require('../lib/isloggedin'); // Test if we are logged before loading a page
var couchDb         = require('../lib/database'); // Database wrapper

var router  = express.Router();

// Ranking pages (absolute path is /ranking)
router.get('/', isLoggedIn, function(req, res) {
    
    couchDb.GetAiContestRanking( function(status, data) {
        res.render('ranking/index', { usersection: "ranking", bots: data } );
    });
});

module.exports = router;
