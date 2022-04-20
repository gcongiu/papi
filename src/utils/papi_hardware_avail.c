/** file papi_hardware_avail.c
  * @page papi_hardware_avail
  * @brief papi_hardware_avail utility.
  * @section  NAME
  *     papi_hardware_avail - provides detailed information on the hardware available in the system.
  *
  * @section Synopsis
  *
  * @section Description
  *     papi_hardware_avail is a PAPI utility program that reports information
  *     about the hardware devices equipped in the system.
  *
  * @section Options
  *      <ul>
  *     <li>-h help message
  *      </ul>
  *
  * @section Bugs
  *     There are no known bugs in this utility.
  *     If you find a bug, it should be reported to the
  *     PAPI Mailing List at <ptools-perfapi@icl.utk.edu>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "papi.h"
#include "print_header.h"

typedef enum {
    PAPI_DEV_TYPE_ATTR__ID,
    PAPI_DEV_TYPE_ATTR__VENDOR_ID,
    PAPI_DEV_TYPE_ATTR__NAME,
    PAPI_DEV_TYPE_ATTR__COUNT,
    PAPI_DEV_TYPE_ATTR__STATUS,
} PAPI_dev_type_attr_e;

typedef enum {
    PAPI_DEV_ATTR__CPU_CHAR_NAME,
    PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_LINE_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_LINE_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_LINE_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_LINE_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_LINE_COUNT,
    PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_LINE_COUNT,
    PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_LINE_COUNT,
    PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_LINE_COUNT,
    PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_ASSOC,
    PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_ASSOC,
    PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_ASSOC,
    PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_ASSOC,
    PAPI_DEV_ATTR__CPU_UINT_SOCKET_COUNT,
    PAPI_DEV_ATTR__CPU_UINT_NUMA_COUNT,
    PAPI_DEV_ATTR__CPU_UINT_CORE_COUNT,
    PAPI_DEV_ATTR__CPU_UINT_THREAD_COUNT,
    PAPI_DEV_ATTR__CPU_UINT_FAMILY,
    PAPI_DEV_ATTR__CPU_UINT_MODEL,
    PAPI_DEV_ATTR__CPU_UINT_STEPPING,
    PAPI_DEV_ATTR__CPU_UINT_NUMA_MEM_SIZE,
    PAPI_DEV_ATTR__CPU_UINT_THR_NUMA_AFFINITY,
    PAPI_DEV_ATTR__CPU_UINT_NUMA_THR_LIST,
    PAPI_DEV_ATTR__CPU_UINT_THR_PER_NUMA,
    PAPI_DEV_ATTR__CUDA_ULONG_UID,
    PAPI_DEV_ATTR__CUDA_CHAR_DEVICE_NAME,
    PAPI_DEV_ATTR__CUDA_UINT_WARP_SIZE,
    PAPI_DEV_ATTR__CUDA_UINT_SHM_PER_BLK,
    PAPI_DEV_ATTR__CUDA_UINT_SHM_PER_SM,
    PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_X,
    PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_Y,
    PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_Z,
    PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_X,
    PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_Y,
    PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_Z,
    PAPI_DEV_ATTR__CUDA_UINT_THR_PER_BLK,
    PAPI_DEV_ATTR__CUDA_UINT_SM_COUNT,
    PAPI_DEV_ATTR__CUDA_UINT_MULTI_KERNEL,
    PAPI_DEV_ATTR__CUDA_UINT_MAP_HOST_MEM,
    PAPI_DEV_ATTR__CUDA_UINT_MEMCPY_OVERLAP,
    PAPI_DEV_ATTR__CUDA_UINT_UNIFIED_ADDR,
    PAPI_DEV_ATTR__CUDA_UINT_MANAGED_MEM,
    PAPI_DEV_ATTR__CUDA_UINT_CPU_THR_AFFINITY_LIST,
    PAPI_DEV_ATTR__CUDA_UINT_CPU_THR_PER_DEVICE,
    PAPI_DEV_ATTR__CUDA_UINT_COMP_CAP_MAJOR,
    PAPI_DEV_ATTR__CUDA_UINT_COMP_CAP_MINOR,
    PAPI_DEV_ATTR__CUDA_UINT_BLK_PER_SM,
    PAPI_DEV_ATTR__ROCM_ULONG_UID,
    PAPI_DEV_ATTR__ROCM_CHAR_DEVICE_NAME,
    PAPI_DEV_ATTR__ROCM_UINT_WAVEFRONT_SIZE,
    PAPI_DEV_ATTR__ROCM_UINT_WORKGROUP_SIZE,
    PAPI_DEV_ATTR__ROCM_UINT_WAVE_PER_CU,
    PAPI_DEV_ATTR__ROCM_UINT_SHM_PER_WG,
    PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_X,
    PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_Y,
    PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_Z,
    PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_X,
    PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_Y,
    PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_Z,
    PAPI_DEV_ATTR__ROCM_UINT_CU_COUNT,
    PAPI_DEV_ATTR__ROCM_UINT_SIMD_PER_CU,
    PAPI_DEV_ATTR__ROCM_UINT_COMP_CAP_MAJOR,
    PAPI_DEV_ATTR__ROCM_UINT_COMP_CAP_MINOR,
} PAPI_dev_attr_e;

enum {
    PAPI_DEV_TYPE_ENUM__CPU  = (1     ),
    PAPI_DEV_TYPE_ENUM__CUDA = (1 << 1),
    PAPI_DEV_TYPE_ENUM__ROCM = (1 << 2),
    PAPI_DEV_TYPE_ENUM__ALL  = (1 << 9) - 1,
};

static int PAPI_enum_dev_type(void **handle, int enum_modifier);
static int PAPI_get_dev_type_attr(void *handle, PAPI_dev_type_attr_e attr, void *value);
static int PAPI_get_dev_attr(void *handle, int id, PAPI_dev_attr_e attr, void *value);

typedef struct command_flags
{
    int help;
} command_flags_t;

static void
print_help( char **argv )
{
    printf( "This is the PAPI hardware avail program.\n" );
    printf( "It provides availability of system's equipped hardware devices.\n" );
    printf( "Usage: %s [options]\n", argv[0] );
    printf( "Options:\n\n" );
    printf( "  --help, -h    print this help message\n" );
}

static void
parse_args( int argc, char **argv, command_flags_t * f )
{
    int i;

    /* Look for all currently defined commands */
    memset( f, 0, sizeof ( command_flags_t ) );
    for ( i = 1; i < argc; i++ ) {
        if ( !strcmp( argv[i], "-h" ) || !strcmp( argv[i], "--help" ) )
            f->help = 1;
        else
            printf( "%s is not supported\n", argv[i] );
    }

    /* if help requested, print and bail */
    if ( f->help ) {
        print_help( argv );
        exit( 1 );
    }

}

