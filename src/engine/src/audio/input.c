#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "stargate.h"
#include "csv/2d.h"
#include "files.h"
#include "audio/input.h"


void g_pyaudio_input_init(t_pyaudio_input * f_result, SGFLT a_sr)
{
    f_result->channels = 1;
    f_result->stereo_ch = -1;
    f_result->sf_info.channels = 1;
    f_result->sf_info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    f_result->sf_info.samplerate = (int)(a_sr);

    f_result->sndfile = NULL;

    f_result->buffer_iterator[0] = 0;
    f_result->buffer_iterator[1] = 0;

    int f_i;

    for(f_i = 0; f_i < AUDIO_INPUT_REC_BUFFER_SIZE; ++f_i)
    {
        f_result->rec_buffers[0][f_i] = 0.0f;
        f_result->rec_buffers[1][f_i] = 0.0f;
    }

    f_result->rec = 0;
    f_result->monitor = 0;
    f_result->current_buffer = 0;
    f_result->buffer_to_flush = 0;
    f_result->flush_last_buffer_pending = 0;
    f_result->output_track = 0;
    f_result->output_mode = 0;
    f_result->vol = 0.0f;
    f_result->vol_linear = 1.0f;
}

void v_audio_input_record_set(
        t_pyaudio_input * self, char * a_file_out)
{
    if(self->sndfile)
    {
        sf_close(self->sndfile);
        self->sndfile = NULL;
    }

    if(i_file_exists(a_file_out))
    {
        remove(a_file_out);
    }

    if(self->rec)
    {
        if(self->stereo_ch == -1)
        {
            self->channels = 1;
            self->sf_info.channels = 1;
        }
        else
        {
            self->channels = 2;
            self->sf_info.channels = 2;
        }

        self->sndfile = sf_open(a_file_out, SFM_WRITE, &self->sf_info);
    }
}

void v_audio_input_run(
    int f_index,
    SGFLT ** output,
    SGFLT ** sc_output,
    SGFLT * a_input,
    int sample_count,
    int * a_sc_dirty
){
    int f_i2;
    SGFLT f_tmp_sample;
    t_pyaudio_input * f_ai = &STARGATE->audio_inputs[f_index];

    int f_output_mode = f_ai->output_mode;

    if(f_output_mode){
        *a_sc_dirty = 1;
    }

    if(f_ai->rec && STARGATE->playback_mode == PLAYBACK_MODE_REC){
        int f_buffer_pos = f_index;

        if(
            (
                f_ai->buffer_iterator[(f_ai->current_buffer)]
                +
                (sample_count * f_ai->channels)
            ) >= AUDIO_INPUT_REC_BUFFER_SIZE
        ){
            f_ai->buffer_to_flush = (f_ai->current_buffer);
            f_ai->flush_last_buffer_pending = 1;

            if((f_ai->current_buffer) == 0){
                f_ai->current_buffer = 1;
            } else {
                f_ai->current_buffer = 0;
            }
        }

        int f_current_buffer = (f_ai->current_buffer);
        int f_orig_buffer_pos = f_ai->buffer_iterator[f_current_buffer];

        for(f_i2 = 0; f_i2 < sample_count; ++f_i2)
        {
            f_tmp_sample = a_input[f_buffer_pos] * (f_ai->vol_linear);

            f_ai->rec_buffers[f_current_buffer][
                f_ai->buffer_iterator[f_current_buffer]] = f_tmp_sample;
            f_ai->buffer_iterator[f_current_buffer] += f_ai->channels;

            f_buffer_pos += AUDIO_INPUT_TRACK_COUNT;
        }

        if(f_ai->stereo_ch >= 0)
        {
            f_buffer_pos = f_ai->stereo_ch;
            f_ai->buffer_iterator[f_current_buffer] = f_orig_buffer_pos + 1;

            for(f_i2 = 0; f_i2 < sample_count; ++f_i2)
            {
                f_tmp_sample = a_input[f_buffer_pos] * (f_ai->vol_linear);

                f_ai->rec_buffers[f_current_buffer][
                    f_ai->buffer_iterator[f_current_buffer]] = f_tmp_sample;
                f_ai->buffer_iterator[f_current_buffer] += f_ai->channels;

                f_buffer_pos += AUDIO_INPUT_TRACK_COUNT;
            }

            // Move it back to the correct position
            --f_ai->buffer_iterator[f_current_buffer];
        }
    }

    if(f_ai->monitor)
    {
        int f_buffer_pos = f_index;

        for(f_i2 = 0; f_i2 < sample_count; ++f_i2)
        {
            f_tmp_sample = a_input[f_buffer_pos] * (f_ai->vol_linear);

            if(f_output_mode != 1)
            {
                output[0][f_i2] += f_tmp_sample;
            }

            if(f_output_mode > 0)
            {
                sc_output[0][f_i2] += f_tmp_sample;
            }

            if(f_ai->stereo_ch == -1)
            {
                if(f_output_mode != 1)
                {
                    output[1][f_i2] += f_tmp_sample;
                }

                if(f_output_mode > 0)
                {
                    sc_output[1][f_i2] += f_tmp_sample;
                }
            }

            f_buffer_pos += AUDIO_INPUT_TRACK_COUNT;
        }

        if(f_ai->stereo_ch >= 0)
        {
            f_buffer_pos = f_ai->stereo_ch;

            for(f_i2 = 0; f_i2 < sample_count; ++f_i2)
            {
                f_tmp_sample = a_input[f_buffer_pos] * (f_ai->vol_linear);

                if(f_output_mode != 1)
                {
                    output[1][f_i2] += f_tmp_sample;
                }

                if(f_output_mode > 0)
                {
                    sc_output[1][f_i2] += f_tmp_sample;
                }

                f_buffer_pos += AUDIO_INPUT_TRACK_COUNT;
            }
        }
    }
}

