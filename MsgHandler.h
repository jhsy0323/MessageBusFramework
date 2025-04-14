#pragma once

#include <functional>
#include <unordered_map>
#include <map>
#include <string>
#include "any.h"
#include "Log.h"
#include "function_traits.h"

class MsgHandler {
#ifdef _DEBUG
	using MsgType = std::string;
#else
	using MsgType = const char*;
#endif
	using InstID = void*;
	friend class MsgHandlerUnitTest;
public:
	static MsgHandler* GetInstance() {
		static MsgHandler i;
		return &i;
	}
	void SetAsyncExecFunc(std::function<void(std::function<void()>)> func) { asyncExecFunc_ = func; }
	void SetMainExecFunc(std::function<void(std::function<void()>)> func) { mainExecFunc_ = func; }

	template<typename ...Args>
	void TriggerMsg_(MsgType msg, Args... args) {
		ExecMsgHandler(msg, defaultExecFunc_, args...);
	}
	template<typename ...Args>
	void AsyncExecMsg_(MsgType msg, Args... args) {
		if (!asyncExecFunc_) throw std::runtime_error("dont set async exec func");
		ExecMsgHandler(msg, asyncExecFunc_, args...);
	}
	template<typename ...Args>
	void MainExecMsg_(MsgType msg, Args... args) {
		if (!mainExecFunc_) throw std::runtime_error("dont set main exec func");
		ExecMsgHandler(msg, mainExecFunc_, args...);
	}
	template<typename F>
	void RegisterMsgHandler_(MsgType msg, InstID t, F func, int priority = 0) {
		globalMsgHandler_[msg][priority].emplace(t, to_std_function(func));
	}
	template<typename Obj, typename Ret, typename... Args>
	void RegisterMsgHandler_(MsgType msg, Obj* obj, Ret(Obj::*mem_func)(Args...), int priority = 0) {
		globalMsgHandler_[msg][priority].emplace(obj, wrap(obj, mem_func));
	}
	template<typename Obj, typename Ret, typename... Args>
	void RegisterMsgHandler_(MsgType msg, Obj* obj, Ret(Obj::*mem_func)(Args...) const, int priority = 0) {
		globalMsgHandler_[msg][priority].emplace(obj, wrap(obj, mem_func));
	}
	void Deregister_(InstID t) {
		for (auto& msgHandler : globalMsgHandler_) {
			PriorityMap& priorityMap = msgHandler.second;
			for (auto& priorityEntry : priorityMap) {
				HandlerMap& handlerMap = priorityEntry.second;
				handlerMap.erase(t);
			}
		}
	}
private:
	template<typename ...Args>
	void ExecMsgHandler(MsgType msg, std::function<void(std::function<void()>)> func, Args... args) {
		auto instHandler = GetMsgInstHandler(msg);
		for (auto& handlerEntry : instHandler) {
			std::pair<const InstID, std::any>& entry = handlerEntry;
			auto& handler = entry.second;
			auto fun = std::bind([&handler](Args... args) {
				try {
					auto func = std::any_cast<std::function<void(Args...)>>(handler);
					func(args...);
				}
				catch (std::runtime_error) {
					auto func = std::any_cast<std::function<void()>>(handler);
					func();
				}
			}, args...);
			func(fun);
		}
	}

	std::vector<std::reference_wrapper<std::pair<const InstID, std::any>>> GetMsgInstHandler(MsgType msg) {
		std::vector<std::reference_wrapper<std::pair<const InstID, std::any>>> ret;
		auto it = globalMsgHandler_.find(msg);
		if (it != globalMsgHandler_.end()) {
			PriorityMap& priorityMap = it->second;
			for (auto& priorityEntry : priorityMap) {
				HandlerMap& handlerMap = priorityEntry.second;
				for (auto& handlerEntry : handlerMap) {
					ret.push_back(handlerEntry);
				}
			}
		}
		return ret;
	}
private:
	MsgHandler() = default;
	using HandlerMap = std::unordered_map<InstID, std::any>;
	using PriorityMap = std::map<int, HandlerMap, std::greater<int>>;
	std::function<void(std::function<void()>)> defaultExecFunc_ = [](std::function<void()> func) { func(); };
	std::function<void(std::function<void()>)> asyncExecFunc_;
	std::function<void(std::function<void()>)> mainExecFunc_;
	std::unordered_map<MsgType, PriorityMap> globalMsgHandler_;
};

#define RegisterMsgHandler(msg, ...) MsgHandler::GetInstance()->RegisterMsgHandler_(#msg, this, __VA_ARGS__)
#define TriggerMsg(msg, ...) MsgHandler::GetInstance()->TriggerMsg_(#msg, __VA_ARGS__)
#define AsyncExecMsg(msg, ...)  MsgHandler::GetInstance()->AsyncExecMsg_(#msg, __VA_ARGS__)
#define MainExecMsg(msg, ...)  MsgHandler::GetInstance()->MainExecMsg_(#msg, __VA_ARGS__)
#define Deregister() MsgHandler::GetInstance()->Deregister_(this)
