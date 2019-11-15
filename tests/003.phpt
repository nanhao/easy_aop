--TEST--
EasyAop::add_advice Basic test : after advice
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
    echo "after@Test::f called" . PHP_EOL;
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
f called
after@Test::f called
string(13) "after@Test::f"
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
int(3)
