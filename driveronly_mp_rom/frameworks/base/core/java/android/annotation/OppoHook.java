/*
 * Copyright 2012-2012 OPPO Mobile Comm Corp., Ltd.
 * All rights reserved.
 *
 * Filename      : OppoRomHook
 * Versin Number : 1.0.1
 * Description   : Oppo annotation
 * Auther        : danjianjun
 * Date          : 2012-12-03
 * History       : (ID,     Date,       Author,     Description)
 */
package android.annotation;

import java.lang.annotation.ElementType;
import java.lang.annotation.Target;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

import static java.lang.annotation.ElementType.CONSTRUCTOR;
import static java.lang.annotation.ElementType.METHOD;
import static java.lang.annotation.ElementType.TYPE;
import static java.lang.annotation.ElementType.FIELD;
import static java.lang.annotation.ElementType.PARAMETER;;

@Target({TYPE, FIELD, METHOD, CONSTRUCTOR, ElementType.LOCAL_VARIABLE})
@Retention(RetentionPolicy.CLASS)
public @interface OppoHook {
    public abstract OppoHookType level();
    public abstract OppoRomType property() default OppoRomType.ROM;
    public abstract String note() default "null";

    public static enum OppoHookType {
        CHANGE_ACCESS,
        CHANGE_CODE,
        CHANGE_CODE_AND_ACCESS, 
        CHANGE_RESOURCE,
        CHANGE_BASE_CLASS,
        CHANGE_PARAMETER,
        NEW_FIELD, 
        NEW_METHOD,
        NEW_CLASS;
    }
    
    public static enum OppoRomType {
        ROM,   
        OPPO,
        QCOM,
        MTK;
    }
}
