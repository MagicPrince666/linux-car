/**
 * @file audio_encode.h
 * @author 黄李全 (846863428@qq.com)
 * @brief 从PCM接口获取音频并压缩成AAC格式
 * 音频格式 :音频源 UAC接口 格式是 PCM SND_PCM_FORMAT_S16_LE 32KHz 单声道
 * @version 0.1
 * @date 2023-02-26
 * @copyright Copyright (c) {2023} 个人版权所有,仅供学习
 */
#pragma once

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <faac.h>

#include <iostream>
#include <memory>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>

#define RATE 44100 // PCM采样频率
#define SIZE 16    // PCM量化位数
#define CHANNELS 1 // PCM声道数目

class EnCodeAAC
{
public:
    explicit EnCodeAAC(std::string sound_card = "hw:0,0", int rate = RATE, int formate = SND_PCM_FORMAT_S16_LE, int channels = CHANNELS, snd_pcm_uframes_t frames = (snd_pcm_uframes_t)48);
    ~EnCodeAAC();

    int recodeAAC(unsigned char *&bufferOut);

    bool recodeRun(bool flag);

private:
    void initPcm();
    void initAAC();
    int recodePcm(char *&buffer, snd_pcm_uframes_t frame); // 执行录音操作：参数1:采集到的音频的存放数据，参数2：大小

    int volumeAdjust(char *in_buf, float vol);

    std::string sound_card_;
    snd_pcm_t *pcm_handle_;
    snd_pcm_uframes_t pcm_frames_; // set frames of each period
    int pcm_rate_;                 // 采样率
    int pcm_format_;               // pcm采样格式
    int pcm_channels_;             // pcm声道

    faacEncHandle hEncoder_;
    int aac_factor_;
    int aac_size_;
    int sizeOfperiod_;

    int looptimes_;
    int loopmode_;

    uint64_t nInputSamples_;
    int nPCMBufferSize_;
    uint64_t nMaxOutputBytes_;

    char *buffer_;
    uint8_t *pbPCMBuffer_;
    uint8_t *pbAACBuffer_;
    // std::unique_ptr<char> buffer_;
    // std::unique_ptr<uint8_t> pbPCMBuffer_;
    // std::unique_ptr<uint8_t> pbAACBuffer_;

    bool bThreadRunFlag_;
    std::mutex pcm_lock_;
};
