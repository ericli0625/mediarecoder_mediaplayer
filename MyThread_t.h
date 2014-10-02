#include <stdio.h>
#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/SurfaceControl.h>
#include <ui/DisplayInfo.h>
#include <media/mediaplayer.h>
#include <fcntl.h>
#include <sys/stat.h>

using namespace android;

class MyClass {
	public:
		MyClass();
	virtual ~MyClass();

	status_t setupSurface(int width, int height);
	status_t setFilePath();
	status_t setMediaPlayer();

    sp<SurfaceComposerClient> mClient;
    sp<SurfaceControl> mSurfaceControl;
    sp<Surface> mSurface;
    sp<MediaPlayer> mp;
    int pFile;
    struct stat st;


};
