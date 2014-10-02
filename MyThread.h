#pragma once
#include <utils/threads.h>

class MyThread : public android::Thread {
				public:
						MyThread();
            					MyThread(char );
						virtual ~MyThread();
						virtual android::status_t readyToRun();

			   			virtual bool threadLoop();
						

				private:
					  class Priv;
					  Priv* d;
					char flag;
};
