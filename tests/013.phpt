--TEST--
EasyAop::intercept test
--SKIPIF--
<?php
if (!extension_loaded('easy_aop')) {
    echo 'skip';
}
?>
--FILE--
<?php
function test() {
    echo 'test called' . PHP_EOL;
}

EasyAop::add_advice([
    'before@test',
], function($joinpoint, $args, $ret) {
    echo 'advice called' . PHP_EOL;
    EasyAop::intercept();
});

test();
?>
--EXPECT--
advice called