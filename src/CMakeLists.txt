# Create new project
project (libLSH)

# Create variable containing all the CXX sources
set(SOURCES
    characteristic_matrix.cpp
    minhash_signature.cpp
)

# Add all the sources to the project
add_library(${PROJECT_NAME} ${SOURCES})
