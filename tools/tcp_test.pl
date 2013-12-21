use IO::Select;
use IO::Socket::INET;

# Flush on socket
$| = 1;

# Configuration
my $remAddr = '127.0.0.1';
my $remPort = '4269';

# Create the client TCP socket
print "Creating the remote TCP socket\n";
my $locSocket = new IO::Socket::INET (   
    PeerHost => $remAddr,
	PeerPort => $remPort,
	Proto => "tcp"
);
die "Cannot create remote TCP socket: $@\n" unless $locSocket;
 
my $sockets = IO::Select->new;
$sockets->add($locSocket);


my $data = "Coucou!\n";
SendToSocket($data);

$locSocket->close();

################################################################
sub SendToSocket
################################################################
{
	my $data_to_send  = shift(@_);
    my $data;

    sleep(10);
    print "Sending data...\n";
    $locSocket->send($data_to_send);
    

	sleep(2);
     print "Waiting for received data...\n";
    $locSocket->recv($data, 1024);
    
sleep(10);

    # Data has been received
 #   my $payloadLength = length($data);
 #  print "Remote RX: $payloadLength bytes\n";
 #   print uc(join('', unpack('H*',$data)))."\n";
    print $data;
}
