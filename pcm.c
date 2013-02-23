/*
 * This extra small demo opens thermo.wav and loops 4 times
 * sending it to the speakers. Based on the examples on
 * alsa page.
 */
#include <alsa/asoundlib.h>
#define BUFSZ 64*1024

static char *device = "default"; /* playback device */
snd_output_t *output = NULL;
unsigned char buffer[BUFSZ]; /* some random data */

void fill_buffer(char *buf)
{
	FILE* wavfp = fopen("./thermo.wav", "r");

	if (wavfp == NULL)
		abort();

	fread(buffer, BUFSZ, 1, wavfp);
	fclose(wavfp);
}

int main(void)
{
	int err;
	unsigned int i;
	snd_pcm_t *handle;
	snd_pcm_sframes_t frames;
	fill_buffer(buffer);
	if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	if ((err = snd_pcm_set_params(handle,
					SND_PCM_FORMAT_U8,
					SND_PCM_ACCESS_RW_INTERLEAVED,
					1,
					11025,
					1,
					500000)) < 0) { /* 0.5sec */
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < 4; i++) {
		frames = snd_pcm_writei(handle, buffer, sizeof(buffer));
		if (frames < 0)
			frames = snd_pcm_recover(handle, frames, 0);
		if (frames < 0) {
			printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
			break;
		}
		if (frames > 0 && frames < (long)sizeof(buffer))
			printf("Short write (expected %li, wrote %li)\n", (long)sizeof(buffer), frames);
	}
	snd_pcm_close(handle);
	return 0;
}
