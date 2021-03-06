/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <AK/Time.h>
#include <Kernel/Process.h>

namespace Kernel {

void compute_relative_timeout_from_absolute(const timeval& absolute_time, timeval& relative_time)
{
    // Convert absolute time to relative time of day.
    timeval_sub(absolute_time, kgettimeofday(), relative_time);
}

void compute_relative_timeout_from_absolute(const timespec& absolute_time, timeval& relative_time)
{
    timeval tv_absolute_time;
    timespec_to_timeval(absolute_time, tv_absolute_time);
    compute_relative_timeout_from_absolute(tv_absolute_time, relative_time);
}

WaitQueue& Process::futex_queue(i32* userspace_address)
{
    auto& queue = m_futex_queues.ensure((FlatPtr)userspace_address);
    if (!queue)
        queue = make<WaitQueue>();
    return *queue;
}

int Process::sys$futex(const Syscall::SC_futex_params* user_params)
{
    REQUIRE_PROMISE(thread);

    Syscall::SC_futex_params params;
    if (!validate_read_and_copy_typed(&params, user_params))
        return -EFAULT;

    i32* userspace_address = params.userspace_address;
    int futex_op = params.futex_op;
    i32 value = params.val;
    const timespec* user_timeout = params.timeout;

    if (!validate_read_typed(userspace_address))
        return -EFAULT;

    if (user_timeout && !validate_read_typed(user_timeout))
        return -EFAULT;

    switch (futex_op) {
    case FUTEX_WAIT: {
        i32 user_value;
        copy_from_user(&user_value, userspace_address);
        if (user_value != value)
            return -EAGAIN;

        timespec ts_abstimeout { 0, 0 };
        if (user_timeout && !validate_read_and_copy_typed(&ts_abstimeout, user_timeout))
            return -EFAULT;

        WaitQueue& wait_queue = futex_queue(userspace_address);
        timeval* optional_timeout = nullptr;
        timeval relative_timeout { 0, 0 };
        if (user_timeout) {
            compute_relative_timeout_from_absolute(ts_abstimeout, relative_timeout);
            optional_timeout = &relative_timeout;
        }

        // FIXME: This is supposed to be interruptible by a signal, but right now WaitQueue cannot be interrupted.
        Thread::BlockResult result = Thread::current()->wait_on(wait_queue, "Futex", optional_timeout);
        if (result == Thread::BlockResult::InterruptedByTimeout) {
            return -ETIMEDOUT;
        }

        break;
    }
    case FUTEX_WAKE:
        if (value == 0)
            return 0;
        if (value == 1) {
            futex_queue(userspace_address).wake_one();
        } else {
            futex_queue(userspace_address).wake_n(value);
        }
        break;
    }

    return 0;
}

}
