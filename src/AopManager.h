#ifndef __AOPMANAGER_H__
#define __AOPMANAGER_H__

#include <string>
#include <vector>
#include <map>
#include "php_phpaop.h"
#include "Advice.h"

namespace PHPAOP
{
    using std::string;
    using std::map;
    using std::vector;

    class AopManager
    {
    public:
        ~AopManager();
        void add_advice(vector<string> joinpoint_names, Advice* p_advice);
        void add_before(string where, Advice* p_advice);
        void add_after(string where, Advice* p_advice);
        vector<Advice*>* get_advice_before(string where);
        vector<Advice*>* get_advice_after(string where);
        void call_advice(string joinpoint, Advice* p_advice);
        void joinpoint_begin(string joinpoint);
        void joinpoint_end(string joinpoint);
        void* operator new(size_t size);
        void operator delete(void* ptr);

    private:
        map<string, vector<Advice*>> _before;
        map<string, vector<Advice*>> _after;
        map<Advice*, bool> _all_advices;
        map<string, bool> _joinpoint_map;
    };
}

#endif
