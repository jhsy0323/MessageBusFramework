#pragma once

#include <typeinfo>
#include <memory>
namespace std {
class any
{
	class AnyHelperBase
	{
	public:
		virtual const std::type_info& type()const = 0;
		virtual AnyHelperBase* clone()const = 0;
		virtual ~AnyHelperBase() {}
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
private:
	std::unique_ptr<AnyHelperBase> pdata{};
public:
	any() :pdata(nullptr) {}
	template<typename T>
	any(T&& value) : pdata(new AnyHelper<std::decay_t<T>>(value)) {}
	explicit any(const any& rhs) {
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
}

