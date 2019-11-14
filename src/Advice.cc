#include "Advice.h"

namespace PHPAOP
{
    Advice::Advice(zend_fcall_info fci, zend_fcall_info_cache fcc, zval* closure)
    {
        _fci = fci;
        _fcc = fcc;
        ZVAL_COPY(&_zval_closure, closure);
        _fci.retval = &_result;
    }

    Advice::~Advice()
    {
        zval_ptr_dtor(&_zval_closure);
    }

    void Advice::set_param_count(int c)
    {
        _fci.param_count = c;
    }

    void Advice::set_params(zval* params)
    {
        _fci.params = params;
    }

    void Advice::call()
    {
        zend_call_function(&_fci, &_fcc);
    }

    void* Advice::operator new(size_t size)
    {
        return emalloc(size);
    }

    void Advice::operator delete(void* ptr)
    {
        efree(ptr);
    }
}

