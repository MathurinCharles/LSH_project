cmake_minimum_required (VERSION 2.8.11)
project(Demo)

find_package(Boost REQUIRED system filesystem)

add_executable(Jsim Jsim.cpp)
add_executable(minHash_Similarity minHash_Similarity.cpp)
add_executable(LSH LSH.cpp)

target_link_libraries(Jsim PUBLIC ${Boost_LIBRARIES} ${OpenCL_LIBRARY} libLSH)
target_include_directories(Jsim PUBLIC src)

target_link_libraries(minHash_Similarity PUBLIC ${Boost_LIBRARIES} ${OpenCL_LIBRARY} libLSH)
target_include_directories(minHash_Similarity PUBLIC src)

target_link_libraries(LSH PUBLIC ${Boost_LIBRARIES} ${OpenCL_LIBRARY} libLSH)
target_include_directories(LSH PUBLIC src)

add_custom_command(TARGET Jsim POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/write_sim.cl
                       $<TARGET_FILE_DIR:Jsim>)
