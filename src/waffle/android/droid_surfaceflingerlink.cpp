// Copyright 2012 Intel Corporation
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

#if WAFFLE_ANDROID_MAJOR_VERSION == 4 && \
    WAFFLE_ANDROID_MINOR_VERSION >= 1 || \
    WAFFLE_ANDROID_MAJOR_VERSION >= 5
#	include <gui/Surface.h>
#	include <gui/SurfaceComposerClient.h>
#elif WAFFLE_ANROID_MAJOR_VERSION >= 4 && \
	  WAFFLE_ANDROID_MINOR_VERSION == 0
#	include <surfaceflinger/SurfaceComposerClient.h>
#else
#	error "android >= 4.0 is required"
#endif

#include "droid_surfaceflingerlink.h"

#include "wcore_util.h"
#include "wcore_error.h"

using namespace android;

namespace waffle {

struct droid_surfaceflinger_container {
    sp<SurfaceComposerClient> composer_client;
};

struct droid_ANativeWindow_container {
    // it is important ANativeWindow* is the first element in this structure
    ANativeWindow* native_window;
    sp<SurfaceControl> surface_control;
    sp<ANativeWindow> window;
};

const uint32_t droid_magic_surface_width = 32;
const uint32_t droid_magic_surface_height = 32;
const int32_t  droid_magic_surface_z = 0x7FFFFFFF;

void droid_tear_down_surfaceflinger_link(
    droid_surfaceflinger_container* pSFContainer);

droid_surfaceflinger_container*
droid_setup_surfaceflinger_link()
{
    bool bRVal;
    EGLint iRVal;
    droid_surfaceflinger_container* pSFContainer;

    pSFContainer = new droid_surfaceflinger_container;

    if (pSFContainer == NULL)
        goto error;

    pSFContainer->composer_client = new SurfaceComposerClient;
    iRVal = pSFContainer->composer_client->initCheck();
    if (iRVal != NO_ERROR) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "android::SurfaceComposerClient->initCheck != NO_ERROR");
        goto error;
    }

    return pSFContainer;
error:
    droid_tear_down_surfaceflinger_link(pSFContainer);
    return NULL;
}

droid_ANativeWindow_container*
droid_setup_surface(
    int width,
    int height,
    droid_surfaceflinger_container* pSFContainer)
{
    bool bRVal;
    EGLint iRVal;

    droid_ANativeWindow_container* pANWContainer;

    pANWContainer = new droid_ANativeWindow_container;

    if (pANWContainer == NULL)
        goto error;

    // The signature of SurfaceComposerClient::createSurface() differs across
    // Android versions.
#if WAFFLE_ANDROID_MAJOR_VERSION == 4 &&        \
    WAFFLE_ANDROID_MINOR_VERSION >= 2 || \
    WAFFLE_ANDROID_MAJOR_VERSION >= 5
    pANWContainer->surface_control =
            pSFContainer->composer_client->createSurface(
            String8("Waffle Surface"),
            droid_magic_surface_width, droid_magic_surface_height,
            PIXEL_FORMAT_RGB_888, 0);
#else
    pANWContainer->surface_control =
            pSFContainer->composer_client->createSurface(
            String8("Waffle Surface"), 0,
            droid_magic_surface_width, droid_magic_surface_height,
            PIXEL_FORMAT_RGB_888, 0);
#endif

    if (pANWContainer->surface_control == NULL) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "Unable to get android::SurfaceControl");
        delete pANWContainer;
        goto error;
    }

    bRVal = pANWContainer->surface_control->isValid();
    if (bRVal != true) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "Acquired android::SurfaceControl is invalid");
        delete pANWContainer;
        goto error;
    }

    pSFContainer->composer_client->openGlobalTransaction();
    iRVal = pANWContainer->surface_control->setLayer(droid_magic_surface_z);
    if (iRVal != NO_ERROR) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "Error in android::SurfaceControl->setLayer");
        delete pANWContainer;
        goto error_closeTransaction;
    }

    pSFContainer->composer_client->closeGlobalTransaction();

    pANWContainer->window = pANWContainer->surface_control->getSurface();

    pSFContainer->composer_client->openGlobalTransaction();
    iRVal = pANWContainer->surface_control->setSize(width, height);
    pSFContainer->composer_client->closeGlobalTransaction();

    if (iRVal != NO_ERROR) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "error in android::SurfaceControl->setSize");
        delete pANWContainer;
        goto error;
    }

    pANWContainer->native_window = pANWContainer->window.get();

    return pANWContainer;

