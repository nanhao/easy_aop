--TEST--
Check if easy_aop is loaded
--SKIPIF--
<?php
if (!extension_loaded('easy_aop')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "easy_aop" is available';
?>
--EXPECT--
The extension "easy_aop" is available