static int *
get_num_threads_per_numa( int *numa_affinity, int numas, int threads )
{
    int *threads_per_numa = calloc(numas, sizeof(int));

    int k;
    for (k = 0; k < threads; ++k) {
        threads_per_numa[numa_affinity[k]]++;
    }

    return threads_per_numa;
}

static int **
get_threads_per_numa( int *numa_affinity, int *threads_per_numa, int numas, int threads )
{
    int **numa_threads = malloc(numas * sizeof(*numa_threads));
    int *numa_threads_cnt = calloc(numas, sizeof(*numa_threads_cnt));

    int k;
    for (k = 0; k < numas; ++k) {
        numa_threads[k] = malloc(threads_per_numa[k] * sizeof(int));
    }

    for (k = 0; k < threads; ++k) {
        int node = numa_affinity[k];
        numa_threads[node][numa_threads_cnt[node]++] = k;
    }

    free(numa_threads_cnt);

    return numa_threads;
}

int
main( int argc, char **argv )
{
    int i;
    int retval;
    const PAPI_component_info_t *cmpinfo = NULL;
    command_flags_t flags;
    int numcmp;
    int sysdetect_avail = 0;

    /* Initialize before parsing the input arguments */
    retval = PAPI_library_init( PAPI_VER_CURRENT );
    if ( retval != PAPI_VER_CURRENT ) {
        fprintf(stderr,"Error!  PAPI_library_init\n");
        return retval;
    }

    parse_args( argc, argv, &flags );

    retval = PAPI_set_debug( PAPI_VERB_ECONT );
    if ( retval != PAPI_OK ) {
        fprintf(stderr,"Error!  PAPI_set_debug\n");
        return retval;
    }

    numcmp = PAPI_num_components( );
    for (i = 0; i < numcmp; i++) {
        cmpinfo = PAPI_get_component_info( i );
        if (strcmp("sysdetect", cmpinfo->name) == 0)
            sysdetect_avail = 1;
    }

    if (sysdetect_avail == 0) {
        fprintf(stderr, "Error! Sysdetect component not enabled\n");
        return 0;
    }

    printf( "\nDevice Summary -----------------------------------------------------------------\n" );
    void *handle;
    int enum_modifier = PAPI_DEV_TYPE_ENUM__ALL;
    int id, vendor_id, dev_count;
    const char *vendor_name, *status;

    printf( "Vendor           DevCount \n" );
    while (PAPI_enum_dev_type(&handle, enum_modifier) == PAPI_OK) {
        PAPI_get_dev_type_attr(handle, PAPI_DEV_TYPE_ATTR__NAME, &vendor_name);
        PAPI_get_dev_type_attr(handle, PAPI_DEV_TYPE_ATTR__COUNT, &dev_count);
        PAPI_get_dev_type_attr(handle, PAPI_DEV_TYPE_ATTR__STATUS, &status);

        printf( "%-18s (%d)\n", vendor_name, dev_count);
        printf( " \\-> Status: %s\n", status );
        printf( "\n" );
    }

    printf( "\nDevice Information -------------------------------------------------------------\n" );

    while (PAPI_enum_dev_type(&handle, enum_modifier) == PAPI_OK) {
        PAPI_get_dev_type_attr(handle, PAPI_DEV_TYPE_ATTR__ID, &id);
        PAPI_get_dev_type_attr(handle, PAPI_DEV_TYPE_ATTR__VENDOR_ID, &vendor_id);
        PAPI_get_dev_type_attr(handle, PAPI_DEV_TYPE_ATTR__NAME, &vendor_name);
        PAPI_get_dev_type_attr(handle, PAPI_DEV_TYPE_ATTR__COUNT, &dev_count);

        if ( id == PAPI_DEV_TYPE_ID__CPU && dev_count > 0 ) {
            int numas = 1;
            for ( i = 0; i < dev_count; ++i ) {
                const char *cpu_name;
                unsigned int family, model, stepping;
                unsigned int sockets, cores, threads;
                unsigned int l1i_size, l1d_size, l2u_size, l3u_size;
                unsigned int l1i_line_sz, l1d_line_sz, l2u_line_sz, l3u_line_sz;
                unsigned int l1i_line_cnt, l1d_line_cnt, l2u_line_cnt, l3u_line_cnt;
                unsigned int l1i_cache_ass, l1d_cache_ass, l2u_cache_ass, l3u_cache_ass;

                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_CHAR_NAME, &cpu_name);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_FAMILY, &family);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_MODEL, &model);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_STEPPING, &stepping);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_SOCKET_COUNT, &sockets);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_NUMA_COUNT, &numas);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_CORE_COUNT, &cores);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_THREAD_COUNT, &threads);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_SIZE, &l1i_size);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_SIZE, &l1d_size);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_SIZE, &l2u_size);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_SIZE, &l3u_size);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_LINE_SIZE, &l1i_line_sz);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_LINE_SIZE, &l1d_line_sz);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_LINE_SIZE, &l2u_line_sz);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_LINE_SIZE, &l3u_line_sz);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_LINE_COUNT, &l1i_line_cnt);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_LINE_COUNT, &l1d_line_cnt);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_LINE_COUNT, &l2u_line_cnt);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_LINE_COUNT, &l3u_line_cnt);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_ASSOC, &l1i_cache_ass);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_ASSOC, &l1d_cache_ass);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_ASSOC, &l2u_cache_ass);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_ASSOC, &l3u_cache_ass);

                printf( "Vendor                                : %s (%u,0x%x)\n",
                                                                 vendor_name,
                                                                 vendor_id,
                                                                 vendor_id );
                printf( "Id                                    : %u\n", i );
                printf( "Name                                  : %s\n", cpu_name );
                printf( "CPUID                                 : Family/Model/Stepping %u/%u/%u 0x%02x/0x%02x/0x%02x\n",
                                                                 family, model, stepping, family, model, stepping );
                printf( "Sockets                               : %u\n", sockets );
                printf( "Numa regions                          : %u\n", numas );
                printf( "Cores per socket                      : %u\n", cores );
                printf( "Cores per NUMA region                 : %u\n", threads / numas );
                printf( "SMT threads per core                  : %u\n", threads / sockets / cores );

                if (l1i_size > 0) {
                    printf( "L1i Cache                             : Size/LineSize/Lines/Assoc %uKB/%uB/%u/%u\n",
                            l1i_size >> 10, l1i_line_sz, l1i_line_cnt, l1i_cache_ass);
                    printf( "L1d Cache                             : Size/LineSize/Lines/Assoc %uKB/%uB/%u/%u\n",
                            l1d_size >> 10, l1d_line_sz, l1d_line_cnt, l1d_cache_ass);
                }

                if (l2u_size > 0) {
                    printf( "L2 Cache                              : Size/LineSize/Lines/Assoc %uKB/%uB/%u/%u\n",
                            l2u_size >> 10, l2u_line_sz, l2u_line_cnt, l2u_cache_ass );
                }

                if (l3u_size > 0) {
                    printf( "L3 Cache                              : Size/LineSize/Lines/Assoc %uKB/%uB/%u/%u\n",
                            l3u_size >> 10, l3u_line_sz, l3u_line_cnt, l3u_cache_ass );
                }
            }

            for ( i = 0; i < numas; ++i ) {
                unsigned int memsize, thread_count;
                const unsigned int *list;

                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_NUMA_MEM_SIZE, &memsize);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_NUMA_THR_LIST, &list);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CPU_UINT_THR_PER_NUMA, &thread_count);

                printf( "Numa Node %u Threads                   : ", i );
                unsigned int j;
                for (j = 0; j < thread_count; ++j) {
                    printf( "%u ", list[j] );
                }
                printf( "\n" );
                printf( "\n" );
            }
        }

        if ( id == PAPI_DEV_TYPE_ID__NVIDIA_GPU && dev_count > 0 ) {
            printf( "Vendor                                : %s\n", vendor_name );

            for ( i = 0; i < dev_count; ++i ) {
                unsigned long uid;
                unsigned int warp_size, thread_per_block, block_per_sm;
                unsigned int shm_per_block, shm_per_sm;
                unsigned int blk_dim_x, blk_dim_y, blk_dim_z;
                unsigned int grd_dim_x, grd_dim_y, grd_dim_z;
                unsigned int sm_count, multi_kernel, map_host_mem, async_memcpy;
                unsigned int unif_addr, managed_mem;
                unsigned int cc_major, cc_minor;
                const char *dev_name;
                const unsigned int *list;
                unsigned int list_len;

                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_ULONG_UID, &uid);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_CHAR_DEVICE_NAME, &dev_name);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_WARP_SIZE, &warp_size);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_THR_PER_BLK, &thread_per_block);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_BLK_PER_SM, &block_per_sm);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_SHM_PER_BLK, &shm_per_block);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_SHM_PER_SM, &shm_per_sm);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_X, &blk_dim_x);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_Y, &blk_dim_y);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_Z, &blk_dim_z);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_X, &grd_dim_x);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_Y, &grd_dim_y);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_Z, &grd_dim_z);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_SM_COUNT, &sm_count);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_MULTI_KERNEL, &multi_kernel);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_MAP_HOST_MEM, &map_host_mem);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_MEMCPY_OVERLAP, &async_memcpy);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_UNIFIED_ADDR, &unif_addr);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_MANAGED_MEM, &managed_mem);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_CPU_THR_AFFINITY_LIST, &list);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_CPU_THR_PER_DEVICE, &list_len);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_COMP_CAP_MAJOR, &cc_major);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__CUDA_UINT_COMP_CAP_MINOR, &cc_minor);

                printf( "Id                                    : %d\n", i );
                printf( "UID                                   : %lu\n", uid );
                printf( "Name                                  : %s\n", dev_name );
                printf( "Warp size                             : %u\n", warp_size );
                printf( "Max threads per block                 : %u\n", thread_per_block );
                printf( "Max blocks per multiprocessor         : %u\n", block_per_sm );
                printf( "Max shared memory per block           : %u\n", shm_per_block );
                printf( "Max shared memory per multiprocessor  : %u\n", shm_per_sm );
                printf( "Max block dim x                       : %u\n", blk_dim_x );
                printf( "Max block dim y                       : %u\n", blk_dim_y );
                printf( "Max block dim z                       : %u\n", blk_dim_z );
                printf( "Max grid dim x                        : %u\n", grd_dim_x );
                printf( "Max grid dim y                        : %u\n", grd_dim_y );
                printf( "Max grid dim z                        : %u\n", grd_dim_z );
                printf( "Multiprocessor count                  : %u\n", sm_count );
                printf( "Multiple kernels per context          : %s\n", multi_kernel ? "yes" : "no" );
                printf( "Can map host memory                   : %s\n", map_host_mem ? "yes" : "no");
                printf( "Can overlap compute and data transfer : %s\n", async_memcpy ? "yes" : "no" );
                printf( "Has unified addressing                : %s\n", unif_addr ? "yes" : "no" );
                printf( "Has managed memory                    : %s\n", managed_mem ? "yes" : "no" );
                printf( "Compute capability                    : %u.%u\n", cc_major, cc_minor );

                if (list_len > 0) {
                    printf( "Affinity                              : ");
                    unsigned int k;
                    for (k = 0; k < list_len; ++k) {
                        printf( "%u ", list[k] );
                    }
                    printf( "\n" );
                }
                printf( "\n" );
            }
        }

        if ( id == PAPI_DEV_TYPE_ID__AMD_GPU && dev_count > 0 ) {
            printf( "Vendor                                : %s\n", vendor_name );

            unsigned long uid;
            const char *dev_name;
            unsigned int wf_size, simd_per_cu, wg_size;
            unsigned int wf_per_cu, shm_per_wg, wg_dim_x, wg_dim_y, wg_dim_z;
            unsigned int grd_dim_x, grd_dim_y, grd_dim_z;
            unsigned int cu_count;
            unsigned int cc_major, cc_minor;

            for ( i = 0; i < dev_count; ++i ) {
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_ULONG_UID, &uid);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_CHAR_DEVICE_NAME, &dev_name);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_WAVEFRONT_SIZE, &wf_size);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_SIMD_PER_CU, &simd_per_cu);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_WORKGROUP_SIZE, &wg_size);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_WAVE_PER_CU, &wf_per_cu);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_SHM_PER_WG, &shm_per_wg);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_X, &wg_dim_x);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_Y, &wg_dim_y);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_Z, &wg_dim_z);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_X, &grd_dim_x);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_Y, &grd_dim_y);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_Z, &grd_dim_z);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_CU_COUNT, &cu_count);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_COMP_CAP_MAJOR, &cc_major);
                PAPI_get_dev_attr(handle, i, PAPI_DEV_ATTR__ROCM_UINT_COMP_CAP_MINOR, &cc_minor);

                printf( "Id                                    : %d\n", i );
                printf( "Name                                  : %s\n", dev_name );
                printf( "Wavefront size                        : %u\n", wf_size );
                printf( "SIMD per compute unit                 : %u\n", simd_per_cu );
                printf( "Max threads per workgroup             : %u\n", wg_size );
                printf( "Max waves per compute unit            : %u\n", wf_per_cu );
                printf( "Max shared memory per workgroup       : %u\n", shm_per_wg );
                printf( "Max workgroup dim x                   : %u\n", wg_dim_x );
                printf( "Max workgroup dim y                   : %u\n", wg_dim_y );
                printf( "Max workgroup dim z                   : %u\n", wg_dim_z );
                printf( "Max grid dim x                        : %u\n", grd_dim_x );
                printf( "Max grid dim y                        : %u\n", grd_dim_y );
                printf( "Max grid dim z                        : %u\n", grd_dim_z );
                printf( "Compute unit count                    : %u\n", cu_count );
                printf( "Compute capability                    : %u.%u\n", cc_major, cc_minor );
                printf( "\n" );
            }
        }
    }

    printf( "--------------------------------------------------------------------------------\n" );

    PAPI_shutdown();
    return 0;
}

