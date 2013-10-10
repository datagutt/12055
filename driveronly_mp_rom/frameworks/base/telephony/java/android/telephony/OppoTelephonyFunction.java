package android.telephony;

import android.annotation.SdkConstant;
import android.annotation.SdkConstant.SdkConstantType;

import android.telephony.PhoneNumberUtils;

import android.os.RemoteException;
import android.os.ServiceManager;

import android.text.TextUtils;

import com.android.internal.telephony.ISms;

@android.annotation.OppoHook(level=android.annotation.OppoHook.OppoHookType.NEW_CLASS,
                             property=android.annotation.OppoHook.OppoRomType.ROM,
                             note="sub class of OppoTelephonyFunction")
public class OppoTelephonyFunction {
    //#lixuan@Plf.CommSrv.Telephony add for send and receive vcard
    public static final int CONCATENATED_8_BIT_REFERENCE_LENGTH = 5;
    public static final int PORT_ADDRESS_16_REFERENCE_LENGTH = 6;

    public static String oppoStripSeparators(String phoneNumber) {
        if (phoneNumber == null) {
            return null;
        }

        int len = phoneNumber.length();
        StringBuilder ret = new StringBuilder(len);

        for (int i = 0; i < len; i++) {
            char c = phoneNumber.charAt(i);
            if (PhoneNumberUtils.isNonSeparator(c)) {
                ret.append(c);
            }
        }

        return ret.toString();
    }

    /**
    * Divide a message Data into several fragments, none bigger than
    * the maximum SMS message size.
    *
    * @param data the original message. Must not be null.
    * @return an array of byte
    */
    public static byte[][] divideDataMessage(byte[] data) {
		int dataLen = data.length;
		/* Subtract the UDHL (1), and 16 Bit Port Address IE Length (6) */
		int bytePreSeg = SmsMessage.MAX_USER_DATA_BYTES - 1 - PORT_ADDRESS_16_REFERENCE_LENGTH;

		/* If it is a concatenated SMS, need to subtract the concatenated header length */
		if (dataLen > bytePreSeg) {
			bytePreSeg = bytePreSeg - CONCATENATED_8_BIT_REFERENCE_LENGTH;
		}

		int total = (dataLen + bytePreSeg - 1) / bytePreSeg;
		int remainLen = dataLen;
		int count = 0;
		byte[][] dataSegList = new byte[total][];

		while (remainLen > 0) {
			int copyLen;

			if (remainLen > bytePreSeg) {
				copyLen = bytePreSeg;
			} else {
				copyLen = remainLen;
			}

			remainLen -= copyLen;

			dataSegList[count] = new byte[copyLen];

			System.arraycopy(data, (count * bytePreSeg), dataSegList[count], 0, copyLen);

			count++;
		}

		return dataSegList;
    }
}