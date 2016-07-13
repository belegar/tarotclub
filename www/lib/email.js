// email.js
// Functions to easily send email messages through a SMTP server

var email         = require("emailjs");
var server        = email.server.connect({
   user:    "website@tarotclub.fr", 
   password:"7qCagyMi1epmcRrrBatA", 
   host:    "mail.gandi.net", 
   ssl:     true
});

function replaceURLWithHTMLLinks(text) {
    var exp = /(\b(https?|ftp|file):\/\/[-A-Z0-9+&@#\/%?=~_|!:,.;]*[-A-Z0-9+&@#\/%=~_|])/i;
    return text.replace(exp,"<a href='$1'>$1</a>"); 
}

(function () {

    var email = {};

  	email.SendEmail = function(to, subject, message)
  	{
          // send the message and get a callback with an error or details of the message that was sent
          server.send({
             text:    message, 
             from:    "TarotClub <website@tarotclub.fr>", 
             to:      to,            //"someone <someone@your-email.com>, another <another@your-email.com>",
           //  cc:      "else <else@your-email.com>",
             subject: subject,
             attachment: [
                  { data:"<html>" + replaceURLWithHTMLLinks(message) + "</html>", alternative: true }
              ]
          }, function(err, msg) {
              if (err) {
                console.log(err);
              }
          });
  	};

	module.exports = email;
	
}());
	
