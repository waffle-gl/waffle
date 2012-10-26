headers := \
    EGL/egl.h \
    EGL/eglext.h \
    EGL/eglplatform.h \
    KHR/khrplatform.h \

.PHONY: khronos-headers
khronos-headers: $(headers)

EGL/egl.h EGL/eglext.h KHR/khrplatform.h: FORCE
	wget -O $@ "http://www.khronos.org/registry/egl/api/$@"

EGL/eglplatform.h: FORCE EGL/eglplatform.h.patch
	wget -O $@ "http://www.khronos.org/registry/egl/api/$@"
	patch $@ < $@.patch

FORCE:
