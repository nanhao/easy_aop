/* phpaop extension for PHP */

#include "php_phpaop.h"
#include "AopExtension.h"
using namespace PHPAOP;

ZEND_DECLARE_MODULE_GLOBALS(phpaop)

PHP_MINIT_FUNCTION(phpaop)
{
    return AopExtension::hook_minit();
}

PHP_MSHUTDOWN_FUNCTION(phpaop)
{
    return AopExtension::hook_mshutdown();
}

PHP_RINIT_FUNCTION(phpaop)
{
    return AopExtension::hook_rinit();
}

PHP_RSHUTDOWN_FUNCTION(phpaop)
{
    return AopExtension::hook_rshutdown();
}

PHP_MINFO_FUNCTION(phpaop)
{
    AopExtension::minfo();
}

/* {{{ phpaop_functions[]
 */
static const zend_function_entry phpaop_functions[] = {
    PHPAOP_FUNCTIONS
    PHP_FE_END
};
/* }}} */

/* {{{ phpaop_module_entry
 */
zend_module_entry phpaop_module_entry = {
	STANDARD_MODULE_HEADER,
	"phpaop",					/* Extension name */
	phpaop_functions,			/* zend_function_entry */
	PHP_MINIT(phpaop),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(phpaop),							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(phpaop),			/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(phpaop),							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(phpaop),			/* PHP_MINFO - Module info */
	PHP_PHPAOP_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PHPAOP
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(phpaop)
#endif

