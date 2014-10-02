#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include "MyThread_t.h"

using namespace android;

int main(int argc , char *arvg[]){

	printf("gogogogo\n");

	sp<ProcessState> proc(ProcessState::self());
	proc->startThreadPool();


	//MyClass *xx = new MyClass();

    status_t err;

    sp<IBinder> display = SurfaceComposerClient::getBuiltInDisplay(
        ISurfaceComposer::eDisplayIdMain);

    DisplayInfo dinfo;
    err = SurfaceComposerClient::getDisplayInfo(display, &dinfo);
    if (err != NO_ERROR) {
        fprintf(stderr, "could not get display info, err=%d\n", err);
        return err;
    }
    printf("Main display is %dx%d @%.2ffps (orientation=%u)\n",
           dinfo.w, dinfo.h, dinfo.fps, dinfo.orientation);
/*
    if ((err = xx->setupSurface(dinfo.w, dinfo.h)) != NO_ERROR){
		fprintf(stderr, "could not Setup Surface, err=%d\n", err);
		return err; 
	}
*/


    sp<SurfaceComposerClient> videoClient = new SurfaceComposerClient;

    sp<SurfaceControl> videoSurface(videoClient->createSurface(String8("video"),480, 320,PIXEL_FORMAT_CUSTOM, 0));
    videoClient->openGlobalTransaction();
    videoSurface->setLayer(0x40000000);
    videoClient->closeGlobalTransaction();

/*
	if((err = xx->setFilePath()) != NO_ERROR){
		fprintf(stderr, "could not Set file, err=%d\n", err);
	    return err;
	}
*/
	const char *url = "/sdcard/Movies/test1.mp4";
	struct stat st;
    int pFile = open(url, O_RDONLY);

		if (pFile==-1){
			printf("open file failed.\n");
		}else{
			printf("open file success.\n");
		}
										
		stat(url,&st);
		printf("file length in bytes: %lld\n",st.st_size);


	sp<MediaPlayer> mp = new MediaPlayer;
	mp->setDataSource(pFile, 0,st.st_size);
	

	sp<IGraphicBufferProducer> mG  = videoSurface->getSurface()->getIGraphicBufferProducer();

   	err = mp->setVideoSurfaceTexture(mG);
		if (err != OK) {
			fprintf(stderr, "could not Video Surface Texture, err=%d\n", err);
		}


		mp->prepare();

		mp->start();
		sleep(15);
		mp.clear();

		close(pFile);
    

	return 0;
}
