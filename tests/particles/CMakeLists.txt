add_executable(test_particles tests_particles.cpp)
add_test(test_particles test_particles)
target_link_libraries(test_particles cloudphxx_lgrngn)
