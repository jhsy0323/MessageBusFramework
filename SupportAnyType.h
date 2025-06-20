#pragma once
#include <tuple>
#include <type_traits>
#include <functional>
#include <any>

//类型占位符，保存可变参类型信息
template<typename ...Types>
struct TypeHolder : std::integral_constant<size_t, sizeof...(Types)> {
	using func_type = std::function<void(Types...)>;
	using type = TypeHolder<Types...>;
};
template<typename ...T1, typename ...T2, typename ...T3>
struct TypeHolder<TypeHolder<T1...>, TypeHolder<T2...>, TypeHolder<T3...>> {
	using type = TypeHolder<T1..., T2..., T3...>;
};
template<typename ...Types>
using TypeHolder_t = typename TypeHolder<Types...>::type;

//从类型占位符TH中获取索引I处的类型
//例：TypeHolderElement_t<3, TypeHolder<int,int,char,double,string,double>> 获取到double类型
template<size_t I, typename TH>
struct TypeHolderElement;
template<size_t I, typename TH>
using TypeHolderElement_t = typename TypeHolderElement<I, TH>::type;
template<typename First, typename... Types>
struct TypeHolderElement<0, TypeHolder<First, Types...>> {
	using type = First;
};
template<size_t I, typename First, typename... Types>
struct TypeHolderElement<I, TypeHolder<First, Types...>> {
	using type = TypeHolderElement_t<I - 1, TypeHolder<Types...>>;
};

//整型占位符，保存可变参整型数值信息
template<size_t ...N>
struct IntHolter {};

//编译期生成3的N次方全排列
template<bool b, size_t N, size_t... Is>
struct GenerateAllPermutationSeq;
template<bool b, size_t N, size_t... Is>
using GenerateAllPermutationSeq_t = typename GenerateAllPermutationSeq<b, N, Is...>::type;
template<size_t N, size_t... Is>
struct GenerateAllPermutationSeq<true, N, Is...> {
	using type = TypeHolder<IntHolter<Is...>>;
};
template<size_t N, size_t... Is>
struct GenerateAllPermutationSeq<false, N, Is...> {
	using type = TypeHolder_t<
		GenerateAllPermutationSeq_t<sizeof...(Is)+1 == N, N, 0, Is...>,
		GenerateAllPermutationSeq_t<sizeof...(Is)+1 == N, N, 1, Is...>,
		GenerateAllPermutationSeq_t<sizeof...(Is)+1 == N, N, 2, Is...>>;
};
template<size_t N, size_t ...Is>
using AllPermutationSeq_t = GenerateAllPermutationSeq_t<sizeof...(Is) == N, N, Is...>;

template<typename T>
struct CastType;
template<typename T>
struct CastType<T&> { using type = TypeHolder<T, T&, const T&>; };
template<typename T>
struct CastType<const T&> { using type = TypeHolder<const T&, T, T>; };
template<typename T>
struct CastType<T&&> { using type = TypeHolder<T, const T&, T>; };
template<typename T>
struct CastType<T*&> { using type = TypeHolder<T*, T*&, T* const&>; };
template<typename T>
using CastType_t = typename CastType<T>::type;

template<typename ...Args>
struct CreateType;
template<typename ...Args>
using CreateType_t = typename CreateType<Args...>::type;
template<typename ...Types, size_t ...Idx>
struct CreateType<IntHolter<Idx...>, TypeHolder<Types...>> {
	using type = TypeHolder<TypeHolderElement_t<Idx, Types>...>;
};

template<typename ...Args>
struct AllTypeCombine;
template<typename ...Args>
using AllTypeCombine_t = typename AllTypeCombine<Args...>::type;
template<typename Types, typename ...AllSeq>
struct AllTypeCombine<TypeHolder<AllSeq...>, Types> {
	using type = TypeHolder<CreateType_t<AllSeq, Types>...>;
};

template<typename ...Args>
using AllCombineModify_t = AllTypeCombine_t<AllPermutationSeq_t<sizeof...(Args)>, TypeHolder<CastType_t<Args>...>>;

// 酌情使用多参数版本，使用结构体封装参数减少参数个数
// 递归深度为3的N次方，N为参数个数，目前测试极限可以传递6个参数
// 当参数超过4个时，可能出现编译慢或编译报错
template<typename Types, typename... Args>
std::function<void()> wrap_to_std_fuction_impl(std::any& a, Args&&... args) {
	using FuncType = typename Types::func_type;
	if (a.type() == typeid(FuncType)) {
		auto func = std::any_cast<FuncType>(&a);
		using args_type = typename function_traits<FuncType>::args_type;
		return[func, tuple = args_type(args...)]()mutable { std::apply((*func), std::move(tuple)); };
	}else {
		return nullptr;
	}
}

template<typename ...Types, typename... Args>
std::function<void()> wrap_to_std_fuction_impl(std::any& a, const TypeHolder<Types...>&, Args&&... args) {
	std::function<void()> arr[] = { wrap_to_std_fuction_impl<Types>(a, args...)... };
	for (auto it = std::begin(arr); it != std::end(arr); ++it) if (*it) return *it;
	auto func = std::any_cast<std::function<void()>>(&a);
	if (func == nullptr) {
		throw std::runtime_error("any_cast function fail");
	}
	return std::any_cast<std::function<void()>>(a);
}

template <typename ...Args>
std::enable_if_t<sizeof...(Args) != 0, std::function<void()>> wrap_to_std_fuction(std::any& a, Args&&... args)
{
	static_assert(sizeof...(Args) < 7, "param can not exceed 6");
	return wrap_to_std_fuction_impl(a, AllCombineModify_t<Args&&...>{}, std::forward<Args>(args)...);
}

template <typename ...Args>
std::enable_if_t<sizeof...(Args) == 0, std::function<void()>> wrap_to_std_fuction(std::any& a, Args&&... args)
{
	return std::any_cast<std::function<void()>>(a);
}
