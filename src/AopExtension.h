#ifndef __AOPEXTENSION_H__
#define __AOPEXTENSION_H__

#include "php_phpaop.h"
#include "php_internal.h"
#include <string>
#include <vector>

#define PHPAOP_FUNCTIONS

namespace PHPAOP
{
    using std::string;
    using std::vector;

    class AopExtension
    {
    public:
        static void (*ori_zend_execute_ex) (zend_execute_data *execute_data);

        static int hook_minit();
        static int hook_mshutdown();
        static int hook_rinit();
        static int hook_rshutdown();
        static void minfo();

        ZEND_DLEXPORT static void phpaop_execute_ex(zend_execute_data* execute_data);

    private:
        static void register_classes();
        static void call_before(string func, vector<param_info>* p_param_infos, zend_execute_data* execute_data);
        static void call_after(string func, vector<param_info>* p_param_infos, zval* p_result);
    };

    void register_PHPAOP();
}

#endif
