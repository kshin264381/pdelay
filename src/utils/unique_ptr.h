#ifndef __unique_ptr_h__
#define __unique_ptr_h__

/**
 *
 * The missing make_unique
 *
 * Adopted from Effective Modern C++ by Scott Meyers, p. 139
 *
 * --> C++14 has it already... FYI
 *
**/

#include <memory>

#ifndef _MSC_VER

#if __cplusplus >= 201402L
    using std::make_unique ;
#else
namespace std {
    template <typename T, typename... Ts>
    std::unique_ptr<T> make_unique(Ts&&... params)
    {
	return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
    }
}
#endif /* __cplusplus */

#endif /* _MSC_VER */

#endif /* __unique_ptr_h__ */
