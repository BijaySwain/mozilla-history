/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 * Copyright (C) 2008 Sun Microsystems, Inc.,
 *                Brian Lu <brian.lu@sun.com>
 *
 * Contributor(s): 
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** *
 */
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/audio.h>
#include <sys/mixer.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include "sydney_audio.h"

#define DEFAULT_AUDIO_DEVICE "/dev/audio" 

#define LOOP_WHILE_EINTR(v,func) do { (v) = (func); } \
                while ((v) == -1 && errno == EINTR);

struct sa_stream 
{
  int        audio_fd;

  /* audio format info */
  /* default setting */
  unsigned int      default_n_channels;
  unsigned int      default_rate;
  unsigned int      default_precision;

  /* used settings */
  unsigned int      rate;
  unsigned int      n_channels;
  unsigned int      precision;
  int64_t           bytes_played;

};


/*
 * -----------------------------------------------------------------------------
 * Startup and shutdown functions
 * -----------------------------------------------------------------------------
 */

int
sa_stream_create_pcm(
  sa_stream_t      ** _s,
  const char        * client_name,
  sa_mode_t           mode,
  sa_pcm_format_t     format,
  unsigned  int       rate,
  unsigned  int       n_channels
) 
{
  sa_stream_t   * s = 0;

  /* Make sure we return a NULL stream pointer on failure. */
  if (_s == NULL) 
    return SA_ERROR_INVALID;

  *_s = NULL;

  if (mode != SA_MODE_WRONLY) 
    return SA_ERROR_NOT_SUPPORTED;

  if (format != SA_PCM_FORMAT_S16_LE) 
    return SA_ERROR_NOT_SUPPORTED;

  /*
   * Allocate the instance and required resources.
   */
  if ((s = malloc(sizeof(sa_stream_t))) == NULL) 
    return SA_ERROR_OOM;

  s->audio_fd = NULL;

  s->rate = rate;
  s->n_channels = n_channels;
  s->precision = 16;
  s->bytes_played = 0;

  *_s = s;

  return SA_SUCCESS;
}


int
sa_stream_open(sa_stream_t *s) 
{
  int fd,err;
  audio_info_t audio_info;
  char *device_name;
  
  /* according to the sun audio manual (man audio(7I))
   * use the device name set in AUDIODEV
   * environment variaible if it is set 
   */
  device_name = getenv("AUDIODEV");
  if (!device_name)
    device_name = DEFAULT_AUDIO_DEVICE;

  if (s == NULL) 
    return SA_ERROR_NO_INIT;

  if (s->audio_fd != NULL) 
    return SA_ERROR_INVALID;

  fd = open(device_name,O_WRONLY | O_NONBLOCK);
  if (fd >= 0) 
  {
     close (fd);
     fd = open (device_name, O_WRONLY);
  }

  if ( fd < 0 )
  {
    printf("Open %s failed:%s ",device_name,strerror(errno));
    return SA_ERROR_NO_DEVICE;
  }
  
  AUDIO_INITINFO(&audio_info);

  // save the default settings for resetting
  err = ioctl(fd, AUDIO_GETINFO, &audio_info); 
  if (err == -1)
  {
    perror("ioctl AUDIO_GETINFO failed");
    close(fd);
    return SA_ERROR_SYSTEM;
  }

  s->default_n_channels = audio_info.play.channels; 
  s->default_rate = audio_info.play.sample_rate; 
  s->default_precision =  audio_info.play.precision; 

  AUDIO_INITINFO(&audio_info)

  audio_info.play.sample_rate = s->rate;
  audio_info.play.channels = s->n_channels;
  audio_info.play.precision = s->precision;

  /* Signed Linear PCM encoding */
  audio_info.play.encoding = AUDIO_ENCODING_LINEAR;

  err=ioctl(fd,AUDIO_SETINFO,&audio_info);
  if (err== -1)
    return SA_ERROR_NOT_SUPPORTED;

  AUDIO_INITINFO(&audio_info)
  err=ioctl(fd,AUDIO_GETINFO,&audio_info);
  if (err== -1)
  {
    perror("ioctl AUDIO_SETINFO failed"); 
    return SA_ERROR_NOT_SUPPORTED;
  }

  s->audio_fd = fd;

  return SA_SUCCESS;
}

int
sa_stream_destroy(sa_stream_t *s) 
{
  int result = SA_SUCCESS;

  if (s == NULL) 
    return SA_SUCCESS;

  /*
   * Shut down the audio output device.
   */
  if (s->audio_fd != NULL) 
  {
    if (close(s->audio_fd) < 0) 
    {
      perror("Close sun audio fd failed");
      result = SA_ERROR_SYSTEM;
    }
  }

  free(s);

  return result;
}

/*
 * -----------------------------------------------------------------------------
 * Data read and write functions
 * -----------------------------------------------------------------------------
 */

