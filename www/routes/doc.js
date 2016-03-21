var express = require('express');
var router = express.Router();

// --------------- Doc home page 
router.get('/', function(req, res) {
    res.render('doc/index', { section: "doc" });
});

// --------------- Doc subpage /doc/net
router.get('/net', function(req, res) {
    res.render('doc/net', { section: "doc" });
});
// --------------- Doc subpage /doc/dev
router.get('/dev', function(req, res) {
    res.render('doc/dev', { section: "doc" });
});
// --------------- Doc subpage /doc/dic
router.get('/dic', function(req, res) {
    res.render('doc/dic', { section: "doc" });
});
// --------------- Doc subpage /doc/build
router.get('/build', function(req, res) {
    res.render('doc/build', { section: "doc" });
});
// --------------- Doc subpage /doc/scripts
router.get('/scripts', function(req, res) {
    res.render('doc/scripts', { section: "doc" });
});

module.exports = router;
