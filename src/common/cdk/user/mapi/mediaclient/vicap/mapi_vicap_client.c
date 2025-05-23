/* Copyright (c) 2023, Canaan Bright Sight Co., Ltd
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
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "k_comm_ipcmsg.h"
#include "mapi_vicap_api.h"
#include "msg_client_dispatch.h"
#include "mapi_vicap_comm.h"
#include "mpi_vicap_api.h"
#include "msg_vicap.h"
#include "k_type.h"
#include "k_vicap_comm.h"
#include "../../component/ipcmsg/include/k_ipcmsg.h"

#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

k_s32 kd_mapi_vicap_get_sensor_fd(k_vicap_sensor_attr *sensor_attr)
{
    msg_vicap_sensor_attr_t info;
    k_s32 ret = 0;

    if(sensor_attr->dev_num >= VICAP_MAX_DEV_NUMS)
    {
        return K_MAPI_ERR_VICAP_INVALID_DEVID;
    }

    info.dev_num = sensor_attr->dev_num;
    info.chn_num = sensor_attr->chn_num;

    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_GET_SENSOR_FD,
            &info, sizeof(info), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("kd_mapi_vicap_get_sensor_fd failed\n");
    }

    memcpy(sensor_attr, &info, sizeof(k_vicap_sensor_attr));
    return ret;
}

k_s32 kd_mapi_vicap_dump_frame(k_vicap_dev dev_num, k_vicap_chn chn_num, k_vicap_dump_format foramt, k_video_frame_info *vf_info, k_u32 milli_sec)
{
    msg_vicap_frame_t info;
    k_s32 ret = 0;

    if(dev_num >= VICAP_MAX_DEV_NUMS)
    {
        return K_MAPI_ERR_VICAP_INVALID_DEVID;
    }
    if(chn_num >= VICAP_MAX_CHN_NUMS)
    {
        return K_MAPI_ERR_VICAP_INVALID_CHNID;
    }

    info.vicap_dev = dev_num;
    info.vicap_chn = chn_num;
    info.dump_format = foramt;
    info.milli_sec = milli_sec;

    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_DUMP_FRAME,
            &info, sizeof(info), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("kd_mapi_vicap_dump_frame failed\n");
    }
    memcpy(vf_info, &info.vf_info, sizeof(k_video_frame_info));
    return ret;
}

k_s32 kd_mapi_vicap_release_frame(k_vicap_dev dev_num, k_vicap_chn chn_num, const k_video_frame_info *vf_info)
{
    msg_vicap_frame_t info;
    k_s32 ret;

    if(dev_num >= VICAP_MAX_DEV_NUMS)
    {
        return K_MAPI_ERR_VICAP_INVALID_DEVID;
    }
    if(chn_num >= VICAP_MAX_CHN_NUMS)
    {
        return K_MAPI_ERR_VICAP_INVALID_CHNID;
    }

    info.vicap_dev = dev_num;
    info.vicap_chn = chn_num;

    memcpy(&info.vf_info, vf_info, sizeof(k_video_frame_info));

    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_RELEASE_FRAME,
            &info, sizeof(info), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }

    return ret;
}

k_s32 kd_mapi_vicap_get_sensor_info(k_vicap_sensor_info *sensor_info)
{
    k_s32 ret;
    msg_vicap_sensor_info_t msg_sensor_info;
    if(sensor_info->sensor_type >= SENSOR_TYPE_MAX)
    {
        return K_MAPI_ERR_VICAP_ILLEGAL_PARAM;
    }
    msg_sensor_info.sensor_type = sensor_info->sensor_type;

    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_GET_SENSOR_INFO,
            &msg_sensor_info, sizeof(msg_sensor_info), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("kd_mapi_vicap_get_sensor_info failed\n");
    }
    memcpy(sensor_info, &msg_sensor_info, sizeof(k_vicap_sensor_info));
    return ret;
}

k_s32 kd_mapi_vicap_set_dev_attr(k_vicap_dev_set_info attr_info)
{
    msg_vicap_dev_set_info_t msg_attr_info;
    memset(&msg_attr_info, 0, sizeof(msg_vicap_dev_set_info_t));
    memcpy(&msg_attr_info, &attr_info, sizeof(msg_vicap_dev_set_info_t));
    k_s32 ret = 0;
    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_SET_DEV_ATTR,
            &msg_attr_info, sizeof(msg_attr_info), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}

k_s32 kd_mapi_vicap_set_chn_attr(k_vicap_chn_set_info attr_info)
{
    msg_vicap_chn_set_info_t msg_attr_info;
    memset(&msg_attr_info, 0, sizeof(msg_vicap_chn_set_info_t));
    memcpy(&msg_attr_info, &attr_info, sizeof(msg_vicap_chn_set_info_t));
    k_s32 ret = 0;
    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_SET_CHN_ATTR,
            &msg_attr_info, sizeof(msg_attr_info), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}

k_s32 kd_mapi_vicap_start(k_vicap_dev vicap_dev)
{
    k_s32 ret = 0;
    k_vicap_dev dev_num = vicap_dev;
    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_START,
            &dev_num, sizeof(dev_num), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}

k_s32 kd_mapi_vicap_init(k_vicap_dev vicap_dev)
{
    k_s32 ret = 0;
    k_vicap_dev dev_num = vicap_dev;
    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_INIT,
            &dev_num, sizeof(dev_num), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}

k_s32 kd_mapi_vicap_start_stream(k_vicap_dev vicap_dev)
{
    k_s32 ret = 0;
    k_vicap_dev dev_num = vicap_dev;
    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_START_STREAM,
            &dev_num, sizeof(dev_num), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}

k_s32 kd_mapi_vicap_stop(k_vicap_dev vicap_dev)
{
    k_s32 ret = 0;
    k_vicap_dev dev_num = vicap_dev;
    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_STOP,
            &dev_num, sizeof(dev_num), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}


k_s32 kd_mapi_vicap_set_vi_drop_frame(k_vicap_csi_num csi, k_vicap_drop_frame *frame, k_bool enable)
{
    k_s32 ret = 0;
    msg_vicap_drop_frame_info_t drop_frame;

    drop_frame.csi = csi;
    drop_frame.enable = enable;
    memcpy(&drop_frame.frame, frame, sizeof(k_vicap_drop_frame));

    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_DROP_FRAME,
            &drop_frame, sizeof(drop_frame), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}

k_s32 kd_mapi_vicap_set_mclk(k_vicap_mclk_id id, k_vicap_mclk_sel sel, k_u8 mclk_div, k_u8 mclk_en)
{
    k_s32 ret = 0;
    msg_vicap_mclk_set_t mclk_set_info;
    mclk_set_info.id = id;
    mclk_set_info.sel = sel;
    mclk_set_info.mclk_div = mclk_div;
    mclk_set_info.mclk_en = mclk_en;

    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_SET_MCLK,
            &mclk_set_info, sizeof(mclk_set_info), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}

#define pr(fmt,...) fprintf(stderr,"\033[0m[client] "fmt"\n",##__VA_ARGS__)
#define DEBUG_MESSAGE 0

k_s32 kd_mapi_vicap_tuning(char* string, k_u32 size, char** response, k_u32* response_len)
{
    k_s32 ret = 0;
    k_ipcmsg_message_t *req = NULL;
    k_ipcmsg_message_t *resp = NULL;
    // FIXME: full size cause data error
    const k_u32 block_size = K_IPCMSG_MAX_CONTENT_LEN / 2;
    extern k_s32 mapi_media_msg_get_id(void);
    req = kd_ipcmsg_create_message(
        MODFD(K_MAPI_MOD_VICAP, 0, 0),
        MSG_CMD_MEDIA_VICAP_TUNING,
        &size, sizeof(k_u32)
    );
    if(req == NULL) {
        return -1;
    }
    pr("send command length %u", size);
    ret = kd_ipcmsg_send_only(mapi_media_msg_get_id(), req);
    kd_ipcmsg_destroy_message(req);
    if (ret) {
        return ret;
    }
    k_u32 offset = 0;
    do {
        k_u32 fixed_size = (size > offset + block_size) ? block_size : (size - offset);
        req = kd_ipcmsg_create_message(
            MODFD(K_MAPI_MOD_VICAP, 0, 0),
            MSG_CMD_MEDIA_VICAP_TUNING,
            string + offset, fixed_size
        );
        if(req == NULL) {
            return -1;
        }
        offset += fixed_size;
        if (offset >= size) {
            // last message
            ret = kd_ipcmsg_send_sync(mapi_media_msg_get_id(), req, &resp, K_IPCMSG_SEND_SYNC_TIMEOUT);
        } else {
            ret = kd_ipcmsg_send_only(mapi_media_msg_get_id(), req);
        }
        pr("send command %u/%u", offset, size);
        #if DEBUG_MESSAGE
        fwrite(req->pBody, 1, req->u32BodyLen, stderr);
        fprintf(stderr, "\n");
        #endif
        kd_ipcmsg_destroy_message(req);
        if (ret) {
            return ret;
        }
    } while(offset < size);
    k_u32 buffer_phys = 0;
    static k_u32 last_buffer_phys = 0;
    if (resp->u32BodyLen == 8) {
        *response_len = (*(k_u64*)resp->pBody) >> 32;
        buffer_phys = (*(k_u64*)resp->pBody) & 0xffffffffUL;
    }
    pr("get response length %u, code: %d, phys: %08x", *response_len, resp->s32RetVal, buffer_phys);
    ret = resp->s32RetVal;
    kd_ipcmsg_destroy_message(resp);

    static int mem_fd = -1;
    if (mem_fd < 0) {
        mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
        if (mem_fd < 0) {
            perror("open(\"/dev/mem\") error");
            return K_FAILED;
        }
    }

    static void* buffer_virt = NULL;
    static size_t buffer_size = 0;
    if ((last_buffer_phys != buffer_phys) || (buffer_size < *response_len)) {
        // mmap
        if (buffer_virt != MAP_FAILED) {
            munmap(buffer_virt, buffer_size);
            buffer_virt = MAP_FAILED;
        }
        // align 4k
        buffer_size = (*response_len & 0xfffff000UL) + 0x1000UL;
        buffer_virt = mmap(NULL, buffer_size, PROT_READ, MAP_SHARED, mem_fd, buffer_phys);
        if (buffer_virt == MAP_FAILED) {
            perror("mmap(\"/dev/mem\") error");
            return K_FAILED;
        }
    }

    *response = malloc(*response_len);
    memcpy(*response, buffer_virt, *response_len);
    return ret;
}

k_s32 kd_mapi_vicap_3d_mode_crtl(k_u8 mode_en)
{
    k_s32 ret = 0;
    k_u8 en ;

    en = mode_en;

    ret = mapi_send_sync(MODFD(K_MAPI_MOD_VICAP, 0, 0), MSG_CMD_MEDIA_VICAP_SET_3D_MODE_EN,
            &en, sizeof(k_u8), NULL);

    if(ret != K_SUCCESS) {
        mapi_vicap_error_trace("mapi_send_sync failed\n");
    }
    return ret;
}