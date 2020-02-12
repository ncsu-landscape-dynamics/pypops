find_package(Python3 COMPONENTS Interpreter)

add_test(NAME test_new_and_modify_array
    COMMAND env PYTHONPATH=${PROJECT_BINARY_DIR} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_new_and_modify_array.py)

add_test(NAME test_new_and_modify_matrix
    COMMAND env PYTHONPATH=${PROJECT_BINARY_DIR} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_new_and_modify_matrix.py)

add_test(NAME test_pypops
    COMMAND env PYTHONPATH=${PROJECT_BINARY_DIR} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_type_compatibility.py)

add_test(NAME test_simulation_function_principles
    COMMAND env PYTHONPATH=${PROJECT_BINARY_DIR} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_simulation_function_principles.py)

add_test(NAME test_simulation_function
    COMMAND env PYTHONPATH=${PROJECT_BINARY_DIR} ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_simulation_function.py)
