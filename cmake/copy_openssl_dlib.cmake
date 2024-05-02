if(NOT DEFINED OPENSSL_USE_STATIC_LIBS OR NOT ${OPENSSL_USE_STATIC_LIBS})
    get_filename_component(OPENSSL_ROOT_DIR ${OPENSSL_INCLUDE_DIR} DIRECTORY)
    set(OPENSSL_BIN_DIR "${OPENSSL_ROOT_DIR}/bin")
    # get openssl bin dir to search dynamic libs

    file(GLOB OPENSSL_CRYPTO_DLL "${OPENSSL_BIN_DIR}/libcrypto*.dll")
    file(GLOB OPENSSL_SSL_DLL "${OPENSSL_BIN_DIR}/libssl*.dll")
    # get target dlls with special name tags

    get_filename_component(OPENSSL_CRYPTO_DLL_NAME ${OPENSSL_CRYPTO_DLL} NAME)
    get_filename_component(OPENSSL_SSL_DLL_NAME ${OPENSSL_SSL_DLL} NAME)

    if(NOT EXISTS "${EXECUTABLE_OUTPUT_PATH}/${OPENSSL_CRYPTO_DLL_NAME}")
        message(STATUS "Cannot find ${OPENSSL_CRYPTO_DLL_NAME} under executable output dir, copy it")
        file(COPY ${OPENSSL_CRYPTO_DLL}
            DESTINATION ${EXECUTABLE_OUTPUT_PATH}
        )
    endif()
    if(NOT EXISTS "${EXECUTABLE_OUTPUT_PATH}/${OPENSSL_SSL_DLL_NAME}")
        message(STATUS "Cannot find ${OPENSSL_SSL_DLL_NAME} under executable output dir, copy it")
        file(COPY ${OPENSSL_SSL_DLL}
            DESTINATION ${EXECUTABLE_OUTPUT_PATH}
        )
    endif()
endif()
