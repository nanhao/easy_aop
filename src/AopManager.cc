#include "AopManager.h"

namespace PHPAOP
{
    using std::pair;

    static void supersplit(const string& s, vector<string>& v, const string& c)
    {
        string::size_type pos1, pos2;
        size_t len = s.length();
        pos2 = s.find(c);
        pos1 = 0;
        while(string::npos != pos2)
        {
            v.emplace_back(s.substr(pos1, pos2-pos1));

            pos1 = pos2 + c.size();
            pos2 = s.find(c, pos1);
        }
        if(pos1 != len)
            v.emplace_back(s.substr(pos1));
    }

    AopManager::~AopManager()
    {
        for (auto iter = _all_advices.begin(); iter != _all_advices.end(); ++iter) {
            delete iter->first;
        }
    }

    void AopManager::add_advice(vector<string> joinpoint_names, Advice* p_advice)
    {
        vector<string>::iterator it;
        for (it = joinpoint_names.begin(); it != joinpoint_names.end(); ++it) {
            vector<string> v;
            string explode = "@";
            supersplit(*it, v, explode);

            if (v.size() != 2) {
                zend_error(E_ERROR, "joinpoint_names should match the format: [before|after]@class_name::method_name or [before|after]@function_name");
                return;
            }

            string type = v[0];
            string where = v[1];

            if (type == "before") {
                this->add_before(where, p_advice);
            }
            else if (type == "after"){
                this->add_after(where, p_advice);
            }
            else {
                zend_error(E_ERROR, "joinpoint_names should match the format: [before|after]@class_name::method_name or [before|after]@function_name");
                return;
            }
        }
    }

    void AopManager::add_before(string where, Advice* p_advice)
    {
        _before[where].push_back(p_advice);
        _all_advices[p_advice] = 1;
    }

    void AopManager::add_after(string where, Advice* p_advice)
    {
        _after[where].push_back(p_advice);
        _all_advices[p_advice] = 1;
    }

    vector<Advice*>* AopManager::get_advice_before(string where)
    {
        return &_before[where];
    }

    vector<Advice*>* AopManager::get_advice_after(string where)
    {
        return &_after[where];
    }

    void AopManager::call_advice(string joinpoint, Advice* p_advice)
    {
        this->joinpoint_begin(joinpoint);
        p_advice->call();
        this->joinpoint_end(joinpoint);
    }

    void AopManager::joinpoint_begin(string joinpoint)
    {
        auto iter = _joinpoint_map.find(joinpoint);
        if (iter != _joinpoint_map.end()) {
            string err = "advice recursion detected: ";
            err += iter->first;
            zend_error(E_ERROR, err.c_str());
            return;
        }

        _joinpoint_map[joinpoint] = 1;
    }

    void AopManager::joinpoint_end(string joinpoint)
    {
        _joinpoint_map.erase(joinpoint);
    }

    void* AopManager::operator new(size_t size)
    {
        return emalloc(size);
    }

    void AopManager::operator delete(void* ptr)
    {
        efree(ptr);
    }
}

