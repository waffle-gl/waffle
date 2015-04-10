// Copyright 2014 Intel Corporation
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/utility/threading/simple_thread.h"
#include "wcore_error.h"
#include <semaphore.h>

// Thread takes care that we do not issue another buffer
// swap before previous swap has completed.
class NaclSwapThread : public pp::SimpleThread
{
public:
    explicit NaclSwapThread(const pp::InstanceHandle &instance,
                            pp::Graphics3D *_ctx) :
        pp::SimpleThread(instance),
        ctx(_ctx),
        cbf(this)
    {
        Start();
        sem_init(&sem, 0, 0);
    }

    ~NaclSwapThread()
    {
        sem_destroy(&sem);
        message_loop().PostQuit(true);
    }

    bool swap()
    {
        pp::CompletionCallback cb =
            cbf.NewCallback(&NaclSwapThread::swap_buffers);

        if (message_loop().PostWork(cb) != PP_OK)
            return false;

        sem_wait(&sem);

        return true;
    }

private:

    void swap_buffers(int32_t result)
    {
        int32_t error = ctx->SwapBuffers(pp::BlockUntilComplete());
        switch (error) {
            case PP_OK:
                break;
            case PP_ERROR_NOMEMORY:
                wcore_errorf(WAFFLE_ERROR_BAD_ALLOC,
                             "pp::Graphics3D::SwapBuffers: Out of memory.");
                break;
            case PP_ERROR_CONTEXT_LOST:
                wcore_errorf(WAFFLE_ERROR_FATAL,
                             "pp::Graphics3D::SwapBuffers: 3D context lost.");
                break;
            default:
                wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                             "pp::Graphics3D::SwapBuffers: Unknown error.");
                break;
        }
        sem_post(&sem);
    }

    pp::Graphics3D *ctx;
    pp::CompletionCallbackFactory<NaclSwapThread> cbf;
    sem_t sem;
};
