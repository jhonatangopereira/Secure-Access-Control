/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "soc/rmt_struct.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RMT_LL_MAX_LOOP_COUNT           (1023)/*!< Max loop count that hardware is supported */

#define RMT_LL_HW_BASE  (&RMT)
#define RMT_LL_MEM_BASE (&RMTMEM)

// Note: TX and RX channel number are all index from zero in the LL driver
// i.e. tx_channel belongs to [0,2], and rx_channel belongs to [0,2]

__attribute__((always_inline))
static inline void rmt_ll_enable_drive_clock(rmt_dev_t *dev, bool enable)
{
    dev->sys_conf.clk_en = enable; // register clock gating
    dev->sys_conf.mem_clk_force_on = enable; // memory clock gating
}

__attribute__((always_inline))
static inline void rmt_ll_power_down_mem(rmt_dev_t *dev, bool enable)
{
    dev->sys_conf.mem_force_pu = !enable;
    dev->sys_conf.mem_force_pd = enable;
}

__attribute__((always_inline))
static inline bool rmt_ll_is_mem_power_down(rmt_dev_t *dev)
{
    // the RTC domain can also power down RMT memory
    // so it's probably not enough to detect whether it's powered down or not
    // mem_force_pd has higher priority than mem_force_pu
    return (dev->sys_conf.mem_force_pd) || !(dev->sys_conf.mem_force_pu);
}

