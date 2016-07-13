#!/bin/perl
# =========================================================
# Vera++ list of files generation script
# =========================================================
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


# =========================================================
# Definitions
# =========================================================	
my $SRC_ROOT 		= @ARGV[0];

# This list gather all directories or files that must be excluded from the Vera++ analysis
# The script will add the "TarotClub" root directory
@exclude_dir = (

);

@exclude_files = (
"duktape.h",
"sqlite3.h",
"sqlite3ext.h"
);

@exclude_extensions = (
".mk",
".c",
".ui"
);

# =========================================================
# Main
# =========================================================		
open FILE_LIST_OUT, ">files.txt" or die $!;

# Recursively walk on source directory searching for files
my $root_path = "$SRC_ROOT/src";
ParseDirectory($root_path);

print "Output file generated\n\r";

close(FILE_LIST_OUT);

# =========================================================
# @sub 	PrintFile
# =========================================================
sub PrintFile
{
	my $path = shift(@_);
	
	($base_name, $curr_path, $file_suffix) = fileparse($path, qr/\.[^.]*/);
	( $volume, $directories, $file ) = File::Spec->splitpath( $path );
	
	# Skip this file if it belongs to the exclude list
	foreach $item (@exclude_files)
	{
		if ($file =~ m/^$item/g)
		{
			print "Excluded file: $file\n";
			return;
		}
	}
	
	# Skip unwanted extensions
	foreach $ext (@exclude_extensions)
	{	
		if ($file_suffix eq $ext)
		{
			print "Excluded file: $file\n";
			return;
		}
	}

	print FILE_LIST_OUT $path."\n";
	print $line;
}

# =========================================================
# @sub 	ParseDirectory
# =========================================================
sub ParseDirectory
{
	my $dir = shift(@_);
	
	print "Entering: $dir\n";
	
	# Skip this directory if it belongs to the exclude list
	foreach $item (@exclude_dir)
	{
		my $excluded = $root_path."/".$item;
		if ($dir eq $excluded)
		{
			print "Excluded directory: $excluded\n";
			return;
		}
	}
	
	# We open the directory
	opendir(DIR, $dir) or die "Open dir failed: $dir\n";
	
	# We get the directory contents
	my @files = readdir(DIR);
	closedir(DIR);

	# For each files, we retrieve the status (directory -d or file -f)
	foreach my $file (@files)
	{
		next if $file =~ /^\.\.?$/;
		
		# Get the full path of the element
		my $fullpath = $dir."/".$file;

		if (-d $fullpath)
		{
			print "Dir found: $file\n";
			# Recursive walk!
			ParseDirectory($fullpath);
		}
		else
		{
			print "File found: $file\n";
			PrintFile($fullpath);
		}
	}
}

# End of script
