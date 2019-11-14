/* phpaop extension for PHP */

#ifndef PHP_PHPAOP_H
#define PHP_PHPAOP_H

extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
}

extern zend_module_entry phpaop_module_entry;
# define phpext_phpaop_ptr &phpaop_module_entry

# define PHP_PHPAOP_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_PHPAOP)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#define PHPAOP_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(phpaop, v)

ZEND_BEGIN_MODULE_GLOBALS(phpaop)
    void* p_manager; // PHPAOP::AopManager*
ZEND_END_MODULE_GLOBALS(phpaop)

extern ZEND_DECLARE_MODULE_GLOBALS(phpaop)

#endif	/* PHP_PHPAOP_H */

