/************************************************************
  Copyright (C), 2008-2010, OPPO Mobile Comm Corp., Ltd
  VENDOR_EDIT
  FileName: FileSource.h
  Author: zouf
  Create Date: 2011-10-24
  Description: 
  History: 
      <author>  <time>   <version >   <desc>
      zouf       2011-10-24     1.0     create this moudle  
***********************************************************/

#ifndef ID3_FILE_SOURCE_H_

#define ID3_FILE_SOURCE_H_

#include <stdio.h>
#include <utils/Errors.h>
#include <utils/threads.h>


namespace android {

class ID3FileSource  : public RefBase {
public:
    ID3FileSource(const char *filename);
    ID3FileSource(int fd, int64_t offset, int64_t length);

    status_t initCheck() const;

    ssize_t readAt(off64_t offset, void *data, size_t size);

    status_t getSize(off64_t *size);
	
	ssize_t write(off64_t offset, const void *data, size_t size);

	int truncate(off64_t length);
	
	const char* getFilename();
    virtual ~ID3FileSource();

private:
    FILE *mFile;
    int64_t mOffset;
    int64_t mLength;
    Mutex mLock;
	char mFilename[1024];

    ID3FileSource(const ID3FileSource &);
    ID3FileSource &operator=(const ID3FileSource &);
};

}  // namespace android

#endif  // ID3_FILE_SOURCE_H_

