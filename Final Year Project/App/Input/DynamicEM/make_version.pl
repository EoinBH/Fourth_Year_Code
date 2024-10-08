#!/usr/bin/perl

print "\#include \"Version.h\"";
print "\n";
print "\n";

while(<>) {
    $line = $_;
    chop $line;
    if($line =~ /^commit/) {    
	$line =~ s/^commit //; 
	print "string Version::git_commit_id = \"$line\";\n";
    }
    elsif($line =~ /^Author/) {  
	$line =~ s/^Author: //;
	print "string Version::git_commit_person = \"$line\";\n";
    }
    elsif($line =~ /^Date/) {    
	$line =~ s/^Date:   //;
	print "string Version::git_commit_date = \"$line\";\n";
    }

}
