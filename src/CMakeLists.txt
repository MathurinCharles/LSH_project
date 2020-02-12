# Create new project
project (libLSH)

# Create variable containing all the CXX sources
set(SOURCES
    characteristic_matrix.cpp
    minhash_signature.cpp
)

add_custom_command(TARGET ${SOURCES} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/compute_cm.cl
                       $<TARGET_FILE_DIR:${SOURCES}>
                   )

# Add all the sources to the project
add_library(${PROJECT_NAME} ${SOURCES})
