# Tests

function(spaudio_add_test name)
    add_executable(${name} "${name}.cpp")
    target_link_libraries(${name} PRIVATE spatialaudio)
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
    set_property(TEST ${name}
        PROPERTY
            ENVIRONMENT_MODIFICATION
            "PATH=path_list_append:$<TARGET_RUNTIME_DLL_DIRS:${name}>")
endfunction()

spaudio_add_test(TestInsideAngleRange)
spaudio_add_test(TestAmbisonicShelfFilters)
spaudio_add_test(TestAmbisonicDecoderPresets)
spaudio_add_test(TestRenderer)
