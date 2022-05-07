#include "AopExtension.h"
#include "AopManager.h"
#include "php_internal.h"
#include <iostream>

namespace easy_aop
{
    // void (*AopExtension::ori_zend_execute_ex)(zend_execute_data *execute_data) = zend_execute_ex;

    void AopExtension::hook_minit()
    {
        AopExtension::register_classes();
#if PHP_VERSION_ID >= 80000
        zend_observer_fcall_register(AopExtension::easy_aop_execute_ex);
#else
        zend_execute_ex = AopExtension::easy_aop_execute_ex;
#endif
    }

    void AopExtension::hook_mshutdown()
    {
#if PHP_VERSION_ID < 80000
        zend_execute_ex = AopExtension::ori_zend_execute_ex;
#endif
    }

    void AopExtension::hook_rinit()
    {
        EASY_AOP_G(p_manager) = (void *)new AopManager();
    }

    void AopExtension::hook_rshutdown()
    {
        delete (easy_aop::AopManager *)EASY_AOP_G(p_manager);
    }

    void AopExtension::minfo()
    {
        php_info_print_table_start();
        php_info_print_table_header(2, "easy_aop support", "enabled");
        php_info_print_table_end();
    }

#if PHP_VERSION_ID >= 80000
    void AopExtension::observer_begin(zend_execute_data *execute_data)
    {
        string func = easy_aop::get_function_name(execute_data);
        vector<param_info> param_infos;
        easy_aop::collect_params(&param_infos, execute_data);

        EASY_AOP_G(advice_trace).push_back(BeforeAdviceCall{.p_target = EG(current_execute_data), .intercept = 0});

        AopExtension::call_before(func, &param_infos, execute_data);

        BeforeAdviceCall advice_call_info = EASY_AOP_G(advice_trace).back();
        EASY_AOP_G(advice_trace).pop_back();
    }

    void AopExtension::observer_end(zend_execute_data *execute_data, zval *return_value)
    {
        string func = easy_aop::get_function_name(execute_data);
        vector<param_info> param_infos;
        easy_aop::collect_params(&param_infos, execute_data);

        zval *p_result = execute_data->return_value;
        AopExtension::call_after(func, &param_infos, p_result);
    }

    ZEND_DLEXPORT zend_observer_fcall_handlers AopExtension::easy_aop_execute_ex(zend_execute_data *execute_data)
    {
        zend_observer_fcall_handlers handlers = {NULL, NULL};
        string func = easy_aop::get_function_name(execute_data);

        if (func.empty())
        {
            return handlers;
        }

        handlers.begin = AopExtension::observer_begin;
        handlers.end = AopExtension::observer_end;
        return handlers;
    }
#else
    ZEND_DLEXPORT void AopExtension::easy_aop_execute_ex(zend_execute_data *execute_data)
    {
        string func = easy_aop::get_function_name(execute_data);

        if (func.empty())
        {
            AopExtension::ori_zend_execute_ex(execute_data);
            return;
        }

        vector<param_info> param_infos;
        easy_aop::collect_params(&param_infos, execute_data);

        zval *p_result = execute_data->return_value;

        EASY_AOP_G(advice_trace).push_back(BeforeAdviceCall{.p_target = EG(current_execute_data), .intercept = 0});

        AopExtension::call_before(func, &param_infos, execute_data);

        BeforeAdviceCall advice_call_info = EASY_AOP_G(advice_trace).back();
        EASY_AOP_G(advice_trace).pop_back();

        if (!advice_call_info.intercept)
        {
            AopExtension::ori_zend_execute_ex(execute_data);

            if (!EG(exception))
            {
                AopExtension::call_after(func, &param_infos, p_result);
            }
        }
        else
        {
            zend_vm_stack_free_args(execute_data);
            zend_vm_stack_free_call_frame(execute_data);
        }

        for (auto iter = param_infos.begin(); iter != param_infos.end(); ++iter)
        {
            zval_ptr_dtor(&iter->val);
        }
    }
#endif

    void AopExtension::register_classes()
    {
        register_EasyAop();
    }

    void AopExtension::call_before(string func, vector<param_info> *p_param_infos, zend_execute_data *execute_data)
    {
        easy_aop::AopManager *p_manager = ((easy_aop::AopManager *)EASY_AOP_G(p_manager));

        vector<easy_aop::Advice *> *p_advice_vector = p_manager->get_advice_before(func);

        if (p_advice_vector == NULL)
        {
            return;
        }

        for (auto it = p_advice_vector->begin(); it != p_advice_vector->end(); ++it)
        {
            zval params[3];

            string joinpoint = string("before@") + func;
            zend_string *str_val = zend_string_init(joinpoint.c_str(), joinpoint.size(), 0);
            ZVAL_STR(&params[0], str_val);

            array_init(&params[1]);
            zend_hash_init(Z_ARRVAL(params[1]), 10, NULL, NULL, 0);

            for (auto iter = p_param_infos->begin(); iter != p_param_infos->end(); ++iter)
            {
                zend_hash_update(Z_ARRVAL(params[1]), iter->name, &iter->val);
            }

            ZVAL_NULL(&params[2]);

            (*it)->set_param_count(3);
            (*it)->set_params(params);
            p_manager->call_advice(joinpoint, *it);

            zval_ptr_dtor(&params[0]);
            zval_ptr_dtor(&params[1]);
        }
    }

    void AopExtension::call_after(string func, vector<param_info> *p_param_infos, zval *p_result)
    {
        easy_aop::AopManager *p_manager = ((easy_aop::AopManager *)EASY_AOP_G(p_manager));

        vector<easy_aop::Advice *> *p_advice_vector = p_manager->get_advice_after(func);

        if (p_advice_vector == NULL)
        {
            return;
        }

        for (auto it = p_advice_vector->begin(); it != p_advice_vector->end(); ++it)
        {
            zval params[3];

            string joinpoint = string("after@") + func;
            zend_string *str_val = zend_string_init(joinpoint.c_str(), joinpoint.size(), 0);
            ZVAL_STR(&params[0], str_val);

            array_init(&params[1]);
            zend_hash_init(Z_ARRVAL(params[1]), 10, NULL, NULL, 0);

            for (auto iter = p_param_infos->begin(); iter != p_param_infos->end(); ++iter)
            {
                zend_hash_update(Z_ARRVAL(params[1]), iter->name, &iter->val);
            }

            if (p_result != NULL)
            {
                ZVAL_COPY_VALUE(&params[2], p_result);
            }
            else
            {
                ZVAL_NULL(&params[2]);
            }

            (*it)->set_param_count(3);
            (*it)->set_params(params);
            p_manager->call_advice(joinpoint, *it);

            zval_ptr_dtor(&params[0]);
            zval_ptr_dtor(&params[1]);
        }
    }
}