__attribute__((always_inline))
static inline void rmt_ll_enable_mem_access(rmt_dev_t *dev, bool enable)
{
    dev->sys_conf.fifo_mask = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_set_group_clock_src(rmt_dev_t *dev, uint32_t channel, uint8_t src, uint8_t div_num, uint8_t div_a, uint8_t div_b)
{
    // Formula: rmt_sclk = module_clock_src / (1 + div_num + div_a / div_b)
    dev->sys_conf.sclk_active = 0;
    dev->sys_conf.sclk_sel = src;
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->sys_conf, sclk_div_num, div_num);
    dev->sys_conf.sclk_div_a = div_a;
    dev->sys_conf.sclk_div_b = div_b;
    dev->sys_conf.sclk_active = 1;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_group_clock_src(rmt_dev_t *dev, uint32_t channel)
{
    return dev->sys_conf.sclk_sel;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_reset_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->ref_cnt_rst.val |= (1 << channel);
}

__attribute__((always_inline))
static inline void rmt_ll_tx_reset_channels_clock_div(rmt_dev_t *dev, uint32_t channel_mask)
{
    dev->ref_cnt_rst.val |= channel_mask;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_reset_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    dev->ref_cnt_rst.val |= (1 << (channel + 2));
}

__attribute__((always_inline))
static inline void rmt_ll_tx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_conf[channel].mem_rd_rst = 1;
    dev->tx_conf[channel].mem_rd_rst = 0;
    dev->tx_conf[channel].mem_rst = 1;
    dev->tx_conf[channel].mem_rst = 0;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_reset_pointer(rmt_dev_t *dev, uint32_t channel)
{
    dev->rx_conf[channel].conf1.mem_wr_rst = 1;
    dev->rx_conf[channel].conf1.mem_wr_rst = 0;
    dev->rx_conf[channel].conf1.mem_rst = 1;
    dev->rx_conf[channel].conf1.mem_rst = 0;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_start(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_conf[channel].conf_update = 1;
    dev->tx_conf[channel].tx_start = 1;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_stop(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_conf[channel].tx_stop = 1;
    dev->tx_conf[channel].conf_update = 1;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_enable(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->rx_conf[channel].conf1.rx_en = enable;
    dev->rx_conf[channel].conf1.conf_update = 1;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->tx_conf[channel].mem_size = block_num;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_mem_blocks(rmt_dev_t *dev, uint32_t channel, uint8_t block_num)
{
    dev->rx_conf[channel].conf0.mem_size = block_num;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_conf[channel].mem_size;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_mem_blocks(rmt_dev_t *dev, uint32_t channel)
{
    return dev->rx_conf[channel].conf0.mem_size;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->tx_conf[channel], div_cnt, div);
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_channel_clock_div(rmt_dev_t *dev, uint32_t channel, uint32_t div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->rx_conf[channel].conf0, div_cnt, div);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->tx_conf[channel], div_cnt);
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_channel_clock_div(rmt_dev_t *dev, uint32_t channel)
{
    return HAL_FORCE_READ_U32_REG_FIELD(dev->rx_conf[channel].conf0, div_cnt);
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_pingpong(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].mem_tx_wrap_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_idle_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    dev->rx_conf[channel].conf0.idle_thres = thres;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_idle_thres(rmt_dev_t *dev, uint32_t channel)
{
    return dev->rx_conf[channel].conf0.idle_thres;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_mem_owner(rmt_dev_t *dev, uint32_t channel, uint8_t owner)
{
    dev->rx_conf[channel].conf1.mem_owner = owner;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_mem_owner(rmt_dev_t *dev, uint32_t channel)
{
    return dev->rx_conf[channel].conf1.mem_owner;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_loop(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].tx_conti_mode = enable;
}

__attribute__((always_inline))
static inline bool rmt_ll_is_tx_loop_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_conf[channel].tx_conti_mode;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_loop_count(rmt_dev_t *dev, uint32_t channel, uint32_t count)
{
    dev->tx_lim[channel].tx_loop_num = count;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_reset_loop(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_lim[channel].loop_count_reset = 1;
    dev->tx_lim[channel].loop_count_reset = 0;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_loop_count(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_lim[channel].tx_loop_cnt_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_sync(rmt_dev_t *dev, bool enable)
{
    dev->tx_sim.en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_add_to_sync_group(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_sim.val |= 1 << channel;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_remove_from_sync_group(rmt_dev_t *dev, uint32_t channel)
{
    dev->tx_sim.val &= ~(1 << channel);
}

__attribute__((always_inline))
static inline void rmt_ll_rx_enable_filter(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->rx_conf[channel].conf1.rx_filter_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_filter_thres(rmt_dev_t *dev, uint32_t channel, uint32_t thres)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->rx_conf[channel].conf1, rx_filter_thres, thres);
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_idle(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].idle_out_en = enable;
}

__attribute__((always_inline))
static inline bool rmt_ll_is_tx_idle_enabled(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_conf[channel].idle_out_en;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_idle_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->tx_conf[channel].idle_out_lv = level;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_idle_level(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_conf[channel].idle_out_lv;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->rx_status[channel].val;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_tx_get_channel_status(rmt_dev_t *dev, uint32_t channel)
{
    return dev->tx_status[channel].val;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->tx_lim[channel].limit = limit;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_limit(rmt_dev_t *dev, uint32_t channel, uint32_t limit)
{
    dev->rx_lim[channel].rx_lim = limit;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_rx_get_limit(rmt_dev_t *dev, uint32_t channel)
{
    return dev->rx_lim[channel].rx_lim;
}

__attribute__((always_inline))
static inline void rmt_ll_enable_interrupt(rmt_dev_t *dev, uint32_t mask, bool enable)
{
    if (enable) {
        dev->int_ena.val |= mask;
    } else {
        dev->int_ena.val &= ~mask;
    }
}

__attribute__((always_inline))
static inline void rmt_ll_enable_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << channel);
    } else {
        dev->int_ena.val &= ~(1 << channel);
    }
}

__attribute__((always_inline))
static inline void rmt_ll_enable_tx_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 4));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 4));
    }
}

__attribute__((always_inline))
static inline void rmt_ll_enable_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 2));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 2));
    }
}

__attribute__((always_inline))
static inline void rmt_ll_enable_rx_err_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 6));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 6));
    }
}

__attribute__((always_inline))
static inline void rmt_ll_enable_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 8));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 8));
    }
}

__attribute__((always_inline))
static inline void rmt_ll_enable_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 12));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 12));
    }
}

