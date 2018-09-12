// app.js

// ============================================================================
// LOADING OFFICIAL EXPRESS MIDDLEWARE
// ============================================================================
var express       = require('express');
var https         = require('https');
var http	      = require('http');
var nano          = require('nano')('http://localhost:5984');
var hbs           = require('hbs');
var bodyParser    = require("body-parser"); // for reading POSTed form data into `req.body`
var cookieParser  = require("cookie-parser"); // the session can be saved in a cookie, so we use this to parse it
var session       = require("cookie-session");
var log4js        = require('log4js');  // improved logger system
var csrf          = require('csurf'); // CSRF protection
var uuid          = require('uuid');
var multer        = require('multer');  // upload files utility
var fs            = require('fs');
var os            = require('os');
var helmet        = require('helmet');  // protect from well-known web vulnerabilities by setting HTTP headers appropriately

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
var isWin = true; // (os.platform() === 'win32');  // Force not using SSL (nginx use it)

// ============================================================================
// HTTPS SSL CERTIFICATES
// ============================================================================
var options = {};
if (!isWin) {
    options = {
        key: fs.readFileSync('/opt/monserveur.key'),
        cert: fs.readFileSync('/opt/tarotclub.cert')
    };
}

// ============================================================================
// CONSOLE AND LOG CONFIGURATION
// ============================================================================
log4js.configure({
  appenders: [
    { type: 'console' },
    { type: 'file', filename: 'logs/app.log' }
  ],
  replaceConsole: true
});


// ============================================================================
// SETUP SERVER
// ============================================================================
var app = express();
app.use(express.static(__dirname + '/public'));
app.use(bodyParser.urlencoded({ extended: false }));

// ============================================================================
// SESSION, COOKIES AND VARIOUS SECURITY PARAMETERS
// ============================================================================
app.use(cookieParser());

app.set('trust proxy', 1) // trust first proxy


var expiryDate = new Date(Date.now() + 60 * 60 * 1000) // 1 hour

var key1 = uuid();
var key2 = uuid();

console.log('Using session keys: ', key1, ' and ', key2);

app.use(session({
    name: 'session', // use a generic name
    keys: [ key1, key2 ],

    cookie: {
        secure: true,   // Ensures the browser only sends the cookie over HTTPS.
        httpOnly: true, // Ensures the cookie is sent only over HTTP(S), not client JavaScript, helping to protect against cross-site scripting attacks.
        domain: 'tarotclub.fr', //  indicates the domain of the cookie
        expires: expiryDate
    }
}));

app.use(csrf());
app.use(helmet());

// ============================================================================
// LOADING OUR OWN MODULES
// ============================================================================
var usersDb       = require('./lib/database'); // Database wrapper
var limiter       = require('./lib/limiter')(app);
var uploadConf    = require('./lib/upload');

// Limit all post requests to 50 requests per hour 
limiter({
  path: '*',
  method: 'post',
  lookup: ['connection.remoteAddress', 'headers.x-forwarded-for'],
  total: 50,
  expire: 1000 * 60 * 60
});

app.use(multer(uploadConf).single('ia-path'));


// ============================================================================
// TEMPLATE ENGINE AND VIEWS PARAMETERS
// ============================================================================
// set the view engine to Handlebars
app.set('view engine', 'hbs');
app.set('views', __dirname + '/views');

hbs.registerPartials(__dirname + '/views/partials');
hbs.registerPartial("doc_topic", "doc_topic");

/**
 * @brief Manage the main horizontal navigation bar active buttons
 */
function BuildNavBar(menu, currentSection)
{
    var out = "";

    for(var i=0; i<menu.length; i++) {
        out = out + "<li";
        if (menu[i].section == currentSection)
        {
            out = out + " class=\"active\"";
        }
        out = out + "><a href=\"" + menu[i].url + "\">" + menu[i].title + "</a></li>";
    }
    return out;
}

hbs.registerHelper('navbar', function(currentSection) {
    var menu = [
        { title: "Accueil",           section: 'index',       url: '/'},
        { title: "Captures d'écran",  section: 'screenshots', url: '/screenshots'},
        { title: "Documentation",     section: 'doc',         url: '/doc'},
        { title: "Classements",  section: 'classements', url: '/ranking'}
    ];
    return BuildNavBar(menu, currentSection);
});

/**
 * @brief Manage the vertical user account menu on the left
 */
hbs.registerHelper('usernavbar', function(currentSection, username, role) {
    var menu = [
        { title: "Paramètres du compte",  section: 'account', url: '/user'},
        { title: "Gestion des bots",      section: 'bots',    url: '/user/bots'}
    ];

    var adminMenu = { title: "Administration",     section: 'admin',   url: '/user/admin'};

    if (role == "admin") {
        menu[menu.length] = adminMenu;
    }
    return BuildNavBar(menu, currentSection);
});

// General purpose incrementation function
hbs.registerHelper('incr', function(value) {
    return parseInt(value) + 1;
});


// ============================================================================
// ROUTES, ORDER OF DECLARATION IS IMPORTANT
// ============================================================================
var doc_home        = require('./routes/home'); // Routes for the home page
var doc_routes      = require('./routes/doc'); // Routes for the documentation
var user_routes     = require('./routes/user'); // Routes for the user pages
var ranking_routes  = require('./routes/ranking'); // Routes for the ranking pages

// Whatever is the request, we look for the session cookie if the user is already logged
app.use(function(req, res, next) {
    res.locals.username = req.session.username;
    res.locals.role     = req.session.role;
    res.locals.nickname = req.session.nickname;
    res.locals.email    = req.session.email;
    res.locals.avatar   = req.session.avatar;
    res.locals.csrf     = req.csrfToken();
    next();
});

// screenshots page 
app.get('/screenshots', function(req, res) {
    res.render('pages/screenshots', { section: "screenshots" });
});

// Specific routes for these URLs, go to dedicated javascript file to learn more...
app.use('/', doc_home);
app.use('/doc', doc_routes);
app.use('/user', user_routes);
app.use('/ranking', ranking_routes);

// ============================================================================
// 404 CUSTOM ERROR PAGE
// ============================================================================
// catch 404 and forward to error handler
// Please call this function after all defined routes, as this is the default one!
app.use(function(req, res, next) {
    res.status(404);
    res.render('404', { url: req.url });
});


// ============================================================================
// START THE SERVER AND THE DATABASE !
// ============================================================================
// Initialize the database, and if Ok, start the web server
usersDb.Create(function(createStatus) {
  	if (createStatus)	{
        usersDb.CheckViews(function(createStatus) {
            if (createStatus) {
                usersDb.UpgradeViews(function(upgradeStatus) {
                    if (upgradeStatus) {
                        if (isWin) {
                            var serverPort = 8082;
                            app.listen(serverPort);
			    console.log('Server started on port ' + serverPort);	
                        } else {

			// Redirect from http port 8080 to https
			var http = require('http');
			http.createServer(function (req, res) {
				res.writeHead(301, { "Location": "https://" + req.headers['host'] + req.url });
				res.end();
			}).listen(8080);			   


                           https.createServer(options, app).listen(8043);
                        }
                        
                    } else {
                        console.log('Failed to upgrade the views, server not started.');
                        process.exit();
                    }
                });
            } else {
                console.log('Failed to check the views, server not started.');
                process.exit();
            }
        });
  	} else {
  		  console.log('Failed to initialize database, server not started.');
        process.exit();
  	}
});

// End if the file
