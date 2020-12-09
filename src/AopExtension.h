#ifndef __AOPEXTENSION_H__
#define __AOPEXTENSION_H__

#include "php_easy_aop.h"
#include "php_internal.h"
#include <string>
#include <vector>

#define EASY_AOP_FUNCTIONS

namespace easy_aop
{
    using std::string;
    using std::vector;

    class AopExtension
    {
    public:
        static void (*ori_zend_execute_ex) (zend_execute_data *execute_data);

        static void hook_minit();
        static void hook_mshutdown();
        static void hook_rinit();
        static void hook_rshutdown();
        static void minfo();

        ZEND_DLEXPORT static void easy_aop_execute_ex(zend_execute_data* execute_data);

    private:
        static void register_classes();
        static void call_before(string func, vector<param_info>* p_param_infos, zend_execute_data* execute_data);
        static void call_after(string func, vector<param_info>* p_param_infos, zval* p_result);
    };

    void register_EasyAop();
}

#endif
