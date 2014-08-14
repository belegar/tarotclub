#!/usr/local/bin/perl
# =========================================================
# TarotClub coverage report generation
# =========================================================
use IO::Handle;
use IO::String;
use FileHandle();
use File::Copy qw(move);
use File::Path;
use File::Spec;
use File::Basename;
use integer;
use Cwd;
use Getopt::Long;

# =========================================================
# Definitions
# =========================================================	
my $OBJECT_DIR 	= $ARGV[0];

my $TEMP_DIR_GCOV		= ".";
my $GCOV_EXTENSION		= ".gcno";
my $GCOV_SUMMARY_EXT	= ".summary";
my $GCOV_GENERATED_EXT	= ".gcov";

my $SCRIPT_DIR	= cwd();

@excluded_files = (
"duktape",
"sqlite3"
);

# =========================================================
# Main
# =========================================================		

# FIXME : delete temp_dir

# 1. We generate all Gcov file by reading the output object dir. 
#	 	-> Read all generate files. 
#		-> We execute gcov and save new file into TempDirGcov.
print "Get all files under test ...\n";
my @Files 		= GetFilesList ($OBJECT_DIR, $GCOV_EXTENSION);

print "Run gcov on files ...\n";
foreach my $File  (@Files) 
{
 #   RunGcov($File);
}

# Then open the summary file and print a general summary
my @Files 		= GetFilesList (".", $GCOV_SUMMARY_EXT);
foreach my $File  (@Files) 
{
    ParseSummary($File);
    unlink($File);
}

# Then move all the gcov files into a dedicated directory
my @Files 		= GetFilesList (".", $GCOV_GENERATED_EXT);
system("mkdir -p gen");
foreach my $File  (@Files) 
{
    move($File, "gen/$File");
}

# =========================================================
# @sub 	RunGcov
# =========================================================
sub RunGcov
{
    my $GcovFile  = shift(@_);
    
    my ($filename, $path, $suffix) = fileparse($GcovFile, $GCOV_EXTENSION);
    
    # Skip this file if it belongs to the exclude list
	foreach $item (@excluded_files)
	{
		if ($filename =~ m/^$item/g)
		{
			print "Excluded file: $filename\n";
			return;
		}
	}
	system ("gcov -r -o $OBJECT_DIR $filename > $filename$GCOV_SUMMARY_EXT");
}

# =========================================================
# @sub 	ParseSummary
#
# Format of a summary file:
#
# File '../src/library/ByteStreamReader.cpp'
# Lines executed:69.39% of 49
# =========================================================
sub ParseSummary
{
    my $filename  = shift(@_);
    
    open SUMMARY_FILE_IN, $filename or die $!;
    while(<SUMMARY_FILE_IN>)
    { 
        my $line = $_;
        if (($sourceFile) = $line =~ m/File '(\s+)'/g)
        {
            print $sourceFile;
        }
    }
    close(SUMMARY_FILE_IN);
}


# =========================================================
# @sub 	GetFilesList
# This function gets all files of one directory
# recursively with the extension provided in parameter
#
# @param	$_[0] => The path of directory where we want to find the files
# @param	$_[1] => The extension of files to find
# @return	Array of files with path
# =========================================================
sub GetFilesList
{
	my $Path 		= $_[0];
	my $Extension 	= $_[1];
	my $FileFound;
	my @FilesList=();

	# Get all elements of directory
	opendir (my $FhRep, $Path) or die "Cannot open directory $Path\n";
	my @Contenu = grep { !/^\.\.?$/ } readdir($FhRep);
	closedir ($FhRep);

	# Loop to the elements found
	foreach my $FileFound (@Contenu) 
	{
		# File => Just keep $suffix file extension.
		if (-f "$Path/$FileFound") 
		{
			my ($filename, $directories, $suffix) = fileparse("$Path/$FileFound", $Extension);
			if ($suffix eq $Extension)
			{
				push (@FilesList, "$Path/$FileFound");
			}
		}	
		
		# Directory => Call this function recursively
		elsif (-d "$Path/$FileFound") 
		{
			push (@FilesList, GetFilesList("$Path/$FileFound", $Extension));
		}
	}
	
	# Return all Files found. 
	return @FilesList;
}
