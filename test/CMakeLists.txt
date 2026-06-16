add_executable(vix_error_test
  error_test.cpp
)

target_compile_features(vix_error_test PRIVATE cxx_std_20)

target_link_libraries(vix_error_test
  PRIVATE
    vix::error
)

if (VIX_ENABLE_SANITIZERS AND NOT MSVC)
  target_compile_options(vix_error_test
    PRIVATE
      -fno-omit-frame-pointer
      -fsanitize=address,undefined
  )

  target_link_options(vix_error_test
    PRIVATE
      -fsanitize=address,undefined
  )
endif()

add_test(NAME vix_error_test COMMAND vix_error_test)