__attribute__((always_inline))
static inline void rmt_ll_enable_rx_thres_interrupt(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    if (enable) {
        dev->int_ena.val |= (1 << (channel + 10));
    } else {
        dev->int_ena.val &= ~(1 << (channel + 10));
    }
}

__attribute__((always_inline))
static inline void rmt_ll_clear_tx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_rx_end_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 2));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_tx_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 4));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_rx_err_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 6));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_tx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 8));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_tx_loop_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 12));
}

__attribute__((always_inline))
static inline void rmt_ll_clear_rx_thres_interrupt(rmt_dev_t *dev, uint32_t channel)
{
    dev->int_clr.val = (1 << (channel + 10));
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_end_interrupt_status(rmt_dev_t *dev)
{
    return dev->int_st.val & 0x03;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_rx_end_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 2) & 0x03;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_err_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 4) & 0x03;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_rx_err_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 6) & 0x03;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_thres_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 8) & 0x03;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_rx_thres_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 10) & 0x03;
}

__attribute__((always_inline))
static inline uint32_t rmt_ll_get_tx_loop_interrupt_status(rmt_dev_t *dev)
{
    return (dev->int_st.val >> 12) & 0x03;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    // In case the compiler optimise a 32bit instruction (e.g. s32i) into two 16bit instruction (e.g. s16i, which is not allowed to access a register)
    // We take care of the "read-modify-write" procedure by ourselves.
    typeof(dev->tx_carrier[0]) reg;
    reg.high = high_ticks;
    reg.low = low_ticks;
    dev->tx_carrier[channel].val = reg.val;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t high_ticks, uint32_t low_ticks)
{
    typeof(dev->rx_carrier[0]) reg;
    reg.high_thres = high_ticks;
    reg.low_thres = low_ticks;
    dev->rx_carrier[channel].val = reg.val;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks)
{
    *high_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->tx_carrier[channel], high);
    *low_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->tx_carrier[channel], low);
}

__attribute__((always_inline))
static inline void rmt_ll_rx_get_carrier_high_low_ticks(rmt_dev_t *dev, uint32_t channel, uint32_t *high_ticks, uint32_t *low_ticks)
{
    *high_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->rx_carrier[channel], high_thres);
    *low_ticks = HAL_FORCE_READ_U32_REG_FIELD(dev->rx_carrier[channel], low_thres);
}

__attribute__((always_inline))
static inline void rmt_ll_tx_enable_carrier_modulation(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].carrier_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_enable_carrier_demodulation(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->rx_conf[channel].conf0.carrier_en = enable;
}

__attribute__((always_inline))
static inline void rmt_ll_tx_set_carrier_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->tx_conf[channel].carrier_out_lv = level;
}

__attribute__((always_inline))
static inline void rmt_ll_rx_set_carrier_level(rmt_dev_t *dev, uint32_t channel, uint8_t level)
{
    dev->rx_conf[channel].conf0.carrier_out_lv = level;
}

// set true, enable carrier in all RMT state (idle, reading, sending)
// set false, enable carrier only in sending state (i.e. there're effective data in RAM to be sent)
__attribute__((always_inline))
static inline void rmt_ll_tx_set_carrier_always_on(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->tx_conf[channel].carrier_eff_en = !enable;
}

//Writes items to the specified TX channel memory with the given offset and length.
//the caller should ensure that (length + off) <= (memory block * SOC_RMT_MEM_WORDS_PER_CHANNEL)
__attribute__((always_inline))
static inline void rmt_ll_write_memory(rmt_mem_t *mem, uint32_t channel, const void *data, size_t length_in_words, size_t off)
{
    volatile uint32_t *to = (volatile uint32_t *)&mem->chan[channel].data32[off];
    uint32_t *from = (uint32_t *)data;
    while (length_in_words--) {
        *to++ = *from++;
    }
}

__attribute__((always_inline))
static inline void rmt_ll_rx_enable_pingpong(rmt_dev_t *dev, uint32_t channel, bool enable)
{
    dev->rx_conf[channel].conf1.mem_rx_wrap_en = enable;
}

#ifdef __cplusplus
}
#endif
