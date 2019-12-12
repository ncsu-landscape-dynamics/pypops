find_package(Python3 COMPONENTS Interpreter)

add_test(NAME test_pypops
    COMMAND env PYTHONPATH=${PROJECT_BINARY_DIR} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_pypops.py)
