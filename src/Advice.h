#ifndef __ADVICE_H__
#define __ADVICE_H__

#include "php_easy_aop.h"
#include <string>
using std::string;

#define ZEND_CLOSURE_OBJECT(op_array) \
	((zend_object*)((char*)(op_array) - sizeof(zend_object)))

ZEND_API ZEND_COLD void zend_throw_exception_internal(zval *exception);

namespace easy_aop
{
    class Advice
    {
    public:
        Advice(zend_fcall_info fci, zend_fcall_info_cache fcc, zval* closure);
        ~Advice();
        void set_param_count(int c);
        void set_params(zval* params);
        void call();
        void* operator new(size_t size);
        void operator delete(void* ptr);
        int call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache);

    private:
        zend_fcall_info _fci;
        zend_fcall_info_cache _fcc;
        zval _zval_closure;
        zval _result;
    };
}

#endif
