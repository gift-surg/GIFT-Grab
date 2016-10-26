CMAKE_MINIMUM_REQUIRED(VERSION 3.2.0)

if(NOT DEFINED ENV{EpiphanSDK_DIR})
    MESSAGE(FATAL_ERROR
        "EpiphanSDK_DIR does not seem to be defined in your environment. This is needed to find Epiphan video grabbing SDK components.")
endif(NOT DEFINED ENV{EpiphanSDK_DIR})

# ZLIB
FIND_PACKAGE(ZLIB REQUIRED)

# pthreads
SET(CMAKE_THREAD_PREFER_PTHREAD TRUE)
FIND_PACKAGE(Threads REQUIRED)

# Include directories
SET(INCLUDE_DIRS
    $ENV{EpiphanSDK_DIR}/epiphan/include
    $ENV{EpiphanSDK_DIR}/epiphan/frmgrab/include
    $ENV{EpiphanSDK_DIR}/opensource/slib/include
    $ENV{EpiphanSDK_DIR}/opensource/slib/src
    )
INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

# Frmgrab library
SET(FrmGrab_HEADERS
    $ENV{EpiphanSDK_DIR}/epiphan/include/v2u_compression.h
    $ENV{EpiphanSDK_DIR}/epiphan/include/v2u_defs.h
    $ENV{EpiphanSDK_DIR}/epiphan/include/v2u_dshow.h
    $ENV{EpiphanSDK_DIR}/epiphan/include/v2u_id.h
    $ENV{EpiphanSDK_DIR}/epiphan/include/v2u_ioctl.h
    $ENV{EpiphanSDK_DIR}/epiphan/include/v2u_pack_off.h
    $ENV{EpiphanSDK_DIR}/epiphan/include/v2u_pack_on.h
    $ENV{EpiphanSDK_DIR}/epiphan/include/v2u_version.h
    $ENV{EpiphanSDK_DIR}/epiphan/frmgrab/include/frmgrab.h
    )
SET(FrmGrab_LIB
    $ENV{EpiphanSDK_DIR}/epiphan/frmgrab/lib/linux/x86_64/libfrmgrab.a
    ) # CMAKE_CURRENT_SOURCE_DIR necessary because of tests linking to this

# SLIB
SET(SLIB_HEADERS
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_base32.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_base64.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_bitset.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_buf.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_cs.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_def.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_dom.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_event.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_file.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_futil.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_hash.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_hist.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_init.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_itr.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_lib.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_lock.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_math.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_md.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_mem.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_mutex.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_ntk.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_ntutil.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_opt.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_os.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_prop.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_queue.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_random.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_ring.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_rwlock.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_stack.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_strbuf.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_thread.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_unix.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_util.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_vector.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_ver.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_win32.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_wkq.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/include/s_xml.h
    )
SET(SLIB_SOURCES
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_base32.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_base64.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_bitset.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_buf.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_cs.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_dom.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_event.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_file.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fio.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fio.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fmem.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fnull.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fsock.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fsplit.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fsub.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_futil.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fwrap.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fzio.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_fzip.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_hash.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_hist.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_init.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_iop.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_itra.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_itr.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_itrc.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_itrf.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_itrp.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_itrs.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_lib.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_libp.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_lock.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_lockp.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_math.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_md5.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_md.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_mdp.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_mem.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_mfp.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_mpm.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_mutex.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_net.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_opt.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_parse.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_prop.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_propx.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_queue.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_random.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_ring.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_rwlock.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_sha1.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_stack.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_strbuf.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_str.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_thread.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_thrp.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_time.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_trace.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_utf8.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_util.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_vector.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_wkq.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_xml.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_xmlp.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/s_xmlp.h
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/unix/u_event.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/unix/u_furl.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/unix/u_futil.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/unix/u_mutex.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/unix/u_thread.c
    $ENV{EpiphanSDK_DIR}/opensource/slib/src/unix/u_trace.c
    )
SET(SLIB_LIB slib)
ADD_LIBRARY(${SLIB_LIB} ${SLIB_HEADERS} ${SLIB_SOURCES})
SET_TARGET_PROPERTIES(${SLIB_LIB} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Headers to include
SET(HEADERS ${FrmGrab_HEADERS})
# Libs to link against
SET(LIBS
    ${FrmGrab_LIB}
    ${SLIB_LIB}
    ${CMAKE_THREAD_LIBS_INIT}
    ${ZLIB_LIBRARIES}
    )

# Variables to be used by linking libraries
SET(EpiphanSDK_INCLUDE_DIRS
    ${INCLUDE_DIRS}
    CACHE INTERNAL "Epiphan video grabbing SDK include dirs"
    )
SET(EpiphanSDK_LIBS
    ${LIBS}
    CACHE INTERNAL "Epiphan video grabbing SDK link libraries"
    )
# Variables to be used for Epiphan DVI2PCIe Duo port specs
SET(V2U "$ENV{EpiphanSDK_DIR}/epiphan/samples/v2u/build/release/v2u")
EXECUTE_PROCESS(
    COMMAND ${V2U} -s
    OUTPUT_VARIABLE V2U_OUT
    RESULT_VARIABLE V2U_EXIT_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
if(NOT V2U_EXIT_CODE EQUAL 0)
    MESSAGE(FATAL_ERROR
        "No Epiphan cards seem to be installed on your system."
        )
endif(NOT V2U_EXIT_CODE EQUAL 0)
SET(Epiphan_DVI2PCIeDuo_DVI
    "\"${V2U_OUT}.vga\""
    CACHE INTERNAL "Epiphan DVI2PCIe Duo DVI port device file on Linux"
    )
SET(Epiphan_DVI2PCIeDuo_SDI
    "\"${V2U_OUT}.sdi\""
    CACHE INTERNAL "Epiphan DVI2PCIe Duo SDI port device file on Linux"
    )
SET(Epiphan_DVI2PCIeDuo_DVI_MAX_FRAME_RATE
    60  # TODO: automate?
    CACHE INTERNAL "Epiphan DVI2PCIe Duo DVI port max frame rate"
    )
SET(Epiphan_DVI2PCIeDuo_SDI_MAX_FRAME_RATE
    60  # TODO: automate?
    CACHE INTERNAL "Epiphan DVI2PCIe Duo SDI port max frame rate"
    )
