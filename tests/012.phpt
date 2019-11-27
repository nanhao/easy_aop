--TEST--
EasyAop::add_advice test : exception4
--SKIPIF--
<?php
if (!extension_loaded('easy_aop')) {
    echo 'skip';
}
?>
--FILE--
<?php
function test() {
    throw new \Exception("exception thrown");
}

EasyAop::add_advice([
    'before@test',
    'after@test',
], function($joinpoint, $args, $ret) {
    echo 'advice called' . PHP_EOL;
});

try {
    test();
}
catch (\Exception $e) {
    echo 'caught outside' . PHP_EOL;
}
?>
--EXPECT--
advice called
caught outside