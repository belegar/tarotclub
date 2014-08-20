#!/bin/perl
#############################################################################
#   Script          : arm_map_analyzer.pl	                                #
#   Author          : Platform Team                                         #
#   Parameter       : @ARGV[0] : Perforce workspace root directory          #
#                     @ARGV[1] : output directory                           #
#   Return          : void.                                                 #
#   Descript.       : Generate a Keil uVision project file with source tree #
#                     view and ready to use debug configuration file        #
#                                                                           #
#  Perl modules     : Win32-OLE                                             #
#                     XML-Writer                                            #
#############################################################################


#------------------ Open result file and parse each line
open LINT_FILE_IN, "out.txt" or die $!;

while(<LINT_FILE_IN>)
{ 
	ParseModule($_);
}

close(LINT_FILE_IN);

################################################################
sub ParseModule
################################################################
{
    my $line  = shift(@_);
	   
    if ($line =~ m/tarotclub/g)
    {
        print $line;
    }
}


# End of script
