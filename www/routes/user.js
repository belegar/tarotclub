// user.js - User and Admin panel management

// ============================================================================
// LOADING OFFICIAL EXPRESS MIDDLEWARE
// ============================================================================
var express         = require('express');
var util            = require("util"); 
var fs              = require("fs");
var JSZip           = require("jszip");

// ============================================================================
// LOADING OUR OWN MODULES
// ============================================================================
var isLoggedIn      = require('../lib/isloggedin'); // Test if we are logged before loading a page
var usersDb         = require('../lib/database'); // Database wrapper
var validateEmail   = require('../public/js/tarotclub'); // Shared functions between the front-end and the back-end
var emailServer     = require('../lib/email'); // send emails

var router  = express.Router();

/**
 * @brief Test the validity of the username
 * @param[in] name (string) 
 */
function IsValidName(name) {
    var valid = false;
    if ((name.length > 0) && (name.length < 15)) {
        valid = true;
    }
    return valid;
};

function IsValidPassword(password) {
    var valid = false;
    if (password.length >= 8 ) {
        valid = true;
    }
    return valid;
}

function IsValidAiArchve(path) {
    var valid = false;

    var data = fs.readFileSync(path);
    if (data) {
        var counter = 0;
        var zip = new JSZip(data);
        Object.keys(zip.files).forEach(function(filename) {
           // var content = zip.files[filename].asNodeBuffer();
            //console.log(filename);
            if (filename == 'main.js') { counter++; }
            if (filename == 'package.json') { 
                // FIXME: test the JSON file structure validity
                counter++;
            }
        });
        if (counter == 2) {
            valid = true;
        }
    }
    return valid;
}

// User dashboard (absolute path is /user)
router.get('/', isLoggedIn, function(req, res) {
    res.render('user/index', { usersection: "account" } );
});

// User parameters changes
router.post('/', isLoggedIn, function(req, res) {
    var nickname  = req.body.nickname;
    var password  = req.body.password;
    var avatar    = req.body.avatar;
    var username  = req.session.username;

    if (nickname == '' && password == '' && avatar == '') {
        res.render('user/index', { usersection: "account", failure: "Aucun changement détecté." } );
    } else {
        if (!IsValidName(nickname)) {
            nickname = undefined;
        }
        if (!IsValidPassword(password)) {
            password = undefined;
        }
        if (avatar == '') {
            avatar = undefined;
        }

        if (nickname == undefined && password == undefined && avatar == undefined) {
            res.render('user/index', { usersection: "account", failure: "Erreurs dans les champs." } );
        } else {
            usersDb.SetInfos(username, nickname, password, avatar, function(msg) {
                if (msg) {
                    res.render('user/index', { usersection: "account", failure: msg } );
                } else {
                    // Update locals & session
                    if (nickname) {
                        req.session.nickname = nickname;
                        res.locals.nickname = nickname;
                    }
                    if (avatar) {
                        req.session.avatar = avatar;
                        res.locals.avatar = avatar;
                    }
                    res.render('user/index', { usersection: "account", success: "Paramètres modifiés." } );
                }
            });
        }
    }
});

// Get the bots
router.get('/bots', isLoggedIn, function(req, res) {
    usersDb.GetInfos(res.locals.username, function(body) {
        if (!body.bots) {
            body.bots = [];
            body.bots_limit = 1;
        }
        for (var i = 0; i < body.bots.length; i++) {
            if (body.bots[i].status != "waiting") {
                body.bots[i].status = "completed";
            }
        }
        res.render('user/bots', { usersection: "bots", bots: body.bots } );
    });
});

// Delete one bot
router.get('/delbot/:id', isLoggedIn, function(req, res) {
    usersDb.DeleteBot(res.locals.username, req.params.id, function(bots) {
		// FIXME: also delete the corresponding document (games played) if status != waiting
        res.render('user/bots', { usersection: "bots", bots: bots } );
    });
});

router.post('/bots', isLoggedIn, function(req, res, next) {
    var botname  = req.body.botname;

    if (req.files && botname) { 
       // console.log(util.inspect(req.files)); // enable this line to show some uploaded file characteristics
        if (req.files.iaFile === undefined) {
            res.render('user/bots', { usersection: "bots", failure: "Sélectionnez un fichier." } );
        } else {
            var filePath = req.files.iaFile.path;
            fs.exists(filePath, function(exists) { 
                if(exists && IsValidAiArchve(filePath)) {
                    // Add bot to the database
                    usersDb.AddBot(res.locals.username, botname, filePath, function(result, msg, bots) {
                        if (result == true) {
                            res.render('user/bots', { success: "Le téléchargement a réussi.", bots: bots});
                        } else {
                            res.render('user/bots', { usersection: "bots", failure: "Impossible de sauvegarder le bot." } );
                        }
                    });
                } else { 
                    res.render('user/bots', { usersection: "bots", failure: "Le téléchargement a échoué." } );
                } 
            });
        }
    } 
});

