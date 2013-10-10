/************************************************************************************
** Copyright (C), 2000-2013, OPPO Mobile Comm Corp., Ltd
** All rights reserved.
** 
** VENDOR_EDIT
** 
** Description: - 
**      Notify for charging
** 
** 
** --------------------------- Revision History: --------------------------------
** <author>		                      <data> 	<version >  <desc>
** ------------------------------------------------------------------------------
** yaolang@Plf.DevSrv.Storage  2013/08/21   1.0	    create file
** ------------------------------------------------------------------------------
** 
************************************************************************************/

package com.oppo.exserviceui;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.net.Uri;
import android.os.Vibrator;
import android.util.Log;

public class OppoChargeNotifyReceiver extends BroadcastReceiver {
    private static final String TAG = "OppoChargeNotifyReceiver";
    
    private Vibrator mVibrator;
    private MediaPlayer mSound;
    private String mChargingNotifySoundPath;
    
    private boolean usedSound = false;
    private boolean usedVibrate = false;
    
    private final int audioStreamType = AudioManager.STREAM_SYSTEM;
    private final long[] DEFAULT_VIBRATE_PATTERN = {0, 250, 250, 250};
    
    @Override
    public void onReceive(final Context context, Intent intent) {
        String action = intent.getAction();
        if (action.equals(Intent.ACTION_POWER_CONNECTED)) {
            notifyCharging(context);
        } else if (action.equals(Intent.ACTION_POWER_DISCONNECTED)) {
            tearDown();
        }
    }

    private void notifyCharging(Context context) {
        final AudioManager audioManager = (AudioManager)context.getSystemService(Context.AUDIO_SERVICE);
        final int ringerMode = audioManager.getRingerMode();

        if (ringerMode == AudioManager.RINGER_MODE_NORMAL) {
            usedSound = true;
            playSound(context);
        } else if (ringerMode == AudioManager.RINGER_MODE_VIBRATE) {
            usedVibrate = true;
            vibrate(context);
        }
    }

    private void tearDown() {
        if (usedSound && mSound != null) {
            usedSound = false;
            cleanupPlayer(mSound);
        } else if (usedVibrate && mVibrator != null) {
            usedVibrate = false;
            mVibrator.cancel();
        }
    }

    private void playSound(Context context) {
        mChargingNotifySoundPath = context.getResources().getString(
                com.oppo.internal.R.string.oppo_config_charging_notify_sound);
        if (mChargingNotifySoundPath != null) {
            final Uri soundUri = Uri.parse("file://" + mChargingNotifySoundPath);
            if (soundUri != null) {
                if (mSound == null) {
                    mSound = new MediaPlayer();
                }
                if (mSound != null && !mSound.isPlaying()) {
                    try {
                        mSound.setDataSource(context, soundUri);
                        mSound.setAudioStreamType(audioStreamType);
                        mSound.prepare();
                        mSound.setOnCompletionListener(new OnCompletionListener() {
                            public void onCompletion(MediaPlayer mp) {
                                cleanupPlayer(mp);
                            }
                        });
                        mSound.setOnErrorListener(new OnErrorListener() {
                            public boolean onError(MediaPlayer mp, int what, int extra) {
                                cleanupPlayer(mp);
                                return true;
                            }
                        });
                        mSound.start();
                    } catch (Exception e) {
                        Log.w(TAG, "MediaPlayer Exception: " + e);
                        if (mSound != null) {
                            mSound.reset();
                            mSound.release();
                            mSound = null;
                        }
                    }
                } else {
                    Log.w(TAG, "playSounds: failed to new MediaPlayer");
                }
            } else {
                Log.w(TAG, "playSounds: could not parse Uri: " + mChargingNotifySoundPath);
            }
        } else {
            Log.w(TAG, "playSounds: whichSound = " + mChargingNotifySoundPath + " was null!!!");
        }
    }        

    private void vibrate(Context context) {
        if (mVibrator == null) {
            mVibrator = (Vibrator)context.getSystemService(Context.VIBRATOR_SERVICE);;
        }

        if (mVibrator != null) {
            mVibrator.vibrate(DEFAULT_VIBRATE_PATTERN, -1);
        }
    }

    private void cleanupPlayer(MediaPlayer mp) {
        if (mp != null) {
            try {
                mp.stop();
                //mp.reset();
                mp.release();
                mp = null;
                mSound = null;
            } catch (IllegalStateException e) {
                Log.w(TAG, "MediaPlayer IllegalStateException: " + e);
            }
        }
    }

}

