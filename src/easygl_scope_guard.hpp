//
//  Created by Pi on 2021/2/18.
//

#ifndef easygl_scope_guard_h
#define easygl_scope_guard_h

#include "easygl_macro_helper.hpp"

NS_EASYGL_BEGIN

template <typename Func>
class ScopeGuard
{
public:
    ~ScopeGuard()
    {
        if (!_dissmiss)
        {
            _onExitScope();
        }
    }
    
    ScopeGuard(Func &&onExitScope) : _onExitScope(std::forward<Func>(onExitScope)), _dissmiss(false)
    {
    }
    
    ScopeGuard(ScopeGuard&& rhs) : _onExitScope(std::move(rhs._onExitScope)), _dissmiss(rhs._dissmiss)
    {
        rhs.dismiss();
    }
    
    void dismiss()
    {
        _dissmiss = true;
    }
    
    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
private:
    bool _dissmiss = false;
    Func _onExitScope;
};

enum class ScopeGuardOnExit{};
template <typename Func>
inline ScopeGuard<Func> operator+(ScopeGuardOnExit, Func &&func)
{
    return ScopeGuard<Func>(std::forward<Func>(func));
}

NS_EASYGL_END

#define _EASYGL_ON_SCOPE_EXIT(_n) \
auto _n = easy_gl::ScopeGuardOnExit() + [&]()

#define EASYGL_ON_SCOPE_EXIT _EASYGL_ON_SCOPE_EXIT(EASYGL_CAT_MACRO(__ScopeGuard_, __LINE__))
#define EASYGL_ON_SCOPE_EXIT_WITH_NAME(_n) _EASYGL_ON_SCOPE_EXIT(_n)

#endif /* easygl_scope_guard_h */
