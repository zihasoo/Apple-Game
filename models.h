#pragma once
#include <coroutine>

struct DragRect {
	int left, top, right, bottom;
};

class Task {
public:
	struct promise_type {
		Task get_return_object() {
			return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
		}
		std::suspend_always initial_suspend() { return std::suspend_always{}; }
		void return_void() { return; }
		std::suspend_always final_suspend() const noexcept { return std::suspend_always{}; }
		void unhandled_exception() { exit(1); }
	};
	std::coroutine_handle<promise_type> co_handler;
	Task(std::coroutine_handle<promise_type> handler) : co_handler(handler) {};
	~Task() {
		if ((bool)co_handler) co_handler.destroy();
	}
};
