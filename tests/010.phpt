--TEST--
EasyAop::add_advice test : exception2
--SKIPIF--
<?php
if (!extension_loaded('easy_aop')) {
    echo 'skip';
}
?>
--FILE--
<?php
function test() {
    try {
        return 123;
    }
    catch (\Exception $e) {
        echo $e->getMessage() . PHP_EOL;
    }
}

EasyAop::add_advice([
    'after@test',
], function($joinpoint, $args, $ret) {
    throw new \Exception("exception thrown in before-advice");
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