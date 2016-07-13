// database.js
// Functions to easily manage the CouchDB documents for the TarotClub website

// ============================================================================
// LOADING OFFICIAL NODE.JS MODULES
// ============================================================================
var nano		= require('nano')('http://localhost:5984');
var bcrypt 		= require('bcryptjs');
var async 		= require('async');
var serialize 	= require('serialize-javascript');

// ============================================================================
// DEFINITIONS
// ============================================================================
var USERS_DATABASE = 'tc_users';
var AICONTEST_DATABASE = 'tc_aicontest';

function HashedPassword(password)
{
	var salt = bcrypt.genSaltSync(10);
	var hash = bcrypt.hashSync(password, salt);
	return hash;
};

var bots_to_run = function(doc) {
	if (doc.bots) {
		var list = new Array();
		for (var i = 0; i < doc.bots.length; i++) {
			
			if( doc.bots[i].status == "waiting")
			{
				list[list.length] = doc.bots[i];
			}
		}
		emit(doc._id, list);
	}
}

var bots_completed = function(doc) {
	if (doc.bots) {
		for (var i = 0; i < doc.bots.length; i++) {
			
			if( doc.bots[i].status !== "waiting")
			{
				
				var data = {};
				data.botname = doc.bots[i].botname;
				data.docid = doc.bots[i].status;
				emit(doc._id, data);
			}
		}
		
	}
}


var score_list = function(doc) {
	emit(doc._id, doc.score);
}


function UsersViews()
{
	var views = {
	"views": {
       "bots_to_run": {
       "map": serialize(bots_to_run)
       },
       "bots_completed": {
       "map": serialize(bots_completed)
       }
   }};

   return views;
};

function AiContestViews()
{
	var views = {
	"views": {
       "score_list": {
       "map": serialize(score_list)
       }
   }};

   return views;
};


