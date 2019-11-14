#include "AopExtension.h"
#include <string>
#include <vector>
#include "AopManager.h"
using std::string;
using std::vector;

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

PHP_METHOD(PHPAOP, add_advice)
{
    HashTable* ht_joinpoints;
    zend_fcall_info fci_advice;
    zend_fcall_info_cache fcc_advice;
    zval* p_closure;

    ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ARRAY_HT(ht_joinpoints)
		Z_PARAM_FUNC(fci_advice, fcc_advice)
		p_closure = _real_arg;
	ZEND_PARSE_PARAMETERS_END();

    zval* each_joinpoint;
    vector<string> joinpoint_names;
    ZEND_HASH_FOREACH_VAL(ht_joinpoints, each_joinpoint) {
        string joinpoint = Z_STRVAL_P(each_joinpoint);
        joinpoint_names.push_back(joinpoint);
    } ZEND_HASH_FOREACH_END();

    PHPAOP::AopManager* p_manager = (PHPAOP::AopManager*)PHPAOP_G(p_manager);
    p_manager->add_advice(joinpoint_names, new PHPAOP::Advice(fci_advice, fcc_advice, p_closure));
}

namespace PHPAOP
{
    void register_PHPAOP()
    {
        ZEND_BEGIN_ARG_INFO_EX(arginfo_PHPAOP_add_advice, 0, 0, 2)
            ZEND_ARG_ARRAY_INFO(0, joinpoints, 0)
            ZEND_ARG_CALLABLE_INFO(0, advice, 0)
        ZEND_END_ARG_INFO()

        zend_function_entry phpaop_methods[] = {
            PHP_ME(PHPAOP, add_advice, arginfo_PHPAOP_add_advice, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
            PHP_FE_END
        };

        zend_class_entry _ce;

        INIT_CLASS_ENTRY(_ce, "PHPAOP", phpaop_methods);

        zend_class_entry* p_ce = zend_register_internal_class(&_ce);
    }
}
