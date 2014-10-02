#include <stdio.h>
#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/SurfaceControl.h>
#include <ui/DisplayInfo.h>
#include <media/mediaplayer.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <media/mediarecorder.h>
#include <camera/Camera.h>
#include <camera/ICamera.h>
#include <camera/CameraParameters.h>
#include <time.h>
#include <string.h>
#include "MyThread.h"

using namespace android;

struct MyThread::Priv {
    sp<SurfaceComposerClient> mClient;
    sp<SurfaceControl> mSurfaceControl;
    sp<Surface> mSurface;
    sp<MediaPlayer> mp;
	sp<Camera> mCamera;
	sp<MediaRecorder> mMediaRecorder;
    int pFile;
    struct stat st;

	CameraParameters mParams;

	
	
	Priv()
	: mClient(new SurfaceComposerClient()),mp(new MediaPlayer()),mMediaRecorder(new MediaRecorder()) {
    }

    status_t setupSurface(int width, int height) {
        sp<SurfaceControl> control = mClient->createSurface(
            String8("test-surface"),
            width, height, PIXEL_FORMAT_CUSTOM, 0);

        SurfaceComposerClient::openGlobalTransaction();
        control->setLayer(0x40000000);
		//control->show();
        SurfaceComposerClient::closeGlobalTransaction();

        sp<Surface> surface = control->getSurface();

        mSurfaceControl = control;
        mSurface = surface;
        return NO_ERROR;

    }

	status_t setFilePath(char flag){

	char *url = "/sdcard/Movies/test.mp4";
	char *path = "/sdcard/Movies/test1.mp4";

		if(flag == 'm'){
			printf("%s\n",url);
			pFile = open(url,O_RDONLY);
			stat(url,&st);
		}else{
/*
			char *filetype = ".mp4";
			char *str;
			time_t rawtime;
			time(&rawtime);
			struct tm *timeinfo = loCameraParameters(caltime (&rawtime);

			sprintf(str,"%d%d%d%d%d%d",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
			strcat (str,filetype);
*/

			printf("%s\n",path);
			pFile = open(path,O_CREAT|O_WRONLY|O_TRUNC,0777);
			//pFile = open(path,O_APPEND,0777);
			stat(path,&st);
		}

		if (pFile==-1){
			printf("open file failed.\n");
		}else{
			printf("open file success.\n");
		}
										
		if(flag == 'm')
		printf("file length in bytes: %lld\n",st.st_size);

		return NO_ERROR;
	}

