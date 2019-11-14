--TEST--
PHPAOP::add_advice Basic test : before advice
--SKIPIF--
<?php
if (!extension_loaded('phpaop')) {
	echo 'skip';
}
?>
--FILE--
<?php
PHPAOP::add_advice([
    'before@Test::f',
], function($joinpoint, $args, $ret) {
    echo "before@Test::f called" . PHP_EOL;
    var_dump($joinpoint);
    var_dump($args);
    var_dump($ret);
});

class Test
{
    public function f($a, $b)
    {
        echo 'f called' . PHP_EOL;
        return $a + $b;
    }
}

$t = new Test();
$r = $t->f(1, 2);
?>
--EXPECT--
before@Test::f called
string(14) "before@Test::f"
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
NULL
f called
