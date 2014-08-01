#!/bin/perl
#############################################################################
#   Script          : create_debug_dir.pl                                   #
#   Description     : Generate directory environment for 3 opponents        #
#                                                                           #
#############################################################################
use IO::Handle;
use IO::String;
use FileHandle();
use Win32::OLE;
use File::Copy;
use File::Path;
use File::Spec;
use File::Basename;
use Data::Dumper;
use XML::Writer;
use Cwd;
use Getopt::Long;
use File::Path qw(make_path remove_tree);

# Global variables
my $QT_LOC = "C:/Qt/Qt5.2.1/5.2.1/mingw48_32/bin";
my $output_root = "../build-desktop";

@players = (
"Bender",
"Leela",
"Zoidberg"
);

@qt_dll = (
"Qt5CLucened.dll",
"Qt5Scriptd.dll",
"libstdc++-6.dll",
"Qt5Cored.dll",
"Qt5Sqld.dll",
"libwinpthread-1.dll",
"Qt5Guid.dll",
"Qt5Svgd.dll",
"icudt51.dll",
"Qt5Helpd.dll",
"Qt5Widgetsd.dll",
"icuin51.dll",
"Qt5Networkd.dll",
"Qt5Xmld.dll",
"icuuc51.dll",
"Qt5ScriptToolsd.dll",
"libgcc_s_dw2-1.dll"
);

foreach $player (@players)
{
	my $dirName = "$output_root/$player";
	# Create output directory
	make_path($dirName);

    # Copy opponent configuration file
    copy ("tarotclub-$player.json", "$dirName/tarotclub.json");

    # Copy executable, Windows version
    if (-f "$output_root/debug/TarotClub.exe")
    {
        copy ("$output_root/debug/TarotClub.exe", "$dirName/TarotClub.exe");
    }

    # Copy executable, Linux version
    if (-f "$output_root/debug/TarotClub")
    {
        copy ("$output_root/debug/TarotClub", "$dirName/TarotClub");
    }

    foreach $dll (@qt_dll)
    {
    	
    	my $fileName = "$dirName/$dll";
    	if (-f $fileName)
    	{
        	print "Skipping $dll\n";
    	}
    	else
    	{
    		if (-d $dirName)
    		{
	    		print "Copying $dll\n";
	    		copy("$QT_LOC/$dll", $fileName);
	    	}
	    	else
	    	{
	    		print "Directory $dirName does not exists\n";
	    	}
    	}
    }
}

# end of script
