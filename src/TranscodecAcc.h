/*
 * TranscodecAcc.h
 *
 *  Created on: Jan 28, 2019
 *      Author: jx
 */

#ifndef SRC_TRANSCODECACC_H_
#define SRC_TRANSCODECACC_H_
#include <iostream>
extern "C"
{
	#include <stdio.h>

	#include "libavformat/avformat.h"
	#include "libavformat/avio.h"

	#include "libavcodec/avcodec.h"

	#include "libavutil/audio_fifo.h"
	#include "libavutil/avassert.h"
	#include "libavutil/avstring.h"
	#include "libavutil/frame.h"
	#include "libavutil/opt.h"

	#include "libswresample/swresample.h"
}

const char *get_error_text(const int error);

int open_input_file(const char *filename,
                           AVFormatContext **input_format_context,
                           AVCodecContext **input_codec_context);
int open_output_file(const char *filename,
                            AVCodecContext *input_codec_context,
                            AVFormatContext **output_format_context,
                            AVCodecContext **output_codec_context);
int init_resampler(AVCodecContext *input_codec_context,
                          AVCodecContext *output_codec_context,
                          SwrContext **resample_context);
int init_fifo(AVAudioFifo **fifo, AVCodecContext *output_codec_context);
int write_output_file_header(AVFormatContext *output_format_context);


int read_decode_convert_and_store(AVAudioFifo *fifo,
                                         AVFormatContext *input_format_context,
                                         AVCodecContext *input_codec_context,
                                         AVCodecContext *output_codec_context,
                                         SwrContext *resampler_context,
                                         int *finished);
int load_encode_and_write(AVAudioFifo *fifo,
                                 AVFormatContext *output_format_context,
                                 AVCodecContext *output_codec_context);
int encode_audio_frame(AVFrame *frame,
                              AVFormatContext *output_format_context,
                              AVCodecContext *output_codec_context,
                              int *data_present);
int write_output_file_trailer(AVFormatContext *output_format_context);


int decode_audio_frame(AVFrame *frame,
                              AVFormatContext *input_format_context,
                              AVCodecContext *input_codec_context,
                              int *data_present, int *finished);
void init_packet(AVPacket *packet);
int init_input_frame(AVFrame **frame);
int init_converted_samples(uint8_t ***converted_input_samples,
                                  AVCodecContext *output_codec_context,
                                  int frame_size);
int convert_samples(const uint8_t **input_data,
                           uint8_t **converted_data, const int frame_size,
                           SwrContext *resample_context);
int add_samples_to_fifo(AVAudioFifo *fifo,
                               uint8_t **converted_input_samples,
                               const int frame_size);
int init_output_frame(AVFrame **frame,
                             AVCodecContext *output_codec_context,
                             int frame_size);

#endif /* SRC_TRANSCODECACC_H_ */