void v_update_audio_inputs(char * a_project_folder){
    char f_inputs_file[2048];
    char f_tmp_file_name[2048];

    t_pyaudio_input * f_ai;
    sprintf(f_inputs_file, "%s%sinput.txt", a_project_folder, PATH_SEP);

    if(a_project_folder && i_file_exists(f_inputs_file))
    {
        int f_i;
        t_2d_char_array * f_2d_array = g_get_2d_array_from_file(
            f_inputs_file, LARGE_STRING);

        pthread_mutex_lock(&STARGATE->audio_inputs_mutex);

        for(f_i = 0; f_i < AUDIO_INPUT_TRACK_COUNT; ++f_i)
        {
            v_iterate_2d_char_array(f_2d_array);

            if(f_2d_array->eof) //!strcmp(f_index_str, "\\"))
            {
                break;
            }

            int f_index = atoi(f_2d_array->current_str);

            v_iterate_2d_char_array(f_2d_array);
            int f_rec = atoi(f_2d_array->current_str);

            v_iterate_2d_char_array(f_2d_array);
            int f_monitor = atoi(f_2d_array->current_str);

            v_iterate_2d_char_array(f_2d_array);
            int f_vol = atoi(f_2d_array->current_str);

            v_iterate_2d_char_array(f_2d_array);
            int f_out = atoi(f_2d_array->current_str);

            v_iterate_2d_char_array(f_2d_array);
            int f_right_ch = atoi(f_2d_array->current_str);

            if(f_right_ch >= AUDIO_INPUT_TRACK_COUNT)
            {
                f_right_ch = -1;
            }

            v_iterate_2d_char_array(f_2d_array);
            int f_output_mode = atoi(f_2d_array->current_str);

            // name, ignored by the engine
            v_iterate_2d_char_array_to_next_line(f_2d_array);

            if(f_index >= AUDIO_INPUT_TRACK_COUNT)
            {
                continue;
            }

            f_ai = &STARGATE->audio_inputs[f_index];
            f_ai->rec = f_rec;
            f_ai->monitor = f_monitor;
            f_ai->output_track = f_out;
            f_ai->output_mode = f_output_mode;
            f_ai->stereo_ch = f_right_ch;
            f_ai->vol = f_vol;
            f_ai->vol_linear = f_db_to_linear_fast(f_vol);

            sprintf(f_tmp_file_name, "%s%i",
                STARGATE->audio_tmp_folder, f_index);

            v_audio_input_record_set(f_ai, f_tmp_file_name);
        }

        pthread_mutex_unlock(&STARGATE->audio_inputs_mutex);
        g_free_2d_char_array(f_2d_array);
    }
    else
    {
        printf("%s not found, setting default values\n", f_inputs_file);
        pthread_mutex_lock(&STARGATE->audio_inputs_mutex);
        int f_i;
        for(f_i = 0; f_i < AUDIO_INPUT_TRACK_COUNT; ++f_i)
        {
            f_ai = &STARGATE->audio_inputs[f_i];
            f_ai->rec = 0;
            f_ai->monitor = 0;
            f_ai->output_track = 0;
            f_ai->output_track = 0;
            f_ai->stereo_ch = -1;

            f_ai->vol = 0.0f;
            f_ai->vol_linear = 1.0f;

            sprintf(f_tmp_file_name, "%s%i",
                STARGATE->audio_tmp_folder, f_i);

            v_audio_input_record_set(f_ai, f_tmp_file_name);
        }
        pthread_mutex_unlock(&STARGATE->audio_inputs_mutex);
    }
}

