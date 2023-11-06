#include <stdio.h>
#include <papi.h>
#include <stdlib.h>

char separator[] = "--------------------------------------------------------------------------------";

int print_event_info(int code)
{
    const char *data_type_to_string[] = {
        "PAPI_DATATYPE_INT64" ,
        "PAPI_DATATYPE_UINT64",
        "PAPI_DATATYPE_FP64"  ,
        "PAPI_DATATYPE_BIT64" ,
    };

    const char *value_type_to_string[] = {
        "PAPI_VALUETYPE_RUNNING_SUM",
        "PAPI_VALUETYPE_ABSOLUTE"   ,
    };

    const char *timescope_to_string[] = {
        "PAPI_TIMESCOPE_SINCE_START",
        "PAPI_TIMESCOPE_SINCE_LAST" ,
        "PAPI_TIMESCOPE_UNTIL_NEXT" ,
        "PAPI_TIMESCOPE_POINT"      ,
    };

    const char *update_type_to_string[] = {
        "PAPI_UPDATETYPE_ARBITRARY",
        "PAPI_UPDATETYPE_PUSH"     ,
        "PAPI_UPDATETYPE_PULL"     ,
        "PAPI_UPDATETYPE_FIXEDFREQ",
    };

    PAPI_event_info_t info;
    int papi_errno = PAPI_get_event_info(code, &info);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "%s\n", PAPI_strerror(papi_errno));
        return EXIT_FAILURE;
    }


    int i;
    fprintf(stdout, "%4sevent_code      : %u\n", " ", info.event_code);
    fprintf(stdout, "%4ssymbol          : %s\n", " ", info.symbol);
    fprintf(stdout, "%4sshort_descr     : %s\n", " ", info.short_descr);
    fprintf(stdout, "%4slong_descr      : %s\n", " ", info.long_descr);
    fprintf(stdout, "%4scomponent_index : %i\n", " ", info.component_index);
    fprintf(stdout, "%4sunits           : %s\n", " ", info.units);
    fprintf(stdout, "%4slocation        : %i\n", " ", info.location);
    fprintf(stdout, "%4sdata_type       : %s\n", " ", data_type_to_string[info.data_type]);
    fprintf(stdout, "%4svalue_type      : %s\n", " ", value_type_to_string[info.value_type]);
    fprintf(stdout, "%4stimescope       : %s\n", " ", timescope_to_string[info.timescope]);
    fprintf(stdout, "%4supdate_type     : %s\n", " ", update_type_to_string[info.update_type]);
    fprintf(stdout, "%4supdate_freq     : %i\n", " ", info.update_freq);
    fprintf(stdout, "%4scount           : %u\n", " ", info.count);
    fprintf(stdout, "%4sevent_type      : %u\n", " ", info.event_type);
    fprintf(stdout, "%4sderived         : %s\n", " ", info.derived);
    fprintf(stdout, "%4spostfix         : %s\n", " ", info.postfix);

    for (i = 0; i < PAPI_MAX_INFO_TERMS; ++i) {
        fprintf(stdout, "%4s(code,name)[%2i] : %u, %s\n", " ", i, info.code[i], info.name[i]);
    }

    fprintf(stdout, "%4snote            : %s\n", " ", info.note);
    fprintf(stdout, "%s\n", separator);
    fprintf(stdout, "\n");
    return PAPI_OK;
}

int print_event_qualifiers(int code)
{
    int i = code;
    int papi_errno = PAPI_enum_event(&i, PAPI_NTV_ENUM_UMASKS);
    if (papi_errno == PAPI_OK) {
        do {
            print_event_info(i);
        } while (PAPI_enum_event(&i, PAPI_NTV_ENUM_UMASKS) == PAPI_OK);
    }
    return PAPI_OK;
}

int main(int argc, char *argv[])
{
    int papi_errno;

    int ver = PAPI_library_init(PAPI_VER_CURRENT);
    if (ver != PAPI_VER_CURRENT) {
        return EXIT_FAILURE;
    }

    const char *name = argv[1];
    int code;
    papi_errno = PAPI_event_name_to_code(name, &code);
    if (papi_errno != PAPI_OK) {
        fprintf(stderr, "%s\n", PAPI_strerror(papi_errno));
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Event Information:\n");
    print_event_info(code);

    fprintf(stdout, "Event Qualifier Information:\n");
    print_event_qualifiers(code);

    return EXIT_SUCCESS;
}
