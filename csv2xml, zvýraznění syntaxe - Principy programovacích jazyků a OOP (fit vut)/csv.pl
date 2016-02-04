#!/usr/bin/env perl

#CSV:xsevci44

# ===================================================================
# Soubor:  csv.pl 
# Datum:   2012/03/29
# Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
# Projekt: CSV2XML v Perlu, uloha c. 1 pro predmet IPP
# Popis:   Prevod dokumentu z formatu CSV do XML
# ===================================================================

# interpret config
use strict;
use warnings;
use utf8; # source code in utf8

# includes
use Getopt::Long;
use Text::CSV;
use XML::LibXML;

use locale;
use POSIX qw(locale_h);

# global vars
our $input = '';
our $output = '';
our $root = '';
our $separator = ',';
our $csv_header = 0;
our $xml_noheader = 0;
our $row_name = 'row';
our $row_index = 0;
our $row_start = 1;
our $error_recovery = 0;
our $missing_value = '';
our $all_columns = 0;
our $help = 0;

use constant HELPMSG => <<END;
CSV2XML v Perlu, uloha c. 1 pro predmet IPP
Autor: Radek Sevcik, xsevci44\@stud.fit.vutbr.cz

Pouziti: csv [-n] [-h] [-i] [-r=root-element] [-s=separator] [-l=line-element]
             [--start=n] [-e|--error-recovery] [--missing-value=val]
             [--all-columns] --input=filename --output=filename

Parametry:
    --help\t\tVypise tuto napovedu.
    --input=filename\tVstupni CSV soubor v kodovani UTF-8.
    --output=filename\tVystupni XML soubor.

Parametry XML:
    -n\t\t\tNegenerovat XML hlavicku.
    -r=root-element\tJmeno korenoveho elementu. Neni-li zadan tak vysledny
                   \tsoubor nebude korenovy element obsahovat.
    -l=line-element\tJmeno elementu obalujici zaznamy. [default: row]
    -i\t\t\tVlozi atribut index do line-element.
    --start=n\t\tCislovani zaznamu atributu index od n. [default: 1]

Parametry CSV:
    -s=separator\tPouzity oddelovac v CSV souboru.
    -h\t\t\tPrvni zaznam v CSV souboru tvori hlavicku.
    -e,--error-recovery\tZotaveni z chybneho poctu sloupcu na neprvnim radku.
                       \tChybejici sloupec bude doplnen, prebyvajici ignorovan.
    --missing-value=val\tChybejici bunka bude nahrazena hodnotou val. [default: ]
    --all-columns\tPrebyvajici sloupce nebudou ignorovany.
END

# error codes
use constant {
    ESUCCESS => 0,
    EINVALIDSEPARATOR => 1,
    EINVALIDPARAMS => 1,
    ENEGATIVESTART => 1,
    EFILEIN => 2,
    EFILEOUT => 3,
    EBADFORMAT => 4,
    EINVALIDXMLNAME => 30,
    EINVALIDXMLNAMEHEAD => 31,
    ENUMCOLUMN => 32,
    ECSV => 100,
    EXML => 101,
};

use constant {
    STR_EINVALIDSEPARATOR => 'Invalid separator',
    STR_EINVALIDPARAMS => 'Invalid combination of parameters',
    STR_ENEGATIVESTART => 'Start has to be non-negative',
    STR_EINVALIDXMLNAME => 'Invalid element name',
    STR_EINVALIDXMLNAMEHEAD => 'Invalid header/column element',
    STR_ENUMCOLUMN => 'Missing or too many columns',
};

# die w/ error code
# arg1: (int)code
# arg2: (string)msg
sub die_code($%) {
    my $errcode = shift @_;
    my $errmsg = shift @_;

    print STDERR "$errmsg\n";
    exit $errcode;
}

# xml entities escape
# arg1: (string)text
# return: (string)escaped_text
sub xml_escape_col(%) {
    my $text = shift @_;
    $text =~ s/[^\p{IsAlnum}_\.\-]/-/g;

    return $text;
}

# xml text escape
# arg1: (string)text
# return: (string)escaped_text
sub xml_escape_text(%) {
    my $text = shift @_;
    return $text;
}

# is xml element name valid
# arg1: (string)text
# return: (bool)valid
sub xml_is_valid_elemname(%) {
    my $text = shift @_;
    return $text =~ /^[\p{IsAlnum}_][\p{IsAlnum}_\.\-]*$/;
}

# set locale from environment
setlocale(LC_ALL, "");

# parsing vars
GetOptions(
    'input=s' => \$input,
    'output=s' => \$output,
    'n' => \$xml_noheader,
    'r=s' => \$root,
    's=s' => \$separator,
    'h' => \$csv_header,
    'l=s' => \$row_name,
    'i' => \$row_index,
    'start=i' => \$row_start,
    'e|error-recovery' => \$error_recovery,
    'missing-value=s' => \$missing_value,
    'all-columns' => \$all_columns,
    'help' => \$help
) or exit EINVALIDPARAMS;

