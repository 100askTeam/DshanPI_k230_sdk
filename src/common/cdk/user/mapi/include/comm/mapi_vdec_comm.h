/**
 * @file mapi_vdec_comm.h
 * @author  ()
 * @brief
 * @version 1.0
 * @date 2023-06-19
 *
 * @copyright
 * Copyright (c) 2023, Canaan Bright Sight Co., Ltd
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __MAPI_VDEC_COMM_H__
#define __MAPI_VDEC_COMM_H__

#include "mapi_log.h"
#include "k_video_comm.h"

#ifdef __cplusplus
extern "C"{
#endif /* End of #ifdef __cplusplus */

#define mapi_vdec_emerg_trace(fmt, ...)                                                                                                 \
        do {                                                                                                                           \
            kendyrte_mapi_log_printf(K_MAPI_MOD_VDEC, K_ERR_LEVEL_FATAL, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)

#define mapi_vdec_alter_trace(fmt, ...)                                                                                                 \
        do {                                                                                                                           \
            kendyrte_mapi_log_printf(K_MAPI_MOD_VDEC, K_ERR_LEVEL_ALERT, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)

#define mapi_vdec_crit_trace(fmt, ...)                                                                                                 \
        do {                                                                                                                           \
            kendyrte_mapi_log_printf(K_MAPI_MOD_VDEC, K_ERR_LEVEL_CRIT, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)

#define mapi_vdec_error_trace(fmt, ...)                                                                                                 \
        do {                                                                                                                           \
            kendyrte_mapi_log_printf(K_MAPI_MOD_VDEC, K_ERR_LEVEL_ERROR, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)

#define mapi_vdec_warn_trace(fmt, ...)                                                                                                  \
        do {                                                                                                                           \
            kendyrte_mapi_log_printf(K_MAPI_MOD_VDEC, K_ERR_LEVEL_WARNING, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)

#define mapi_vdec_notic_trace(fmt, ...)                                                                                                 \
        do {                                                                                                                           \
            kendyrte_mapi_log_printf(K_MAPI_MOD_VDEC, K_ERR_LEVEL_NOTICE, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)

#define mapi_vdec_debug_trace(fmt, ...)                                                                                                 \
        do {                                                                                                                           \
            kendyrte_mapi_log_printf(K_MAPI_MOD_VDEC, K_ERR_LEVEL_DEBUG, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)

#define mapi_vdec_info_trace(fmt, ...)                                                                                                 \
        do {                                                                                                                           \
            kendyrte_mapi_log_printf(K_MAPI_MOD_VDEC, K_ERR_LEVEL_INFO, "[Func]:%s [Line]:%d [Info]:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)

#define VDEC_RET_MPI_TO_MAPI(ret) (((ret) & 0x0000FFFF) | (K_MAPI_ERR_APPID | ((K_MAPI_MOD_VDEC) << 16 )))


#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif