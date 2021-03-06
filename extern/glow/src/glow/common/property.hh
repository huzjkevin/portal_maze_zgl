#pragma once

#include "macro_join.hh"
#include "nodiscard.hh"

/** GLOW_GETTER(name) GLOW_SETTER(name) GLOW_PROPERTY(name)
 * as non-typed getter/setter macros
 *
 * Usage:
 * class Foo {
 * private:
 *    int mBar;
 *    bool mFinished;
 *    std::string mName;
 *
 * public:
 *    GLOW_PROPERTY(Bar); // generates getBar() and setBar(...) with const& set and get
 *    GLOW_PROPERTY_IS(Finished); // isFinished() and setFinished()
 *    GLOW_BUILDER(name, Name); // name() and name(...)
 * };
 *
 * CAUTION: these macros can only be used _after_ the member is declared (due to type deduction)
 */

#define GLOW_GETTER(name)                                                               \
    GLOW_NODISCARD auto get##name() const->decltype(m##name) const& { return m##name; } \
    friend class GLOW_MACRO_JOIN(GLOW_MACRO_JOIN(_glow_property_get_, name), __COUNTER__) // force ;

#define GLOW_GETTER_IS(name)                                                           \
    GLOW_NODISCARD auto is##name() const->decltype(m##name) const& { return m##name; } \
    friend class GLOW_MACRO_JOIN(GLOW_MACRO_JOIN(_glow_property_get_is_, name), __COUNTER__) // force ;

#define GLOW_GETTER_CONSTEXPR(name)                                                                        \
    GLOW_NODISCARD constexpr auto get##name() const noexcept->decltype(m##name) const& { return m##name; } \
    friend class GLOW_MACRO_JOIN(GLOW_MACRO_JOIN(_glow_property_get_, name), __COUNTER__) // force ;

#define GLOW_SETTER(name)                                               \
    void set##name(decltype(m##name) const& value) { m##name = value; } \
    friend class GLOW_MACRO_JOIN(GLOW_MACRO_JOIN(_glow_property_set_, name), __COUNTER__) // force ;

#define GLOW_PROPERTY(name) \
    GLOW_GETTER(name);      \
    GLOW_SETTER(name)

#define GLOW_PROPERTY_IS(name) \
    GLOW_GETTER_IS(name);      \
    GLOW_SETTER(name)

#define GLOW_BUILDER(pname, name)                                    \
    auto pname(decltype(m##name) const& value)->decltype(this)       \
    {                                                                \
        m##name = value;                                             \
        return this;                                                 \
    }                                                                \
    auto pname() const->decltype(m##name) const& { return m##name; } \
    friend class GLOW_MACRO_JOIN(GLOW_MACRO_JOIN(_glow_property_builder_, name), __COUNTER__) // force ;
