if(waffle_has_wayland OR waffle_has_x11_egl OR waffle_has_gbm OR
   waffle_has_surfaceless_egl)
    set(waffle_has_egl TRUE)
else()
    set(waffle_has_egl FALSE)
endif()

if(waffle_has_glx OR waffle_has_x11_egl)
    set(waffle_has_x11 TRUE)
else()
    set(waffle_has_x11 FALSE)
endif()
