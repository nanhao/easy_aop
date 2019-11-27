--TEST--
EasyAop::add_advice test : exception1
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
    'before@test',
], function($joinpoint, $args, $ret) {
    throw new \Exception("exception thrown in before-advice");
});

test();
?>
--EXPECT--
exception thrown in before-advice