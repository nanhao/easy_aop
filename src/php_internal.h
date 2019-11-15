#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include "php_phpaop.h"
#include <string>
#include <vector>

namespace PHPAOP
{
    using std::string;
    using std::vector;

    string get_function_name(zend_execute_data* execute_data);
    int get_param_num(zend_execute_data* execute_data);

    typedef struct {
        zend_string* name;
        zval val;
    } param_info;

    param_info get_param(zend_execute_data *execute_data, int index);
    void collect_params(vector<param_info>* p_param_infos, zend_execute_data* execute_data);

    int lookup_cv(zend_op_array *op_array, zend_string *name);
}

#endif
