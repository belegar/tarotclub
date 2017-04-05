// home.js - Home page blog and news

// ============================================================================
// LOADING OFFICIAL EXPRESS MIDDLEWARE
// ============================================================================
var express         = require('express');
var util            = require("util"); 
var fs              = require("fs");
var async           = require('async');
var marked          = require('marked'); // markdown to HTML
var net             = require("net");
var moment          = require('moment'); //date-time parser and renderer
var log4js          = require('log4js');  // improved logger system
var yaml            = require('js-yaml');

// ============================================================================
// LOADING OUR OWN MODULES
// ============================================================================


// ============================================================================
// MODULES PARAMETERS
// ============================================================================


// ============================================================================
// GLOBAL FUNCTIONS
// ============================================================================

// List all files [as directory tree] in Node.js recursively in a synchronous fashion
var walkSync = function(dir) {

    var files = fs.readdirSync(dir);
    var filelist = [];
    files.forEach(function(file) {
        if (fs.statSync(dir + '/' + file).isDirectory()) {
            filelist.push(walkSync(dir + '/' + file));
        }
        else { filelist.push(file); }
    });
    return filelist;
};

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

var logger = log4js.getLogger();

moment.locale('fr');

var NEWS_PER_PAGE = 4;

// This global variable is refreshed periodically by a background timer
var ServerInfos = {};

// Cache of blog entries
// Key is the date, then the object containing the title and the text
var BlogEntries = {
//       '2011-12-31':  { title: "blahblah",  text:"Dans quelle état j'ère ?" },
//       '2013-12-31':  { title: "Deus",  text:"Machina" }
};


var MarkeOptions = {
  renderer: new marked.Renderer(),
  gfm: true,
  tables: true,
  breaks: false,
  pedantic: false,
  sanitize: false,
  smartLists: true,
  smartypants: false
};


function GetFrontMatter (lines) {
  if ((/^---/).test(lines[0].trim())) {
    var frontMatter = [];
    lines.shift();
    var line;
    // Keep shifting off lines till we find the next ---
    while (!(/^---/).test(line = lines.shift().trim())) {
      frontMatter.push(line);
    } 
    return frontMatter.join('\n');
  } else {
    return '';
  }
};

// Periodically check new blog entries in the file system
function CheckNews()
{  
    var entries = walkSync("blog");

    // entries is now filled with something like:
    // [ '2011-12-31-new-years-eve-is-awesome.md', '2015-06-25-version-2.6.2-disponible.md' ]

    entries.forEach(function(value, index) {

        try
        {
            var contents = fs.readFileSync('blog/' + value, 'utf8');
            // Extract the date
            var arr = value.match(/(\d{4}-\d{2}-\d{2})/) || [];

            if (arr.length == 2)
            {
                var date = arr[1];

                // Parse the YAML header

                 // Generate frontMatter and actual markdown content
                var lines = contents.split('\n');
                var frontMatter = GetFrontMatter(lines);
                var newContent = lines.join('\n');
                var html = marked(newContent, MarkeOptions);

                var matter = frontMatter ? yaml.load(frontMatter) : {};

                // Add entry, key is the date
                BlogEntries[date] = { 
                   // yaml: frontMatter.trim(),
                  //  markdown: newContent.trim(),
                    content: html.trim(),
                    meta: matter
                  };
            }
            else
            {
                logger.error('Bad array size: ', arr.length, value);
            }
   
        }
        catch(e)
        {
            logger.error('Error while parsing markdown blog entries');
        }
    
    });
}

var GetBlogEntries = function(page) {
    var list = [];

    Object.keys(BlogEntries)
      .sort(function keyOrder(k1, k2) {

            // Sort descending (from most recent news to oldest one)
            if (moment(k1).isAfter(k2)) return -1;
            else if (moment(k1).isBefore(k2)) return +1;
            else return 0;
        })
      .forEach(function(v, i) {
            var entry = {};
            entry.date = moment(v).format("dddd D MMMM YYYY");
            entry.text = BlogEntries[v].content;
            entry.title = BlogEntries[v].meta.title;
            list.push(entry);
       });

    //console.log(list);
    return list;
};


var router  = express.Router();


// ============================================================================
// ROUTE ACTIONS (GET/POST)
// ============================================================================

// index page 
router.get('/', function(req, res) {
    
    var data = {};

    data.section = "index";

    //  Tarot server informations
    if (Object.keys(ServerInfos).length > 0) {
      data.servers = ServerInfos;
    }

    // News feed
    data.news = GetBlogEntries(0);
    
    res.render('pages/index', data);
});


// ============================================================================
// CHECK SERVERS STATUS AND PRINT IT
// ============================================================================
function ReadServerStatus(port)
{
    var socket = new net.Socket();
    socket.setEncoding('utf8');

    socket.on("data", function(data) {

        // Replace every esccape character
        data = data.replace(/\\n/g, "\\n")
                  .replace(/\\'/g, "\\'")
                  .replace(/\\"/g, '\\"')
                  .replace(/\\&/g, "\\&")
                  .replace(/\\r/g, "\\r")
                  .replace(/\\t/g, "\\t")
                  .replace(/\\b/g, "\\b")
                  .replace(/\\f/g, "\\f");

        // There is some data, parse it
        // Use a try/catch to avoid crashing NodeJS in case of error
        try {
            var jsonObj = JSON.parse(data);
        } catch (e) {
            return console.error(e);
        }
        //console.log(jsonObj);
        ServerInfos[port] = jsonObj;
        socket.end();

    });

    socket.on("error", function() {
        socket.end();
        ServerInfos[port] = {};
    });

    socket.connect(port, function(){
      socket.write("JSON.stringify(Server)"); // ask the server status
    });
};

/**
 * @brief Checks TCDS servers presence and status
 */
function CheckServers()
{  
    async.series(
        [
            function() {
                ReadServerStatus(8090);
            },
            function() {
                ReadServerStatus(8091);
            }
        ]
    );

}

var env = process.env.NODE_ENV || 'dev';

// Check every minute several website stuff
var scanInterval = 60000; // Production value


if (env == 'dev')
{
    scanInterval = 5000; // development value
}

setInterval(CheckServers, scanInterval);
setInterval(CheckNews, scanInterval);


module.exports = router;
