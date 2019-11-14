--TEST--
Check if phpaop is loaded
--SKIPIF--
<?php
if (!extension_loaded('phpaop')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "phpaop" is available';
?>
--EXPECT--
The extension "phpaop" is available
