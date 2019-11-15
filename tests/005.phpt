--TEST--
EasyAop::add_advice test : add multi times
--SKIPIF--
<?php
if (!extension_loaded('easy_aop')) {
    echo 'skip';
}
?>
--FILE--
<?php
EasyAop::add_advice([
    'after@Test::f',
], function($joinpoint, $args, $ret) {
    echo "after1@Test::f called" . PHP_EOL;
});

EasyAop::add_advice([
    'after@Test::f',
], function($joinpoint, $args, $ret) {
    echo "after2@Test::f called" . PHP_EOL;
});

class Test
{
    public function f()
    {
        echo 'Test::f called' . PHP_EOL;
    }
}

$t = new Test();
$t->f();
?>
--EXPECT--
Test::f called
after1@Test::f called
after2@Test::f called