// Signin form
router.get('/signin', function(req, res) {
    res.render('user/signin');
});

router.post('/signin', function(req, res) {
    var username  = req.body.username;
    var password  = req.body.password;

    if (username && password)
    {
        if (username != '' && password != '')
        {
            usersDb.CheckUser(username, password, function(msg, doc) {
                if (msg == '')
                {
                    // Store the user's primary key
                    // in the session store to be retrieved,
                    // or the entire user object
                    req.session.username = username;
                    req.session.nickname = doc.nickname;
                    req.session.role = doc.role;
                    req.session.email = doc.email;
                    req.session.avatar = doc.avatar;
                    res.redirect('/');
                }
                else
                {
                    // Render signin page with a failure message
                    res.render('user/signin', { message: msg });
                }
            });
        }
        else
        {
            res.render('user/signin', { message: "Veuillez compléter le formulaire" });
        }
    }
    else
    {
        res.render('user/signin', { message: "Veuillez compléter le formulaire" });
    }

});

// Signup form
router.get('/signup', function(req, res) {
    res.render('user/signup');
});

router.post('/signup', function(req, res) {
    var username  = req.body.username;
    var email     = req.body.email;
    var password  = req.body.password;
    var ip = req.header('x-forwarded-for') || req.connection.remoteAddress;

    // Warning: in case of multiple proxies, the ip address is a coma-separated list such as:
    // X-Forwarded-For: OriginatingClientIPAddress, proxy1-IPAddress, proxy2-IPAddress
    // This is something we can filter after

    // spam bot detection: 
    //   * honey pot must exists and empty
    //   * generated check-box must be checked :)
    
    if ((req.body.password_again === undefined) || (req.body.turing === undefined))
    {
        console.log("Spammer detected (missing form elements) from IP: ", ip);
        res.redirect('/');
    }
    else if (req.body.password_again != "")
    {
        console.log("Spammer detected (honey pot not empty) from IP: ", ip);
        res.redirect('/');
    }
    else if (username && password && email)
    {
        if (username != '' && IsValidPassword(password) && req.body.turing == "21")
        {
            if (validateEmail(email))
            {
                if (IsValidName(username))
                {
                    usersDb.AddNewUser(username, username, email, password, 'player', ip, function(success, argument) {
                        if (success === true)
                        {
                            // Store the user's primary key
                            // in the session store to be retrieved,
                            // or the entire user object
                            req.session.username = username;
                            var link="http://www.tarotclub.fr/user/verify?username=" + username + "&id=" + argument;
                            console.log("Sent validation email to new user: ", link);
                            emailServer.SendEmail(email, "TarotClub - validation du compte", 
                              "Merci de valider votre compte créé sur le site TarotClub en cliquant sur le lien suivant: " + 
                              link);
                             res.render('user/signup', { success: "Succès ! Un email de vérification a été envoyé. Vous avez 24h pour valider votre compte." });

                        }
                        else
                        {
                            // Render signup page with a failure message
                            res.render('user/signup', { failure: argument });
                        }
                    });
                }
                else
                {
                    res.render('user/signup', { failure: "Pseudonyme trop long." });
                }
            }
            else
            {
                res.render('user/signup', { failure: "Adresse email invalide." });
            }
        }
        else
        {
            res.render('user/signup', { failure: "Champs incorrects." });
        }
    }
});

// Forgot password form
router.get('/reset', function(req, res) {
    res.render('user/reset');
});

router.post('/reset', function(req, res) {
    var username  = req.body.username;

    if (username)
    {
        if (username != '')
        {
            usersDb.ChangePassword(username, '', function(newPassword, message, email) {
                if (message === undefined)
                {
                    res.render('user/reset', { success: "Un email contenant votre nouveau mot de passe vous a été envoyé" });
                    console.log("Sent password reset to user: ", username);
                    emailServer.SendEmail(email, "TarotClub - nouveau mot de passe", 
                              "Vous avez demandé la génération d'un nouveau mot de passe. Nous vous conseillons de le changer au plus tôt en vous " +
                              "connectant à votre compte personnel sur http://www.tarotclub.fr. Votre nouveau mot de passe est : " + newPassword);
                }
                else
                {
                    // Render signup page with a failure message
                    res.render('user/reset', { message: message });
                }
            });
        }
        else
        {
            res.render('user/reset', { message: "Veuillez entrer un nom d'utilisateur." });
        }
    }
    else
    {
        res.render('user/reset', { message: "Veuillez entrer un nom d'utilisateur." });
    }

});

router.get('/verify', function(req, res)
{
    var username = req.query.username;
    var id = req.query.id;
    usersDb.VerifyUser(username, id, function(msg) {
        if (!msg) {
            res.render('user/verify');    
        } else {
            res.render('user/verify', { message: msg });
        }
    });

});

router.get('/logout', function(req, res) {
    // delete the session variable
    req.session = null;
    // destroy the user's session to log them out
    // will be re-created next request
    //req.session.destroy(function(){
       
    //});
    res.redirect('/');
});

module.exports = router;
