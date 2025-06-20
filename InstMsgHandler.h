#pragma once

#include <unordered_map>
#include <functional>
#include <unordered_set>
#include "any.h"
#include "function_traits.h"
#include "SupportAnyType.h"

class InstMsgHandler {
#ifdef _DEBUG
	using MsgType = std::string;
#else
	using MsgType = const char*;
#endif
	using InstID = void*;
	friend class InstMsgHandlerUnitTest;
public:
	static InstMsgHandler* GetInstance() {
		static InstMsgHandler i;
		return &i;
	}

	template<typename ...Args>
	void XTriggerMsg_(MsgType msg, InstID trigger, Args&&... args) {
		auto findHandler = globalMsgHandler_.find(msg);
		if (findHandler != globalMsgHandler_.end()) {
			auto it = communicators_.find(trigger);
			if (it != communicators_.end()) {
				HandlerMap& handlers = findHandler->second;
				for (const auto& communicator : it->second) {
					auto handler = handlers.find(communicator);
					if (handler != handlers.end()) {
						auto& func = handler->second;
						wrap_to_std_fuction(func, std::forward<Args>(args)...)();
					}
				}
			}
		}
	}
	template<typename Obj, typename F>
	void XRegisterMsgHandler_(MsgType msg, Obj* obj, F&& func) {
		globalMsgHandler_[msg].emplace(obj, to_std_function(obj, std::forward<F>(func)));
	}
	void XConnect_(InstID c1, InstID c2) {
		if (c1 == c2) {
			communicators_[c1].insert(c2);
			return;
		}
		communicators_[c1].insert(c2);
		communicators_[c2].insert(c1);
	}
	void XDisconnect_(InstID c1, InstID c2) {
		if (c1 == c2) {
			communicators_[c1].erase(c2);
			return;
		}
		communicators_[c1].erase(c2);
		communicators_[c2].erase(c1);
	}
	void XDeregister_(InstID t) {
		XDisconnect_(t);
		DeregisterMsgHandler(t);
	}
private:
	void XDisconnect_(InstID t) {
		auto it = communicators_.find(t);
		if (it != communicators_.end()) {
			auto communicators = it->second;
			for (auto communicator : communicators) {
				communicators_[communicator].erase(t);
			}
		}
		communicators_.erase(t);
	}
	void DeregisterMsgHandler(InstID t) {
		for (auto& msgHandler : globalMsgHandler_) {
			HandlerMap& handlerMap = msgHandler.second;
			handlerMap.erase(t);
		}
	}
private:
	InstMsgHandler() = default;
	using HandlerMap = std::unordered_map<InstID, std::any>;
	std::unordered_map<InstID, std::unordered_set<InstID>> communicators_;
	std::unordered_map<MsgType, HandlerMap> globalMsgHandler_;
};

#define EXPAND(...) __VA_ARGS__
#define COUNT_ARGS_IMPL(_1, _2, _3, N, ...) N
#define COUNT_ARGS(...) EXPAND(COUNT_ARGS_IMPL(__VA_ARGS__, 3, 2, 1, 0))
#define XCONNECT_IMPL_1(a) InstMsgHandler::GetInstance()->XConnect_(this, a)
#define XCONNECT_IMPL_2(a, b) InstMsgHandler::GetInstance()->XConnect_(a, b)
#define XCONNECT_IMPL(n) XCONNECT_IMPL_##n
#define XCONNECT_DISPATCH(n, ...) EXPAND(XCONNECT_IMPL(n)(__VA_ARGS__))
#define XConnect(...) XCONNECT_DISPATCH(EXPAND(COUNT_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define XDISCONNECT_IMPL_1(a) InstMsgHandler::GetInstance()->XDisconnect_(this, a)
#define XDISCONNECT_IMPL_2(a, b) InstMsgHandler::GetInstance()->XDisconnect_(a, b)
#define XDISCONNECT_IMPL(n) XDISCONNECT_IMPL_##n
#define XDISCONNECT_DISPATCH(n, ...) EXPAND(XDISCONNECT_IMPL(n)(__VA_ARGS__))
#define XDisconnect(...) XDISCONNECT_DISPATCH(EXPAND(COUNT_ARGS(__VA_ARGS__)), __VA_ARGS__)

#define XRegisterMsgHandler(msg, ...) InstMsgHandler::GetInstance()->XRegisterMsgHandler_(#msg, this, __VA_ARGS__)
#define XTriggerMsg(msg, ...) InstMsgHandler::GetInstance()->XTriggerMsg_(#msg, this, __VA_ARGS__)
#define XDeregister() InstMsgHandler::GetInstance()->XDeregister_(this)
