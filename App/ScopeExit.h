#pragma once
#include<functional>

using std::function;

/**
 * Class for helping with scope clean up.
 */
class ScopeGuard
{
public:
    ScopeGuard()=default;
    ~ScopeGuard()
    {
        if (func) { func(); };
    }
    void setFunctionToCallOnDestroy(function<void()> inputFunc)
    {
        func = inputFunc;
    }
    void operator+=(function<void()> inputFunc)
    {
        setFunctionToCallOnDestroy(inputFunc);
    }
private:
    function<void()> func;
};

// preprocessor magic
#define PREPROCESOR_CONCAT_(a, b)   a##b
#define PREPROCESOR_CONCAT(a, b)    PREPROCESOR_CONCAT_(a, b)
#define PREPROCESOR_ANONYMOUS_VARIABLE(str) PREPROCESOR_CONCAT(str, __LINE__)
#define SCOPE_EXIT ScopeGuard PREPROCESOR_ANONYMOUS_VARIABLE(guard); PREPROCESOR_ANONYMOUS_VARIABLE(guard) += [&]()