/**
* middleware/isloggedin.js
* this middleware just checks if the use session is valid.
* If the user is not logged in, it will be redirected to login page.
* https://github.com/pello-io/simple-express-mongoose
* Pello Altadill - http://pello.info
*/

module.exports = function isLoggedIn (req, res, next) {
    if (!(req.session && req.session.username)) {
        return res.redirect('/user/signin');
    }
    next();
} 
