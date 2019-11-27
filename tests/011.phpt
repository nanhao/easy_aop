--TEST--
EasyAop::add_advice test : exception3
--SKIPIF--
<?php
if (!extension_loaded('easy_aop')) {
    echo 'skip';
}
?>
--FILE--
<?php
function test() {
    echo 'test' . PHP_EOL;
}

EasyAop::add_advice([
    'before@test',
    'after@test',
], function($joinpoint, $args, $ret) {
    throw new \Exception("exception thrown");
});

try {
    test();
}
catch (\Exception $e) {
    echo 'caught outside' . PHP_EOL;
}
?>
--EXPECT--
caught outside