int
sa_stream_write(sa_stream_t *s, const void *data, size_t nbytes) 
{

  int result = SA_SUCCESS;
  int total = 0;
  int bytes = 0;
  int fd;
  int i;
  audio_info_t ainfo;

  if (s == NULL || s->audio_fd == NULL) 
    return SA_ERROR_NO_INIT;

  if (nbytes == 0) 
    return SA_SUCCESS;

  fd = s->audio_fd;

  while (total < nbytes ) 
  {
    LOOP_WHILE_EINTR(bytes,(write(fd, (void *)(((unsigned char *)data)  total), nbytes-total)));

    total = bytes;
    if (total != nbytes)
      printf("SunAudio\tWrite completed short - %d vs %d. Write more data\n",total,nbytes);
  }

  s->bytes_played += nbytes;

  return result;
}



/*
 * -----------------------------------------------------------------------------
 * General query and support functions
 * -----------------------------------------------------------------------------
 */

int
sa_stream_get_position(sa_stream_t *s, sa_position_t position, int64_t *pos) 
{

  if (s == NULL || s->audio_fd == NULL) 
    return SA_ERROR_NO_INIT;

  if (position != SA_POSITION_WRITE_SOFTWARE) 
    return SA_ERROR_NOT_SUPPORTED;

  *pos = s->bytes_played;
  return SA_SUCCESS;
}

/*
 * -----------------------------------------------------------------------------
 * Extension functions
 * -----------------------------------------------------------------------------
 */

int
sa_stream_set_volume_abs(sa_stream_t *s, float vol) 
{
  unsigned int newVolume = 0;
  int err;
  audio_info_t audio_info;


  newVolume = (AUDIO_MAX_GAIN-AUDIO_MIN_GAIN)*volAUDIO_MIN_GAIN;

  /* Check if the new volume is valid or not */
  if ( newVolume < AUDIO_MIN_GAIN || newVolume > AUDIO_MAX_GAIN )
    return SA_ERROR_INVALID;

  AUDIO_INITINFO(&audio_info);
  audio_info.play.gain = newVolume;
  err=ioctl(s->audio_fd,AUDIO_SETINFO,&audio_info);    // The actual setting of the parameters
  if (err == -1)
  {
    perror("sa_stream_set_volume_abs failed") ; 
    return SA_ERROR_SYSTEM;
  }

  return SA_SUCCESS;
}

int
sa_stream_get_volume_abs(sa_stream_t *s, float *vol) 
{
  float volume;
  int err;
  audio_info_t audio_info;

  if (s == NULL || s->audio_fd == NULL) {
    return SA_ERROR_NO_INIT;
  }

  AUDIO_INITINFO(&audio_info);
  err=ioctl(s->audio_fd,AUDIO_GETINFO,&audio_info);
  if (err == -1)
  {
    perror("sa_stream_get_volume_abs failed");
    return SA_ERROR_SYSTEM;
  }

  volume =  (float)((audio_info.play.gain - AUDIO_MIN_GAIN))/(AUDIO_MAX_GAIN - AUDIO_MIN_GAIN); 

  *vol = volume;

  return SA_SUCCESS;
}

/*
 * -----------------------------------------------------------------------------
 * Unsupported functions
 * -----------------------------------------------------------------------------
 */
#define UNSUPPORTED(func)   func { return SA_ERROR_NOT_SUPPORTED; }

