--TEST--
PHPAOP::add_advice test : arguments passing by reference
--SKIPIF--
<?php
if (!extension_loaded('phpaop')) {
    echo 'skip';
}
?>
--FILE--
<?php
function test(&$a) {
    $a++;
}

PHPAOP::add_advice([
    'after@test',
    'before@test',
], function($joinpoint, $args, $ret) {
    $args['a']++;
});

$a = 1;
test($a);
var_dump($a);
?>
--EXPECT--
int(4)