int
PAPI_enum_dev_type(void **handle, int enum_modifier)
{
    static int dev_type_id;
    const PAPI_hw_info_t *hwinfo = PAPI_get_hardware_info();

    if (dev_type_id >= PAPI_DEV_TYPE_ID__MAX_NUM) {
        dev_type_id = 0;
        return PAPI_EINVAL;
    }

    *handle = &hwinfo->dev_type_arr[dev_type_id++];

    return PAPI_OK;
}

int
PAPI_get_dev_type_attr(void *handle, PAPI_dev_type_attr_e attr, void *val)
{
    int papi_errno = PAPI_OK;

    PAPI_dev_type_info_t *dev_type_info = (PAPI_dev_type_info_t *) handle;

    switch(attr) {
        case PAPI_DEV_TYPE_ATTR__ID:
            *(int *) val = dev_type_info->id;
            break;
        case PAPI_DEV_TYPE_ATTR__VENDOR_ID:
            *(int *) val = dev_type_info->vendor_id;
            break;
        case PAPI_DEV_TYPE_ATTR__NAME:
            *(const char **) val = dev_type_info->vendor;
            break;
        case PAPI_DEV_TYPE_ATTR__COUNT:
            *(int *) val = dev_type_info->num_devices;
            break;
        case PAPI_DEV_TYPE_ATTR__STATUS:
            *(const char **) val = dev_type_info->status;
            break;
        default:
            papi_errno = PAPI_ENOSUPP;
    }

    return papi_errno;
}

