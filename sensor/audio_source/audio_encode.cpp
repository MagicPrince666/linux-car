#include <new>

#include "audio_encode.h"

EnCodeAAC::EnCodeAAC(std::string sound_card, int rate, int formate, int channels, snd_pcm_uframes_t frames)
{
    bThreadRunFlag_ = true;
    sound_card_     = sound_card;
    pcm_rate_       = rate;
    pcm_format_     = formate;
    pcm_channels_   = channels;
    pcm_frames_     = frames;

    aac_size_ = 1024;
    buffer_   = new (std::nothrow) char[10240];
    // buffer_ = std::make_unique<char>(10240);

    initPcm();
    initAAC();
    std::cout << "Start acc recorder" << std::endl;
}

EnCodeAAC::~EnCodeAAC()
{
    std::cout << "Stop acc recorder" << std::endl;
    bThreadRunFlag_ = false;
    if (pcm_handle_) {
        snd_pcm_drain(pcm_handle_);
        snd_pcm_close(pcm_handle_);
        pcm_handle_ = nullptr;
    }

    if (hEncoder_) {
        faacEncClose(hEncoder_);
        hEncoder_ = nullptr;
    }

    if (buffer_) {
        delete[] buffer_;
        buffer_ = nullptr;
    }
    if (pbAACBuffer_) {
        delete[] pbAACBuffer_;
        pbAACBuffer_ = nullptr;
    }
    if (pbPCMBuffer_) {
        delete[] pbPCMBuffer_;
        pbPCMBuffer_ = nullptr;
    }
}