(function () {

    var couchDb = {};

	/**
	 * @param[in] callback: function callback fn(success), success = bool if no error, otherwise false
	 */
	couchDb.Create = function(callback)
	{
		nano.db.list(function(err, body) {

			if (err) {
				console.log("Cannot list databases!");
				callback(false);
			} else {
				var exists = false;
				var aicontestExists = false;

				// body is an array
				body.forEach(function(db) {
					//console.log(db); // debug
					if (db == USERS_DATABASE)
					{
						exists = true;
					}
					if (db == AICONTEST_DATABASE)
					{
						aicontestExists = true;
					}
				});

				async.series([
					function(cb) {
						if (!exists)
						{
							console.log('Creating database ', USERS_DATABASE);
							nano.db.create(USERS_DATABASE, function(err, body, header) {
								if (err) {
									console.log('[db.create] ', err.message);
									console.log(body);
									cb(false);
								} else {
									couchDb.db = nano.use(USERS_DATABASE);
									// Create default admin user
									couchDb.AddNewUser('anthony', 'Belegar', 'belegar@tarotclub.fr', 'azertyui', 'admin', '127.0.0.1', function(status, token) {
										if (status) {
											couchDb.VerifyUser('anthony', token, function(){
												// nothing special
												cb();
											});
										}
									});
								}
							});
						} else {
							console.log('Database', USERS_DATABASE,  'exists');
							couchDb.db = nano.use(USERS_DATABASE);
							cb();
						}

					},
					function(cb) {
						if (!aicontestExists)
						{
							console.log('Creating database ', AICONTEST_DATABASE);
							nano.db.create(AICONTEST_DATABASE, function(err, body, header) {
								if (err) {
									console.log('[db.create] ', err.message);
									console.log(body);
									cb(false);
								} else {
									couchDb.ai = nano.use(AICONTEST_DATABASE);
									cb();
								}
							});
						} else {
							console.log('Database', AICONTEST_DATABASE,  'exists');
							couchDb.ai = nano.use(AICONTEST_DATABASE);
							cb();
						}
					}

					], function(err, results) { //This function gets called after the two tasks have called their "task callbacks"
				        if (err) {
				        	callback(false);
				        } else {
				        	callback(true);
				        }
			   		}
			   	);
			}
		});
	};

	couchDb.CheckViews = function(callback)
	{
		async.series([
			function(cb) {

				couchDb.db.get("_design/users", function(err, body) {
					couchDb.db.insert({ views: null }, "_design/users", function(err, body, header) {
				      cb();
				    });
				});

			}, function(cb) {
				couchDb.ai.get("_design/scores", function(err, body) {
					couchDb.ai.insert({ views: null }, "_design/scores", function(err, body, header) {
				      cb();
				    });
				});
			}
			], function(err, results) { //This function gets called after the two tasks have called their "task callbacks"
		        callback(true);
	   		}
	   	);

	};

	couchDb.UpgradeViews = function(callback)
	{
		async.series([
			function(cb) {
				couchDb.db.get("_design/users", function(err, body) {
					//console.log(body);
					var str = UsersViews();
					if (serialize(str.views) !== serialize(body.views)) {
						console.log("Views not equal, update them");
						body.views = str.views;
						couchDb.db.insert( body, "_design/users", function (error, response) {
							cb();
						});
					} else {
						cb();
					}
				});
			}, function(cb) {
				couchDb.ai.get("_design/scores", function(err, body) {
					//console.log(body);
					var str = AiContestViews();
					if (serialize(str.views) !== serialize(body.views)) {
						console.log("Views not equal, update them");
						body.views = str.views;
						couchDb.ai.insert( body, "_design/scores", function (error, response) {
							cb();
						});
					} else {
						cb();
					}
				});
			}
			], function(err, results) { //This function gets called after the two tasks have called their "task callbacks"
		        if (err) {
		        	callback(false);
		        } else {
		        	callback(true);
		        }
	   		}
	   	);
	};

	couchDb.GetAiContestRanking = function(callback)
	{
		var bots;
		var results;

		async.series([
				function(cb) {
					couchDb.db.view('users', 'bots_completed', function(err, body) {
						if (!err) {
							bots = body.rows;
							//console.log(body.rows);
							cb();
						}
					});
				},
				function(cb) {
					couchDb.ai.view('scores', 'score_list', function(err, body) {
						if (!err) {
							results = body.rows;
							//console.log(body.rows);
							cb();
						}
					});
				}

			], function(err) { //This function gets called after the two tasks have called their "task callbacks"
		        if (err) {
		        	callback(false);
		        } else {
		        	// We have the two result sets, then create an array of object, sorted by score
		        	//	points.sort(function(a, b){return a-b});

		        	var data = [];
		        	for (var i in results) {

	        			var row = {};
	        			for (var j in bots) {
	        				// look for the bot that matches the key
	        				if (results[i].key === bots[j].value.docid) {
	        					row.botname = bots[j].value.botname;
	        					row.score = results[i].value;
		        				data.push(row);	
	        					break;
	        				}
	        			}
		        	}

		        	data.sort(function(a, b){
					    return a.score - b.score;
					}).reverse();
					
					//console.log(data);
		        	callback(true, data);
		        }
		    }
		);
	};

	couchDb.AddNewUser = function(username, nickname, email, password, role, ip, callback)
	{
		var isodate = new Date().toISOString();
		var token = rand=Math.floor((Math.random() * 100000) + 567407);

        var newUser = {
        	"nickname": nickname, // default nickname is the username
        	"email": email,
        	"password": HashedPassword(password),
        	"role": role,
        	"bots" : [],
        	"bots_limit": 1,
        	"avatar": "", 				// default avatar is generated using Gravatar and email address
        	"register_date": isodate,
        	"verified": token.toString(),			// true if email verified, otherwise contains the token (verification waiting)
        	"ip": ip					// IP address at the time of the registration
        };

        couchDb.db.get(username, function(err, body) {
			if (!err) {
				callback(false, "Le nom d'utilisateur est déjà pris.");
			} else {
				// The Id of the document is the username
				couchDb.db.insert(newUser, username, function (error, response) {
				    if (error) {
				      	callback(false, "L'enregistrement a échoué. Contactez l'administrateur du site.");
				    } else {
				    	callback(true, token);
				    }
				});
			}
		});
	};

	/*
	 * @brief Check username and password for login
     *
     * callback(message, body)
	 */
	couchDb.CheckUser = function(username, password, callback)
	{
        couchDb.db.get(username, function(err, body) {
			if (!err) {
				// Check password
				if (bcrypt.compareSync(password, body.password)) {
					callback('', body); // access granted
				} else {
					callback("Paramètres incorrects.");
				}
			} else {
		      	callback("Paramètres incorrects.");
		    }
		});
	};

	/**
	 * @brief Verify an account email (anti-bot!)
	 * @param[in] username: database key (document id)
	 * @param[in] id: validation number (string format)
	 * @param[in] callback: response
	 */
	couchDb.VerifyUser = function(username, id, callback)
	{
        couchDb.db.get(username, function(err, body) {
			if (!err) {
				// Check received id with the database (string comparison)
				if (id == body.verified)
				{
					// Update the verified status to 'true'
					body.verified = "true";
					couchDb.db.insert( body, username, function (error, response) {
						if (error) {
							callback("La validation de votre compte a échoué. Contactez l'administrateur du site.");
						} else {
							console.log("Validation of user " + username + " succeded");
							callback();	 // without parameters is a success
						}
					});
				} else {
					console.log("Account verification failure. Username: ", username, " sent ID: ", id, " DB ID: ", body.verified);
					callback("Paramètres incorrects.");
				}
			} else {
		      	callback("Paramètres incorrects.");
		    }
		});
	};

	/**
	 * @brief Get the document of a user
	 */
	couchDb.GetInfos = function(username, callback)
	{
		couchDb.db.get(username, function(err, body) {
			callback(body);
		});
	};

	couchDb.SetInfos = function(username, nickname, password, avatar, callback)
	{
		couchDb.db.get(username, function(err, body) {
			if (!err) {
				if (nickname != undefined) {
					body.nickname = nickname;
					console.log("new nickname");
				}
				if (avatar != undefined) {
					body.avatar = avatar;	
					console.log("new avatar");
				}
				if (password != undefined) {
					body.password = HashedPassword(password);
					console.log("new password");
				}
				couchDb.db.insert( body, username, function (error, response) {
					if (error) {
						callback("Impossible de modifier les paramètres.");
					} else {
						callback(); // good!
					}
				});
			} else {
				callback("Impossible de trouver l'utilisateur.");
			}
		});
	};

	/**
	 * @brief Change the user password
     * 
     * If newPassword is empty, then generate a random password

     * @return callback(newPassword, message, email )
     * 
	 */
	couchDb.ChangePassword = function(username, newPassword, callback)
	{
        couchDb.db.get(username, function(err, body) {
			if (!err) {
				if (newPassword == '')
				{
					// new password not defined, use the random one
					newPassword = Math.random().toString(36).slice(-8);	
				}

				body.password = HashedPassword(newPassword);

				couchDb.db.insert( body, username, function (error, response) {
					if (error) {
						callback('', "Impossible de modifier le mot de passe.");
					} else {
						callback(newPassword, undefined, body.email); // good!
					}
				});

			} else {
		      	callback('', "Utilisateur inconnu.");
		    }
		});
	};

	couchDb.AddBot = function(username, botname, aiFilePath, callback)
	{
		var isodate = new Date().toISOString();

        var newBot = {
        	"botname": botname,
        	"role": 'bot',
        	"avatar": "",			// Future usage
        	"added_date": isodate,
        	"status": 'waiting',	// 'waiting': just added, 'playing', 'waiting'
        	"aifile": aiFilePath	// AI archive file
        };

        couchDb.db.get(username, function(err, body) {
			if (err) {
				callback(false, "Utilisateur inconnu.");
			} else {
				// The Id of the document is the username
				if (!body.bots) {
					body.bots_limit = 1; // default number of bots is one (can be upgraded later)
					// Create the array of bots
					body.bots = [];
				}
				// Insert the new bot (erase the last one if limit is reached)
				var index = body.bots.length;
				if (index >= body.bots_limit) {
					index--;
				}
				body.bots[index] = newBot;

				couchDb.db.insert(body, username, function (error, response) {
				    if (error) {
				      	callback(false, "L'enregistrement a échoué. Retentez.");
				    } else {
				    	callback(true, '', body.bots);
				    }
				});
			}
		});
	};

	couchDb.DeleteBot = function(username, index, callback)
	{
        couchDb.db.get(username, function(err, body) {
			if (err) {
				callback(false, "Utilisateur inconnu.");
			} else {
				body.bots.splice(index, 1); // properly delete the element in the array
				couchDb.db.insert(body, username, function (error, response) {
				    if (error) {
				      	callback(false, "L'enregistrement a échoué. Retentez.");
				    } else {
				    	callback(true, '', body.bots);
				    }
				});
			}
		});
	};

	module.exports = couchDb;
	
}());
	
