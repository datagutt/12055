
#include <jni.h>
#include <string.h>
#include <assert.h>
#include <utils/Log.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define LOG_TAG "Vold"

#include <cutils/log.h>
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , "ProjectName", __VA_ARGS__)
#define DEVICE_NAME "/proc/synaptics_reset"
#define DEVICE_LCD_DISPLAY "/proc/lcd_change"
#define DEVICE_TP_DOUBLE "/proc/driver_attr_oppo_tp_double_tap_enable"
#ifdef VENDOR_EDIT
//SheTao@Prd.BasicSrv.PowerManager, 2013/08/13, Add for 13015 open button light for black touch panel.
#define DEICE_TP_COLOUR "sys/bus/platform/drivers/synaptics-platform/oppo_tp_colour_detect"
#endif /* VENDOR_EDIT */

static jint oppo_tp_double_tap_enable(JNIEnv* env, jobject obj,jint tmp) {
	
	printf("oppo_tp_double_tap_enable is called: tmp = %d\n",tmp);
	int fd;
    int ret; 
	char buf[2] = {0};
	if (tmp > 0)
		buf[0]={'1'};
	else
		buf[0]={'0'};
    fd = open(DEVICE_TP_DOUBLE,O_RDWR);//Open device ,get the handle 
	SLOGW(" oppo_tpreset_tpreset is called: \n");
	
    if(fd < 0) //open fail 
    { 
      SLOGW("open device %s error \n",DEVICE_TP_DOUBLE); 
	  return 0;
    } 
    else 
    { 
		ret = write(fd,buf,1);
		if(ret < 0)
		{
			SLOGW("write device %s error \n",DEVICE_TP_DOUBLE); 
			close(fd);
			return 0;
		}
	}
	
	close(fd);
	return 1;


}

static jint oppo_tpreset_tpreset(JNIEnv* env, jobject obj) {
	

	int fd;
    int ret; 
	char buf[2]={'a'};
    fd = open(DEVICE_NAME,O_RDWR);//Open device ,get the handle 
	SLOGW(" oppo_tpreset_tpreset is called : \n");
	
    if(fd < 0) //open fail 
    { 
      SLOGW("open device %s error \n",DEVICE_NAME); 
	  return 0;
    } 
    else 
    { 
		ret = write(fd,buf,1);
		if(ret < 0)
		{
			SLOGW("write device %s error \n",DEVICE_NAME); 
			close(fd);
			return 0;
		}
	}
	ret = read(fd,buf,1);
	if(ret<0)
		{
		SLOGW("read device %s error \n",DEVICE_NAME);
		return 0;
	}
	else
		{
		if(buf[0]=='3')
			{
				SLOGW("read TP_WHITE \n",DEVICE_NAME);
				close(fd);
				return 3;
			}
		if(buf[0]=='2')
			{
				SLOGW("read TP_BLACK \n",DEVICE_NAME);
				close(fd);
				return 2;
			}
		
	}
	
	close(fd);
	return 1;
}


static jint oppo_lcd_display_read(JNIEnv* env, jobject obj) {
	

	int fd_lcd;
    int ret; 
	char buf[2] = {0};
	printf("srd oppo_lcd_display_read is called!!!  \n");
		
    fd_lcd = open(DEVICE_LCD_DISPLAY,O_RDWR);//Open device ,get the handle
    if(fd_lcd < 0) //open fail 
    { 
   
	  return 0;
    } 
    else 
    { 
		   ret = read( fd_lcd,buf,1);
		   if(ret < 0)
		   {
	
			  close(fd_lcd);
			  return 0;
		   }
		   close(fd_lcd);
		
		   
		   if(buf[0]=='1')
		   {
		    
			  return 1;
		   }
		   else if (buf[0]=='2')
		    {
		   
			  return 2;
		   }
		   else if (buf[0]=='3')
		    {
		   
			  return 3;
		   }
		   else
		    {
		   
			  return 0;
		    }
		
	}

}

static jint oppo_lcd_display_change(JNIEnv* env, jobject obj,jint tmp) {
	

	int fd_lcd;
    int ret; 
	char buf[2] = {0};
	printf("srd oppo_lcd_change is called : tmp=%d \n",tmp);
	SLOGW("srd oppo_lcd_change is called : tmp=%d \n",tmp);
	if(tmp==1)
	{
	   buf[0]={'1'};
	}
	else if(tmp==2)
	{
	   buf[0]={'2'};
	}
	else if(tmp==3)
	{
	   buf[0]={'3'};
	}
	else 
	{
	   buf[0]={'0'};
	}
		
    fd_lcd = open(DEVICE_LCD_DISPLAY,O_RDWR);//Open device ,get the handle 
    if(fd_lcd < 0) //open fail 
    { 
      SLOGW("srd opne oppo_lcd_change fail \n");
	  return 0;
    } 
    else 
    { 
		ret = write(fd_lcd,buf,1);
			
		if(ret < 0)
		{
			SLOGW("srd write device fail %s\n",DEVICE_LCD_DISPLAY); 
			close(fd_lcd);
			return 0;
		}
	}
	SLOGW("srd lcd change is ok \n");
	close(fd_lcd);	
	return 1;
}

#ifdef VENDOR_EDIT
//SheTao@Prd.BasicSrv.PowerManager, 2013/08/13, Add for 13015 open button light for black touch panel, 
static jint oppo_tp_colour_read(JNIEnv* env, jobject obj)
{
#if 0 //for test
    return 0;
#else
	int fd = -1;
	int result = 1;
	fd = open(DEICE_TP_COLOUR,O_RDONLY);
	if (fd >=0)
	{
	    SLOGW("open device %s error \n",DEICE_TP_COLOUR); 
		char buffer[2] = {0};
		if (read(fd, buffer, 1) >= 0)
		{	
			SLOGW("wyj read buffer = %c!",buffer[0]);
			if(buffer[0] == '0')
				result = 0;
			else if (buffer[0] == '1')
				result = 1;
			else if (buffer[0] == '2')
			    result = 2;
		}
		else
		{
			SLOGW("wyj read failed!");
			result = 2;
		}
		close(fd);
	}
	return result;
#endif
}
#endif /* VENDOR_EDIT */

static JNINativeMethod gMethods[] = {
    /* name, signature, funcPtr */
    { "tp_reset",      "()I", (void*) oppo_tpreset_tpreset },
	{ "oppo_lcd_display_change",      "(I)I", (void*) oppo_lcd_display_change },
    { "oppo_lcd_display_read",      "()I", (void*)  oppo_lcd_display_read },
	{ "tp_double_tap_enable",      "(I)I", (void*)  oppo_tp_double_tap_enable },
#ifdef VENDOR_EDIT
//SheTao@Prd.BasicSrv.PowerManager, 2013/08/13, Add for 13015 open button light for black touch panel.
	{ "oppo_tp_colour_read",      "()I", (void*)  oppo_tp_colour_read },
#endif /* VENDOR_EDIT */
};


/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL)
        return JNI_FALSE;

    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0)
        return JNI_FALSE;

    return JNI_TRUE;
}


/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env, "com/oppo/tpreset/tpreset",
            gMethods, sizeof(gMethods) / sizeof(gMethods[0])))
        return JNI_FALSE;
        
    return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
        goto bail;

    assert(env != NULL);

    if (!registerNatives(env))
        goto bail;

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

bail:
    return result;
}