# print help
if ($help) {
    print STDOUT HELPMSG;
    exit ESUCCESS;
}

# locale-strcmp won't work with utf8 encoded string
utf8::decode($root);
utf8::decode($row_name);

# validate args
if ($separator eq 'TAB') {
    $separator = "\t";
}
elsif ($separator !~ /^[\p{IsP}\p{IsZ}]$/) {
    die_code(EINVALIDSEPARATOR, STR_EINVALIDSEPARATOR);
}

if ($root && !xml_is_valid_elemname($root)) {
    die_code(EINVALIDXMLNAME, STR_EINVALIDXMLNAME . " <${root}>");
}

if (!xml_is_valid_elemname($row_name)) {
    die_code(EINVALIDXMLNAME, STR_EINVALIDXMLNAME . " <${row_name}>");
}

if ($row_start < 0) {
    die_code(1, 'start has to be non-negative');
}
elsif ($row_start == 1) {
    # implicit n=1
}
elsif (!$row_index) {
    die_code(EINVALIDPARAMS, STR_EINVALIDPARAMS);
}

if ($missing_value && !$error_recovery) {
    die_code(EINVALIDPARAMS, STR_EINVALIDPARAMS);
}

if ($all_columns && !$error_recovery) {
    die_code(EINVALIDPARAMS, STR_EINVALIDPARAMS); 
}

# open fds
my $fdin;
my $fdout;

if ($input) {
    open $fdin, "<", $input
      or die_code(EFILEIN, "$input: $!");
}
else {
    open $fdin, "<-"
      or die_code(EFILEIN, "STDIN: $!");
}

if ($output) {
    open $fdout, ">", $output
      or die_code(EFILEOUT, "$output: $!");
}
else {
    open $fdout, ">-"
      or die_code(EFILEOUT, "STDOUT: $!");
}

# create csv parser and xml writer
my $csv = Text::CSV->new({
    binary => 1,
    sep_char => $separator,
    #allow_whitespace => 1,
    #eol => "\r\n",
}) or die_code(ECSV, "Cannot use CSV: " . Text::CSV->error_diag());

my $xmlDoc = XML::LibXML::Document->new('1.0', 'utf-8');
my $xmlRoot = XML::LibXML::Element->new($root);
$xmlDoc->setDocumentElement($xmlRoot);

my $lineno = 0;
my $numcol = -1;
my @header;

while (my $row = $csv->getline($fdin)) {
    my @cols = @$row;

    if ($numcol == -1) {
        # bereme pocet sloupcu podle prvniho zaznamu
        $numcol = @cols;
        @header = @cols, next if $csv_header;
    }
    elsif ($numcol != scalar(@cols)) {
        # pocet sloupcu se lisi
        if ($error_recovery) {
            # ignorujeme nadbytecne sloupce
            while (!$all_columns && scalar(@cols) > $numcol) {
                pop(@cols);
            };
            # doplnime chybejici sloupce
            while (scalar(@cols) < $numcol) {
                push(@cols, $missing_value);
            }
        }
        else {
            my $actline = $lineno + 1;
            die_code(ENUMCOLUMN, STR_ENUMCOLUMN . ", line ${actline}");
        }
    }

    my $xmlRow = $xmlRoot->addNewChild('', $row_name);
    $xmlRow->setAttribute('index', $row_start + $lineno) if $row_index;

    for (my $i = 0; $i < scalar(@cols); ++$i) {        
        my $col = $cols[$i];
        my $col_name = $header[$i];
        $col_name = "col" . ($i + 1) if !$col_name;

        utf8::decode($col_name);
        utf8::decode($col);

        my $escaped_text = xml_escape_text($col);
        my $escaped_name = xml_escape_col($col_name);

        if (!xml_is_valid_elemname($escaped_name)) {
            close $fdout;
            close $fdin;
            die_code(EINVALIDXMLNAMEHEAD, STR_EINVALIDXMLNAMEHEAD . " <${escaped_name}>");
        }

        $xmlRow->appendTextChild($escaped_name, $escaped_text);
    }

    ++$lineno;
}

$csv->eof or $csv->error_diag();

# write xml
if ($root && !$xml_noheader) {
    $xmlDoc->toFH($fdout, 2);
}
elsif ($root) {
    print {$fdout} $xmlRoot->toString(2, 1);
}
elsif (!$xml_noheader) {
    $xmlDoc->setDocumentElement('');
    $xmlDoc->toFH($fdout, 2);
    $xmlDoc->setDocumentElement($xmlRoot);

    for my $xmlNode ($xmlRoot->childNodes()) {
        print {$fdout} $xmlNode->toString(2, 1);
        print {$fdout} "\n";
    }
}
else {
    for my $xmlNode ($xmlRoot->childNodes()) {
        print {$fdout} $xmlNode->toString(2, 1);
        print {$fdout} "\n";
    }
}

# clean up
close $fdout or die_code(EFILEOUT, "$output: $!");
close $fdin or die_code(EFILEIN, "$input: $!");

exit ESUCCESS;

