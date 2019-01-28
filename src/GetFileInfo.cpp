/*
 * GetFileInfo.cpp
 *
 *  Created on: Jan 28, 2019
 *      Author: jx
 */

#include "GetFileInfo.h"
#include <stdio.h>
using namespace std;
char* BytesToSize( double Bytes )
{
    float tb = 1099511627776;
    float gb = 1073741824;
    float mb = 1048576;
    float kb = 1024;

    char returnSize[256];

    if( Bytes >= tb )
        sprintf(returnSize, "%.2f TB", (float)Bytes/tb);
    else if( Bytes >= gb && Bytes < tb )
        sprintf(returnSize, "%.2f GB", (float)Bytes/gb);
    else if( Bytes >= mb && Bytes < gb )
        sprintf(returnSize, "%.2f MB", (float)Bytes/mb);
    else if( Bytes >= kb && Bytes < mb )
        sprintf(returnSize, "%.2f KB", (float)Bytes/kb);
    else if ( Bytes < kb)
        sprintf(returnSize, "%.2f Bytes", Bytes);
    else
        sprintf(returnSize, "%.2f Bytes", Bytes);

    static char ret[256];
    strcpy(ret, returnSize);
    return ret;
}

void read_av_info(const char * av_pathfile)
{
	printf("%s\n", av_pathfile);
    AVFormatContext     *pfmtCxt    = NULL;

    int                 audioStreamIdx  = -1;
    int                 videoStreamIdx  = -1;
    //初始化 libavformat和注册所有的muxers、demuxers和protocols
    av_register_all();

    //以输入方式打开一个媒体文件,也即源文件
    int ok = avformat_open_input(&pfmtCxt, av_pathfile, NULL, NULL);
    if (ok != 0) {
        printf("Could not open file.\n");
        //fflush(stdout);
    }

    //通过读取媒体文件的中的包来获取媒体文件中的流信息,对于没有头信息的文件如(mpeg)是非常有用的
    //也就是把媒体文件中的音视频流等信息读出来,保存在容器中,以便解码时使用
    ok = avformat_find_stream_info(pfmtCxt, NULL);
    if (ok != 0) {
        printf("find stream info error.\n");
    }

    for (int i = 0; i < pfmtCxt->nb_streams; i++) {
        if (pfmtCxt->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIdx = i;
        } else if (pfmtCxt->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIdx = i;
        }
    }

    AVStream *videostream = NULL;
    AVStream *audiostream = NULL;
    if (videoStreamIdx != -1) {
        videostream = pfmtCxt->streams[videoStreamIdx];
    }
    if (audioStreamIdx != -1) {
        audiostream = pfmtCxt->streams[audioStreamIdx];
    }
    printf("==============================av_dump_format==================================\n");
    av_dump_format(pfmtCxt, 0, 0, 0);
    //******************************输出相关media文件信息******************************
    printf("===========================================================================\n");
    //printf("文件名 : %s \n",pfmtCxt->url);
    printf("输入格式 : %s \n全称 : %s \n",pfmtCxt->iformat->name,pfmtCxt->iformat->long_name);

    int64_t tns, thh, tmm, tss;
    tns  = pfmtCxt->duration / 1000000;
    thh  = tns / 3600;
    tmm  = (tns % 3600) / 60;
    tss  = (tns % 60);

    printf("总时长 : %f ms,fmt:%02lld:%02lld:%02lld \n总比特率 : %f kbs\n",(pfmtCxt->duration * 1.0 / AV_TIME_BASE) * 1000,thh,tmm,tss,pfmtCxt->bit_rate / 1000.0);//1000 bit/s = 1 kbit/s
    double fsize = (pfmtCxt->duration * 1.0 / AV_TIME_BASE * pfmtCxt->bit_rate / 8.0);
    printf("文件大小 : %s\n",BytesToSize(fsize));
    printf("协议白名单 : %s \n协义黑名单 : %s\n",pfmtCxt->protocol_whitelist,pfmtCxt->protocol_blacklist);
    printf("数据包的最大数量 : %d\n",pfmtCxt->max_ts_probe);
    printf("最大缓冲时间 : %lld\n",pfmtCxt->max_interleave_delta);
    printf("缓冲帧的最大缓冲 : %u Bytes\n",pfmtCxt->max_picture_buffer);
    printf("metadata:\n");
    AVDictionary *metadata = pfmtCxt->metadata;
    if (metadata) {
        AVDictionaryEntry *entry = NULL;
        while ((entry = av_dict_get(metadata, "", entry, AV_DICT_IGNORE_SUFFIX))) {
            printf("\t%s : %s\n",entry->key,entry->value);
        }
    }
    if (videostream) {
        printf("视频流信息(%s):\n",av_get_media_type_string(videostream->codec->codec_type));
        printf("\tStream #%d\n",videoStreamIdx);
        printf("\t总帧数 : %lld\n",videostream->nb_frames);
        const char *avcodocname = avcodec_get_name(videostream->codec->codec_id);
        const char *profilestring = avcodec_profile_name(videostream->codec->codec_id,videostream->codec->profile);
        //char * codec_fourcc = av_fourcc2str(videostream->codec->codec_tag);
        //printf("\t编码方式 : %s\n\tCodec Profile : %s\n\tCodec FourCC : %s\n",avcodocname,profilestring,codec_fourcc);
        ///如果是C++引用(AVPixelFormat)注意下强转类型
        //const char *pix_fmt_name = videostream->codec->format == AV_PIX_FMT_NONE ? "none" : av_get_pix_fmt_name(videostream->codec->format);

        //printf("\t显示编码格式(color space) : %s \n",pix_fmt_name);
        printf("\t宽 : %d pixels,高 : %d pixels \n",videostream->codec->width,videostream->codec->height);
        AVRational display_aspect_ratio;
        av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
                  videostream->codec->width  * (int64_t)videostream->sample_aspect_ratio.num,
                  videostream->codec->height * (int64_t)videostream->sample_aspect_ratio.den,
                  1024 * 1024);
        printf("\tsimple_aspect_ratio(SAR) : %d : %d\n\tdisplay_aspect_ratio(DAR) : %d : %d \n",videostream->sample_aspect_ratio.num,
               videostream->sample_aspect_ratio.den,display_aspect_ratio.num,display_aspect_ratio.den);
        printf("\t最低帧率 : %f fps\n\t平均帧率 : %f fps\n",av_q2d(videostream->r_frame_rate),av_q2d(videostream->avg_frame_rate));
        printf("\t每个像素点的比特数 : %d bits\n",videostream->codec->bits_per_raw_sample);
        printf("\t每个像素点编码比特数 : %d bits\n",videostream->codec->bits_per_coded_sample); //YUV三个分量每个分量是8,即24
        printf("\t视频流比特率 : %f kbps\n",videostream->codec->bit_rate / 1000.0);
        printf("\t基准时间 : %d / %d = %f \n",videostream->time_base.num,videostream->time_base.den,av_q2d(videostream->time_base));
        printf("\t视频流时长 : %f ms\n",videostream->duration * av_q2d(videostream->time_base) * 1000);
        printf("\t帧率(tbr) : %f\n",av_q2d(videostream->r_frame_rate));
        printf("\t文件层的时间精度(tbn) : %f\n",1/av_q2d(videostream->time_base));
        printf("\t视频层的时间精度(tbc) : %f\n",1/av_q2d(videostream->codec ->time_base));

        double s = videostream->duration * av_q2d(videostream->time_base);
        int64_t tbits = videostream->codec->bit_rate * s;
        double stsize = tbits / 8;
        printf("\t视频流大小(Bytes) : %s \n",BytesToSize(stsize));
        printf("\tmetadata:\n");

        AVDictionary *metadata = videostream->metadata;
        if (metadata) {
            AVDictionaryEntry *entry = NULL;
            while ((entry = av_dict_get(metadata, "", entry, AV_DICT_IGNORE_SUFFIX))) {
                printf("\t\t%s : %s\n",entry->key,entry->value);
            }
        }
    }

    if (audiostream) {
        printf("音频流信息(%s):\n",av_get_media_type_string(audiostream->codec->codec_type));
        printf("\tStream #%d\n",audioStreamIdx);
        printf("\t音频时长 : %f ms\n",audiostream->duration * av_q2d(audiostream->time_base) * 1000);
        const char *avcodocname = avcodec_get_name(audiostream->codec->codec_id);
        const char *profilestring = avcodec_profile_name(audiostream->codec->codec_id,audiostream->codec->profile);
        //char * codec_fourcc = av_fourcc2str(audiostream->codec->codec_tag);
        //printf("\t编码格式 %s (%s,%s)\n",avcodocname,profilestring,codec_fourcc);
        printf("\t音频采样率 : %d Hz\n",audiostream->codec->sample_rate);
        printf("\t音频声道数 : %d \n",audiostream->codec->channels);
        printf("\t音频流比特率 : %f kbps\n",audiostream->codec->bit_rate / 1000.0);
        double s = audiostream->duration * av_q2d(audiostream->time_base);
        int64_t tbits = audiostream->codec->bit_rate * s;
        double stsize = tbits / 8;
        printf("\t音频流大小(Bytes) : %s\n",BytesToSize(stsize));
    }
    avformat_close_input(&pfmtCxt);
}
