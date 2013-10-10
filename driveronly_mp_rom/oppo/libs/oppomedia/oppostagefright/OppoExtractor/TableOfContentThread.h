/*****************************************************************************
*
* Filename:
* ---------
*   OppoAACExtractor.cpp
*
* Project:
* --------
*   OPPO Rom
*
* Description:
* ------------
*   Oppo AAC Parser
*
* Author: luodexiang
* -------
****************************************************************************/

#ifndef TABLE_OF_CONTENT_THREAD_H_

#define TABLE_OF_CONTENT_THREAD_H_

///#include <media/stagefright/MediaDebug.h>

#include <media/oppostagefright/MediaErrors.h>
#include <utils/threads.h>
#include <utils/KeyedVector.h>


namespace android {
	/********************************************************
	class TableOfContentThread  to build Toc table
	********************************************************/


	class TableOfContentThread{
	public:
		enum{
			TOC_TABLE_SIZE=256,
			TOC_TABLE_INTERVAL_INTIAL=32
		};
		struct TableEntry{
			Vector<off_t> TocPos;
			Vector<int64_t> TocTS;
			uint32_t size;
		};

		TableOfContentThread();
		virtual ~TableOfContentThread();
		//start thread  the firstFramePos
		// is the fist Frame Position except File Header.
		void startTOCThread(off_t firstFramePos,uint32_t TocSize = TOC_TABLE_SIZE,uint32_t TocInterval = TOC_TABLE_INTERVAL_INTIAL);
		void stopTOCThread();  
		// the most important function to used by TableofContentThread, this function must be implemented if
		//TableofContentThread is used to build seek table. ipCurPos is the current position to parser, this position will be modifed if
		// this position is not an valid frame position. pNextPos is the next frame
		//position reference to current position,frameTsUs is the time of one frame in us,
		virtual status_t getNextFramePos(off_t *pCurpos, off_t *pNextPos,int64_t * frameTsUs)=0;
		// base class must implements this function to support sending actural duration to app.
		virtual status_t  sendDurationUpdateEvent(int64_t duration)=0;
		//get frame pos according to targetTimeUs,pActualTimeUs and pActualPos will save
		//the actual time and pos found in toc, bMandatory indicates  whether to parse to
		//the targetTimeUs even toc is in beening built process and has not built up to targetTimeUs.
		status_t getFramePos(int64_t targetTimeUs, int64_t *pActualTimeUs, off_t *pActualPos, bool bMandatory = false);
		//Set TOC_TABLE_SIZE and TOC_TABLE_INTERVAL_INTIAL

	private:
		off_t mCurFilePos;
		off_t mNextFilePos;
		off_t mTocIntervalLeft;
		uint32_t mFrameNum;
		bool mRunning;
		pthread_t mThread;
		bool mStopped;
		TableEntry m_Toc;
		uint32_t mTocSize;
		uint32_t mEntryCount;
		bool mTocComplete;
		uint32_t mTocInterval;
		Mutex mLock;
		int64_t mDuration;
		int64_t mTocTimeUs;// Table Of Content Duration
		off_t mFirstFramePos;  // differs with different format
		static void *threadWrapper(void *me);
		void threadEntry();
		status_t useAFrameToTOC();
	};

}

#endif
