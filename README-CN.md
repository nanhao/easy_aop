[English](./README.md) | 中文

# easy_aop
Easy_aop是一个PHP7的AOP（面向切面编程）扩展。它使你可以用最便捷的方式在任意一个函数/方法的开头或结尾动态地添加代码。同时它也支持对目标代码的拦截。

## 文档  

[什么是AOP？](#什么是AOP)  
[安装](#安装)  
[使用方法](#使用方法)  
[两种Advice](#两种Advice)  
[Before-advice](#before-advice)  
[After-advice](#after-advice)  
[何时调用EasyAop::add_advice](#何时调用easyaopadd_advice)  
[Advice的执行可能触发另一个Advice](#Advice的执行可能触发另一个Advice)  
[Advice递归](#Advice递归)  
[命名空间](#命名空间)  
[引用参数](#引用参数)  
[返回引用](#返回引用)  
[异常](#异常)  
[EasyAop::intercept](#easyaopintercept)

## 什么是AOP?
假设有下面这个类：
```php
class MyClass
{
    public function method1()
    {
        log(); // 写日志
        
        // method1的主要逻辑
        // ...
    }
    
    public function method2()
    {
        log(); // 写日志
        
        // method2的主要逻辑
        // ...
    }
}
```
我们看到 log() 重复出现在 method1() 和 method2() 的开头。它们是必须的，但并属于方法的主要逻辑。事实上，类似这种代码可能散布在你系统当中的各个地方。所有这些 log() 形成了一个系统切面。在AOP的支持下，我么可以用一种更好的方式来写代码：
```php
class MyClass
{
    public function method1()
    {       
        // method1 的主要逻辑
        // ...
    }
    
    public function method2()
    {   
        // method2 的主要逻辑
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
在上面的代码中，我们把这个切面单独提取了出来，log()只需要写一次。EasyAop::add_advice()会自动把它添加到 method1 和 method2 的开头。  
用这种方式，我们至少获得了两种好处：
- 提取出了切面，这使得这个切面更容易维护
- 使各个方法的主逻辑保持干净

日志只是典型切面中的一个。其他常见的切面包括访问控制，统计等。  
这种被动态添加的代码称为“Advice”。

## 安装

```bash
git clone https://github.com/nanhao/easy_aop.git
cd easy_aop
phpize
./configure
make
make test
make install
```

在php.ini中添加
```bash
[easy_aop]
extension=easy_aop.so
```

## 使用方法
```php
EasyAop::add_advice([
    'before@class_name::method_name',
    'after@class_name::method_name',
    'before@function_name',
], function($joinpoint, $args, $ret) {
    // todo
});
```

## 两种Advice
有两种Advice: before-advice 和 after-advice:
```php
before@class_name::method_name
after@class_name::method_name
```  
Before-advice被添加在目标代码的开头，after-advice被添加在目标代码的末尾。

## Before-advice
Before-advice 是在调用方向被调用的函数传递了参数之后，但被调用的函数接收到参数之前执行的：  
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
输出:
```text
string(8) "before@sum"
array(2) {
  ["a"]=>
  int(1)
}
NULL
```
- 因为 $b 的默认值是在 sum 接收参数的时候被设置的，所以在 $args 中找不到 $b。换句话说，$args代表的是调用方实际传过来的参数，而不是被调用方实际接收到的参数
- $ret 为NULL，因为函数还没执行

## After-advice
After-advice 是在return执行完之后才被执行的。通过 $ret 可以获得实际返回的值。但在一种特殊情况下，$ret被设置为NULL，即使实际返回的似乎不是NULL:  
```php
function sum($a, $b) {
    return $a + $b;
}

EasyAop::add_advice(['after@sum'], function($joinpoint, $args, $ret) {
    var_dump($ret);
});

sum(1, 2);
```
上面的代码输出NULL而不是3。原因是：  
- sum(1, 2)的返回值没有被使用，PHP内核出于优化的目的把它丢弃了

## 何时调用EasyAop::add_advice?
- EasyAop::add_advice 可以在目标代码定义之前调用：  
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
- EasyAop::add_advice 应该在目标代码执行之前调用：  
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

## Advice的执行可能触发另一个Advice 
考虑下面代码:
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
输出:
```text
sum called
after@sum called
div called
after@div called
```

## Advice递归
Advice递归是不允许的：
```php
EasyAop::add_advice(['after@sum'], function($joinpoint, $args, $ret) {
    sum(3, 4);
});

function sum($a, $b) {
    return $a + $b;
}

sum(1, 2);
```
运行上面代码会导致抛出一个错误:
```text
Fatal error: advice recursion detected: after@sum
```

## 命名空间
如果目标代码属于某个命名空间下，需要指定相对于全局命名空间的名称：  
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
输出:
```text
after@A\sum called
```

## 引用参数
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

## 返回引用
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

## 异常
如果在 before-advice 中抛出异常，在目标代码第一行的 try 语句会捕获到这个异常：
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
但是，上面一样的代码无法捕获到 after-advice 中跑出的异常。  
原因是，before-advice被视为是目标代码的一部分，而after-advice是属于外层的。


## EasyAop::intercept
你可以通过在 before-advice 中调用 EasyAop::intercept() 来拦截目标代码，拦截之后， before-advice 会取代目标代码：
```php
function sum($a, $b) {
    return $a + $b;
}

EasyAop::add_advice(['before@sum'], function($joinpoint, $args, $ret) {
    EasyAop::intercept();
    return $args['a'] * $args['b'];
});

$ret = sum(5, 5);
echo $ret;
```
Output:
```text
25
```