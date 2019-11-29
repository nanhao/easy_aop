/* phpaop extension for PHP */

#ifndef PHP_EASY_AOP_H
#define PHP_EASY_AOP_H

extern "C" {
    #ifdef HAVE_CONFIG_H
    # include "config.h"
    #endif

    #include "php.h"
    #include "ext/standard/info.h"
}

#include <vector>
using std::vector;

typedef struct {
    zend_execute_data* p_target;
    zend_execute_data* p_before;
    bool intercept;
} BeforeAdviceCall;

extern zend_module_entry easy_aop_module_entry;
# define phpext_easy_aop_ptr &easy_aop_module_entry

# define PHP_EASY_AOP_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_EASY_AOP)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#define EASY_AOP_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(easy_aop, v)

ZEND_BEGIN_MODULE_GLOBALS(easy_aop)
    void* p_manager; // easy_aop::AopManager*
    vector<BeforeAdviceCall> advice_trace;
ZEND_END_MODULE_GLOBALS(easy_aop)

extern ZEND_DECLARE_MODULE_GLOBALS(easy_aop)

#endif	/* PHP_EASY_AOP_H */

