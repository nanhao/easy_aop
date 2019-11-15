--TEST--
EasyAop::add_advice test : advice in advice
--SKIPIF--
<?php
if (!extension_loaded('easy_aop')) {
    echo 'skip';
}
?>
--FILE--
<?php
EasyAop::add_advice([
    'after@Test1::f',
], function($joinpoint, $args, $ret) {
    echo "after@Test1::f called" . PHP_EOL;
    $t = new Test2();
    $t->f();
});

EasyAop::add_advice([
    'after@Test2::f',
], function($joinpoint, $args, $ret) {
    echo "after@Test2::f called" . PHP_EOL;
});

class Test1
{
    public function f()
    {
        echo 'Test1::f called' . PHP_EOL;
    }
}

$t = new Test1();
$t->f();

class Test2
{
    public function f()
    {
        echo 'Test2::f called' . PHP_EOL;
    }
}
?>
--EXPECT--
Test1::f called
after@Test1::f called
Test2::f called
after@Test2::f called