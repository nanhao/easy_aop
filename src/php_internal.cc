#include "php_internal.h"
#include "AopManager.h"
#include <vector>
#include <string>

namespace easy_aop
{
    using std::vector;
    using std::string;

    void collect_params(vector<param_info>* p_param_infos, zend_execute_data* execute_data)
    {
        int param_num = get_param_num(execute_data);

        for (int i = 0; i< param_num; ++i) {
            param_info info = get_param(execute_data, i);
            p_param_infos->push_back(info);
        }
    }

    string get_function_name(zend_execute_data* execute_data)
    {
        if (!execute_data) {
            return string("");
        }

        zend_function* curr_func = execute_data->func;
        /* extract function name from the meta info */
        zend_string* func = curr_func->common.function_name;

        if (!func) {
            return string("");
        }

        if (curr_func->common.scope != NULL) {
            char* cls = curr_func->common.scope->name->val;
            return string(cls) + "::" + func->val;
        } else {
            return string(func->val);
        }
    }

    int get_param_num(zend_execute_data* execute_data)
    {
        return execute_data->func->common.num_args;
    }

    param_info get_param(zend_execute_data *execute_data, int index)
    {
        param_info ret;

        zend_arg_info* p_arg_info = execute_data->func->common.arg_info;
        ret.name = (p_arg_info + index)->name;
        int offset = lookup_cv(&execute_data->func->op_array, ret.name);
        ZVAL_COPY(&ret.val, (zval*)((char*)execute_data + offset));

        return ret;
    }

    int lookup_cv(zend_op_array *op_array, zend_string *name)
    {
        int i = 0;
        zend_ulong hash_value = zend_string_hash_val(name);

        while (i < op_array->last_var) {
            if (ZSTR_VAL(op_array->vars[i]) == ZSTR_VAL(name) ||
                (ZSTR_H(op_array->vars[i]) == hash_value &&
                 zend_string_equals(op_array->vars[i], name))) {
                return (int)(zend_intptr_t)ZEND_CALL_VAR_NUM(NULL, i);
            }
            i++;
        }

        return -1;
    }
}
