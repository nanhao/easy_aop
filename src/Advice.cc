#include "Advice.h"

namespace easy_aop
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
        call_function(&_fci, &_fcc);
    }

    void* Advice::operator new(size_t size)
    {
        return emalloc(size);
    }

    void Advice::operator delete(void* ptr)
    {
        efree(ptr);
    }

    int Advice::call_function(zend_fcall_info *fci, zend_fcall_info_cache *fci_cache) /* {{{ */
    {
        uint32_t i;
        zend_execute_data *call, dummy_execute_data;
        zend_fcall_info_cache fci_cache_local;
        zend_function *func;

        ZVAL_UNDEF(fci->retval);

        if (!EG(active)) {
            return FAILURE; /* executor is already inactive */
        }

        if (EG(exception)) {
            return FAILURE; /* we would result in an instable executor otherwise */
        }

        ZEND_ASSERT(fci->size == sizeof(zend_fcall_info));

        /* Initialize execute_data */
        if (!EG(current_execute_data)) {
            /* This only happens when we're called outside any execute()'s
             * It shouldn't be strictly necessary to NULL execute_data out,
             * but it may make bugs easier to spot
             */
            memset(&dummy_execute_data, 0, sizeof(zend_execute_data));
            EG(current_execute_data) = &dummy_execute_data;
        } else if (EG(current_execute_data)->func &&
                   ZEND_USER_CODE(EG(current_execute_data)->func->common.type) &&
                   EG(current_execute_data)->opline->opcode != ZEND_DO_FCALL &&
                   EG(current_execute_data)->opline->opcode != ZEND_DO_ICALL &&
                   EG(current_execute_data)->opline->opcode != ZEND_DO_UCALL &&
                   EG(current_execute_data)->opline->opcode != ZEND_DO_FCALL_BY_NAME) {
            /* Insert fake frame in case of include or magic calls */
            dummy_execute_data = *EG(current_execute_data);
            dummy_execute_data.prev_execute_data = EG(current_execute_data);
            dummy_execute_data.call = NULL;
            dummy_execute_data.opline = NULL;
            dummy_execute_data.func = NULL;
            EG(current_execute_data) = &dummy_execute_data;
        }

        if (!fci_cache || !fci_cache->function_handler) {
            char *error = NULL;

            if (!fci_cache) {
                fci_cache = &fci_cache_local;
            }

            if (!zend_is_callable_ex(&fci->function_name, fci->object, IS_CALLABLE_CHECK_SILENT, NULL, fci_cache, &error)) {
                if (error) {
                    zend_string *callable_name
                        = zend_get_callable_name_ex(&fci->function_name, fci->object);
                    zend_error(E_WARNING, "Invalid callback %s, %s", ZSTR_VAL(callable_name), error);
                    efree(error);
                    zend_string_release_ex(callable_name, 0);
                }
                if (EG(current_execute_data) == &dummy_execute_data) {
                    EG(current_execute_data) = dummy_execute_data.prev_execute_data;
                }
                return FAILURE;
            } else if (error) {
                /* Capitalize the first latter of the error message */
                if (error[0] >= 'a' && error[0] <= 'z') {
                    error[0] += ('A' - 'a');
                }
                zend_error(E_DEPRECATED, "%s", error);
                efree(error);
                if (UNEXPECTED(EG(exception))) {
                    if (EG(current_execute_data) == &dummy_execute_data) {
                        EG(current_execute_data) = dummy_execute_data.prev_execute_data;
                    }
                    return FAILURE;
                }
            }
        }

        func = fci_cache->function_handler;
        fci->object = (func->common.fn_flags & ZEND_ACC_STATIC) ?
            NULL : fci_cache->object;

        call = zend_vm_stack_push_call_frame(ZEND_CALL_TOP_FUNCTION | ZEND_CALL_DYNAMIC,
            func, fci->param_count, fci_cache->called_scope, fci->object);

        EASY_AOP_G(advice_trace).back().p_before = call;

        if (UNEXPECTED(func->common.fn_flags & ZEND_ACC_DEPRECATED)) {
            zend_error(E_DEPRECATED, "Function %s%s%s() is deprecated",
                func->common.scope ? ZSTR_VAL(func->common.scope->name) : "",
                func->common.scope ? "::" : "",
                ZSTR_VAL(func->common.function_name));
            if (UNEXPECTED(EG(exception))) {
                zend_vm_stack_free_call_frame(call);
                if (EG(current_execute_data) == &dummy_execute_data) {
                    EG(current_execute_data) = dummy_execute_data.prev_execute_data;
                }
                return FAILURE;
            }
        }

        for (i=0; i<fci->param_count; i++) {
            zval *param;
            zval *arg = &fci->params[i];

            if (ARG_SHOULD_BE_SENT_BY_REF(func, i + 1)) {
                if (UNEXPECTED(!Z_ISREF_P(arg))) {
                    if (!fci->no_separation) {
                        /* Separation is enabled -- create a ref */
                        ZVAL_NEW_REF(arg, arg);
                    } else if (!ARG_MAY_BE_SENT_BY_REF(func, i + 1)) {
                        /* By-value send is not allowed -- emit a warning,
                         * but still perform the call with a by-value send. */
                        zend_error(E_WARNING,
                            "Parameter %d to %s%s%s() expected to be a reference, value given", i+1,
                            func->common.scope ? ZSTR_VAL(func->common.scope->name) : "",
                            func->common.scope ? "::" : "",
                            ZSTR_VAL(func->common.function_name));
                        if (UNEXPECTED(EG(exception))) {
                            ZEND_CALL_NUM_ARGS(call) = i;
                            zend_vm_stack_free_args(call);
                            zend_vm_stack_free_call_frame(call);
                            if (EG(current_execute_data) == &dummy_execute_data) {
                                EG(current_execute_data) = dummy_execute_data.prev_execute_data;
                            }
                            return FAILURE;
                        }
                    }
                }
            } else {
                if (Z_ISREF_P(arg) &&
                    !(func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE)) {
                    /* don't separate references for __call */
                    arg = Z_REFVAL_P(arg);
                }
            }

            param = ZEND_CALL_ARG(call, i+1);
            ZVAL_COPY(param, arg);
        }

        if (UNEXPECTED(func->op_array.fn_flags & ZEND_ACC_CLOSURE)) {
            uint32_t call_info;

            GC_ADDREF(ZEND_CLOSURE_OBJECT(func));
            call_info = ZEND_CALL_CLOSURE;
            if (func->common.fn_flags & ZEND_ACC_FAKE_CLOSURE) {
                call_info |= ZEND_CALL_FAKE_CLOSURE;
            }
            ZEND_ADD_CALL_FLAG(call, call_info);
        }

        if (func->type == ZEND_USER_FUNCTION) {
            int call_via_handler = (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
            const zend_op *current_opline_before_exception = EG(opline_before_exception);

            zend_init_func_execute_data(call, &func->op_array, fci->retval);
            zend_execute_ex(call);
            EG(opline_before_exception) = current_opline_before_exception;
            if (call_via_handler) {
                /* We must re-initialize function again */
                fci_cache->function_handler = NULL;
            }
        } else if (func->type == ZEND_INTERNAL_FUNCTION) {
            int call_via_handler = (func->common.fn_flags & ZEND_ACC_CALL_VIA_TRAMPOLINE) != 0;
            ZVAL_NULL(fci->retval);
            call->prev_execute_data = EG(current_execute_data);
            call->return_value = NULL; /* this is not a constructor call */
            EG(current_execute_data) = call;
            if (EXPECTED(zend_execute_internal == NULL)) {
                /* saves one function call if zend_execute_internal is not used */
                func->internal_function.handler(call, fci->retval);
            } else {
                zend_execute_internal(call, fci->retval);
            }
            EG(current_execute_data) = call->prev_execute_data;
            zend_vm_stack_free_args(call);

            if (EG(exception)) {
                zval_ptr_dtor(fci->retval);
                ZVAL_UNDEF(fci->retval);
            }

            if (call_via_handler) {
                /* We must re-initialize function again */
                fci_cache->function_handler = NULL;
            }
        } else { /* ZEND_OVERLOADED_FUNCTION */
            ZVAL_NULL(fci->retval);

            /* Not sure what should be done here if it's a static method */
            if (fci->object) {
                call->prev_execute_data = EG(current_execute_data);
                EG(current_execute_data) = call;
                fci->object->handlers->call_method(func->common.function_name, fci->object, call, fci->retval);
                EG(current_execute_data) = call->prev_execute_data;
            } else {
                zend_throw_error(NULL, "Cannot call overloaded function for non-object");
            }

            zend_vm_stack_free_args(call);

            if (func->type == ZEND_OVERLOADED_FUNCTION_TEMPORARY) {
                zend_string_release_ex(func->common.function_name, 0);
            }
            efree(func);

            if (EG(exception)) {
                zval_ptr_dtor(fci->retval);
                ZVAL_UNDEF(fci->retval);
            }
        }

        zend_vm_stack_free_call_frame(call);

        if (EG(current_execute_data) == &dummy_execute_data) {
            EG(current_execute_data) = dummy_execute_data.prev_execute_data;
        }

        if (UNEXPECTED(EG(exception))) {
            if (UNEXPECTED(!EG(current_execute_data))) {
                zend_throw_exception_internal(NULL);
            } else if (EG(current_execute_data)->func &&
                       ZEND_USER_CODE(EG(current_execute_data)->func->common.type)) {
                // expand zend_rethrow_exception(EG(current_execute_data))
                zend_execute_data* execute_data = EG(current_execute_data);
                if (EX(opline)->opcode != ZEND_HANDLE_EXCEPTION) {
                    EG(opline_before_exception) = EX(opline);
                    EX(opline) = EG(exception_op);
                }
            }
        }

        return SUCCESS;
    }
}

