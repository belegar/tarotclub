# Debian server setup

## Users management

```
adduser tarot
adduser systemd-sysv
```

## Install packages

```
apt-get install build-essential git iptables-persistent qt5base qt5-qmake
```

## Install CouchDb


Compile and install CouchDb 1.6.1 from the sources.

```
apt-get install libmozjs185-dev erlang libicu-dev
./configure --prefix=/opt/couchdb
make && make install 
```

Download Node.js and extract it in /opt directory as 'tarot' user.

wget https://nodejs.org/dist/v4.6.0/node-v4.6.0-linux-x64.tar.xz

## Install a recent version of GCC on old Debian server

  * Add unstable source to APT : echo "deb http://ftp.fr.debian.org/debian unstable main contrib non-free" > /etc/apt/sources.list.d/unstable.list
  * apt-get update
  * apt-get install gcc-6 g++-6
  

# Web server deployement

The web server Node.js application and various scripts are located in the tarotclub/www sub-directory.
The tarotclub/www/etc directory contains the systemd scripts.

## Install Node.js dependencies

Run 'npm install' at the directory root /opt/tarotclub/www

## Run CouchDB as a systemd service

Copy 'couchdb.service' (as root) to /etc/systemd/system/couchdb.service

Enable it: # systemctl enable couchdb
Start it: # systemctl start couchdb
See status: # systemctl status couchdb
See logs: # journalctl -u couchdb

Service must be active : [info] [<0.31.0>] Apache CouchDB has started on http://127.0.0.1:5984/

## Run Web app as a systemd service

Copy 'tarotclubweb.service' (as root) to /etc/systemd/system/tarotclubweb.service

Enable it: # systemctl enable tarotclubweb
Start it: # systemctl start tarotclubweb
See status: # systemctl status tarotclubweb
See logs: # journalctl -u tarotclubweb

Try killing the node process by its pid and see if it starts back up!

## Redirect ports

In Debian / Ubuntu, iptables rule are applied directly — as soon as you run the following commands the port will be open:

For HTTP:
iptables -A INPUT -p tcp --dport 80 -j ACCEPT
iptables -A PREROUTING -t nat -p tcp --dport 80 -j REDIRECT --to-port 8080

For HTTPS:
iptables -A INPUT -p tcp --dport 443 -j ACCEPT
iptables -A PREROUTING -t nat -p tcp --dport 443 -j REDIRECT --to-port 8043

To save the rules out to a config file later (you need the iptables-persistent package):

iptables-save > /etc/iptables/rules.v4

The config file will be used by the iptables-persistent service when the machine boots.


## SSL configuration

Copy past the following additionnal intermediate certificates at the send of your .cert file:

https://www.gandi.net/static/CAs/GandiStandardSSLCA2.pem

## Mail password configuration

Create a text file containg the mail server password: /opt/mailpassword.txt


## Configure IRC bots

wget ftp://ftp.eggheads.org/pub/eggdrop/source/1.6/eggdrop1.6.18.tar.gz

Code:
tar zxvf eggdrop1.6.18.tar.gz 
cd eggdrop1.6.18 
./configure 
make config 
make 
make install DEST=~/location_to_run_eggdrop_from

Utiliser le fichier de configuration simple.conf.

Configure Web site

Configure Game server

Retreive the TarotClub source code (client & server)
