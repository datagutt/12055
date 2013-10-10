package android.webkit;

import com.mediatek.common.featureoption.FeatureOption;

/**
 * M: HTML5 web notification
 *
 * This class is used to manage permissions for the WebView's Notification
 * JavaScript API.
 *
 * @author
 *
 * @hide
 */
public class NotificationPermissions {
    /**
     * A callback interface used by the host application to set the Notification
     * permission state for an origin.
     *
     * @hide
     */
    public interface Callback {
        public void invoke(String origin, boolean allow);

        public void invokeEvent(String eventName, int pointer);

        public void invokeNotificationID(int id, int counter);
    }

    /**
     * Gets the singleton instance of this class.
     *
     * @return the singleton {@link NotificationPermissions} instance
     *         If do not support notification, return null.
     */
    public static NotificationPermissions getInstance() {
        if (!FeatureOption.MTK_WEB_NOTIFICATION_SUPPORT) {
            return null;
        }

      return WebViewFactory.getProvider().getNotificationPermissions();
    }

    /**
     * Clears the Notification permission state for all origins.
     */
    public void clearAll() {
        // Must be a no-op for backward compatibility: see the hidden constructor for reason.
    }
}
