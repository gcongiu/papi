##
## Copyright (C) by Innovative Computing Laboratory
##     See copyright in top-level directory
##

LDADD = src/utils/cost_utils.o                     \
        src/utils/print_header.o                   \
        $(top_builddir)/lib/lib@PAPILIBNAME@.la    \
        $(top_builddir)/src/testlib/libtestlib.la  \
        -lpfm -lm

bin_PROGRAMS += src/utils/papi_avail               \
                src/utils/papi_clockres            \
                src/utils/papi_command_line        \
                src/utils/papi_component_avail     \
                src/utils/papi_cost                \
                src/utils/papi_decode              \
                src/utils/papi_error_codes         \
                src/utils/papi_event_chooser       \
                src/utils/papi_mem_info            \
                src/utils/papi_multiplex_cost      \
                src/utils/papi_native_avail        \
                src/utils/papi_version             \
                src/utils/papi_xml_event_info

src_utils_papi_avail_LDFLAGS           = @TOOLS_LINK_MODE@
src_utils_papi_clockres_LDFLAGS        = @TOOLS_LINK_MODE@
src_utils_papi_command_line_LDFLAGS    = @TOOLS_LINK_MODE@
src_utils_papi_component_avail_LDFLAGS = @TOOLS_LINK_MODE@
src_utils_papi_cost_LDFLAGS            = @TOOLS_LINK_MODE@
src_utils_papi_decode_LDFLAGS          = @TOOLS_LINK_MODE@
src_utils_papi_error_codes_LDFLAGS     = @TOOLS_LINK_MODE@
src_utils_papi_event_chooser_LDFLAGS   = @TOOLS_LINK_MODE@
src_utils_papi_mem_info_LDFLAGS        = @TOOLS_LINK_MODE@
src_utils_papi_multiplex_cost_LDFLAGS  = @TOOLS_LINK_MODE@
src_utils_papi_native_avail_LDFLAGS    = @TOOLS_LINK_MODE@
src_utils_papi_version_LDFLAGS         = @TOOLS_LINK_MODE@
src_utils_papi_xml_event_info_LDFLAGS  = @TOOLS_LINK_MODE@