int
PAPI_get_dev_attr(void *handle, int id, PAPI_dev_attr_e attr, void *val)
{
    int papi_errno = PAPI_OK;

    PAPI_dev_type_info_t *dev_type_info = (PAPI_dev_type_info_t *) handle;
    PAPI_cpu_info_t *cpu_info = (PAPI_cpu_info_t *) &dev_type_info->dev_info_arr[id];
    PAPI_gpu_info_u *gpu_info = (PAPI_gpu_info_u *) &dev_type_info->dev_info_arr[id];

    int *numa_affinity;
    int *num_threads_per_numa;
    static int **numa_threads;
    int threads;

    switch(attr) {
        /* CPU attributes */
        case PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_SIZE:
            *(int *) val = cpu_info->clevel[0].cache[0].size;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_SIZE:
            *(int *) val = cpu_info->clevel[0].cache[1].size;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_SIZE:
            *(int *) val = cpu_info->clevel[1].cache[0].size;
        case PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_SIZE:
            *(int *) val = cpu_info->clevel[2].cache[0].size;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_LINE_SIZE:
            *(int *) val = cpu_info->clevel[0].cache[0].line_size;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_LINE_SIZE:
            *(int *) val = cpu_info->clevel[0].cache[1].line_size;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_LINE_SIZE:
            *(int *) val = cpu_info->clevel[1].cache[0].line_size;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_LINE_SIZE:
            *(int *) val = cpu_info->clevel[2].cache[0].line_size;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_LINE_COUNT:
            *(int *) val = cpu_info->clevel[0].cache[0].num_lines;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_LINE_COUNT:
            *(int *) val = cpu_info->clevel[0].cache[1].num_lines;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_LINE_COUNT:
            *(int *) val = cpu_info->clevel[1].cache[0].num_lines;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_LINE_COUNT:
            *(int *) val = cpu_info->clevel[2].cache[0].num_lines;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L1I_CACHE_ASSOC:
            *(int *) val = cpu_info->clevel[0].cache[0].associativity;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L1D_CACHE_ASSOC:
            *(int *) val = cpu_info->clevel[0].cache[1].associativity;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L2U_CACHE_ASSOC:
            *(int *) val = cpu_info->clevel[1].cache[0].associativity;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_L3U_CACHE_ASSOC:
            *(int *) val = cpu_info->clevel[2].cache[0].associativity;
            break;
        case PAPI_DEV_ATTR__CPU_CHAR_NAME:
            *(const char **) val = cpu_info->name;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_FAMILY:
            *(unsigned int *) val = cpu_info->cpuid_family;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_MODEL:
            *(unsigned int *) val = cpu_info->cpuid_model;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_STEPPING:
            *(unsigned int *) val = cpu_info->cpuid_stepping;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_SOCKET_COUNT:
            *(unsigned int *) val = cpu_info->sockets;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_NUMA_COUNT:
            *(unsigned int *) val = cpu_info->numas;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_CORE_COUNT:
            *(int *) val = cpu_info->cores;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_THREAD_COUNT:
            *(int *) val = cpu_info->threads * cpu_info->cores * cpu_info->sockets;
            break;
        case PAPI_DEV_ATTR__CPU_UINT_NUMA_THR_LIST:
            numa_affinity = cpu_info->numa_affinity;
            threads = cpu_info->threads * cpu_info->cores * cpu_info->sockets;
            num_threads_per_numa = get_num_threads_per_numa(numa_affinity, cpu_info->numas, threads);
            numa_threads = get_threads_per_numa(numa_affinity, num_threads_per_numa, cpu_info->numas, threads);
            *(int **) val = numa_threads[id];
            break;
        case PAPI_DEV_ATTR__CPU_UINT_THR_PER_NUMA:
            numa_affinity = cpu_info->numa_affinity;
            threads = cpu_info->threads * cpu_info->cores * cpu_info->sockets;
            num_threads_per_numa = get_num_threads_per_numa(numa_affinity, cpu_info->numas, threads);
            *(int *) val = num_threads_per_numa[id];
            break;
        /* NVIDIA GPU attributes */
        case PAPI_DEV_ATTR__CUDA_ULONG_UID:
            *(unsigned long *) val = gpu_info->nvidia.uid;
            break;
        case PAPI_DEV_ATTR__CUDA_CHAR_DEVICE_NAME:
            *(const char **) val = gpu_info->nvidia.name;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_WARP_SIZE:
            *(unsigned int *) val = gpu_info->nvidia.warp_size;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_THR_PER_BLK:
            *(unsigned int *) val = gpu_info->nvidia.max_threads_per_block;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_BLK_PER_SM:
            *(unsigned int *) val = gpu_info->nvidia.max_blocks_per_multi_proc;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_SHM_PER_BLK:
            *(unsigned int *) val = gpu_info->nvidia.max_shmmem_per_block;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_SHM_PER_SM:
            *(unsigned int *) val = gpu_info->nvidia.max_shmmem_per_multi_proc;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_X:
            *(unsigned int *) val = gpu_info->nvidia.max_block_dim_x;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_Y:
            *(unsigned int *) val = gpu_info->nvidia.max_block_dim_y;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_BLK_DIM_Z:
            *(unsigned int *) val = gpu_info->nvidia.max_block_dim_z;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_X:
            *(unsigned int *) val = gpu_info->nvidia.max_grid_dim_x;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_Y:
            *(unsigned int *) val = gpu_info->nvidia.max_grid_dim_y;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_GRD_DIM_Z:
            *(unsigned int *) val = gpu_info->nvidia.max_grid_dim_z;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_SM_COUNT:
            *(unsigned int *) val = gpu_info->nvidia.multi_processor_count;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_MULTI_KERNEL:
            *(unsigned int *) val = gpu_info->nvidia.multi_kernel_per_ctx;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_MAP_HOST_MEM:
            *(unsigned int *) val = gpu_info->nvidia.can_map_host_mem;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_MEMCPY_OVERLAP:
            *(unsigned int *) val = gpu_info->nvidia.can_overlap_comp_and_data_xfer;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_UNIFIED_ADDR:
            *(unsigned int *) val = gpu_info->nvidia.unified_addressing;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_MANAGED_MEM:
            *(unsigned int *) val = gpu_info->nvidia.managed_memory;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_COMP_CAP_MAJOR:
            *(unsigned int *) val = gpu_info->nvidia.major;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_COMP_CAP_MINOR:
            *(unsigned int *) val = gpu_info->nvidia.minor;
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_CPU_THR_AFFINITY_LIST:
            *(unsigned int *) val = gpu_info->nvidia.affinity.proc_id_arr[id];
            break;
        case PAPI_DEV_ATTR__CUDA_UINT_CPU_THR_PER_DEVICE:
            *(unsigned int *) val = gpu_info->nvidia.affinity.proc_count;
            break;
        /* AMD GPU attributes */
        case PAPI_DEV_ATTR__ROCM_ULONG_UID:
            *(unsigned long *) val = gpu_info->amd.uid;
            break;
        case PAPI_DEV_ATTR__ROCM_CHAR_DEVICE_NAME:
            *(const char **) val = gpu_info->amd.name;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_SIMD_PER_CU:
            *(unsigned int *) val = gpu_info->amd.simd_per_compute_unit;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_WORKGROUP_SIZE:
            *(unsigned int *) val = gpu_info->amd.max_threads_per_workgroup;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_WAVEFRONT_SIZE:
            *(unsigned int *) val = gpu_info->amd.wavefront_size;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_WAVE_PER_CU:
            *(unsigned int *) val = gpu_info->amd.max_waves_per_compute_unit;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_SHM_PER_WG:
            *(unsigned int *) val = gpu_info->amd.max_shmmem_per_workgroup;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_X:
            *(unsigned int *) val = gpu_info->amd.max_workgroup_dim_x;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_Y:
            *(unsigned int *) val = gpu_info->amd.max_workgroup_dim_y;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_WG_DIM_Z:
            *(unsigned int *) val = gpu_info->amd.max_workgroup_dim_z;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_X:
            *(unsigned int *) val = gpu_info->amd.max_grid_dim_x;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_Y:
            *(unsigned int *) val = gpu_info->amd.max_grid_dim_y;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_GRD_DIM_Z:
            *(unsigned int *) val = gpu_info->amd.max_grid_dim_z;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_CU_COUNT:
            *(unsigned int *) val = gpu_info->amd.compute_unit_count;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_COMP_CAP_MAJOR:
            *(unsigned int *) val = gpu_info->amd.major;
            break;
        case PAPI_DEV_ATTR__ROCM_UINT_COMP_CAP_MINOR:
            *(unsigned int *) val = gpu_info->amd.minor;
            break;
        default:
            papi_errno = PAPI_ENOSUPP;
    }

    return papi_errno;
}
