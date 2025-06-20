#pragma once
#include <functional>
template<typename T>
struct function_traits;

template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
	using return_type = R;
	using args_type = std::tuple<Args...>;
	using function_type = std::function<R(Args...)>;
};

template<typename R, typename... Args>
struct function_traits<R(&)(Args...)> : function_traits<R(*)(Args...)> {};

template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(*)(Args...)> {};

template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)const> : function_traits<R(*)(Args...)> {};

template<typename T>
struct function_traits : function_traits<decltype(&T::operator())> {};

template <typename Obj, typename Func>
typename std::enable_if<std::is_member_function_pointer<Func>::value, typename function_traits<Func>::function_type>::type to_std_function(Obj* obj, Func&& mem_func) {
	return [obj, mem_func](auto&&... args) {
		return (obj->*mem_func)(std::forward<decltype(args)>(args)...);
	};
}

template <typename Obj, typename Func>
typename std::enable_if<!std::is_member_function_pointer<Func>::value, typename function_traits<Func>::function_type>::type to_std_function(Obj*, Func&& func) {
	return typename function_traits<Func>::function_type(
		std::forward<Func>(func)
	);
}

template<typename F>
auto to_std_function(F&& f) {
	using traits = function_traits<std::decay_t<F>>;
	static_assert(std::is_same_v<typename traits::return_type, void>,
		"Function must return void");
	return typename traits::function_type(std::forward<F>(f));
}

#define WRAP_MEMBER(obj, func) wrap(obj, &std::remove_pointer_t<decltype(obj)>::func)