void EnCodeAAC::initPcm()
{
    int rc;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;

    /* open PCM device for recording (capture) */
    rc = snd_pcm_open(&pcm_handle_, sound_card_.c_str(), SND_PCM_STREAM_CAPTURE, 0);
    if (rc < 0) {
        fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
    }
    assert(rc >= 0);

    /* alloc a hardware params object */
    snd_pcm_hw_params_alloca(&params);

    /* fill it with default values */
    snd_pcm_hw_params_any(pcm_handle_, params);

    /* interleaved mode */
    rc = snd_pcm_hw_params_set_access(pcm_handle_, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    assert(rc >= 0);

    /* signed 16 bit little ending format */
    rc = snd_pcm_hw_params_set_format(pcm_handle_, params, (snd_pcm_format_t)pcm_format_);
    assert(rc >= 0);

    /* two channels */
    rc = snd_pcm_hw_params_set_channels(pcm_handle_, params, pcm_channels_);
    assert(rc >= 0);

    /* 44100 bits/second sampling rate (CD quality) */
    val = pcm_rate_;
    rc  = snd_pcm_hw_params_set_rate_near(pcm_handle_, params, &val, &dir);
    assert(rc >= 0);

    /* set period size to 32 frames */
    snd_pcm_uframes_t frames = pcm_frames_; // 设置每个周期多少帧、一个周期就是一个完整的中断;
    snd_pcm_hw_params_set_period_size_near(pcm_handle_, params, &frames, &dir);
    printf("frames: %lu  pcm_frames_: %lu \n", frames, pcm_frames_);

    // unsigned int buffer_time, period_time;
    // // 获取最大的缓冲时间,buffer_time单位为us,500000us=0.5s
    // snd_pcm_hw_params_get_buffer_time_max(params, &buffer_time, 0);
    // if (buffer_time > 500000) {
    //     buffer_time = 500000;
    // }

    // // 设置缓冲时间
    // rc = snd_pcm_hw_params_set_buffer_time_near(pcm_handle_, params, &buffer_time, 0);
    // if (rc < 0) {
    //     printf("Failed to set PCM device to sample rate\n");
    // }
    // assert(rc >= 0);
    // // 设置周期时间
    // period_time = 26315;
    // rc          = snd_pcm_hw_params_set_period_time_near(pcm_handle_, params, &period_time, 0);
    // if (rc < 0) {
    //     printf("Failed to set PCM device to period time\n");
    // }
    // assert(rc >= 0);

    /*使采集卡处于空闲状态*/
    // snd_pcm_hw_params_free(params);

    rc = snd_pcm_hw_params(pcm_handle_, params);
    if (rc < 0) {
        fprintf(stderr, "unable to set hw params: %s\n", snd_strerror(rc));
    }
    assert(rc >= 0);

    /* use a buffer large enough to hold one period */
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    snd_pcm_hw_params_get_rate(params, &val, &dir);
    printf("frames: %lu  pcm_frames_:%lu rate:%d \n", frames, pcm_frames_, val);
    pcm_frames_ = frames;

    aac_factor_ = pcm_channels_;
    aac_factor_ = aac_factor_ * (pcm_format_ == SND_PCM_FORMAT_S16_LE ? 2 : 1);
    // factor 每一帧的字节

    aac_size_ = frames * aac_factor_; // 一个周期的字节数
    printf("factor_: %d  pcm_channels_: %d aac_size_: %d frames: %lu\n", aac_factor_, pcm_channels_, aac_size_, frames);
}

void EnCodeAAC::initAAC()
{
    uint64_t nSampleRate = pcm_rate_;                                     // 采样率
    uint32_t nChannels   = pcm_channels_;                                 // 声道数
    uint32_t nPCMBitSize = pcm_format_ == SND_PCM_FORMAT_S16_LE ? 16 : 8; // 单样本位数

    faacEncConfigurationPtr pConfiguration;

    // (1) Open FAAC engine
    // nInputSamples_ 帧的长度
    hEncoder_ = faacEncOpen(nSampleRate, nChannels, &nInputSamples_, &nMaxOutputBytes_);
    if (hEncoder_ == nullptr) {
        printf("[ERROR] Failed to call faacEncOpen()\n");
        return;
    }

    // 帧长度 ，每一帧 * 2  (16/8)
    nPCMBufferSize_ = nInputSamples_ * nPCMBitSize / 8;
    pbPCMBuffer_    = new (std::nothrow) uint8_t[nPCMBufferSize_];
    pbAACBuffer_    = new (std::nothrow) uint8_t[nMaxOutputBytes_];
    // pbPCMBuffer_ = std::make_unique<uint8_t>(nPCMBufferSize_);
    // pbAACBuffer_ = std::make_unique<uint8_t>(nMaxOutputBytes_);

    // (2.1) Get current encoding configuration
    pConfiguration                = faacEncGetCurrentConfiguration(hEncoder_);
    pConfiguration->inputFormat   = FAAC_INPUT_16BIT;
    pConfiguration->aacObjectType = LOW; // LC编码

    // (2.2) Set encoding configuration
    // nRet = faacEncSetConfiguration(hEncoder_, pConfiguration);
    faacEncSetConfiguration(hEncoder_, pConfiguration);

    snd_pcm_uframes_t framesofperod = pcm_frames_;
    int factor                      = aac_factor_;

    sizeOfperiod_ = framesofperod * factor;
    looptimes_    = nPCMBufferSize_ / sizeOfperiod_;
    loopmode_     = nPCMBufferSize_ % sizeOfperiod_;
    loopmode_     = loopmode_ / factor;
}

bool EnCodeAAC::recodeRun(bool flag)
{
    std::lock_guard<std::mutex> lck(pcm_lock_);
    bThreadRunFlag_ = flag;
    return bThreadRunFlag_;
}

int EnCodeAAC::recodeAAC(unsigned char *&bufferOut)
{
    if (!bThreadRunFlag_) {
        return 0;
    }
    for (int j = 0; j < looptimes_; j++) {
        recodePcm(buffer_, pcm_frames_);
        memcpy(pbPCMBuffer_ + j * sizeOfperiod_, buffer_, aac_size_);
    }
    loopmode_ = 16;

    recodePcm(buffer_, loopmode_); // 卡在这里 loopmode_ = 0 导致卡住

    memcpy(pbPCMBuffer_ + looptimes_ * sizeOfperiod_, buffer_, loopmode_ * aac_factor_);

    // fwrite(pbPCMBuffer_, 1, nPCMBufferSize_, fpPcm);

    int nRet = faacEncEncode(hEncoder_, (int *)pbPCMBuffer_, nInputSamples_, pbAACBuffer_, nMaxOutputBytes_);

    // printf("faacEncEncode nRet:%d\n ",nRet);
    // fwrite(pbAACBuffer_, 1, nRet, fpOut);

    bufferOut = (unsigned char *)pbAACBuffer_;

    return nRet;
}

int EnCodeAAC::recodePcm(char *&buffer, snd_pcm_uframes_t frame)
{
    if (!bThreadRunFlag_) {
        return 0;
    }
    std::lock_guard<std::mutex> lck(pcm_lock_);

    int rc = -1;
    do {
        if(pcm_handle_ && buffer) {
            rc = snd_pcm_readi(pcm_handle_, buffer, frame);
        } else {
            rc = -1;
        }

        if (rc == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(pcm_handle_);
            usleep(100);
        } else if (rc < 0) {
            fprintf(stderr, "error from read: %s\n", snd_strerror(rc));
            snd_pcm_close(pcm_handle_);
            pcm_handle_ = nullptr;
        } else if (rc != (int)frame) {
            fprintf(stderr, "short read, read %d frames\n", rc);
        }

    } while (rc <= 0);

    return rc;
}

// 音量调节
int volumeAdjust(char *in_buf, float vol)
{
    short buf = 0;
    buf       = *in_buf + (*(in_buf + 1) << 8);

    if (buf >= -1 && buf <= 1) {
        buf = 0;
    }

    buf = buf * vol;

    if (buf >= 32767) {
        buf           = 0;
        *in_buf       = (char)buf;
        *(in_buf + 1) = buf >> 8;
    } else if (buf <= -32768) {
        buf           = 0;
        *in_buf       = (char)buf;
        *(in_buf + 1) = buf >> 8;
    } else {
        *in_buf       = (char)buf;
        *(in_buf + 1) = buf >> 8;
    }
    return 0;
}
