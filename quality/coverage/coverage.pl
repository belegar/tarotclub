#!/usr/local/bin/perl
# =========================================================
# TarotClub coverage report generation
# Perl Packages used:
#   Text-TabularDisplay
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
use Text::TabularDisplay;

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
"sqlite3",
"tst_database",
"tst_hash",
"tst_json",
"tst_tarot_base",
"tst_tarot_rules",
"tst_utilities",
"tst_websocket",
);

my @Header  = ( "Source file", "Completion", "Total lines" );
my $table = Text::TabularDisplay->new(@Header);

# =========================================================
# Main
# =========================================================		

# gets all coverage GCC output files
my @Files 		= GetFileList ($OBJECT_DIR, $GCOV_EXTENSION);
my $totalExecutedLines = 0.0;
my $totalLines = 0.0;

print "Run gcov on files ...\n";
foreach my $File  (@Files) 
{
    RunGcov($File);
}

# Then open the summary file and print a general summary
my @Files 		= GetFileList (".", $GCOV_SUMMARY_EXT);
foreach my $File  (@Files) 
{
    ParseSummary($File);
}

unlink(@Files);

# Then move all the gcov files into a dedicated directory
my @Files 		= GetFileList (".", $GCOV_GENERATED_EXT);
system("mkdir -p gen");
foreach my $File  (@Files) 
{
    move($File, "gen/$File");
}

# Print result in a table
print $table->render;

# Print the general coverage percentage
my  $coverage = ($totalExecutedLines * 100.0) / $totalLines;
print "\r\nGlobal coverage: $coverage %\r\n";

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
# Creating 'Base64.cpp.gcov'
# =========================================================
sub ParseSummary
{
    open SUMMARY_FILE_IN, shift(@_) or die $!;
    while(<SUMMARY_FILE_IN>)
    { 
        my $line = $_;
        my $sourceFile;
        my $executed;
        my $lines;
        
        if (($sourceFile) = ($line =~ m/File\s+'(.+)'/g))
        {
            ($base_name, $curr_path, $file_suffix) = fileparse($sourceFile, qr/\.[^.]*/);
            if ($file_suffix eq ".cpp")
            {
                if ($curr_path =~ m/testu/g)
                {
                    # Exclude test files
                }
                else
                {
                    my $nextLine = <SUMMARY_FILE_IN>;
                    if (($executed, $lines) = ($nextLine =~ m/Lines executed:(.+) of (\d+)/g))
                    {
                        my @row = ($sourceFile, $executed, $lines);
                        $table->add(@row);
						
						# Add to global statistics
						$totalExecutedLines += $executed;
						$totalLines += $lines;
						
                        # print "$sourceFile\t\t\t|\t$toPrint$executed\t\t|\t$lines\t\t|\r\n";
                    }
                }
            }
        }
    }
    close(SUMMARY_FILE_IN);
}


# =========================================================
# @sub 	GetFileList
# This function gets all files of one directory
# recursively with the extension provided in parameter
#
# @param	$_[0] => The path of directory where we want to find the files
# @param	$_[1] => The extension of files to find
# @return	Array of files with path
# =========================================================
sub GetFileList
{
	my $Path 		= $_[0];
	my $Extension 	= $_[1];
	my $FileFound;
	my @FilesList=();

	# Get all elements of directory
	opendir (my $FhRep, $Path) or die "Cannot open directory $Path\n";
	my @Elements = grep { !/^\.\.?$/ } readdir($FhRep);
	closedir ($FhRep);

	# Loop to the elements found
	foreach my $FileFound (@Elements) 
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
			push (@FilesList, GetFileList("$Path/$FileFound", $Extension));
		}
	}
	
	# Return all Files found. 
	return @FilesList;
}
