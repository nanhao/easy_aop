# easy_aop
Easy_aop is a simple php7 extension for AOP (Aspect Oriented Programming), which allow you to attach a piece of code before/after a method or function in the easiest way.

## Document  

[What is AOP?](#what-is-aop)  
[Installation](#installation)  
[Usage](#usage)  
[Two types of advice](#two-types-of-advice)  
[Before-advice](#before-advice)  
[After-advice](#after-advice)  
[When to call EasyAop::add_advice](#when-to-call-easyaopadd_advice)  
[The execution of an advice may trigger another advice](#the-execution-of-an-advice-may-trigger-another-advice)  
[Advice recursion](#advice-recursion)  
[Namespace](#namespace)  
[Arguments passing by reference](#arguments-passing-by-reference)  
[Returning reference](#returning-reference)  
[Exception](#exception)  

## What is AOP?
Let's assume the following class:
```php
class MyClass
{
    public function method1()
    {
        log(); // write some log here
        
        // main logic of method1
        // ...
    }
    
    public function method2()
    {
        log(); // write some log here
        
        // main logic of method2
        // ...
    }
}
```
We see log() appears both at the start of method1() and method2(). They are necessary while they are not part of main logic of the methods.  
In fact, log() may appear repeatedly in many other methods across your system. These log() form an aspect of the system. With AOP, we have a better way to organize them:
```php
class MyClass
{
    public function method1()
    {       
        // main logic of method1
        // ...
    }
    
    public function method2()
    {   
        // main logic of method2
        // ...
    }
}

EasyAop::add_advice([
    'before@MyClass::method1',
    'before@MyClass::method2',
], function() {
    log();
});
```
With codes above, we put the log aspect in a single place, writing log() only once. EasyAop::add_advice() will magically attach it to the beginning of method1 and method2.  
By this way, we gain at least two advantages:
- We extract the aspect, making it easier to maintain.
- We keep the main logic of methods clean, which also makes them easier to maintain.

Logging is only one typical aspect. Other common aspects include access control, statistics and so on.  

## Installation

```bash
git clone https://github.com/nanhao/easy_aop.git
cd easy_aop
phpize
./configure
make
make test
make install
```

Add the following lines to your php.ini
```bash
[easy_aop]
extension=easy_aop.so
```

## Usage
```php
EasyAop::add_advice([
    'before@class_name::method_name',
    'after@class_name::method_name',
    'before@function_name',
], function($joinpoint, $args, $ret) {
    // todo
});
```

## Two types of advice
There are before-advices and after-advices:
```php
before@class_name::method_name
after@class_name::method_name
```  
Before-advices are attached to the beginning of the target code, while after-advices are attached to the end of the target code.

## Before-advice
Before-advice is executed **after** the caller calls the callee and **before** the callee receives the arguments, which means:
```php
function sum($a, $b = 10) {
    return $a + $b;
}

EasyAop::add_advice(['before@sum'], function($joinpoint, $args, $ret) {
    var_dump($joinpoint);
    var_dump($args);
    var_dump($ret);
});

sum(1);
```
The above code will output:
```text
string(8) "before@sum"
array(2) {
  ["a"]=>
  int(1)
}
NULL
```
- Because the default value of $b is set when the callee receives the arguments, we can't find $b in the $args array. In other words, $args represents arguments actually passed by the caller instead of that recieved by the callee.
- $ret is NULL due to obvious reasons.

## After-advice
After-advice is executed **after** the return statement of the callee, So you can get the real return value by reading $ret. However, there is a particular scenario under which $ret is set to NULL even when the real return value seems not NULL:
```php
function sum($a, $b) {
    return $a + $b;
}

EasyAop::add_advice(['after@sum'], function($joinpoint, $args, $ret) {
    var_dump($ret);
});

sum(1, 2);
```
The above code prints NULL instead of 3. The reason is:
- The return value of sum(1, 2) is not assigned to other variables. It's unused, so the underlying engine of php discards the return value on optimization purpose.

## When to call EasyAop::add_advice?
- EasyAop::add_advice **can** be called before the target code's definition:
```php
// ok
EasyAop::add_advice(['after@sum'], function($joinpoint, $args, $ret) {
    var_dump($ret);
});

function sum($a, $b) {
    return $a + $b;
}

sum(1, 2);
```
- EasyAop::add_advice **should** be called before the target code's execution:
```php
// bad. advice will not run
function sum($a, $b) {
    return $a + $b;
}

sum(1, 2);

EasyAop::add_advice(['after@sum'], function($joinpoint, $args, $ret) {
    var_dump($ret);
});
``` 

## The execution of an advice may trigger another advice. 
Consider the following code:
```php
EasyAop::add_advice(['after@sum'], function($joinpoint, $args, $ret) {
    echo "after@sum called";
    div(10, 2);
});

EasyAop::add_advice(['after@div'], function($joinpoint, $args, $ret) {
    echo "after@div called";
});

function sum($a, $b) {
    echo "sum called";
}

function div($a, $b) {
    echo "div called";
}

sum(1, 2);
```
Output:
```text
sum called
after@sum called
div called
after@div called
```

## Advice recursion
Advice recursion is not allowed. Consider the following code:
```php
EasyAop::add_advice(['after@sum'], function($joinpoint, $args, $ret) {
    sum(3, 4);
});

function sum($a, $b) {
    return $a + $b;
}

sum(1, 2);
```
If you run the above script, it will cause a fetal error:
```text
Fatal error: advice recursion detected: after@sum
```

## Namespace
If the target code belongs to a namespace, you need to specify the qualified name of the target code relative to the global namespace:
```php
namespace A {
    function sum($a, $b) {
        return $a + $b;
    }
}

namespace B {
    \EasyAop::add_advice(['after@A\sum'], function($joinpoint, $args, $ret) {
        echo "after@A\sum called" . PHP_EOL;
    });
    
    \A\sum(1, 2);
}
```
Output:
```text
after@A\sum called
```

## Arguments passing by reference
```php
function test(&$a) {
    $a++;
}

EasyAop::add_advice(['after@test'], function($joinpoint, $args, $ret) {
    $args['a']++;
});

$a = 1;
test($a);
var_dump($a);
```
Output:
```text
int(3)
```

## Returning reference
```php
function &test() {
    global $a;
    return $a;
}

EasyAop::add_advice(['after@test'], function($joinpoint, $args, &$ret) {
    $ret++;
});

$a = 1;
$b = &test();
$b++;
var_dump($a);
```
Output:
```text
int(3)
```

## Exception
If any exception is thrown in before-advices, a try statement starts at the first line of the target code will catch the exception:
```php
function test() {
    try {
        return 123;
    }
    catch (\Exception $e) {
        echo $e->getMessage() . PHP_EOL;
    }
}

EasyAop::add_advice(['before@test'], function($joinpoint, $args, &$ret) {
    throw new \Exception('exception thrown in before-advice');
});

test();
```
Output:
```text
exception thrown in before-advice
```
However, if you change 'before@test' to 'after@test', the exception won't be catched.
Because, before-advices are considered to be called from inside target code, while after-advices are considered to be called from the outside scope.
