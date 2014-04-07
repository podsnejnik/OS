#!/usr/bin/perl
use IO::Socket;
use IO::Select;
use Fcntl;

my $port = 1337;

my $server = IO::Socket::INET->new(LocalPort => $port,
                                Listen    => SOMAXCONN );
my $flags = fcntl($server, F_GETFL, 0);
fcntl($server, F_SETFL, $flags | O_NONBLOCK);
my $select = IO::Select->new($server);

while (1) {
    my $client;
    my $rv;
    my $data;

    foreach $client ($select->can_read(1)) {

        if ($client == $server) {

            $client = $server->accept();
            $select->add($client);
            
        } else {
            $data = '';
            $rv   = $client->recv($data, 1024, 0);
            $rv = $client->send($data, 0);
        }
    }
}