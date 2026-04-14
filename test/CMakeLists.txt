add_executable(vix_error_test
  error_test.cpp
)

target_compile_features(vix_error_test PRIVATE cxx_std_20)

target_link_libraries(vix_error_test
  PRIVATE
    vix::error
)

add_test(NAME vix_error_test COMMAND vix_error_test)
