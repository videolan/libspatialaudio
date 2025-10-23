# Tests

function(spaudio_add_test name)
    add_executable(${name} "${name}.cpp")
    target_link_libraries(${name} PRIVATE spatialaudio-shared)
    add_test(NAME ${name}
             COMMAND $<TARGET_FILE:${name}>)
endfunction()

#add_executable(TestInsideAngleRange tests/TestInsideAngleRange.cpp)
#target_link_libraries(TestInsideAngleRange PRIVATE spatialaudio-static)
#add_test(NAME TestInsideAngleRange
#         COMMAND $<TARGET_FILE:TestInsideAngleRange>)

spaudio_add_test(TestInsideAngleRange)
