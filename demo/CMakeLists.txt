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
