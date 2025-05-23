--TEST--
str_pad() function
--INI--
precision=14
--FILE--
<?php
/* Pad a string to a certain length with another string */

echo "\n#### Basic operations ####\n";
$input_string = "str_pad()";
$pad_length = 20;
$pad_string = "-+";
var_dump( str_pad($input_string, $pad_length) ); // default pad_string & pad_type
var_dump( str_pad($input_string, $pad_length, $pad_string) ); // default pad_type
var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_LEFT) ); 
var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_RIGHT) );
var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_BOTH) );

echo "\n#### variations with input string and pad-length ####\n";
/* different input string variation */
$input_strings = array( 
		   "variation", // normal string
                   chr(0).chr(255).chr(128).chr(234).chr(143), 	// >7-bit ASCII
		   "", 	// empty string
                   NULL,  // NULL
                   true,  // boolean 
                   15,  // numeric
                   15.55,  // numeric
                   "2990"  // numeric string
                 );
/* different pad_lengths */
$pad_lengths = array(
		 -PHP_INT_MAX,  // huge negative value
		 -1,  // negative value
                 0,  // pad_length < sizeof(input_string)
                 9,  // pad_length <= sizeof(input_string)
                 10,  // pad_length > sizeof(input_string) 
                 16,  // pad_length > sizeof(input_string)
               );
$pad_string = "=";
/*loop through to use each variant of $pad_length on
  each element of $input_strings array */
foreach ($input_strings as $input_string ) {
  foreach ($pad_lengths as $pad_length ) {
    var_dump( str_pad($input_string, $pad_length) ); // default pad_string & pad_type
    var_dump( str_pad($input_string, $pad_length, $pad_string) ); // default pad_type
    var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_LEFT) ); 
    var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_RIGHT) );
    var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_BOTH) );
  }
}

echo "\n#### variation with pad string ####\n";
$pad_strings = array ("=", 1, true, "string_pad", 1.5, "\t", '\t');
$input_string="variation";
$pad_length = 16;
foreach ( $pad_strings as $pad_string ) {
    var_dump( str_pad($input_string, $pad_length, $pad_string) ); // default pad_type
    var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_LEFT) ); 
    var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_RIGHT) );
    var_dump( str_pad($input_string, $pad_length, $pad_string, STR_PAD_BOTH) );
}

echo "\n#### error conditions ####";
/* args less than min. expected of 2 */
str_pad();
str_pad($input_string);

/* args more than expected,expected 4 */
str_pad($input_tring, $pad_length, $pad_string, STR_PAD_LEFT, NULL );

echo "\n--- padding string as null ---";
str_pad($input_string, 12, NULL);
str_pad($input_string, 12, "");

/* bad pad_type - passing an undefined one */
var_dump( str_pad($input_string, $pad_length, "+", 15) );

echo "Done\n";
?>
--EXPECTF--
#### Basic operations ####
string(20) "str_pad()           "
string(20) "str_pad()-+-+-+-+-+-"
string(20) "-+-+-+-+-+-str_pad()"
string(20) "str_pad()-+-+-+-+-+-"
string(20) "-+-+-str_pad()-+-+-+"

#### variations with input string and pad-length ####
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(9) "variation"
string(10) "variation "
string(10) "variation="
string(10) "=variation"
string(10) "variation="
string(10) "variation="
string(16) "variation       "
string(16) "variation======="
string(16) "=======variation"
string(16) "variation======="
string(16) "===variation===="
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(5) " ���"
string(9) " ���    "
string(9) " ���===="
string(9) "==== ���"
string(9) " ���===="
string(9) "== ���=="
string(10) " ���     "
string(10) " ���====="
string(10) "===== ���"
string(10) " ���====="
string(10) "== ���==="
string(16) " ���           "
string(16) " ���==========="
string(16) "=========== ���"
string(16) " ���==========="
string(16) "===== ���======"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(9) "         "
string(9) "========="
string(9) "========="
string(9) "========="
string(9) "========="
string(10) "          "
string(10) "=========="
string(10) "=========="
string(10) "=========="
string(10) "=========="
string(16) "                "
string(16) "================"
string(16) "================"
string(16) "================"
string(16) "================"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(9) "         "
string(9) "========="
string(9) "========="
string(9) "========="
string(9) "========="
string(10) "          "
string(10) "=========="
string(10) "=========="
string(10) "=========="
string(10) "=========="
string(16) "                "
string(16) "================"
string(16) "================"
string(16) "================"
string(16) "================"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(9) "1        "
string(9) "1========"
string(9) "========1"
string(9) "1========"
string(9) "====1===="
string(10) "1         "
string(10) "1========="
string(10) "=========1"
string(10) "1========="
string(10) "====1====="
string(16) "1               "
string(16) "1==============="
string(16) "===============1"
string(16) "1==============="
string(16) "=======1========"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(9) "15       "
string(9) "15======="
string(9) "=======15"
string(9) "15======="
string(9) "===15===="
string(10) "15        "
string(10) "15========"
string(10) "========15"
string(10) "15========"
string(10) "====15===="
string(16) "15              "
string(16) "15=============="
string(16) "==============15"
string(16) "15=============="
string(16) "=======15======="
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(5) "15.55"
string(9) "15.55    "
string(9) "15.55===="
string(9) "====15.55"
string(9) "15.55===="
string(9) "==15.55=="
string(10) "15.55     "
string(10) "15.55====="
string(10) "=====15.55"
string(10) "15.55====="
string(10) "==15.55==="
string(16) "15.55           "
string(16) "15.55==========="
string(16) "===========15.55"
string(16) "15.55==========="
string(16) "=====15.55======"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(4) "2990"
string(9) "2990     "
string(9) "2990====="
string(9) "=====2990"
string(9) "2990====="
string(9) "==2990==="
string(10) "2990      "
string(10) "2990======"
string(10) "======2990"
string(10) "2990======"
string(10) "===2990==="
string(16) "2990            "
string(16) "2990============"
string(16) "============2990"
string(16) "2990============"
string(16) "======2990======"

#### variation with pad string ####
string(16) "variation======="
string(16) "=======variation"
string(16) "variation======="
string(16) "===variation===="
string(16) "variation1111111"
string(16) "1111111variation"
string(16) "variation1111111"
string(16) "111variation1111"
string(16) "variation1111111"
string(16) "1111111variation"
string(16) "variation1111111"
string(16) "111variation1111"
string(16) "variationstring_"
string(16) "string_variation"
string(16) "variationstring_"
string(16) "strvariationstri"
string(16) "variation1.51.51"
string(16) "1.51.51variation"
string(16) "variation1.51.51"
string(16) "1.5variation1.51"
string(16) "variation							"
string(16) "							variation"
string(16) "variation							"
string(16) "			variation				"
string(16) "variation\t\t\t\"
string(16) "\t\t\t\variation"
string(16) "variation\t\t\t\"
string(16) "\t\variation\t\t"

#### error conditions ####
Warning: str_pad() expects at least 2 parameters, 0 given in %s on line %d

Warning: str_pad() expects at least 2 parameters, 1 given in %s on line %d

Notice: Undefined variable: input_tring in %s on line %d

Warning: str_pad() expects at most 4 parameters, 5 given in %s on line %d

--- padding string as null ---
Warning: str_pad(): Padding string cannot be empty in %s on line %d

Warning: str_pad(): Padding string cannot be empty in %s on line %d

Warning: str_pad(): Padding type has to be STR_PAD_LEFT, STR_PAD_RIGHT, or STR_PAD_BOTH in %s on line %d
NULL
Done
