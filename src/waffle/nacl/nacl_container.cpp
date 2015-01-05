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

#include "ppapi/cpp/graphics_3d.h"
#include "nacl_container.h"

namespace waffle {

struct nacl_container {
    pp::Graphics3D ctx;
};

static void
nacl_container_dtor(waffle::nacl_container *nc)
{
    if (!nc)
        return;
    delete nc;
}

static nacl_container*
nacl_container_ctor()
{
    nacl_container *nc = new nacl_container;

    if (!nc)
        return NULL;

    return nc;
}

}; // namespace waffle ends

extern "C" struct nacl_container*
nacl_init()
{
    return reinterpret_cast<nacl_container*>(waffle::nacl_container_ctor());
}

extern "C" void
nacl_teardown(nacl_container *nc)
{
    waffle::nacl_container_dtor(reinterpret_cast<waffle::nacl_container*>(nc));
}
