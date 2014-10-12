#!/usr/local/bin/perl
# =========================================================
# TarotClub mini SVG deck generator
# Perl Packages used:
#   XML-LibXML
# =========================================================
use strict;
use IO::Handle;
use IO::String;
use FileHandle();
use File::Copy qw(move);
use File::Path;
use File::Spec;
use File::Basename;
use integer;
use Cwd;
use XML::LibXML;

# =========================================================
# Definitions
# =========================================================	
my @Templates	= ( "spade", "heart", "diamond", "club", "trump", "card");
my @cardTags	= ('S', 'H', 'D', 'C', 'T');
my @suitIcon	= ("#spade", "#heart", "#diamond", "#club", "#trump");
my @scales		= ("0.15", "0.15", "0.15", "0.15", "0.25");

# =========================================================
# Main
# =========================================================
my $parser		= XML::LibXML->new(); # for input SVG files

# -------------- Create SVG root node
my $minideck = XML::LibXML::Document->createDocument( "1.0", "UTF-8" );
my $svg = $minideck->createElement("svg");
$svg->setAttribute( "xmlns", "http://www.w3.org/2000/svg" );
$svg->setAttribute( "xmlns:xlink", "http://www.w3.org/1999/xlink" );
$svg->setAttribute( "version", "1.1" );
$svg->setAttribute( "width", "920" );
$svg->setAttribute( "height", "2100" );
$svg->setAttribute( "id", "svg" );

# -------------- Add some root types


# -------------- Add SVG icons templates
my $defsNode = $minideck->createElement("defs");
print "Create icons templates in definition area ...\n";
foreach my $Icon  (@Templates) 
{
	my $comment = " ============================= $Icon SVG file ============================= ";
	my $commentNode = $minideck->createComment( $comment );
	$defsNode->addChild($commentNode);

	my $file = "../icons/$Icon.svg";
	print "Reading $file \n";
	my $tree = $parser->parse_file($file);
	# Root of the XML
	my $root = $tree->getDocumentElement;
	
	# Get the defs part
	my @defs = $root->getElementsByTagName('defs');
	foreach my $node (@defs)
	{
		$defsNode->addChild($node->cloneNode(1));
	}
	
	# Get the main g part
	my @iconNode = $tree->findnodes("//*[\@id=\"$Icon\"]");
	foreach my $node (@iconNode)
	{
		print "found a node\n";
		$defsNode->addChild($node->cloneNode(1));
	}
}
$svg->addChild($defsNode);

# -------------- Create the Tarot deck cards using templates
BuildTarotDeck();
   

# -------------- Finally generate the deck
print "Generate deck ...\n";

$minideck->addChild($svg);
$minideck->toFile("minideck.svg", 1);

# =========================================================
# BuildTarotDeck
# =========================================================	
# Format:
# <g id="c01-S" transform="matrix(1,0,0,1,0,0)">
#       <use xlink:href="#card"></use>
#       <use xlink:href="#spade" transform="matrix(0.15,0,0,0.15,4.5,34.5)"></use>
#       <text x="5" y="30" class="valueText">1</text>
# </g>

sub BuildTarotDeck
{
	# The 4 suits
    for (my $i = 0; $i < 4; $i++)
    {
        # From ace to the king, 14 cards
        for (my $j = 1; $j <= 14; $j++)
        {
			BuildCard($i, $j);
        }
    }

    # The 22 trumps, including the fool
    for (my $i = 0; $i <= 21; $i++)
    {
        BuildCard(4, $i);
    }
}

# First arg is the card suit [0..4]
# Second arg is the card facial value 
sub BuildCard()
{
	my $cardName = sprintf("%02d", $_[1])."-$cardTags[$_[0]]";
	
	# Root node
	my $cardNode = $minideck->createElement("g");
	$cardNode->setAttribute( "id", "c$cardName"); # we add a 'c' letter because beginning with a digit seems not standard
	
	my $background = $minideck->createElement("use");
	$background->setAttribute( "xlink:href", "#card" );
	
	my $icon =  $minideck->createElement("use");
	$icon->setAttribute( "xlink:href", $suitIcon[$_[0]] );
	$icon->setAttribute( "transform", "scale($scales[$_[0]]) translate(30, 230)" );

	my $value =  $minideck->createElement("text");
	$value->setAttribute( "x", "5");
	$value->setAttribute( "y", "30");
	$value->setAttribute( "style", "font-size: 25; font-family: 'kanzlei';");
	$value->appendText("$_[1]");
		
	$cardNode->addChild($background);
	$cardNode->addChild($icon);
	$cardNode->addChild($value);
	$svg->addChild($cardNode);
}

# End of file
