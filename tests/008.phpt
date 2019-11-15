--TEST--
PHPAOP::add_advice test : returning reference
--SKIPIF--
<?php
if (!extension_loaded('phpaop')) {
    echo 'skip';
}
?>
--FILE--
<?php
function &test() {
    global $a;
    return $a;
}

PHPAOP::add_advice(['after@test'], function($joinpoint, $args, &$ret) {
    $ret++;
});

$a = 1;
$b = &test();
$b++;
var_dump($a);
?>
--EXPECT--
int(3)