# phpaop
A php7 extension for AOP

## Installation

```bash
git clone https://github.com/nanhao/phpaop.git
cd phpaop
phpize
./configure
make
make test
make install
```

Add the following lines to your php.ini
```bash
[phpaop]
extension=phpaop.so
```

## Usage
```php
PHPAOP::add_advice([
    'before@class_name::method_name',
    'after@class_name::method_name',
    'before@function_name',
], function($joinpoint, $args, $ret) {
    // todo
});
```
