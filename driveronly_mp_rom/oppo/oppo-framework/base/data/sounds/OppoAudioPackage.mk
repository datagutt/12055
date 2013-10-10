#
# Audio Package 2
# 
# Include this file in a product makefile to include these audio files
#
# This is a larger package of sounds than the 1.0 release for devices
# that have larger internal flash.
# 
#
#

local_path := oppo/oppo-framework/base/data/sounds

PRODUCT_COPY_FILES += \
        $(local_path)/alarms/Sea.ogg:system/media/audio/alarms/alarm_001.ogg \
        $(local_path)/alarms/Forest.ogg:system/media/audio/alarms/alarm_002.ogg \
        $(local_path)/alarms/On_The_Bright_Side.ogg:system/media/audio/alarms/alarm_003.ogg \
        $(local_path)/alarms/Distant.ogg:system/media/audio/alarms/alarm_004.ogg \
        $(local_path)/alarms/Comfortable.ogg:system/media/audio/alarms/alarm_005.ogg \
        $(local_path)/alarms/Alarm_Beep.ogg:system/media/audio/alarms/alarm_006.ogg \
        $(local_path)/alarms/Morning.ogg:system/media/audio/alarms/alarm_007.ogg \
        $(local_path)/alarms/Relax_And_Chill.ogg:system/media/audio/alarms/alarm_008.ogg \
        $(local_path)/effects/Effect_Tick.ogg:system/media/audio/ui/Effect_Tick.ogg \
        $(local_path)/effects/KeypressStandard.ogg:system/media/audio/ui/KeypressStandard.ogg \
        $(local_path)/effects/KeypressSpacebar.ogg:system/media/audio/ui/KeypressSpacebar.ogg \
        $(local_path)/effects/KeypressDelete.ogg:system/media/audio/ui/KeypressDelete.ogg \
        $(local_path)/effects/KeypressReturn.ogg:system/media/audio/ui/KeypressReturn.ogg \
        $(local_path)/effects/VideoRecord.ogg:system/media/audio/ui/VideoRecord.ogg \
        $(local_path)/effects/camera_click.ogg:system/media/audio/ui/camera_click.ogg \
        $(local_path)/effects/Lock.ogg:system/media/audio/ui/Lock.ogg \
        $(local_path)/effects/Unlock.ogg:system/media/audio/ui/Unlock.ogg \
        $(local_path)/effects/extended_effect_a.ogg:system/media/audio/ui/extended_effect_a.ogg \
        $(local_path)/effects/extended_effect_b.ogg:system/media/audio/ui/extended_effect_b.ogg \
        $(local_path)/effects/extended_effect_c.ogg:system/media/audio/ui/extended_effect_c.ogg \
        $(local_path)/effects/extended_effect_d.ogg:system/media/audio/ui/extended_effect_d.ogg \
        $(local_path)/effects/extended_effect_e.ogg:system/media/audio/ui/extended_effect_e.ogg \
        $(local_path)/effects/Dock.ogg:system/media/audio/ui/Dock.ogg \
        $(local_path)/effects/Undock.ogg:system/media/audio/ui/Undock.ogg \
        $(local_path)/effects/LowBattery.ogg:system/media/audio/ui/LowBattery.ogg \
        $(local_path)/effects/WheelPicker.ogg:system/media/audio/ui/WheelPicker.ogg \
        $(local_path)/effects/Usb.ogg:system/media/audio/ui/Usb.ogg \
        $(LOCAL_PATH)/effects/camera_focus.ogg:system/media/audio/ui/camera_focus.ogg \
        $(local_path)/keyboard/dew.wav:system/media/audio/ui/dew.wav \
        $(local_path)/keyboard/touch.mp3:system/media/audio/ui/touch.mp3 \
        $(local_path)/notifications/Joy.ogg:system/media/audio/notifications/notification_001.ogg \
        $(local_path)/notifications/Spar.ogg:system/media/audio/notifications/notification_002.ogg \
        $(local_path)/notifications/Pure.ogg:system/media/audio/notifications/notification_003.ogg \
        $(local_path)/notifications/Echo.ogg:system/media/audio/notifications/notification_004.ogg \
        $(local_path)/notifications/Beginning.ogg:system/media/audio/notifications/notification_005.ogg \
        $(local_path)/notifications/Acoustic_Guitar.ogg:system/media/audio/notifications/notification_006.ogg \
        $(local_path)/notifications/Mira.ogg:system/media/audio/notifications/notification_007.ogg \
        $(local_path)/notifications/Adara.ogg:system/media/audio/notifications/notification_008.ogg \
        $(local_path)/notifications/Beam.ogg:system/media/audio/notifications/notification_009.ogg \
        $(local_path)/notifications/Fresh.ogg:system/media/audio/notifications/notification_010.ogg \
        $(local_path)/notifications/Harp.ogg:system/media/audio/notifications/notification_011.ogg \
        $(local_path)/notifications/Tambourine.ogg:system/media/audio/notifications/notification_012.ogg \
        $(local_path)/notifications/Bird.ogg:system/media/audio/notifications/notification_013.ogg \
        $(local_path)/notifications/Drop.ogg:system/media/audio/notifications/notification_014.ogg \
        $(local_path)/notifications/Naughty.ogg:system/media/audio/notifications/notification_015.ogg \
        $(local_path)/other/ebook_open.mp3:system/media/audio/ui/ebook_open.mp3 \
        $(local_path)/other/ebook_open2.mp3:system/media/audio/ui/ebook_open2.mp3 \
        $(local_path)/other/ebook_page1.mp3:system/media/audio/ui/ebook_page1.mp3 \
        $(local_path)/other/ebook_page3.mp3:system/media/audio/ui/ebook_page3.mp3 \
        $(local_path)/other/lowbattery.mp3:system/media/audio/ui/lowbattery.mp3 \
        $(local_path)/other/delete3.mp3:system/media/audio/ui/delete3.mp3 \
        $(local_path)/other/delete.mp3:system/media/audio/ui/delete.mp3 \
        $(local_path)/other/error.mp3:system/media/audio/ui/error.mp3 \
        $(local_path)/other/inefficacy.mp3:system/media/audio/ui/inefficacy.mp3 \
        $(local_path)/other/machine.mp3:system/media/audio/ui/machine.mp3 \
        $(local_path)/other/uncover.mp3:system/media/audio/ui/uncover.mp3 \
        $(local_path)/other/cover.mp3:system/media/audio/ui/cover.mp3 \
        $(local_path)/other/touch2.mp3:system/media/audio/ui/touch2.mp3 \
        $(local_path)/other/touch3.mp3:system/media/audio/ui/touch3.mp3 \
        $(local_path)/other/warning3.mp3:system/media/audio/ui/warning3.mp3 \
        $(local_path)/other/warrning.mp3:system/media/audio/ui/warrning.mp3 \
        $(local_path)/other/lomo_click.mp3:system/media/audio/ui/lomo_click.mp3 \
        $(local_path)/other/sound_5s.mp3:system/media/audio/ui/sound_5s.mp3 \
        $(local_path)/other/sound_10s.mp3:system/media/audio/ui/sound_10s.mp3 \
        $(local_path)/other/charge.mp3:system/media/audio/ui/charge.mp3 \
        $(local_path)/other/numberpicker_click.ogg:system/media/audio/ui/numberpicker_click.ogg \
        $(local_path)/poweronoff/poweron.mp3:system/media/audio/ui/poweron.mp3 \
        $(local_path)/poweronoff/poweroff.mp3:system/media/audio/ui/poweroff.mp3 \
        $(local_path)/poweronoff/powerlow.mp3:system/media/audio/ui/powerlow.mp3 \
        $(local_path)/ringtones/Enjoy.ogg:system/media/audio/ringtones/ringtone_001.ogg \
        $(local_path)/ringtones/Soul.ogg:system/media/audio/ringtones/ringtone_002.ogg \
        $(local_path)/ringtones/Colorful_Life.ogg:system/media/audio/ringtones/ringtone_003.ogg \
        $(local_path)/ringtones/Dancing_On_The_Tradewinds.ogg:system/media/audio/ringtones/ringtone_004.ogg \
        $(local_path)/ringtones/Colorful_Day.ogg:system/media/audio/ringtones/ringtone_005.ogg \
        $(local_path)/ringtones/Noon_Time.ogg:system/media/audio/ringtones/ringtone_006.ogg \
        $(local_path)/ringtones/Water.ogg:system/media/audio/ringtones/ringtone_007.ogg \
        $(local_path)/ringtones/Delight.ogg:system/media/audio/ringtones/ringtone_008.ogg \
        $(local_path)/ringtones/Rattled.ogg:system/media/audio/ringtones/ringtone_009.ogg \
        $(local_path)/ringtones/Joyful.ogg:system/media/audio/ringtones/ringtone_010.ogg \
        $(local_path)/ringtones/Ringtong_1.ogg:system/media/audio/ringtones/ringtone_011.ogg \
        $(local_path)/ringtones/Ringtong_2.ogg:system/media/audio/ringtones/ringtone_012.ogg \
        $(local_path)/ringtones/Mallets.ogg:system/media/audio/ringtones/ringtone_013.ogg \
        $(local_path)/ringtones/Cordial.ogg:system/media/audio/ringtones/ringtone_014.ogg \
        $(local_path)/ringtones/Funny.ogg:system/media/audio/ringtones/ringtone_015.ogg \
        $(local_path)/ringtones/Sunrise.ogg:system/media/audio/ringtones/ringtone_016.ogg \
        $(local_path)/ringtones/Jumping_Finger.ogg:system/media/audio/ringtones/ringtone_017.ogg \
        $(local_path)/ringtones/Night.ogg:system/media/audio/ringtones/ringtone_018.ogg \
        $(local_path)/ringtones/Village.ogg:system/media/audio/ringtones/ringtone_019.ogg \
        $(local_path)/ringtones/Up_To_Something.ogg:system/media/audio/ringtones/ringtone_020.ogg \
        $(local_path)/other/dial_tone_es_0.ogg:system/media/audio/ui/dial_tone_es_0.ogg \
        $(local_path)/other/dial_tone_es_1.ogg:system/media/audio/ui/dial_tone_es_1.ogg \
        $(local_path)/other/dial_tone_es_2.ogg:system/media/audio/ui/dial_tone_es_2.ogg \
        $(local_path)/other/dial_tone_es_3.ogg:system/media/audio/ui/dial_tone_es_3.ogg \
        $(local_path)/other/dial_tone_es_4.ogg:system/media/audio/ui/dial_tone_es_4.ogg \
        $(local_path)/other/dial_tone_es_5.ogg:system/media/audio/ui/dial_tone_es_5.ogg \
        $(local_path)/other/dial_tone_es_6.ogg:system/media/audio/ui/dial_tone_es_6.ogg \
        $(local_path)/other/dial_tone_es_7.ogg:system/media/audio/ui/dial_tone_es_7.ogg \
        $(local_path)/other/dial_tone_es_8.ogg:system/media/audio/ui/dial_tone_es_8.ogg \
        $(local_path)/other/dial_tone_es_9.ogg:system/media/audio/ui/dial_tone_es_9.ogg \
        $(local_path)/other/dial_tone_es_pound.ogg:system/media/audio/ui/dial_tone_es_pound.ogg \
        $(local_path)/other/dial_tone_es_star.ogg:system/media/audio/ui/dial_tone_es_star.ogg \
        $(local_path)/other/dial_tone_guitar_0.ogg:system/media/audio/ui/dial_tone_guitar_0.ogg \
        $(local_path)/other/dial_tone_guitar_1.ogg:system/media/audio/ui/dial_tone_guitar_1.ogg \
        $(local_path)/other/dial_tone_guitar_2.ogg:system/media/audio/ui/dial_tone_guitar_2.ogg \
        $(local_path)/other/dial_tone_guitar_3.ogg:system/media/audio/ui/dial_tone_guitar_3.ogg \
        $(local_path)/other/dial_tone_guitar_4.ogg:system/media/audio/ui/dial_tone_guitar_4.ogg \
        $(local_path)/other/dial_tone_guitar_5.ogg:system/media/audio/ui/dial_tone_guitar_5.ogg \
        $(local_path)/other/dial_tone_guitar_6.ogg:system/media/audio/ui/dial_tone_guitar_6.ogg \
        $(local_path)/other/dial_tone_guitar_7.ogg:system/media/audio/ui/dial_tone_guitar_7.ogg \
        $(local_path)/other/dial_tone_guitar_8.ogg:system/media/audio/ui/dial_tone_guitar_8.ogg \
        $(local_path)/other/dial_tone_guitar_9.ogg:system/media/audio/ui/dial_tone_guitar_9.ogg \
        $(local_path)/other/dial_tone_guitar_pound.ogg:system/media/audio/ui/dial_tone_guitar_pound.ogg \
        $(local_path)/other/dial_tone_guitar_star.ogg:system/media/audio/ui/dial_tone_guitar_star.ogg \
        $(local_path)/other/dial_tone_marimbas_0.ogg:system/media/audio/ui/dial_tone_marimbas_0.ogg \
        $(local_path)/other/dial_tone_marimbas_1.ogg:system/media/audio/ui/dial_tone_marimbas_1.ogg \
        $(local_path)/other/dial_tone_marimbas_2.ogg:system/media/audio/ui/dial_tone_marimbas_2.ogg \
        $(local_path)/other/dial_tone_marimbas_3.ogg:system/media/audio/ui/dial_tone_marimbas_3.ogg \
        $(local_path)/other/dial_tone_marimbas_4.ogg:system/media/audio/ui/dial_tone_marimbas_4.ogg \
        $(local_path)/other/dial_tone_marimbas_5.ogg:system/media/audio/ui/dial_tone_marimbas_5.ogg \
        $(local_path)/other/dial_tone_marimbas_6.ogg:system/media/audio/ui/dial_tone_marimbas_6.ogg \
        $(local_path)/other/dial_tone_marimbas_7.ogg:system/media/audio/ui/dial_tone_marimbas_7.ogg \
        $(local_path)/other/dial_tone_marimbas_8.ogg:system/media/audio/ui/dial_tone_marimbas_8.ogg \
        $(local_path)/other/dial_tone_marimbas_9.ogg:system/media/audio/ui/dial_tone_marimbas_9.ogg \
        $(local_path)/other/dial_tone_marimbas_pound.ogg:system/media/audio/ui/dial_tone_marimbas_pound.ogg \
        $(local_path)/other/dial_tone_marimbas_star.ogg:system/media/audio/ui/dial_tone_marimbas_star.ogg \
        $(local_path)/other/dial_tone_orchestra_0.ogg:system/media/audio/ui/dial_tone_orchestra_0.ogg \
        $(local_path)/other/dial_tone_orchestra_1.ogg:system/media/audio/ui/dial_tone_orchestra_1.ogg \
        $(local_path)/other/dial_tone_orchestra_2.ogg:system/media/audio/ui/dial_tone_orchestra_2.ogg \
        $(local_path)/other/dial_tone_orchestra_3.ogg:system/media/audio/ui/dial_tone_orchestra_3.ogg \
        $(local_path)/other/dial_tone_orchestra_4.ogg:system/media/audio/ui/dial_tone_orchestra_4.ogg \
        $(local_path)/other/dial_tone_orchestra_5.ogg:system/media/audio/ui/dial_tone_orchestra_5.ogg \
        $(local_path)/other/dial_tone_orchestra_6.ogg:system/media/audio/ui/dial_tone_orchestra_6.ogg \
        $(local_path)/other/dial_tone_orchestra_7.ogg:system/media/audio/ui/dial_tone_orchestra_7.ogg \
        $(local_path)/other/dial_tone_orchestra_8.ogg:system/media/audio/ui/dial_tone_orchestra_8.ogg \
        $(local_path)/other/dial_tone_orchestra_9.ogg:system/media/audio/ui/dial_tone_orchestra_9.ogg \
        $(local_path)/other/dial_tone_orchestra_pound.ogg:system/media/audio/ui/dial_tone_orchestra_pound.ogg \
        $(local_path)/other/dial_tone_orchestra_star.ogg:system/media/audio/ui/dial_tone_orchestra_star.ogg \
        $(local_path)/other/dial_tone_piano_0.ogg:system/media/audio/ui/dial_tone_piano_0.ogg \
        $(local_path)/other/dial_tone_piano_1.ogg:system/media/audio/ui/dial_tone_piano_1.ogg \
        $(local_path)/other/dial_tone_piano_2.ogg:system/media/audio/ui/dial_tone_piano_2.ogg \
        $(local_path)/other/dial_tone_piano_3.ogg:system/media/audio/ui/dial_tone_piano_3.ogg \
        $(local_path)/other/dial_tone_piano_4.ogg:system/media/audio/ui/dial_tone_piano_4.ogg \
        $(local_path)/other/dial_tone_piano_5.ogg:system/media/audio/ui/dial_tone_piano_5.ogg \
        $(local_path)/other/dial_tone_piano_6.ogg:system/media/audio/ui/dial_tone_piano_6.ogg \
        $(local_path)/other/dial_tone_piano_7.ogg:system/media/audio/ui/dial_tone_piano_7.ogg \
        $(local_path)/other/dial_tone_piano_8.ogg:system/media/audio/ui/dial_tone_piano_8.ogg \
        $(local_path)/other/dial_tone_piano_9.ogg:system/media/audio/ui/dial_tone_piano_9.ogg \
        $(local_path)/other/dial_tone_piano_pound.ogg:system/media/audio/ui/dial_tone_piano_pound.ogg \
        $(local_path)/other/dial_tone_piano_star.ogg:system/media/audio/ui/dial_tone_piano_star.ogg \
        $(local_path)/effects/VolumePanelSound.ogg:system/media/audio/ui/VolumePanelSound.ogg \
        $(local_path)/other/dial_tone_guitar_del.ogg:system/media/audio/ui/dial_tone_guitar_del.ogg \
        $(local_path)/other/dial_tone_marimbas_del.ogg:system/media/audio/ui/dial_tone_marimbas_del.ogg \
        $(local_path)/other/dial_tone_piano_del.ogg:system/media/audio/ui/dial_tone_piano_del.ogg \
        $(local_path)/other/dial_tone_es_del.ogg:system/media/audio/ui/dial_tone_es_del.ogg 

    
