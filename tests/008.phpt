--TEST--
EasyAop::add_advice test : returning reference
--SKIPIF--
<?php
if (!extension_loaded('easy_aop')) {
    echo 'skip';
}
?>
--FILE--
<?php
function &test() {
    global $a;
    return $a;
}

EasyAop::add_advice(['after@test'], function($joinpoint, $args, &$ret) {
    $ret++;
});

$a = 1;
$b = &test();
$b++;
var_dump($a);
?>
--EXPECT--
int(3)