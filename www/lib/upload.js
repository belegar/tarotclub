var fs              = require("fs");
var path            = require("path");

uploadConf = {
    dest: path.resolve('./uploads/'),
    rename: function (fieldname, filename, req, res) {
      var newFileName = Date.now() + "_" + filename;
      return newFileName;
    },
    changeDest: function(dest, req, res) {
        var stat = null;
        var userDest = dest;
        if (req.session.username) {
           userDest += '/' + req.session.username; 
        }

        try {
            // using fs.statSync; NOTE that fs.existsSync is now deprecated; fs.accessSync could be used but is only nodejs >= v0.12.0
            stat = fs.statSync(userDest);
        } catch(err) {
            // for nested folders, look at npm package "mkdirp"
            fs.mkdirSync(userDest);
        }

        if (stat && !stat.isDirectory()) {
            // Woh! This file/link/etc already exists, so isn't a directory. Can't save in it. Handle appropriately.
            throw new Error('Directory cannot be created because an inode of a different type exists at "' + dest + '"');
        }
        return userDest;
    },
    limits: {
        fieldNameSize: 50,
        files: 1,
        fileSize: 1024 * 1024,
        fields: 5
    }
};

module.exports = uploadConf;
