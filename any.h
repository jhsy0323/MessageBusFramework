#pragma once
#include <typeinfo>
#include <memory>
#include <tuple>
namespace std {
	class any
	{
		class AnyHelperBase
		{
		public:
			virtual const std::type_info& type()const = 0;
			virtual AnyHelperBase* clone()const = 0;
		};

		template<typename T>
		class AnyHelper :public AnyHelperBase
		{
		public:
			T data;
			template<typename ...Args>
			AnyHelper(Args&&... args) :data(std::forward<Args>(args)...) {}
			AnyHelper(const AnyHelper& rhs) :data(rhs.data) {}
			AnyHelper(const T& value) :data(value) {}
			virtual const std::type_info& type() const
			{
				return typeid(T);
			}
			virtual AnyHelper* clone() const
			{
				return new AnyHelper(*this);
			}
		};
		template<typename T>
		friend T any_cast(const any& a);
		template<typename T>
		friend T* any_cast(const any* a);
	private:
		std::unique_ptr<AnyHelperBase> pdata{};
	public:
		any() :pdata(nullptr) {}
		template<typename T>
		any(T&& value) : pdata(new AnyHelper<std::decay_t<T>>(value)) {}
		any(const any& rhs) {
			if (rhs.pdata != nullptr) {
				pdata.reset(rhs.pdata->clone());
			}
		}
		any(any& rhs) {
			if (rhs.pdata != nullptr) {
				pdata.reset(rhs.pdata->clone());
			}
		}
		any(any&& rhs) :pdata(rhs.pdata.release()) {}

		const std::type_info& type() const
		{
			return pdata->type();
		}
		bool has_value() const {
			return pdata != nullptr;
		}
		void reset() {
			pdata.reset();
		}
		template<typename T>
		any& operator=(T value) {
			pdata.reset(new AnyHelper<std::decay_t<T>>(value));
			return *this;
		}
		any& operator=(any rhs)
		{
			pdata.reset(rhs.pdata->clone());
			return *this;
		}
	};

	template<typename T>
	T any_cast(const any& a)
	{
		auto p = dynamic_cast<any::AnyHelper<std::decay_t<T>>*>(a.pdata.get());
		if (p == nullptr)
			throw std::runtime_error("Bad any cast!");
		return p->data;
	}

	template<typename T>
	T* any_cast(const any* a)
	{
		auto p = dynamic_cast<any::AnyHelper<std::decay_t<T>>*>(a->pdata.get());
		if (p == nullptr)
			return nullptr;
		return &p->data;
	}

	template<typename Fn, typename Tuple, size_t... I>
	auto apply_impl(Fn&& f, Tuple&& t, index_sequence<I...>) {
		return std::forward<Fn>(f)(std::get<I>(std::forward<Tuple>(t))...);
	}
	template<typename Fn, typename Tuple>
	auto apply(Fn&& f, Tuple&& t) {
		return apply_impl(
			std::forward<Fn>(f),
			std::forward<Tuple>(t),
			make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{}
		);
	}
}
