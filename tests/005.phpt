--TEST--
PHPAOP::add_advice test : add multi times
--SKIPIF--
<?php
if (!extension_loaded('phpaop')) {
    echo 'skip';
}
?>
--FILE--
<?php
PHPAOP::add_advice([
    'after@Test::f',
], function($joinpoint, $args, $ret) {
    echo "after1@Test::f called" . PHP_EOL;
});

PHPAOP::add_advice([
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