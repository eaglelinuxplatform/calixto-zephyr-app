if(CONFIG_IOL_MASTER)

    zephyr_library()
    zephyr_library_sources(
        src/iol_mw.c
        src/iol_dl.c
        src/iol_sm.c
        src/iol_main.c
        src/iol_log.c
    )

endif()