error_closeTransaction:
    pSFContainer->composer_client->closeGlobalTransaction();

error:
    droid_tear_down_surfaceflinger_link(pSFContainer);
    return NULL;
}

bool
droid_show_surface(
    droid_surfaceflinger_container* pSFContainer,
    droid_ANativeWindow_container* pANWContainer)
{
    int iRVal;

    pSFContainer->composer_client->openGlobalTransaction();
    iRVal = pANWContainer->surface_control->show();
    pSFContainer->composer_client->closeGlobalTransaction();

    if (iRVal != NO_ERROR) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "Error in android::SurfaceControl->show");
        return false;
    }
    return true;
}

bool
droid_resize_surface(
    droid_surfaceflinger_container* pSFContainer,
    droid_ANativeWindow_container* pANWContainer,
    int width,
    int height)
{
    int iRVal;

    pSFContainer->composer_client->openGlobalTransaction();
    iRVal = pANWContainer->surface_control->setSize(width, height);
    pSFContainer->composer_client->closeGlobalTransaction();

    if (iRVal != NO_ERROR) {
        wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                     "Error in android::SurfaceControl->setSize");
        return false;
    }
    return true;
}

void
droid_destroy_surface(
    droid_surfaceflinger_container* pSFContainer,
    droid_ANativeWindow_container* pANWContainer)
{
    pSFContainer->composer_client->openGlobalTransaction();
    pANWContainer->surface_control->clear();
    pSFContainer->composer_client->closeGlobalTransaction();
    delete pANWContainer;
}


void
droid_tear_down_surfaceflinger_link(
                waffle::droid_surfaceflinger_container* pSFContainer)
{
    if( pSFContainer == NULL)
        return;

    if (pSFContainer->composer_client != NULL) {
        pSFContainer->composer_client->dispose();
        pSFContainer->composer_client = NULL;
    }

    delete pSFContainer;
}

}; // namespace waffle

extern "C" struct droid_surfaceflinger_container*
droid_init_gl()
{
    return reinterpret_cast<droid_surfaceflinger_container*>(
                waffle::droid_setup_surfaceflinger_link());
}
extern "C" bool
droid_deinit_gl(droid_surfaceflinger_container* pSFContainer)
{
    waffle::droid_tear_down_surfaceflinger_link(
                reinterpret_cast<waffle::droid_surfaceflinger_container*>
                (pSFContainer));
    return true;
}

extern "C" droid_ANativeWindow_container*
droid_create_surface(
    int width,
    int height,
    droid_surfaceflinger_container* pSFContainer)
{
    waffle::droid_ANativeWindow_container* container =
        waffle::droid_setup_surface(
            width, height,
            reinterpret_cast<waffle::droid_surfaceflinger_container*> (pSFContainer));
    return reinterpret_cast<droid_ANativeWindow_container*>(container);
}

extern "C" bool
droid_show_surface(
    droid_surfaceflinger_container* pSFContainer,
    droid_ANativeWindow_container* pANWContainer)
{
    return waffle::droid_show_surface(
            reinterpret_cast<waffle::droid_surfaceflinger_container*>
            (pSFContainer),
            reinterpret_cast<waffle::droid_ANativeWindow_container*>
            (pANWContainer));
}

extern "C" bool
droid_resize_surface(
    droid_surfaceflinger_container* pSFContainer,
    droid_ANativeWindow_container* pANWContainer,
    int height,
    int width)
{
    return waffle::droid_resize_surface(
            reinterpret_cast<waffle::droid_surfaceflinger_container*>
            (pSFContainer),
            reinterpret_cast<waffle::droid_ANativeWindow_container*>
            (pANWContainer), height, width);
}

extern "C" void
droid_destroy_surface(
    droid_surfaceflinger_container* pSFContainer,
    droid_ANativeWindow_container* pANWContainer)
{
    return waffle::droid_destroy_surface(
            reinterpret_cast<waffle::droid_surfaceflinger_container*>
            (pSFContainer),
            reinterpret_cast<waffle::droid_ANativeWindow_container*>
            (pANWContainer));
}
