#include "AopExtension.h"
#include "AopManager.h"
#include "php_internal.h"

namespace PHPAOP
{
    void (*AopExtension::ori_zend_execute_ex) (zend_execute_data *execute_data) = zend_execute_ex;

    int AopExtension::hook_minit()
    {
        AopExtension::register_classes();
        zend_execute_ex = AopExtension::phpaop_execute_ex;
        return SUCCESS;
    }

    int AopExtension::hook_mshutdown()
    {
        zend_execute_ex = AopExtension::ori_zend_execute_ex;
        return SUCCESS;
    }

    int AopExtension::hook_rinit()
    {
        PHPAOP_G(p_manager) = (void*)new AopManager();
        return SUCCESS;
    }

    int AopExtension::hook_rshutdown()
    {
        delete (PHPAOP::AopManager*)PHPAOP_G(p_manager);
        return SUCCESS;
    }

    void AopExtension::minfo()
    {
        php_info_print_table_start();
        php_info_print_table_header(2, "phpaop support", "enabled");
        php_info_print_table_end();
    }

    ZEND_DLEXPORT void AopExtension::phpaop_execute_ex(zend_execute_data* execute_data)
    {
        string func = PHPAOP::get_function_name(execute_data);

        if (func.empty()) {
            AopExtension::ori_zend_execute_ex(execute_data);
            return;
        }

        vector<param_info> param_infos;
        PHPAOP::collect_params(&param_infos, execute_data);

        zval* p_result = execute_data->return_value;

        AopExtension::call_before(func, &param_infos, execute_data);
        AopExtension::ori_zend_execute_ex(execute_data);
        AopExtension::call_after(func, &param_infos, p_result);

        for (auto iter = param_infos.begin(); iter != param_infos.end(); ++iter) {
            zval_ptr_dtor(&iter->val);
        }
    }

    void AopExtension::register_classes()
    {
        register_PHPAOP();
    }

    void AopExtension::call_before(string func, vector<param_info>* p_param_infos, zend_execute_data* execute_data)
    {
        PHPAOP::AopManager* p_manager = ((PHPAOP::AopManager*)PHPAOP_G(p_manager));

        vector<PHPAOP::Advice*>* p_advice_vector = p_manager->get_advice_before(func);

        for (auto it = p_advice_vector->begin(); it != p_advice_vector->end(); ++it) {
            zval params[3];

            string joinpoint = string("before@") + func;
            zend_string* str_val = zend_string_init(joinpoint.c_str(), joinpoint.size(), 0);
            ZVAL_STR(&params[0], str_val);

            ZVAL_NEW_ARR(&params[1]);
            zend_hash_init(Z_ARRVAL(params[1]), 10, NULL, NULL, 0);

            for (auto iter = p_param_infos->begin(); iter != p_param_infos->end(); ++iter) {
                zend_hash_update(Z_ARRVAL(params[1]), iter->name, &iter->val);
            }

            ZVAL_NULL(&params[2]);

            (*it)->set_param_count(3);
            (*it)->set_params(params);
            p_manager->call_advice(joinpoint, *it);

            zend_string_release(str_val);
            zend_array_destroy(Z_ARRVAL(params[1]));
        }
    }

    void AopExtension::call_after(string func, vector<param_info>* p_param_infos, zval* p_result)
    {
        PHPAOP::AopManager* p_manager = ((PHPAOP::AopManager*)PHPAOP_G(p_manager));

        vector<PHPAOP::Advice*>* p_advice_vector = p_manager->get_advice_after(func);

        for (auto it = p_advice_vector->begin(); it != p_advice_vector->end(); ++it) {
            zval params[3];

            string joinpoint = string("after@") + func;
            zend_string* str_val = zend_string_init(joinpoint.c_str(), joinpoint.size(), 0);
            ZVAL_STR(&params[0], str_val);

            ZVAL_NEW_ARR(&params[1]);
            zend_hash_init(Z_ARRVAL(params[1]), 10, NULL, NULL, 0);

            for (auto iter = p_param_infos->begin(); iter != p_param_infos->end(); ++iter) {
                zend_hash_update(Z_ARRVAL(params[1]), iter->name, &iter->val);
            }

            if (p_result != NULL) {
                ZVAL_COPY_VALUE(&params[2], p_result);
            } else {
                ZVAL_NULL(&params[2]);
            }

            (*it)->set_param_count(3);
            (*it)->set_params(params);
            p_manager->call_advice(joinpoint, *it);

            zend_string_release(str_val);
            zend_array_destroy(Z_ARRVAL(params[1]));
        }
    }
}
