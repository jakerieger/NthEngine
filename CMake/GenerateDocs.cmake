project(NthEngine)

function(GenerateDocs)
    execute_process(
            COMMAND doxygen ${CMAKE_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE DOXYGEN_RESULT
            OUTPUT_VARIABLE DOXYGEN_OUTPUT
            ERROR_VARIABLE DOXYGEN_ERROR
    )

    if (NOT DOXYGEN_RESULT EQUAL 0)
        message(WARNING "Doxygen generation failed: ${DOXYGEN_ERROR}")
    else ()
        message("-- Generated documentation: ${DOXYGEN_OUTPUT}")
    endif ()
endfunction()