void * v_audio_recording_thread(void* a_arg){
    t_pyaudio_input * f_ai;
    int f_count;
    int f_i;
    int f_frames;

    sleep(3);

    while(1){
        int f_did_something = 0;

        pthread_mutex_lock(&STARGATE->audio_inputs_mutex);

        if(STARGATE->audio_recording_quit_notifier){
            pthread_mutex_unlock(&STARGATE->audio_inputs_mutex);
            printf("audio recording thread exiting...\n");
            break;
        }

        if(STARGATE->playback_mode == PLAYBACK_MODE_REC){
            for(f_i = 0; f_i < AUDIO_INPUT_TRACK_COUNT; ++f_i){
                f_ai = &STARGATE->audio_inputs[f_i];
                if((f_ai->rec) && (f_ai->flush_last_buffer_pending)){
                    f_frames = f_ai->buffer_iterator[(f_ai->buffer_to_flush)]
                        / f_ai->channels;
                    f_did_something = 1;

                    assert(f_ai->channels == f_ai->sf_info.channels);

                    printf(
                        "Flushing record buffer of "
                        "%i frames, %i channels for input %i\n",
                        f_frames,
                        f_ai->channels,
                        f_i
                    );

                    f_count = sg_write_audio(
                        f_ai->sndfile,
                        f_ai->rec_buffers[f_ai->buffer_to_flush],
                        f_frames
                    );

                    printf("sg_write_audio returned %i\n", f_count);

                    f_ai->flush_last_buffer_pending = 0;
                    f_ai->buffer_iterator[f_ai->buffer_to_flush] = 0;
                }
            }
        }

        pthread_mutex_unlock(&STARGATE->audio_inputs_mutex);

        if(!f_did_something)
        {
            usleep(10000);
        }
    }

    return (void*)1;
}

void v_stop_record_audio(){
    int f_i, f_frames, f_count;
    t_pyaudio_input * f_ai;
    char f_file_name_old[2048];
    char f_file_name_new[2048];

    pthread_mutex_lock(&STARGATE->exit_mutex);
    printf("Stopping recording, shutdown is inhibited.\n");
    pthread_mutex_lock(&STARGATE->audio_inputs_mutex);

    for(f_i = 0; f_i < AUDIO_INPUT_TRACK_COUNT; ++f_i){
        f_ai = &STARGATE->audio_inputs[f_i];
        if(f_ai->rec){
            f_frames =
                f_ai->buffer_iterator[(f_ai->current_buffer)]
                / f_ai->channels;

            if(f_frames){
                f_count = sg_write_audio(
                    f_ai->sndfile,
                    f_ai->rec_buffers[(f_ai->current_buffer)],
                    (
                        f_ai->buffer_iterator[(f_ai->current_buffer)]
                        /
                        f_ai->channels
                    )
                );

                printf("sg_write_audio returned %i\n", f_count);
            }

            sf_close(f_ai->sndfile);
            f_ai->sndfile = NULL;

            sprintf(
                f_file_name_old,
                "%s%i",
                STARGATE->audio_tmp_folder,
                f_i
            );

            sprintf(
                f_file_name_new,
                "%s%i.wav",
                STARGATE->audio_tmp_folder,
                f_i
            );

            rename(f_file_name_old, f_file_name_new);

            v_audio_input_record_set(
                &STARGATE->audio_inputs[f_i],
                f_file_name_old
            );
        }
    }

    pthread_mutex_unlock(&STARGATE->audio_inputs_mutex);
    pthread_mutex_unlock(&STARGATE->exit_mutex);
    printf("Finished stopping recording, shutdown is no longer inhibited.\n");
}

void v_prepare_to_record_audio(){
    int f_i;
    t_pyaudio_input * f_ai;

    pthread_mutex_lock(&STARGATE->audio_inputs_mutex);

    for(f_i = 0; f_i < AUDIO_INPUT_TRACK_COUNT; ++f_i){
        f_ai = &STARGATE->audio_inputs[f_i];

        f_ai->current_buffer = 0;
        f_ai->flush_last_buffer_pending = 0;
        f_ai->buffer_iterator[0] = 0;
        f_ai->buffer_iterator[1] = 0;
    }

    pthread_mutex_unlock(&STARGATE->audio_inputs_mutex);
}


