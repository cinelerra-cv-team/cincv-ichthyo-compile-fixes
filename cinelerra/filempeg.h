#ifndef FILEMPEG_H
#define FILEMPEG_H

#include "bitspopup.inc"
#include "file.inc"
#include "filebase.h"
#include "libmpeg3.h"
#include "mutex.h"
#include "thread.h"


#include <lame/lame.h>


extern "C"
{


// Mpeg2enc prototypes
void mpeg2enc_init_buffers();
int mpeg2enc(int argc, char *argv[]);
void mpeg2enc_set_w(int width);
void mpeg2enc_set_h(int height);
void mpeg2enc_set_rate(double rate);
void mpeg2enc_set_input_buffers(int eof, char *y, char *u, char *v);



// Toolame prototypes
void toolame_init_buffers();
int toolame(int argc, char **argv);
int toolame_send_buffer(char *data, int bytes);


}

class FileMPEGVideo;
class FileMPEGAudio;

class FileMPEG : public FileBase
{
public:
	FileMPEG(Asset *asset, File *file);
	~FileMPEG();

	friend class FileMPEGVideo;
	friend class FileMPEGAudio;

	static void get_parameters(BC_WindowBase *parent_window, 
		Asset *asset, 
		BC_WindowBase* &format_window,
		int audio_options,
		int video_options);

	static int check_sig(Asset *asset);
	int open_file(int rd, int wr);
	int close_file();
	int set_video_position(int64_t x);
	int set_audio_position(int64_t x);
	int write_samples(double **buffer, 
			int64_t len);
	int write_frames(VFrame ***frames, int len);

	int read_frame(VFrame *frame);
	int read_samples(double *buffer, int64_t len);
	int read_samples_float(float *buffer, int64_t len);
	int prefer_samples_float();

// Direct copy routines
	static int get_best_colormodel(Asset *asset, int driver);
	int colormodel_supported(int colormodel);
// This file can copy frames directly from the asset
	int can_copy_from(Edit *edit, int64_t position); 
	static char *strtocompression(char *string);
	static char *compressiontostr(char *string);

private:
	void to_streamchannel(int channel, int &stream_out, int &channel_out);
	int reset_parameters_derived();
// File descriptor for decoder
	mpeg3_t *fd;

// Thread for video encoder
	FileMPEGVideo *video_out;
// Command line for video encoder
	ArrayList<char*> vcommand_line;
	void append_vcommand_line(const char *string);

// Thread for audio encoder
	FileMPEGAudio *audio_out;
// Command line for audio encoder
	ArrayList<char*> acommand_line;
	void append_acommand_line(const char *string);
	

// Temporary for color conversion
	VFrame *temp_frame;
	
	unsigned char *toolame_temp;
	int toolame_allocation;
	int toolame_result;


	float *lame_temp[2];
	int lame_allocation;
	char *lame_output;
	int lame_output_allocation;
	FILE *lame_fd;
// Lame puts 0 before stream
	int lame_started;

	lame_global_flags *lame_global;
};


class FileMPEGVideo : public Thread
{
public:
	FileMPEGVideo(FileMPEG *file);
	~FileMPEGVideo();

	void run();

	FileMPEG *file;
};

class FileMPEGAudio : public Thread
{
public:
	FileMPEGAudio(FileMPEG *file);
	~FileMPEGAudio();

	void run();

	FileMPEG *file;
};


class MPEGConfigAudioPopup;
class MPEGABitrate;


class MPEGConfigAudio : public BC_Window
{
public:
	MPEGConfigAudio(BC_WindowBase *parent_window, Asset *asset);
	~MPEGConfigAudio();

	int create_objects();
	int close_event();

	BC_WindowBase *parent_window;
	MPEGABitrate *bitrate;
	char string[BCTEXTLEN];
	Asset *asset;
};


class MPEGLayer : public BC_PopupMenu
{
public:
	MPEGLayer(int x, int y, MPEGConfigAudio *gui);
	void create_objects();
	int handle_event();
	static int string_to_layer(char *string);
	static char* layer_to_string(int derivative);
	
	MPEGConfigAudio *gui;
};

class MPEGABitrate : public BC_PopupMenu
{
public:
	MPEGABitrate(int x, int y, MPEGConfigAudio *gui);

	void create_objects();
	void set_layer(int layer);

	int handle_event();
	static int string_to_bitrate(char *string);
	static char* bitrate_to_string(char *string, int bitrate);
	
	MPEGConfigAudio *gui;
};



class MPEGFixedBitrate;
class MPEGFixedQuant;

class MPEGConfigVideo : public BC_Window
{
public:
	MPEGConfigVideo(BC_WindowBase *parent_window, 
		Asset *asset);
	~MPEGConfigVideo();

	int create_objects();
	int close_event();

	BC_WindowBase *parent_window;
	Asset *asset;
	MPEGFixedBitrate *fixed_bitrate;
	MPEGFixedQuant *fixed_quant;
	
};

class MPEGDerivative : public BC_PopupMenu
{
public:
	MPEGDerivative(int x, int y, MPEGConfigVideo *gui);
	void create_objects();
	int handle_event();
	static int string_to_derivative(char *string);
	static char* derivative_to_string(int derivative);
	
	MPEGConfigVideo *gui;
};

class MPEGColorModel : public BC_PopupMenu
{
public:
	MPEGColorModel(int x, int y, MPEGConfigVideo *gui);
	void create_objects();
	int handle_event();
	static int string_to_cmodel(char *string);
	static char* cmodel_to_string(int cmodel);
	
	MPEGConfigVideo *gui;
};

class MPEGBitrate : public BC_TextBox
{
public:
	MPEGBitrate(int x, int y, MPEGConfigVideo *gui);
	int handle_event();
	MPEGConfigVideo *gui;
};

class MPEGQuant : public BC_TumbleTextBox
{
public:
	MPEGQuant(int x, int y, MPEGConfigVideo *gui);
	int handle_event();
	MPEGConfigVideo *gui;
};

class MPEGIFrameDistance : public BC_TumbleTextBox
{
public:
	MPEGIFrameDistance(int x, int y, MPEGConfigVideo *gui);
	int handle_event();
	MPEGConfigVideo *gui;
};

class MPEGFixedBitrate : public BC_Radial
{
public:
	MPEGFixedBitrate(int x, int y, MPEGConfigVideo *gui);
	int handle_event();
	MPEGConfigVideo *gui;
};

class MPEGFixedQuant : public BC_Radial
{
public:
	MPEGFixedQuant(int x, int y, MPEGConfigVideo *gui);
	int handle_event();
	MPEGConfigVideo *gui;
};

class MPEGSeqCodes : public BC_CheckBox
{
public:
	MPEGSeqCodes(int x, int y, MPEGConfigVideo *gui);
	int handle_event();
	MPEGConfigVideo *gui;
};



#endif
