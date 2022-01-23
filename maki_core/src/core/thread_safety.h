#pragma once

namespace Maki {

#ifndef NDEBUG

enum class ThreadType {
    undefined = 0,
    control   = 1,
    render    = 2,
};

extern thread_local ThreadType g_thread_type;

#define SET_THREAD_TYPE_CONTROL() g_thread_type = ThreadType::control
#define SET_THREAD_TYPE_RENDER()  g_thread_type = ThreadType::render

// have to be macros to get correct line number, function name, etc. in log
#define ASSERT_CONTROL_THREAD() MAKI_ASSERT_CRITICAL(g_thread_type == ThreadType::control, "This function can only be called from the control thread.")
#define ASSERT_RENDER_THREAD()  MAKI_ASSERT_CRITICAL(g_thread_type == ThreadType::render, "This function can only be called from the render thread.")

#else

#define SET_THREAD_TYPE_CONTROL()
#define SET_THREAD_TYPE_RENDER()

#define ASSERT_CONTROL_THREAD()
#define ASSERT_RENDER_THREAD()

#endif

} // namespace Maki
