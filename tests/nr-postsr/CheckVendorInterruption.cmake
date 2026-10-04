execute_process(COMMAND "${EXE}" "${NR_DLL}" "${NGX_CORE}" --vendor-interrupt-visible "${RUNTIME}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors TIMEOUT 180)
file(WRITE "${LOG}" "${output}\n${errors}")
if(result STREQUAL "77" AND NOT output MATCHES "VENDOR_INTERRUPT_INJECTED present=631")
    message("NOT QUALIFIED: actual foreground unavailable before the injected interruption")
    # An unavailable prerequisite is not a successful cleanup test.
    message(FATAL_ERROR "Foreground prerequisite unavailable; log: ${LOG}")
endif()
if(NOT result STREQUAL "77" OR
   NOT output MATCHES "VENDOR_INTERRUPT_CLEANUP retired=1 closed=1 priorDoubles=[1-9][0-9]* apiErrors=0" OR
   NOT output MATCHES "VENDOR_INTERRUPT_INJECTED present=631" OR
   NOT output MATCHES "NOT_QUALIFIED foreground precondition" OR
   output MATCHES "(^|\n)FAIL " OR errors MATCHES "(^|\n)FAIL ")
    message(FATAL_ERROR "Interrupted vendor cleanup failed: exit=${result}; log: ${LOG}")
endif()
message("PASS: injected interruption remains unqualified while vendor owners retire before process exit")
