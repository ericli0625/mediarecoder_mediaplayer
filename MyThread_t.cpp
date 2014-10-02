#include <stdio.h>
#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/SurfaceControl.h>
#include <ui/DisplayInfo.h>
#include <media/mediaplayer.h>

#include "MyThread_t.h"

using namespace android;

status_t MyClass::setupSurface(int width, int height) {
        sp<SurfaceControl> control = mClient->createSurface(
            String8("test-surface"),
            width, height, PIXEL_FORMAT_CUSTOM, 0);

        SurfaceComposerClient::openGlobalTransaction();
        control->setLayer(0x40000000);
				control->show();
        SurfaceComposerClient::closeGlobalTransaction();

        sp<Surface> surface = control->getSurface();

        mSurfaceControl = control;
        mSurface = surface;
        return NO_ERROR;

  }

status_t MyClass::setFilePath(){

		const	char *url = "/sdcard/Movies/test1.mp4";
	//	const char *url = "/sdcard/Music/CountingStars.mp3";

		printf("%s\n",url);

		pFile = open(url,O_RDONLY);

		if (pFile==-1){
			printf("open file failed.\n");
		}else{
			printf("open file success.\n");
		}
										
		stat(url,&st);
		printf("file length in bytes: %lld\n",st.st_size);

		return NO_ERROR;
}

status_t MyClass::setMediaPlayer(){
		status_t err;

		sp<IGraphicBufferProducer> mG  = mSurface->getIGraphicBufferProducer();

   	err = mp->setVideoSurfaceTexture(mG);
		if (err != OK) {
			fprintf(stderr, "could not Video Surface Texture, err=%d\n", err);
		}

		err = mp->setDataSource(pFile,0,st.st_size);
		if(err != NO_ERROR){
			fprintf(stderr,"set Data Source failed, err=%d\n",err);
			return err;
		}


		err = mp->prepare();
		if(err < 0){
			fprintf(stderr,"prepare failed, err=%d\n",err);
			
		}
		
		err = mp->start();
		if(err != NO_ERROR){
			fprintf(stderr,"start failed, err=%d\n",err);
			return err;
		}

		if(mp->isPlaying()){
			printf("Video is Playing\n");
		}

		sleep(10);

		return NO_ERROR;
}

MyClass::MyClass(){

mClient = new SurfaceComposerClient();
mp = new MediaPlayer();

}
 
MyClass::~MyClass() {

}

