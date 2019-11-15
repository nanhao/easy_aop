/* easy_aop extension for PHP */

#include "php_easy_aop.h"
#include "AopExtension.h"
using namespace easy_aop;

ZEND_DECLARE_MODULE_GLOBALS(easy_aop)

PHP_MINIT_FUNCTION(easy_aop)
{
    return AopExtension::hook_minit();
}

PHP_MSHUTDOWN_FUNCTION(easy_aop)
{
    return AopExtension::hook_mshutdown();
}

PHP_RINIT_FUNCTION(easy_aop)
{
    return AopExtension::hook_rinit();
}

PHP_RSHUTDOWN_FUNCTION(easy_aop)
{
    return AopExtension::hook_rshutdown();
}

PHP_MINFO_FUNCTION(easy_aop)
{
    AopExtension::minfo();
}

/* {{{ easy_aop_functions[]
 */
static const zend_function_entry easy_aop_functions[] = {
    EASY_AOP_FUNCTIONS
    PHP_FE_END
};
/* }}} */

/* {{{ easy_aop_module_entry
 */
zend_module_entry easy_aop_module_entry = {
	STANDARD_MODULE_HEADER,
	"easy_aop",					/* Extension name */
	easy_aop_functions,			/* zend_function_entry */
	PHP_MINIT(easy_aop),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(easy_aop),							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(easy_aop),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(easy_aop),							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(easy_aop),			/* PHP_MINFO - Module info */
	PHP_EASY_AOP_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EASY_AOP
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(easy_aop)
#endif

