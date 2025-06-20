#pragma once

#include <unordered_map>
#include <map>
#include <string>
#include "any.h"
#include "Log.h"
#include "function_traits.h"
#include "SupportAnyType.h"

enum class EHandlerExecType {
	sync,
	async,
	main,
};

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
	void SetHandlerExecFunc(EHandlerExecType type, std::function<void(std::function<void()>)> func) { handlerExecFunc_[type] = func; }
	template<typename Obj, typename F>
	void RegisterMsgHandler_(MsgType msg, Obj* obj, F&& func, int priority = 0) {
		globalMsgHandler_[msg][priority].emplace(obj, to_std_function(obj, std::forward<F>(func)));
	}
	template<typename ...Args>
	void TriggerMsg_(EHandlerExecType type, MsgType msg, Args&&... args) {
		auto handlerExecFunc = handlerExecFunc_.at(type);
		auto handlers = GetMsgHandler(msg, std::forward<Args>(args)...);
		for (auto& handler : handlers) {
			handlerExecFunc(handler);
		}
	}
	template<typename Callback, typename ...Args>
	void AsyncExecMsgAutoCallback_(MsgType msg, Callback callback, Args&&... args) {
		auto handlers = GetMsgHandler(msg, std::forward<Args>(args)...);
		auto func = [handlers, callback, this] {
			for (auto& handler : handlers) {
				handler();
			}
			handlerExecFunc_[EHandlerExecType::main](to_std_function(callback));
		};
		handlerExecFunc_[EHandlerExecType::async](func);
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
	std::vector<std::function<void()>> GetMsgHandler(MsgType msg, Args&&... args) {
		std::vector<std::function<void()>> ret;
		auto it = globalMsgHandler_.find(msg);
		if (it != globalMsgHandler_.end()) {
			PriorityMap& priorityMap = it->second;
			for (auto& priorityEntry : priorityMap) {
				HandlerMap& handlerMap = priorityEntry.second;
				for (auto& handlerEntry : handlerMap) {
					std::any& a = handlerEntry.second;
					ret.emplace_back(wrap_to_std_fuction(a, std::forward<Args>(args)...));
				}
			}
		}
		return ret;
	}
private:
	MsgHandler() = default;
	using HandlerMap = std::unordered_map<InstID, std::any>;
	using PriorityMap = std::map<int, HandlerMap, std::greater<int>>;
	std::unordered_map<MsgType, PriorityMap> globalMsgHandler_;
	using HandlerExecFunc = std::function<void(std::function<void()>)>;
	std::unordered_map<EHandlerExecType, HandlerExecFunc> handlerExecFunc_ = { { EHandlerExecType::sync, [](std::function<void()> func) { func(); } } };
};

#define RegisterMsgHandler(msg, ...) MsgHandler::GetInstance()->RegisterMsgHandler_(#msg, this, __VA_ARGS__)
#define ExecMsg(execType, msg, ...) MsgHandler::GetInstance()->TriggerMsg_(execType, msg, __VA_ARGS__)
#define SyncExecMsg(msg, ...) ExecMsg(EHandlerExecType::sync, #msg, __VA_ARGS__)
#define AsyncExecMsg(msg, ...)  ExecMsg(EHandlerExecType::async, #msg, __VA_ARGS__)
#define MainExecMsg(msg, ...)  ExecMsg(EHandlerExecType::main, #msg, __VA_ARGS__)

#define AsyncExecMsgAutoCallback(msg, ...)  MsgHandler::GetInstance()->AsyncExecMsgAutoCallback_(#msg, __VA_ARGS__)
#define Deregister() MsgHandler::GetInstance()->Deregister_(this)
