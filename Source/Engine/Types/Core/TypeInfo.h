#pragma once

template<typename A, typename B> constexpr bool typesEqual(const A&, const B&) { return false; }
template<typename A> constexpr bool typesEqual(const A&, const A&) { return true; }

template<typename A, typename B> struct TypesEqual { static constexpr bool value() { return false; } };
template<typename A> struct TypesEqual<A, A> { static constexpr bool value() { return true; } };

#define TYPES_EQUAL(a, b) TypesEqual<a, b>::value()

template<class T, class Ty> constexpr bool isOfType(const Ty& t) { return TypesEqual<T, Ty>::value(); }

template<typename T> struct IsLReference { static constexpr bool value() { return false; } };
template<typename T> struct IsLReference<T&> { static constexpr bool value() { return true; } };

#define IS_L_REF(type) IsLReference<type>::value()

template<typename T> struct IsRReference { static constexpr bool value() { return false; } };
template<typename T> struct IsRReference<T&&> { static constexpr bool value() { return true; } };

#define IS_R_REF(type) IsRReference<type>::value()

template<typename T> struct RemoveReference { using Type = T; };
template<typename T> struct RemoveReference<T&> { using Type = T; };
template<typename T> struct RemoveReference<T&&> { using Type = T; };

template<class T> struct IsPointer { static constexpr bool value() { return false; } };
template<class T> struct IsPointer<T*> { static constexpr bool value() { return true; } };

template<class T> struct IsPointer2D { static constexpr bool value() { return false; } };
template<class T> struct IsPointer2D<T**> { static constexpr bool value() { return true; } };

template<class T> struct RemovePointer1D { using Type = T; };
template<class T> struct RemovePointer1D<T*> { using Type = T; };

template<class T> struct RemovePointer { using Type = T; };
template<class T> struct RemovePointer<T*> { using Type = T; };
template<class T> struct RemovePointer<T**> { using Type = T; };

template<typename T> constexpr typename RemoveReference<T>::Type&& mov(T&& a) {
	using MoveType = typename RemoveReference<T>::Type;
	return static_cast<MoveType&&>(a);
}