	status_t setMediaPlayer(){
		status_t err;

		sp<IGraphicBufferProducer> mG  = mSurface->getIGraphicBufferProducer();

		/* It need to set before setVideoSurfaceTexture.*/
		err = mp->setDataSource(pFile,0,st.st_size);
		if(err != NO_ERROR){
			fprintf(stderr,"set Data Source failed, err=%d\n",err);
			return err;
		}

  	 	err = mp->setVideoSurfaceTexture(mG);
		if (err != OK) {
			fprintf(stderr, "could not Video Surface Texture, err=%d\n", err);
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

		return NO_ERROR;
	}

	status_t setupCamera(int width, int height, int fps) {
		status_t err;

        // TODO: where can we find the camera !id for front camera?
        sp<Camera> camera = Camera::connect(
            0, String16("test-camera"), Camera::USE_CALLING_UID);
        if (camera == NULL) {
            fprintf(stderr, "could not connect to camera\n");
            return -EBUSY;
        }
        camera->lock();

        // set preview mode to protrait
        const int rotation = 90;
        err = camera->sendCommand(CAMERA_CMD_SET_DISPLAY_ORIENTATION, rotation, 0);
        if (err != OK) {
            fprintf(stderr, "could not set display orientation, err=%d\n", err);
            return err;
        }

        // configure camera parameters
        CameraParameters params(camera->getParameters());
        params.setPreviewSize(width, height);
        params.setPreviewFrameRate(fps);
        

		// dump camera information
        params = CameraParameters(camera->getParameters());
        printf("-----\n");
        printf("  supported preview sizes: %s\n",
               params.get(CameraParameters::KEY_SUPPORTED_PREVIEW_SIZES));
        printf("  supported video sizes: %s\n",
               params.get(CameraParameters::KEY_SUPPORTED_VIDEO_SIZES));
        printf("-----\n");
        printf("  preview format: %s\n", params.get(CameraParameters::KEY_PREVIEW_FORMAT));
        printf("  preview size: %s\n", params.get(CameraParameters::KEY_PREVIEW_SIZE));
        printf("  preview frame rate: %s\n", params.get(CameraParameters::KEY_PREVIEW_FRAME_RATE));
		
        printf("-----\n");
        printf("  record format: %s\n", params.get(CameraParameters::KEY_VIDEO_FRAME_FORMAT));
       // printf("  record video high frame rate: %s\n", params.get(CameraParameters::KEY_VIDEO_HIGH_FRAME_RATE));
        printf("  record size: %s\n", params.get(CameraParameters::KEY_VIDEO_SIZE));
		printf("-----\n");

		mParams = params;

  		err = camera->setPreviewTarget(mSurface->getIGraphicBufferProducer());
   		if (err != OK) {
        	fprintf(stderr, "could not set preview target, err=%d\n", err);
        	return err;
   		}

    	err = camera->startPreview();
    	if (err != OK) {
        	fprintf(stderr, "could not start preview, err=%d\n", err);
        	return err;
    	}

		mCamera = camera;

        return NO_ERROR;
    }

	status_t recording(int width, int height, int fps){

		mCamera->unlock();

		status_t err;

		printf("setCamera in\n");
		mMediaRecorder->setCamera(mCamera->remote(),mCamera->getRecordingProxy());
		printf("setCamera out\n");

		if((err = mMediaRecorder->initCheck())!=NO_ERROR){
			fprintf(stderr, "could not init(), err=%d\n", err);
		}	

		err=mMediaRecorder->setAudioSource(AUDIO_SOURCE_DEFAULT);
		if (err != OK) {
            fprintf(stderr, "could not set--Audio--Source , err=%d\n", err);
            return err;
        }

		err=mMediaRecorder->setVideoSource(VIDEO_SOURCE_CAMERA);
		if (err != OK) {
            fprintf(stderr, "could not set--Video--Source , err=%d\n", err);
            return err;
        }

	//--------setOutputFormat(This function needs to set after audio/video source) -------//
		err=mMediaRecorder->setOutputFormat(OUTPUT_FORMAT_MPEG_4);
		if (err != OK) {
            fprintf(stderr, "could not setOutputFormat, err=%d\n", err);
            return err;
        }

		err=mMediaRecorder->setVideoFrameRate(fps);
		if (err != OK) {
            fprintf(stderr, "could not setVideoFrameRate, err=%d\n", err);
            return err;
        }

		err=mMediaRecorder->setVideoSize(width,height);
		if (err != OK) {
            fprintf(stderr, "could not setVideoSize, err=%d\n", err);
            return err;
        }

		err=mMediaRecorder->setAudioEncoder(AUDIO_ENCODER_HE_AAC);

		if (err != OK) {
            fprintf(stderr, "could not set--Audio--Encoder , err=%d\n", err);
            return err;
        }

		err=mMediaRecorder->setVideoEncoder(VIDEO_ENCODER_H264);
		if (err != OK) {
            fprintf(stderr, "could not set--Video--Encoder , err=%d\n", err);
            return err;
		}

		const char *video_str = "video-param-encoding-bitrate=20000000";
		const char *audio_str = "audio-param-encoding-bitrate=156000";
		const char *audio_str_1 = "audio-param-number-of-channels=2";
		const char *audio_str_2 = "audio-param-sampling-rate=48000";

		err =  mMediaRecorder->setOutputFile(pFile,0,50000000);
		if(err != NO_ERROR){
			fprintf(stderr,"setOutputFile, err=%d\n",err);
			return err;
		}


		err = mMediaRecorder->setParameters(String8(video_str));
		if(err != NO_ERROR){
			fprintf(stderr,"setParameters, err=%d\n",err);
			return err;
		}

		err = mMediaRecorder->setParameters(String8(audio_str));
		if(err != NO_ERROR){
			fprintf(stderr,"setParameters, err=%d\n",err);
			return err;
		}

		err = mMediaRecorder->setParameters(String8(audio_str_1));
		if(err != NO_ERROR){
			fprintf(stderr,"setParameters, err=%d\n",err);
			return err;
		}

		err = mMediaRecorder->setParameters(String8(audio_str_2));
		if(err != NO_ERROR){
			fprintf(stderr,"setParameters, err=%d\n",err);
			return err;
		}

		if((err=mMediaRecorder->setPreviewSurface(mSurface->getIGraphicBufferProducer())!=NO_ERROR)){
			fprintf(stderr, "could not setPreviewSurface err=%d\n", err);
			return err;
		}

		err=mMediaRecorder->prepare();

		if (err != OK) {
            fprintf(stderr, "could not prepare, err=%d\n", err);
            return err;
        }

		printf("recording start in\n");
		err=mMediaRecorder->start();  
		if (err != OK) {
            fprintf(stderr, "could not start, err=%d\n", err);
            return err;
        }
		printf("recording start out\n");


		return NO_ERROR;
	}

};

MyThread::MyThread()
        : Thread(false),d(new Priv()) {
}

MyThread::MyThread(char str):Thread(false),d(new Priv()),flag(str){

}

MyThread::~MyThread() {
    delete d;
}

status_t MyThread::readyToRun() {
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

    	if ((err = d->setupSurface(dinfo.w, dinfo.h)) != NO_ERROR){
		fprintf(stderr, "could not setup Surface, err=%d\n", err);
		return err; 
	}

	if((err = d->setFilePath(flag)) != NO_ERROR){
		fprintf(stderr, "could not set file, err=%d\n", err);
		return err;
	}

	if(flag=='m'){
		if((err = d->setMediaPlayer()) != NO_ERROR){
			fprintf(stderr, "could not set MediaPlayer, err=%d\n", err);
		    return err;
		}
	}

    // swap width and height for camera
	if(flag=='c'){
		if((d->setupCamera(dinfo.h, dinfo.w, dinfo.fps)) != OK){
			fprintf(stderr, "could not set Camera, err=%d\n", err);
       		return err;
		}

		if((d->recording(dinfo.h, dinfo.w, dinfo.fps))!=NO_ERROR){
			fprintf(stderr,"could not set MediaRecorder, err=%d\n",err);
			return err;
		}

	}
	

    return OK;
}

bool MyThread::threadLoop() {
    // TODO: acquire wakelock
	
	sleep(10);

		printf("recording stop in\n");
		d->mMediaRecorder->stop();
		d->mMediaRecorder->release();
		printf("recording stop out\n");

    return false;
}
