/*
 * $Id: thstr.h,v 1.2 2006-07-31 11:07:46 thep Exp $
 * thstr.h - Thai string manipulators
 * Created: 2001-08-03
 */

#ifndef THAI_THSTR_H
#define THAI_THSTR_H

#include "thailib.h"


/**
 * @file   thstr.h
 * @brief  Thai string manipulators
 */

/**
 * @brief  Normalize character order and remove excessive characters
 *
 * @param  dest : the destination string buffer
 * @param  src  : the string to normalize
 * @param  n    : the size of @a dest buffer
 *
 * @return  total bytes written to @a dest, excluding the terminating '\\0'.
 *
 * Corrects combining character order and remove excessive characters.
 * At most @a n characters are put in @a dest.
 */
extern int th_normalize(thchar_t dest[], thchar_t *src, int n);


#endif  /* THAI_THSTR_H */

