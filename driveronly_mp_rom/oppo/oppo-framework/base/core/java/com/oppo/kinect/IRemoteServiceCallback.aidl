/********************************************************************************
 ** Copyright (C), 2008-2013, OPPO Mobile Comm Corp., Ltd
 ** VENDOR_EDIT, All rights reserved.
 ** 
 ** File: - IRemoteServiceCallback.aidl
 ** Description: 
 **     the service used this interface call back!
 **     
 ** Version: 1.0
 ** Date: 2012-12-08
 ** Author: HuangTao@Plf.Framework.Graphics
 ** 
 ** ------------------------------- Revision History: ----------------------------
 ** <author>                         <data>       <version>   <desc>
 ** ------------------------------------------------------------------------------
 ** HuangTao@Plf.Framework.Graphics  2012-12-08   1.0         Create this moudle
 ********************************************************************************/

package com.oppo.kinect;

/**
 * Example of a callback interface used by IRemoteService to send
 * synchronous notifications back to its clients.  Note that this is a
 * one-way interface so the server does not block waiting for the client.
 */
interface IRemoteServiceCallback {
    /**
     * Called when the service has a new value for you.
     */
    void notifyResult(in int[] value);
}
