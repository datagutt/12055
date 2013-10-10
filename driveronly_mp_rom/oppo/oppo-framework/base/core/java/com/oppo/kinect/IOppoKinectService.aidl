/********************************************************************************
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** VENDOR_EDIT, All rights reserved.
 ** 
 ** File: - IOppoKinectService.aidl
 ** Description: 
 **     an interface that used by client!
 **     
 ** Version: 1.0
 ** Date: 2012-12-08
 ** Author: HuangTao@Plf.Framework.Graphics
 ** 
 ** ------------------------------- Revision History: ----------------------------
 ** <author>                          <data>       <version>   <desc>
 ** ------------------------------------------------------------------------------
 ** HuangTao@Plf.Framework.Graphics   2012-12-08   1.0         Create this moudle
 ********************************************************************************/
 
package com.oppo.kinect;

import com.oppo.kinect.IRemoteServiceCallback;

interface IOppoKinectService{

    void enable(IRemoteServiceCallback cb,int gesture); 
    void setparameter(int gesture,in float[] parameter);    
    void disable(IRemoteServiceCallback cb,int gesture);
}