
/*
The latest C++ Standard of 2020 has new
keywords such as 'co_yield' and 'co_return'
to accommodate coroutines, however the standard
library is still lacking the required supporting
types. For the time being, until the supporting
types are added in C++23, I will use the free
open-source 'Generator' class defined in this
header file.
*/

#pragma once

#include <coroutine>
#include <stdexcept>

template<typename T>
struct Generator {

  struct promise_type;

  typedef std::coroutine_handle<promise_type> handle_type;

  struct promise_type {
    T value_;
    std::exception_ptr exception_;

    Generator get_return_object()
    {
      return Generator(handle_type::from_promise(*this));
    }

    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() { exception_ = std::current_exception(); }

    template<typename From>
    std::suspend_always yield_value(From &&from)
    {
      value_ = std::forward<From>(from);
      return {};
    }

    void return_void() {}
  };

  handle_type h_;

  Generator(handle_type h) : h_(h) {}
  ~Generator() { h_.destroy(); }

  explicit operator bool()
  {
    fill();
    return !h_.done();
  }

  T operator()()
  {
    fill();
    full_ = false;
    return std::move(h_.promise().value_);
  }

private:

  bool full_ = false;

  void fill()
  {
    if ( !full_ )
    {
      h_();
      
      if (h_.promise().exception_)
      {
        std::rethrow_exception(h_.promise().exception_);
      }
      full_ = true;
    }
  }
};