UNSUPPORTED(int sa_stream_pause(sa_stream_t *s)) 
UNSUPPORTED(int sa_stream_resume(sa_stream_t *s)) 
UNSUPPORTED(int sa_stream_create_opaque(sa_stream_t **s, const char *client_name, sa_mode_t mode, const char *codec))
UNSUPPORTED(int sa_stream_set_write_lower_watermark(sa_stream_t *s, size_t size))
UNSUPPORTED(int sa_stream_set_read_lower_watermark(sa_stream_t *s, size_t size))
UNSUPPORTED(int sa_stream_set_write_upper_watermark(sa_stream_t *s, size_t size))
UNSUPPORTED(int sa_stream_set_read_upper_watermark(sa_stream_t *s, size_t size))
UNSUPPORTED(int sa_stream_set_channel_map(sa_stream_t *s, const sa_channel_t map[], unsigned int n))
UNSUPPORTED(int sa_stream_set_xrun_mode(sa_stream_t *s, sa_xrun_mode_t mode))
UNSUPPORTED(int sa_stream_set_non_interleaved(sa_stream_t *s, int enable))
UNSUPPORTED(int sa_stream_set_dynamic_rate(sa_stream_t *s, int enable))
UNSUPPORTED(int sa_stream_set_driver(sa_stream_t *s, const char *driver))
UNSUPPORTED(int sa_stream_start_thread(sa_stream_t *s, sa_event_callback_t callback))
UNSUPPORTED(int sa_stream_stop_thread(sa_stream_t *s))
UNSUPPORTED(int sa_stream_change_device(sa_stream_t *s, const char *device_name))
UNSUPPORTED(int sa_stream_change_read_volume(sa_stream_t *s, const int32_t vol[], unsigned int n))
UNSUPPORTED(int sa_stream_change_write_volume(sa_stream_t *s, const int32_t vol[], unsigned int n))
UNSUPPORTED(int sa_stream_change_rate(sa_stream_t *s, unsigned int rate))
UNSUPPORTED(int sa_stream_change_meta_data(sa_stream_t *s, const char *name, const void *data, size_t size))
UNSUPPORTED(int sa_stream_change_user_data(sa_stream_t *s, const void *value))
UNSUPPORTED(int sa_stream_set_adjust_rate(sa_stream_t *s, sa_adjust_t direction))
UNSUPPORTED(int sa_stream_set_adjust_nchannels(sa_stream_t *s, sa_adjust_t direction))
UNSUPPORTED(int sa_stream_set_adjust_pcm_format(sa_stream_t *s, sa_adjust_t direction))
UNSUPPORTED(int sa_stream_set_adjust_watermarks(sa_stream_t *s, sa_adjust_t direction))
UNSUPPORTED(int sa_stream_get_mode(sa_stream_t *s, sa_mode_t *access_mode))
UNSUPPORTED(int sa_stream_get_codec(sa_stream_t *s, char *codec, size_t *size))
UNSUPPORTED(int sa_stream_get_pcm_format(sa_stream_t *s, sa_pcm_format_t *format))
UNSUPPORTED(int sa_stream_get_rate(sa_stream_t *s, unsigned int *rate))
UNSUPPORTED(int sa_stream_get_nchannels(sa_stream_t *s, int *nchannels))
UNSUPPORTED(int sa_stream_get_user_data(sa_stream_t *s, void **value))
UNSUPPORTED(int sa_stream_get_write_lower_watermark(sa_stream_t *s, size_t *size))
UNSUPPORTED(int sa_stream_get_read_lower_watermark(sa_stream_t *s, size_t *size))
UNSUPPORTED(int sa_stream_get_write_upper_watermark(sa_stream_t *s, size_t *size))
UNSUPPORTED(int sa_stream_get_read_upper_watermark(sa_stream_t *s, size_t *size))
UNSUPPORTED(int sa_stream_get_channel_map(sa_stream_t *s, sa_channel_t map[], unsigned int *n))
UNSUPPORTED(int sa_stream_get_xrun_mode(sa_stream_t *s, sa_xrun_mode_t *mode))
UNSUPPORTED(int sa_stream_get_non_interleaved(sa_stream_t *s, int *enabled))
UNSUPPORTED(int sa_stream_get_dynamic_rate(sa_stream_t *s, int *enabled))
UNSUPPORTED(int sa_stream_get_driver(sa_stream_t *s, char *driver_name, size_t *size))
UNSUPPORTED(int sa_stream_get_device(sa_stream_t *s, char *device_name, size_t *size))
UNSUPPORTED(int sa_stream_get_read_volume(sa_stream_t *s, int32_t vol[], unsigned int *n))
UNSUPPORTED(int sa_stream_get_write_volume(sa_stream_t *s, int32_t vol[], unsigned int *n))
UNSUPPORTED(int sa_stream_get_meta_data(sa_stream_t *s, const char *name, void*data, size_t *size))
UNSUPPORTED(int sa_stream_get_adjust_rate(sa_stream_t *s, sa_adjust_t *direction))
UNSUPPORTED(int sa_stream_get_adjust_nchannels(sa_stream_t *s, sa_adjust_t *direction))
UNSUPPORTED(int sa_stream_get_adjust_pcm_format(sa_stream_t *s, sa_adjust_t *direction))
UNSUPPORTED(int sa_stream_get_adjust_watermarks(sa_stream_t *s, sa_adjust_t *direction))
UNSUPPORTED(int sa_stream_get_state(sa_stream_t *s, sa_state_t *state))
UNSUPPORTED(int sa_stream_get_event_error(sa_stream_t *s, sa_error_t *error))
UNSUPPORTED(int sa_stream_get_event_notify(sa_stream_t *s, sa_notify_t *notify))
UNSUPPORTED(int sa_stream_read(sa_stream_t *s, void *data, size_t nbytes))
UNSUPPORTED(int sa_stream_read_ni(sa_stream_t *s, unsigned int channel, void *data, size_t nbytes))
UNSUPPORTED(int sa_stream_write_ni(sa_stream_t *s, unsigned int channel, const void *data, size_t nbytes))
UNSUPPORTED(int sa_stream_pwrite(sa_stream_t *s, const void *data, size_t nbytes, int64_t offset, sa_seek_t whence))
UNSUPPORTED(int sa_stream_pwrite_ni(sa_stream_t *s, unsigned int channel, const void *data, size_t nbytes, int64_t offset, sa_seek_t whence))
UNSUPPORTED(int sa_stream_get_read_size(sa_stream_t *s, size_t *size))
UNSUPPORTED(int sa_stream_drain(sa_stream_t *s))

const char *sa_strerror(int code) { return NULL; }
