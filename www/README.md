# Installation

## Ubuntu 10.04 server setup

```
adduser tarot
adduser systemd-sysv
```

## Install tools

```
apt-get install nodejs npm build-essential mercurial iptables-persistent
```

Compile and install CouchDb 1.6.1 from the sources.

```
apt-get install libmozjs185-dev erlang libicu-dev
```

# Deployement

## Run Web app as a systemd service

Copy this file (as root) to /etc/systemd/system/node-sample.service
Enable it: # systemctl enable node-sample
Start it: # systemctl start node-sample
See status: # systemctl status node-sample
See logs: # journalctl -u node-sample
Try killing the node process by its pid and see if it starts back up!

## Redirect ports

In Debian / Ubuntu, iptables rule are applied directly — as soon as you run the following commands the port will be open:

iptables -A INPUT -p tcp --dport 80 -j ACCEPT
iptables -A PREROUTING -t nat -p tcp --dport 80 -j REDIRECT --to-port 8080

To save the rules out to a config file later (you need the iptables-persistent package):

iptables-save > /etc/iptables/rules.v4

The config file will be used by the iptables-persistent service when the machine boots.


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
