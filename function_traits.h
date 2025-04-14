#pragma once

template<typename T>
struct function_traits;

template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
	using return_type = R;
	using args_type = std::tuple<Args...>;
	using function_type = std::function<R(Args...)>;
};

template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(*)(Args...)> {};

template<typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)const> : function_traits<R(*)(Args...)> {};

template<typename T>
struct function_traits : function_traits<decltype(&T::operator())> {};

template<typename F>
auto to_std_function(F&& f) {
	using traits = function_traits<std::decay_t<F>>;
	static_assert(std::is_same_v<typename traits::return_type, void>,
		"Function must return void");
	return typename traits::function_type(std::forward<F>(f));
}

template<typename Obj, typename Ret, typename... Args>
std::function<Ret(Args...)> wrap(Obj* obj, Ret(Obj::*mem_func)(Args...)) {
	return [obj, mem_func](Args&&... args) -> Ret {
		return (obj->*mem_func)(std::forward<Args>(args)...);
	};
}

template<typename Obj, typename R, typename... Args>
std::function<R(Args...)> wrap(Obj* obj, R(Obj::*mem_func)(Args...) const) {
	return [obj, mem_func](Args&&... args) -> R {
		return (obj->*mem_func)(std::forward<Args>(args)...);
	};
}
#define WRAP_MEMBER(obj, func) wrap(obj, &std::remove_pointer_t<decltype(obj)>::func)
