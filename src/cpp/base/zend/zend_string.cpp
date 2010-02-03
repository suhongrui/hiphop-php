/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
   | Copyright (c) 1998-2010 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
*/

#include <cpp/base/zend/zend_string.h>
#include <cpp/base/zend/zend_printf.h>
#include <cpp/base/zend/zend_math.h>
#include <cpp/base/zend/utf8_to_utf16.h>

#include <util/lock.h>
#include <math.h>
#include <monetary.h>

#include <cpp/base/util/exceptions.h>
#include <cpp/base/type_array.h>
#include <cpp/base/util/string_buffer.h>

#define PHP_QPRINT_MAXL 75

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////
// helpers

/**
 * Calculates and adjusts "start" and "length" according to string's length.
 * This function determines how those two parameters are interpreted in varies
 * substr-related functions.
 */
bool string_substr_check(int len, int &f, int &l) {
  // if "from" position is negative, count start position from the end
  if (f < 0) {
    f += len;
    if (f < 0) {
      return false;
    }
  }
  if (f >= len) {
    return false;
  }

  // if "length" position is negative, set it to the length
  // needed to stop that many chars from the end of the string
  if (l < 0) {
    l += len - f;
    if (l < 0) {
      return false;
    }
  }
  if ((unsigned int)f + (unsigned int)l > (unsigned int)len) {
    l = len - f;
  }
  return true;
}

/**
 * Fills a 256-byte bytemask with input. You can specify a range like 'a..z',
 * it needs to be incrementing. This function determines how "charlist"
 * parameters are interpreted in varies functions that take a list of
 * characters.
 */
void string_charmask(const char *sinput, int len, char *mask) {
  const unsigned char *input = (unsigned char *)sinput;
  const unsigned char *end;
  unsigned char c;

  memset(mask, 0, 256);
  for (end = input+len; input < end; input++) {
    c=*input;
    if ((input+3 < end) && input[1] == '.' && input[2] == '.'
        && input[3] >= c) {
      memset(mask+c, 1, input[3] - c + 1);
      input+=3;
    } else if ((input+1 < end) && input[0] == '.' && input[1] == '.') {
      /* Error, try to be as helpful as possible:
         (a range ending/starting with '.' won't be captured here) */
      if (end-len >= input) { /* there was no 'left' char */
        throw InvalidArgumentException
          ("charlist", "Invalid '..'-range, missing left of '..'");
      }
      if (input+2 >= end) { /* there is no 'right' char */
        throw InvalidArgumentException
          ("charlist", "Invalid '..'-range, missing right of '..'");
      }
      if (input[-1] > input[2]) { /* wrong order */
        throw InvalidArgumentException
          ("charlist", "'..'-range needs to be incrementing");
      }
      /* FIXME: better error (a..b..c is the only left possibility?) */
      throw InvalidArgumentException("charlist", "Invalid '..'-range");
    } else {
      mask[c]=1;
    }
  }
}

int string_copy(char *dst, const char *src, int siz) {
  register char *d = dst;
  register const char *s = src;
  register size_t n = siz;

  /* Copy as many bytes as will fit */
  if (n != 0 && --n != 0) {
    do {
      if ((*d++ = *s++) == 0)
        break;
    } while (--n != 0);
  }

  /* Not enough room in dst, add NUL and traverse rest of src */
  if (n == 0) {
    if (siz != 0)
      *d = '\0';    /* NUL-terminate dst */
    while (*s++)
      ;
  }

  return(s - src - 1);  /* count does not include NUL */
}

///////////////////////////////////////////////////////////////////////////////

char *string_concat(const char *s1, int len1, const char *s2, int len2,
                    int &len) {
  len = len1 + len2;
  char *buf = (char *)malloc(len + 1);
  memcpy(buf, s1, len1);
  memcpy(buf + len1, s2, len2);
  buf[len] = 0;
  return buf;
}

///////////////////////////////////////////////////////////////////////////////
// comparisons

int string_cmp(const char *s1, int len1, const char *s2, int len2) {
  if (len1 <= len2) {
    for (int i = 0; i < len1; i++) {
      char c1 = s1[i];
      char c2 = s2[i];
      if (c1 > c2) return 1;
      if (c1 < c2) return -1;
    }
    return len1 < len2 ? -1 : 0;
  }

  for (int i = 0; i < len2; i++) {
    char c1 = s1[i];
    char c2 = s2[i];
    if (c1 > c2) return 1;
    if (c1 < c2) return -1;
  }
  return 1;
}

int string_casecmp(const char *s1, int len1, const char *s2, int len2) {
  if (len1 <= len2) {
    for (int i = 0; i < len1; i++) {
      char c1 = toupper(s1[i]);
      char c2 = toupper(s2[i]);
      if (c1 > c2) return 1;
      if (c1 < c2) return -1;
    }
    return len1 < len2 ? -1 : 0;
  }

  for (int i = 0; i < len2; i++) {
    char c1 = toupper(s1[i]);
    char c2 = toupper(s2[i]);
    if (c1 > c2) return 1;
    if (c1 < c2) return -1;
  }
  return 1;
}

int string_ncmp(const char *s1, const char *s2, int len) {
  for (int i = 0; i < len; i++) {
    char c1 = s1[i];
    char c2 = s2[i];
    if (c1 > c2) return 1;
    if (c1 < c2) return -1;
  }
  return 0;
}

int string_ncasecmp(const char *s1, const char *s2, int len) {
  for (int i = 0; i < len; i++) {
    char c1 = toupper(s1[i]);
    char c2 = toupper(s2[i]);
    if (c1 > c2) return 1;
    if (c1 < c2) return -1;
  }
  return 0;
}

static int compare_right(char const **a, char const *aend,
                         char const **b, char const *bend) {
  int bias = 0;

  /* The longest run of digits wins.  That aside, the greatest
     value wins, but we can't know that it will until we've scanned
     both numbers to know that they have the same magnitude, so we
     remember it in BIAS. */
  for(;; (*a)++, (*b)++) {
    if ((*a == aend || !isdigit((int)(unsigned char)**a)) &&
        (*b == bend || !isdigit((int)(unsigned char)**b)))
      return bias;
    else if (*a == aend || !isdigit((int)(unsigned char)**a))
      return -1;
    else if (*b == bend || !isdigit((int)(unsigned char)**b))
      return +1;
    else if (**a < **b) {
      if (!bias)
        bias = -1;
    } else if (**a > **b) {
      if (!bias)
        bias = +1;
    }
  }

  return 0;
}

static int compare_left(char const **a, char const *aend,
                        char const **b, char const *bend) {
  /* Compare two left-aligned numbers: the first to have a
     different value wins. */
  for(;; (*a)++, (*b)++) {
    if ((*a == aend || !isdigit((int)(unsigned char)**a)) &&
        (*b == bend || !isdigit((int)(unsigned char)**b)))
      return 0;
    else if (*a == aend || !isdigit((int)(unsigned char)**a))
      return -1;
    else if (*b == bend || !isdigit((int)(unsigned char)**b))
      return +1;
    else if (**a < **b)
      return -1;
    else if (**a > **b)
      return +1;
  }

  return 0;
}

int string_natural_cmp(char const *a, size_t a_len,
                       char const *b, size_t b_len, int fold_case) {
  char ca, cb;
  char const *ap, *bp;
  char const *aend = a + a_len, *bend = b + b_len;
  int fractional, result;

  if (a_len == 0 || b_len == 0)
    return a_len - b_len;

  ap = a;
  bp = b;
  while (1) {
    ca = *ap; cb = *bp;

    /* skip over leading spaces or zeros */
    while (isspace((int)(unsigned char)ca))
      ca = *++ap;

    while (isspace((int)(unsigned char)cb))
      cb = *++bp;

    /* process run of digits */
    if (isdigit((int)(unsigned char)ca)  &&  isdigit((int)(unsigned char)cb)) {
      fractional = (ca == '0' || cb == '0');

      if (fractional)
        result = compare_left(&ap, aend, &bp, bend);
      else
        result = compare_right(&ap, aend, &bp, bend);

      if (result != 0)
        return result;
      else if (ap == aend && bp == bend)
        /* End of the strings. Let caller sort them out. */
        return 0;
      else {
        /* Keep on comparing from the current point. */
        ca = *ap; cb = *bp;
      }
    }

    if (fold_case) {
      ca = toupper((int)(unsigned char)ca);
      cb = toupper((int)(unsigned char)cb);
    }

    if (ca < cb)
      return -1;
    else if (ca > cb)
      return +1;

    ++ap; ++bp;
    if (ap >= aend && bp >= bend)
      /* The strings compare the same.  Perhaps the caller
         will want to call strcmp to break the tie. */
      return 0;
    else if (ap >= aend)
      return -1;
    else if (bp >= bend)
      return 1;
  }
}

///////////////////////////////////////////////////////////////////////////////

char *string_to_lower(const char *s, int len) {
  ASSERT(s);
  char *ret = (char *)malloc(len + 1);
  for (int i = 0; i < len; i++) {
    ret[i] = tolower(s[i]);
  }
  ret[len] = '\0';
  return ret;
}

char *string_to_upper(const char *s, int len) {
  ASSERT(s);
  char *ret = (char *)malloc(len + 1);
  for (int i = 0; i < len; i++) {
    ret[i] = toupper(s[i]);
  }
  ret[len] = '\0';
  return ret;
}

char *string_to_upper_first(const char *s, int len) {
  ASSERT(s);
  char *ret = string_duplicate(s, len);
  if (*ret) {
    *ret = toupper(*ret);
  }
  return ret;
}

char *string_to_upper_words(const char *s, int len) {
  ASSERT(s);
  char *ret = string_duplicate(s, len);
  if (*ret) {
    *ret = toupper(*ret);
    for (int i = 1; i < len; i++) {
      if (isspace(ret[i-1])) {
        ret[i] = toupper(ret[i]);
      }
    }
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////////

char *string_trim(const char *s, int &len,
                  const char *charlist, int charlistlen, int mode) {
  ASSERT(s);
  char mask[256];
  string_charmask(charlist, charlistlen, mask);

  int trimmed = 0;
  if (mode & 1) {
    for (int i = 0; i < len; i++) {
      if (mask[(unsigned char)s[i]]) {
        trimmed++;
      } else {
        break;
      }
    }
    len -= trimmed;
    s += trimmed;
  }
  if (mode & 2) {
    for (int i = len - 1; i >= 0; i--) {
      if (mask[(unsigned char)s[i]]) {
        len--;
      } else {
        break;
      }
    }
  }
  return string_duplicate(s, len);
}

#define STR_PAD_LEFT            0
#define STR_PAD_RIGHT           1
#define STR_PAD_BOTH            2

char *string_pad(const char *input, int &len, int pad_length,
                 const char *pad_string, int pad_str_len,
                 int pad_type) {
  ASSERT(input);
  int num_pad_chars = pad_length - len;

  /* If resulting string turns out to be shorter than input string,
     we simply copy the input and return. */
  if (pad_length < 0 || num_pad_chars < 0) {
    return string_duplicate(input, len);
  }

  /* Setup the padding string values if specified. */
  if (pad_str_len == 0) {
    throw InvalidArgumentException("pad_string", "(empty)");
  }

  char *result = (char *)malloc(pad_length + 1);

  /* We need to figure out the left/right padding lengths. */
  int left_pad, right_pad;
  switch (pad_type) {
  case STR_PAD_RIGHT:
    left_pad = 0;
    right_pad = num_pad_chars;
    break;
  case STR_PAD_LEFT:
    left_pad = num_pad_chars;
    right_pad = 0;
    break;
  case STR_PAD_BOTH:
    left_pad = num_pad_chars / 2;
    right_pad = num_pad_chars - left_pad;
    break;
  default:
    throw InvalidArgumentException("pad_type", pad_type);
  }

  /* First we pad on the left. */
  int result_len = 0;
  for (int i = 0; i < left_pad; i++) {
    result[result_len++] = pad_string[i % pad_str_len];
  }

  /* Then we copy the input string. */
  memcpy(result + result_len, input, len);
  result_len += len;

  /* Finally, we pad on the right. */
  for (int i = 0; i < right_pad; i++) {
    result[result_len++] = pad_string[i % pad_str_len];
  }
  result[result_len] = '\0';

  len = result_len;
  return result;
}

///////////////////////////////////////////////////////////////////////////////

char *string_substr(const char *s, int &len, int start, int length) {
  ASSERT(s);
  if (!string_substr_check(len, start, length)) {
    len = 0;
    return string_duplicate("", 0);
  } else {
    len = length;
    return string_duplicate(s + start, length);
  }
}

int string_find(const char *input, int len, char ch, int pos,
                bool case_sensitive) {
  ASSERT(input);
  if (len && pos < len) {
    if (!case_sensitive) {
      ch = tolower(ch);
      char *lowered = string_to_lower(input, len);
      int ret = string_find(lowered, len, ch, pos, true);
      free(lowered);
      return ret;
    }

    int l = 1;
    if (!string_substr_check(len, pos, l)) {
      return -1;
    }
    for (int i = pos; i < len; i++) {
      if (input[i] == ch) {
        return i;
      }
    }
  }
  return -1;
}

int string_rfind(const char *input, int len, char ch, int pos,
                 bool case_sensitive) {
  ASSERT(input);

  if (pos < 0) pos = len - 1;
  if (len > 0 && pos < len) {
    if (!case_sensitive) {
      ch = tolower(ch);
      char *lowered = string_to_lower(input, len);
      int ret = string_rfind(lowered, len, ch, pos, true);
      free(lowered);
      return ret;
    }

    int l = 1;
    if (!string_substr_check(len, pos, l)) {
      return -1;
    }
    for (int i = pos; i >= 0; i--) {
      if (input[i] == ch) {
        return i;
      }
    }
  }
  return -1;
}

int string_find(const char *input, int len, const char *s, int s_len,
                int pos, bool case_sensitive) {
  ASSERT(input);
  ASSERT(s);
  if (!s_len) {
    return -1;
  }
  if (len && pos < len) {
    if (!case_sensitive) {
      char *lowered_s = string_to_lower(s, s_len);
      char *lowered = string_to_lower(input, len);
      int ret = string_find(lowered, len, lowered_s, s_len, pos, true);
      free(lowered);
      free(lowered_s);
      return ret;
    }

    int l = 1;
    if (!string_substr_check(len, pos, l)) {
      return -1;
    }
    int i_max = len - s_len + 1;
    for (int i = pos; i < i_max; i++) {
      if (input[i] == s[0] && memcmp(input+i, s, s_len) == 0) {
        return i;
      }
    }
  }
  return -1;
}

int string_rfind(const char *input, int len, const char *s, int s_len,
                 int pos, bool case_sensitive) {
  ASSERT(input);
  ASSERT(s);
  if (!s_len) {
    return -1;
  }

  if (pos < 0) pos = len;
  if (len && pos < len) {
    if (!case_sensitive) {
      char *lowered_s = string_to_lower(s, s_len);
      char *lowered = string_to_lower(input, len);
      int ret = string_find(lowered, len, lowered_s, s_len, pos, true);
      free(lowered);
      free(lowered_s);
      return ret;
    }

    int l = 1;
    if (!string_substr_check(len, pos, l)) {
      return -1;
    }
    int i_max = len - s_len;
    for (int i = (pos > i_max ? i_max : pos) ; i >= 0; i--) {
      if (input[i] == s[0] && memcmp(input+i, s, s_len) == 0) {
        return i;
      }
    }
  }
  return -1;
}

const char *string_memnstr(const char *haystack, const char *needle,
                           int needle_len, const char *end) {
  const char *p = haystack;
  char ne = needle[needle_len-1];

  end -= needle_len;
  while (p <= end) {
    if ((p = (char *)memchr(p, *needle, (end-p+1))) && ne == p[needle_len-1]) {
      if (!memcmp(needle, p, needle_len-1)) {
        return p;
      }
    }
    if (p == NULL) {
      return NULL;
    }
    p++;
  }
  return NULL;
}

void *string_memrchr(const void *s, int c, size_t n) {
  register unsigned char *e;

  if (n <= 0) {
    return NULL;
  }
  for (e = (unsigned char *)s + n - 1; e >= (unsigned char *)s; e--) {
    if (*e == (unsigned char)c) {
      return (void *)e;
    }
  }
  return NULL;
}

char *string_replace(const char *s, int &len, int start, int length,
                     const char *replacement, int len_repl) {
  ASSERT(s);
  ASSERT(replacement);
  if (!string_substr_check(len, start, length)) {
    len = 0;
    return string_duplicate("", 0);
  }

  char *ret = (char *)malloc(len + len_repl - length + 1);

  int ret_len = 0;
  if (start) {
    memcpy(ret, s, start);
    ret_len += start;
  }
  if (len_repl) {
    memcpy(ret + ret_len, replacement, len_repl);
    ret_len += len_repl;
  }
  len -= (start + length);
  if (len) {
    memcpy(ret + ret_len, s + start + length, len);
    ret_len += len;
  }

  len = ret_len;
  ret[ret_len] = '\0';
  return ret;
}

char *string_replace(const char *input, int &len,
                     const char *search, int len_search,
                     const char *replacement, int len_replace,
                     int &count, bool case_sensitive) {
  ASSERT(input);
  ASSERT(search && len_search);

  if (len == 0) {
    return NULL;
  }

  std::vector<int> founds;
  founds.reserve(16);
  if (len_search == 1) {
    for (int pos = string_find(input, len, *search, 0, case_sensitive);
         pos >= 0;
         pos = string_find(input, len, *search, pos + len_search,
                           case_sensitive)) {
      founds.push_back(pos);
    }
  } else {
    for (int pos = string_find(input, len, search, len_search, 0,
                               case_sensitive);
         pos >= 0;
         pos = string_find(input, len, search, len_search,
                           pos + len_search, case_sensitive)) {
      founds.push_back(pos);
    }
  }

  count = founds.size();
  if (count == 0) {
    return NULL; // not found
  }

  char *ret = (char *)malloc(len + (len_replace - len_search) * count + 1);
  char *p = ret;
  int pos = 0; // last position in input that hasn't been copied over yet
  int n;
  for (unsigned int i = 0; i < founds.size(); i++) {
    n = founds[i];
    if (n > pos) {
      n -= pos;
      memcpy(p, input, n);
      p += n;
      input += n;
      pos += n;
    }
    if (len_replace) {
      memcpy(p, replacement, len_replace);
      p += len_replace;
    }
    input += len_search;
    pos += len_search;
  }
  n = len;
  if (n > pos) {
    n -= pos;
    memcpy(p, input, n);
    p += n;
  }
  *p = '\0';

  len = p - ret;
  return ret;
}

int string_span(const char *s1, int s1_len, const char *s2, int s2_len) {
  const char *s1_end = s1 + s1_len;
  const char *s2_end = s2 + s2_len;
  register const char *p = s1, *spanp;
  register char c = *p;

 cont:
  for (spanp = s2; p != s1_end && spanp != s2_end;) {
    if (*spanp++ == c) {
      c = *(++p);
      goto cont;
    }
  }
  return (p - s1);
}

int string_cspan(const char *s1, int s1_len, const char *s2, int s2_len) {
  const char *s1_end = s1 + s1_len;
  const char *s2_end = s2 + s2_len;
  register const char *p, *spanp;
  register char c = *s1;

  for (p = s1;;) {
    spanp = s2;
    do {
      if (*spanp == c || p == s1_end) {
        return p - s1;
      }
    } while (spanp++ < (s2_end - 1));
    c = *++p;
  }
  ASSERT(false);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

char *string_reverse(const char *s, int len) {
  ASSERT(s);
  char *n = (char *)malloc(len + 1);
  char *p = n;
  const char *e = s + len;

  while (--e >= s) {
    *p++ = *e;
  }

  *p = '\0';
  return n;
}

char *string_repeat(const char *s, int &len, int count) {
  ASSERT(s);

  if (len == 0 || count <= 0) {
    return NULL;
  }

  char *ret = (char *)malloc(len * count + 1);
  if (len == 1) {
    memset(ret, *s, count);
    len = count;
  } else {
    char *p = ret;
    for (int i = 0; i < count; i++) {
      memcpy(p, s, len);
      p += len;
    }
    len = p - ret;
  }
  ret[len] = '\0';
  return ret;
}

char *string_shuffle(const char *str, int len) {
  ASSERT(str);
  if (len <= 1) {
    return NULL;
  }

  char *ret = string_duplicate(str, len);
  int n_left = len;
  while (--n_left) {
    int rnd_idx = rand() % n_left;
    char temp = ret[n_left];
    ret[n_left] = ret[rnd_idx];
    ret[rnd_idx] = temp;
  }
  return ret;
}

char *string_chunk_split(const char *src, int &srclen, const char *end,
                         int endlen, int chunklen) {
  int chunks = srclen / chunklen; // complete chunks!
  int restlen = srclen - chunks * chunklen; /* srclen % chunklen */

  int out_len = (chunks + 1) * endlen + srclen + 1;
  char *dest = (char *)malloc(out_len);

  const char *p; char *q;
  const char *pMax = src + srclen - chunklen + 1;
  for (p = src, q = dest; p < pMax; ) {
    memcpy(q, p, chunklen);
    q += chunklen;
    memcpy(q, end, endlen);
    q += endlen;
    p += chunklen;
  }

  if (restlen) {
    memcpy(q, p, restlen);
    q += restlen;
    memcpy(q, end, endlen);
    q += endlen;
  }

  *q = '\0';
  srclen = q - dest;
  return(dest);
}

///////////////////////////////////////////////////////////////////////////////

#define PHP_TAG_BUF_SIZE 1023

/**
 * Check if tag is in a set of tags
 *
 * states:
 *
 * 0 start tag
 * 1 first non-whitespace char seen
 */
static int string_tag_find(const char *tag, int len, char *set) {
  char c, *n;
  const char *t;
  int state=0, done=0;
  char *norm;

  if (len <= 0) {
    return 0;
  }

  norm = (char *)malloc(len+1);

  n = norm;
  t = tag;
  c = tolower(*t);
  /*
    normalize the tag removing leading and trailing whitespace
    and turn any <a whatever...> into just <a> and any </tag>
    into <tag>
  */
  while (!done) {
    switch (c) {
    case '<':
      *(n++) = c;
      break;
    case '>':
      done =1;
      break;
    default:
      if (!isspace((int)c)) {
        if (state == 0) {
          state=1;
          if (c != '/')
            *(n++) = c;
        } else {
          *(n++) = c;
        }
      } else {
        if (state == 1)
          done=1;
      }
      break;
    }
    c = tolower(*(++t));
  }
  *(n++) = '>';
  *n = '\0';
  if (strstr(set, norm)) {
    done=1;
  } else {
    done=0;
  }
  free(norm);
  return done;
}

/**
 * A simple little state-machine to strip out html and php tags
 *
 * State 0 is the output state, State 1 means we are inside a
 * normal html tag and state 2 means we are inside a php tag.
 *
 * The state variable is passed in to allow a function like fgetss
 * to maintain state across calls to the function.
 *
 * lc holds the last significant character read and br is a bracket
 * counter.
 *
 * When an allow string is passed in we keep track of the string
 * in state 1 and when the tag is closed check it against the
 * allow string to see if we should allow it.

 * swm: Added ability to strip <?xml tags without assuming it PHP
 * code.
 */
static size_t strip_tags_impl(char *rbuf, int len, int *stateptr,
                              char *allow, int allow_len,
                              bool allow_tag_spaces) {
  char *tbuf, *buf, *p, *tp, *rp, c, lc;
  int br, i=0, depth=0, in_q = 0;
  int state = 0;

  if (stateptr)
    state = *stateptr;

  buf = string_duplicate(rbuf, len);
  c = *buf;
  lc = '\0';
  p = buf;
  rp = rbuf;
  br = 0;
  if (allow) {
    for (char *tmp = allow; *tmp; tmp++) {
      *tmp = tolower((int)*(unsigned char *)tmp);
    }
    tbuf = (char *)malloc(PHP_TAG_BUF_SIZE+1);
    tp = tbuf;
  } else {
    tbuf = tp = NULL;
  }

  while (i < len) {
    switch (c) {
    case '\0':
      break;
    case '<':
      if (isspace(*(p + 1)) && !allow_tag_spaces) {
        goto reg_char;
      }
      if (state == 0) {
        lc = '<';
        state = 1;
        if (allow) {
          tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
          *(tp++) = '<';
        }
      } else if (state == 1) {
        depth++;
      }
      break;

    case '(':
      if (state == 2) {
        if (lc != '"' && lc != '\'') {
          lc = '(';
          br++;
        }
      } else if (allow && state == 1) {
        tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
        *(tp++) = c;
      } else if (state == 0) {
        *(rp++) = c;
      }
      break;

    case ')':
      if (state == 2) {
        if (lc != '"' && lc != '\'') {
          lc = ')';
          br--;
        }
      } else if (allow && state == 1) {
        tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
        *(tp++) = c;
      } else if (state == 0) {
        *(rp++) = c;
      }
      break;

    case '>':
      if (depth) {
        depth--;
        break;
      }

      if (in_q) {
        break;
      }

      switch (state) {
      case 1: /* HTML/XML */
        lc = '>';
        in_q = state = 0;
        if (allow) {
          tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
          *(tp++) = '>';
          *tp='\0';
          if (string_tag_find(tbuf, tp-tbuf, allow)) {
            memcpy(rp, tbuf, tp-tbuf);
            rp += tp-tbuf;
          }
          tp = tbuf;
        }
        break;

      case 2: /* PHP */
        if (!br && lc != '\"' && *(p-1) == '?') {
          in_q = state = 0;
          tp = tbuf;
        }
        break;

      case 3:
        in_q = state = 0;
        tp = tbuf;
        break;

      case 4: /* JavaScript/CSS/etc... */
        if (p >= buf + 2 && *(p-1) == '-' && *(p-2) == '-') {
          in_q = state = 0;
          tp = tbuf;
        }
        break;

      default:
        *(rp++) = c;
        break;
      }
      break;

    case '"':
    case '\'':
      if (state == 2 && *(p-1) != '\\') {
        if (lc == c) {
          lc = '\0';
        } else if (lc != '\\') {
          lc = c;
        }
      } else if (state == 0) {
        *(rp++) = c;
      } else if (allow && state == 1) {
        tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
        *(tp++) = c;
      }
      if (state && p != buf && *(p-1) != '\\' && (!in_q || *p == in_q)) {
        if (in_q) {
          in_q = 0;
        } else {
          in_q = *p;
        }
      }
      break;

    case '!':
      /* JavaScript & Other HTML scripting languages */
      if (state == 1 && *(p-1) == '<') {
        state = 3;
        lc = c;
      } else {
        if (state == 0) {
          *(rp++) = c;
        } else if (allow && state == 1) {
          tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
          *(tp++) = c;
        }
      }
      break;

    case '-':
      if (state == 3 && p >= buf + 2 && *(p-1) == '-' && *(p-2) == '!') {
        state = 4;
      } else {
        goto reg_char;
      }
      break;

    case '?':

      if (state == 1 && *(p-1) == '<') {
        br=0;
        state=2;
        break;
      }

    case 'E':
    case 'e':
      /* !DOCTYPE exception */
      if (state==3 && p > buf+6
          && tolower(*(p-1)) == 'p'
          && tolower(*(p-2)) == 'y'
          && tolower(*(p-3)) == 't'
          && tolower(*(p-4)) == 'c'
          && tolower(*(p-5)) == 'o'
          && tolower(*(p-6)) == 'd') {
        state = 1;
        break;
      }
      /* fall-through */

    case 'l':

      /* swm: If we encounter '<?xml' then we shouldn't be in
       * state == 2 (PHP). Switch back to HTML.
       */

      if (state == 2 && p > buf+2 && *(p-1) == 'm' && *(p-2) == 'x') {
        state = 1;
        break;
      }

      /* fall-through */
    default:
    reg_char:
      if (state == 0) {
        *(rp++) = c;
      } else if (allow && state == 1) {
        tp = ((tp-tbuf) >= PHP_TAG_BUF_SIZE ? tbuf: tp);
        *(tp++) = c;
      }
      break;
    }
    c = *(++p);
    i++;
  }
  if (rp < rbuf + len) {
    *rp = '\0';
  }
  free(buf);
  if (allow)
    free(tbuf);
  if (stateptr)
    *stateptr = state;

  return (size_t)(rp - rbuf);
}

char *string_strip_tags(const char *s, int &len, const char *allow,
                        int allow_len) {
  ASSERT(s);
  ASSERT(allow);

  char *ret = string_duplicate(s, len);
  char *sallow = string_duplicate(allow, allow_len);
  len = strip_tags_impl(ret, len, NULL, sallow, allow_len, false);
  free(sallow);
  return ret;
}

///////////////////////////////////////////////////////////////////////////////

char *string_wordwrap(const char *text, int &textlen, int linelength,
                      const char *breakchar, int breakcharlen, bool docut) {
  ASSERT(text);
  ASSERT(breakchar);

  char *newtext;
  int newtextlen, chk;
  size_t alloced;
  long current = 0, laststart = 0, lastspace = 0;

  if (textlen == 0) {
    return NULL;
  }

  if (breakcharlen == 0) {
    throw InvalidArgumentException("wordbreak", "(empty)");
  }

  if (linelength == 0 && docut) {
    throw InvalidArgumentException("width", "can't force cut when width = 0");
  }

  /* Special case for a single-character break as it needs no
     additional storage space */
  if (breakcharlen == 1 && !docut) {
    newtext = string_duplicate(text, textlen);

    laststart = lastspace = 0;
    for (current = 0; current < textlen; current++) {
      if (text[current] == breakchar[0]) {
        laststart = lastspace = current;
      } else if (text[current] == ' ') {
        if (current - laststart >= linelength) {
          newtext[current] = breakchar[0];
          laststart = current + 1;
        }
        lastspace = current;
      } else if (current - laststart >= linelength && laststart != lastspace) {
        newtext[lastspace] = breakchar[0];
        laststart = lastspace + 1;
      }
    }

    return newtext;
  }

  /* Multiple character line break or forced cut */
  if (linelength > 0) {
    chk = (int)(textlen/linelength + 1);
    alloced = textlen + chk * breakcharlen + 1;
  } else {
    chk = textlen;
    alloced = textlen * (breakcharlen + 1) + 1;
  }
  newtext = (char *)malloc(alloced);

  /* now keep track of the actual new text length */
  newtextlen = 0;

  laststart = lastspace = 0;
  for (current = 0; current < textlen; current++) {
    if (chk <= 0) {
      alloced += (int) (((textlen - current + 1)/linelength + 1) *
                        breakcharlen) + 1;
      newtext = (char *)realloc(newtext, alloced);
      chk = (int) ((textlen - current)/linelength) + 1;
    }
    /* when we hit an existing break, copy to new buffer, and
     * fix up laststart and lastspace */
    if (text[current] == breakchar[0]
        && current + breakcharlen < textlen
        && !strncmp(text+current, breakchar, breakcharlen)) {
      memcpy(newtext+newtextlen, text+laststart,
             current-laststart+breakcharlen);
      newtextlen += current-laststart+breakcharlen;
      current += breakcharlen - 1;
      laststart = lastspace = current + 1;
      chk--;
    }
    /* if it is a space, check if it is at the line boundary,
     * copy and insert a break, or just keep track of it */
    else if (text[current] == ' ') {
      if (current - laststart >= linelength) {
        memcpy(newtext+newtextlen, text+laststart, current-laststart);
        newtextlen += current - laststart;
        memcpy(newtext+newtextlen, breakchar, breakcharlen);
        newtextlen += breakcharlen;
        laststart = current + 1;
        chk--;
      }
      lastspace = current;
    }
    /* if we are cutting, and we've accumulated enough
     * characters, and we haven't see a space for this line,
     * copy and insert a break. */
    else if (current - laststart >= linelength
             && docut && laststart >= lastspace) {
      memcpy(newtext+newtextlen, text+laststart, current-laststart);
      newtextlen += current - laststart;
      memcpy(newtext+newtextlen, breakchar, breakcharlen);
      newtextlen += breakcharlen;
      laststart = lastspace = current;
      chk--;
    }
    /* if the current word puts us over the linelength, copy
     * back up until the last space, insert a break, and move
     * up the laststart */
    else if (current - laststart >= linelength
             && laststart < lastspace) {
      memcpy(newtext+newtextlen, text+laststart, lastspace-laststart);
      newtextlen += lastspace - laststart;
      memcpy(newtext+newtextlen, breakchar, breakcharlen);
      newtextlen += breakcharlen;
      laststart = lastspace = lastspace + 1;
      chk--;
    }
  }

  /* copy over any stragglers */
  if (laststart != current) {
    memcpy(newtext+newtextlen, text+laststart, current-laststart);
    newtextlen += current - laststart;
  }

  textlen = newtextlen;
  newtext[newtextlen] = '\0';
  return newtext;
}

///////////////////////////////////////////////////////////////////////////////

char *string_addcslashes(const char *str, int &length, const char *what,
                         int wlength) {
  ASSERT(str);
  ASSERT(what);

  char flags[256];
  string_charmask(what, wlength, flags);

  char *new_str = (char *)malloc((length << 2) + 1);
  const char *source;
  const char *end;
  char *target;
  for (source = str, end = source + length, target = new_str; source < end;
       source++) {
    char c = *source;
    if (flags[(unsigned char)c]) {
      if ((unsigned char) c < 32 || (unsigned char) c > 126) {
        *target++ = '\\';
        switch (c) {
        case '\n': *target++ = 'n'; break;
        case '\t': *target++ = 't'; break;
        case '\r': *target++ = 'r'; break;
        case '\a': *target++ = 'a'; break;
        case '\v': *target++ = 'v'; break;
        case '\b': *target++ = 'b'; break;
        case '\f': *target++ = 'f'; break;
        default: target += sprintf(target, "%03o", (unsigned char) c);
        }
        continue;
      }
      *target++ = '\\';
    }
    *target++ = c;
  }
  *target = 0;
  length = target - new_str;
  return new_str;
}

char *string_stripcslashes(const char *input, int &nlen) {
  ASSERT(input);
  if (nlen == 0) {
    return NULL;
  }

  char *str = string_duplicate(input, nlen);

  char *source, *target, *end;
  int i;
  char numtmp[4];

  for (source=str, end=str+nlen, target=str; source < end; source++) {
    if (*source == '\\' && source+1 < end) {
      source++;
      switch (*source) {
      case 'n':  *target++='\n'; nlen--; break;
      case 'r':  *target++='\r'; nlen--; break;
      case 'a':  *target++='\a'; nlen--; break;
      case 't':  *target++='\t'; nlen--; break;
      case 'v':  *target++='\v'; nlen--; break;
      case 'b':  *target++='\b'; nlen--; break;
      case 'f':  *target++='\f'; nlen--; break;
      case '\\': *target++='\\'; nlen--; break;
      case 'x':
        if (source+1 < end && isxdigit((int)(*(source+1)))) {
          numtmp[0] = *++source;
          if (source+1 < end && isxdigit((int)(*(source+1)))) {
            numtmp[1] = *++source;
            numtmp[2] = '\0';
            nlen-=3;
          } else {
            numtmp[1] = '\0';
            nlen-=2;
          }
          *target++=(char)strtol(numtmp, NULL, 16);
          break;
        }
        /* break is left intentionally */
      default:
        i=0;
        while (source < end && *source >= '0' && *source <= '7' && i<3) {
          numtmp[i++] = *source++;
        }
        if (i) {
          numtmp[i]='\0';
          *target++=(char)strtol(numtmp, NULL, 8);
          nlen-=i;
          source--;
        } else {
          *target++=*source;
          nlen--;
        }
      }
    } else {
      *target++=*source;
    }
  }
  *target='\0';
  nlen = target - str;
  return str;
}

char *string_addslashes(const char *str, int &length) {
  ASSERT(str);
  if (length == 0) {
    return NULL;
  }

  char *new_str = (char *)malloc((length << 1) + 1);
  const char *source = str;
  const char *end = source + length;
  char *target = new_str;

  while (source < end) {
    switch (*source) {
    case '\0':
      *target++ = '\\';
      *target++ = '0';
      break;
    case '\'':
    case '\"':
    case '\\':
      *target++ = '\\';
      /* break is missing *intentionally* */
    default:
      *target++ = *source;
      break;
    }

    source++;
  }

  *target = 0;
  length = target - new_str;
  return new_str;
}

char *string_stripslashes(const char *input, int &l) {
  ASSERT(input);
  if (!*input) {
    return NULL;
  }

  char *str = string_duplicate(input, l);
  char *s, *t;
  s = str;
  t = str;

  while (l > 0) {
    if (*t == '\\') {
      t++;        /* skip the slash */
      l--;
      if (l > 0) {
        if (*t == '0') {
          *s++='\0';
          t++;
        } else {
          *s++ = *t++;  /* preserve the next character */
        }
        l--;
      }
    } else {
      *s++ = *t++;
      l--;
    }
  }
  if (s != t) {
    *s = '\0';
  }
  l = s - str;
  return str;
}

char *string_quotemeta(const char *input, int &len) {
  ASSERT(input);
  if (len == 0) {
    return NULL;
  }

  char *ret = (char *)malloc((len << 1) + 1);
  char *q = ret;
  for (const char *p = input; *p; p++) {
    char c = *p;
    switch (c) {
    case '.':
    case '\\':
    case '+':
    case '*':
    case '?':
    case '[':
    case '^':
    case ']':
    case '$':
    case '(':
    case ')':
      *q++ = '\\';
      /* break is missing _intentionally_ */
    default:
      *q++ = c;
    }
  }
  *q = 0;
  len = q - ret;
  return ret;
}

///////////////////////////////////////////////////////////////////////////////

static char string_hex2int(int c) {
  if (isdigit(c)) {
    return c - '0';
  }
  if (c >= 'A' && c <= 'F') {
    return c - 'A' + 10;
  }
  if (c >= 'a' && c <= 'f') {
    return c - 'a' + 10;
  }
  return -1;
}

char *string_quoted_printable_encode(const char *input, int &len) {
  const char *hex = "0123456789ABCDEF";

  unsigned char *ret =
    (unsigned char *)malloc(3 * len + 3 * (((3 * len)/PHP_QPRINT_MAXL) + 1));
  unsigned char *d = ret;

  int length = len;
  unsigned char c;
  unsigned long lp = 0;
  while (length--) {
    if (((c = *input++) == '\015') && (*input == '\012') && length > 0) {
      *d++ = '\015';
      *d++ = *input++;
      length--;
      lp = 0;
    } else {
      if (iscntrl (c) || (c == 0x7f) || (c & 0x80) || (c == '=') ||
          ((c == ' ') && (*input == '\015'))) {
        if ((lp += 3) > PHP_QPRINT_MAXL) {
          *d++ = '=';
          *d++ = '\015';
          *d++ = '\012';
          lp = 3;
        }
        *d++ = '=';
        *d++ = hex[c >> 4];
        *d++ = hex[c & 0xf];
      } else {
        if ((++lp) > PHP_QPRINT_MAXL) {
          *d++ = '=';
          *d++ = '\015';
          *d++ = '\012';
          lp = 1;
        }
        *d++ = c;
      }
    }
  }
  *d = '\0';
  len = d - ret;
  return (char*)ret;
}

char *string_quoted_printable_decode(const char *input, int &len) {
  ASSERT(input);
  if (len == 0) {
    return NULL;
  }

  int i = 0, j = 0, k;
  const char *str_in = input;
  char *str_out = (char *)malloc(len + 1);
  while (str_in[i]) {
    switch (str_in[i]) {
    case '=':
      if (str_in[i + 1] && str_in[i + 2] &&
          isxdigit((int) str_in[i + 1]) &&
          isxdigit((int) str_in[i + 2]))
        {
          str_out[j++] = (string_hex2int((int) str_in[i + 1]) << 4)
            + string_hex2int((int) str_in[i + 2]);
          i += 3;
        } else  /* check for soft line break according to RFC 2045*/ {
        k = 1;
        while (str_in[i + k] &&
               ((str_in[i + k] == 32) || (str_in[i + k] == 9))) {
          /* Possibly, skip spaces/tabs at the end of line */
          k++;
        }
        if (!str_in[i + k]) {
          /* End of line reached */
          i += k;
        }
        else if ((str_in[i + k] == 13) && (str_in[i + k + 1] == 10)) {
          /* CRLF */
          i += k + 2;
        }
        else if ((str_in[i + k] == 13) || (str_in[i + k] == 10)) {
          /* CR or LF */
          i += k + 1;
        }
        else {
          str_out[j++] = str_in[i++];
        }
      }
      break;
    default:
      str_out[j++] = str_in[i++];
    }
  }
  str_out[j] = '\0';
  len = j;
  return str_out;
}

char *string_bin2hex(const char *input, int &len) {
  static char hexconvtab[] = "0123456789abcdef";

  ASSERT(input);
  if (len == 0) {
    return NULL;
  }

  int i, j;
  char *result = (char *)malloc((len << 1) + 1);

  for (i = j = 0; i < len; i++) {
    result[j++] = hexconvtab[(unsigned char)input[i] >> 4];
    result[j++] = hexconvtab[(unsigned char)input[i] & 15];
  }
  result[j] = '\0';
  len = j;
  return result;
}

char *string_hex2bin(const char *input, int &len) {
  len <<= 1;
  char *str = (char *)malloc(len + 1);
  int i, j;
  for (i = j = 0; i < len; i++) {
    char c = input[j++];
    if (c >= '0' && c <= '9') {
      str[i] = (c - '0') << 4;
    } else if (c >= 'a' && c <= 'f') {
      str[i] = (c - 'a' + 10) << 4;
    } else if (c >= 'A' && c <= 'F') {
      str[i] = (c - 'A' + 10) << 4;
    } else {
      throw InvalidArgumentException("bad encoding at position", j);
    }
    c = input[j++];
    if (c >= '0' && c <= '9') {
      str[i] |= c - '0';
    } else if (c >= 'a' && c <= 'f') {
      str[i] |= c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
      str[i] |= c - 'A' + 10;
    } else {
      throw InvalidArgumentException("bad encoding at position", j);
    }
  }
  str[len] = '\0';
  return str;
}

long string_base_to_long(const char *s, int len, int base) {
  long num = 0, digit, onum;
  const char *orig = s;
  if (base < 2 || base > 36) {
    return 0;
  }
  for (int i = len; i > 0; i--) {
    char c = *s++;

    digit = (c >= '0' && c <= '9') ? c - '0'
      : (c >= 'A' && c <= 'Z') ? c - 'A' + 10
      : (c >= 'a' && c <= 'z') ? c - 'a' + 10
      : base;

    if (digit >= base) {
      continue;
    }

    onum = num;
    num = num * base + digit;
    if (num >= onum) continue;
    Logger::Warning("Number '%s' is too big to fit in long", orig);
    return LONG_MAX;
  }
  return num;
}

Variant string_base_to_numeric(const char *s, int len, int base) {
  long num = 0;
  double fnum = 0;
  int mode = 0;
  long cutoff;
  int cutlim;

  if (base < 2 || base > 36) {
    return false;
  }

  cutoff = LONG_MAX / base;
  cutlim = LONG_MAX % base;

  for (int i = len; i > 0; i--) {
    char c = *s++;

    /* might not work for EBCDIC */
    if (c >= '0' && c <= '9')
      c -= '0';
    else if (c >= 'A' && c <= 'Z')
      c -= 'A' - 10;
    else if (c >= 'a' && c <= 'z')
      c -= 'a' - 10;
    else
      continue;

    if (c >= base)
      continue;

    switch (mode) {
    case 0: /* Integer */
      if (num < cutoff || (num == cutoff && c <= cutlim)) {
        num = num * base + c;
        break;
      } else {
        fnum = num;
        mode = 1;
      }
      /* fall-through */
    case 1: /* Float */
      fnum = fnum * base + c;
    }
  }

  if (mode == 1) {
    return fnum;
  }
  return num;
}

char *string_long_to_base(unsigned long value, int base) {
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  char buf[(sizeof(unsigned long) << 3) + 1];
  char *ptr, *end;

  if (base < 2 || base > 36) {
    throw InvalidArgumentException("base", base);
  }

  end = ptr = buf + sizeof(buf) - 1;
  *ptr = '\0';

  do {
    *--ptr = digits[value % base];
    value /= base;
  } while (ptr > buf && value);

  return string_duplicate(ptr, end - ptr);
}

char *string_numeric_to_base(CVarRef value, int base) {
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

  if ((!value.isInteger() && !value.isDouble()) || base < 2 || base > 36) {
    return string_duplicate("", 0);
  }

  if (value.isDouble()) {
    double fvalue = floor(value); /* floor it just in case */
    char *ptr, *end;
    char buf[(sizeof(double) << 3) + 1];

    /* Don't try to convert +/- infinity */
    if (fvalue == HUGE_VAL || fvalue == -HUGE_VAL) {
      Logger::Warning("Number too large");
      return string_duplicate("", 0);
    }

    end = ptr = buf + sizeof(buf) - 1;
    *ptr = '\0';

    do {
      *--ptr = digits[(int) fmod(fvalue, base)];
      fvalue /= base;
    } while (ptr > buf && fabs(fvalue) >= 1);

    return string_duplicate(ptr, end - ptr);
  }

  return string_long_to_base(value.toInt64(), base);
}

///////////////////////////////////////////////////////////////////////////////
// uuencode

#define PHP_UU_ENC(c) \
  ((c) ? ((c) & 077) + ' ' : '`')
#define PHP_UU_ENC_C2(c) \
  PHP_UU_ENC(((*(c) << 4) & 060) | ((*((c) + 1) >> 4) & 017))
#define PHP_UU_ENC_C3(c) \
  PHP_UU_ENC(((*(c + 1) << 2) & 074) | ((*((c) + 2) >> 6) & 03))
#define PHP_UU_DEC(c) \
  (((c) - ' ') & 077)

char *string_uuencode(const char *src, int src_len, int &dest_len) {
  ASSERT(src);
  ASSERT(src_len);

  int len = 45;
  char *p;
  const char *s, *e, *ee;
  char *dest;

  /* encoded length is ~ 38% greater then the original */
  p = dest = (char *)malloc((int)ceil(src_len * 1.38) + 46);
  s = src;
  e = src + src_len;

  while ((s + 3) < e) {
    ee = s + len;
    if (ee > e) {
      ee = e;
      len = ee - s;
      if (len % 3) {
        ee = s + (int) (floor(len / 3) * 3);
      }
    }
    *p++ = PHP_UU_ENC(len);

    while (s < ee) {
      *p++ = PHP_UU_ENC(*s >> 2);
      *p++ = PHP_UU_ENC_C2(s);
      *p++ = PHP_UU_ENC_C3(s);
      *p++ = PHP_UU_ENC(*(s + 2) & 077);

      s += 3;
    }

    if (len == 45) {
      *p++ = '\n';
    }
  }

  if (s < e) {
    if (len == 45) {
      *p++ = PHP_UU_ENC(e - s);
      len = 0;
    }

    *p++ = PHP_UU_ENC(*s >> 2);
    *p++ = PHP_UU_ENC_C2(s);
    *p++ = ((e - s) > 1) ? PHP_UU_ENC_C3(s) : PHP_UU_ENC('\0');
    *p++ = ((e - s) > 2) ? PHP_UU_ENC(*(s + 2) & 077) : PHP_UU_ENC('\0');
  }

  if (len < 45) {
    *p++ = '\n';
  }

  *p++ = PHP_UU_ENC('\0');
  *p++ = '\n';
  *p = '\0';

  dest_len = p - dest;
  return dest;
}

char *string_uudecode(const char *src, int src_len, int &total_len) {
  total_len = 0;
  int len;
  const char *s, *e, *ee;
  char *p, *dest;

  p = dest = (char *)malloc((int)ceil(src_len * 0.75) + 1);
  s = src;
  e = src + src_len;

  while (s < e) {
    if ((len = PHP_UU_DEC(*s++)) <= 0) {
      break;
    }
    /* sanity check */
    if (len > src_len) {
      goto err;
    }

    total_len += len;

    ee = s + (len == 45 ? 60 : (int) floor(len * 1.33));
    /* sanity check */
    if (ee > e) {
      goto err;
    }

    while (s < ee) {
      *p++ = PHP_UU_DEC(*s) << 2 | PHP_UU_DEC(*(s + 1)) >> 4;
      *p++ = PHP_UU_DEC(*(s + 1)) << 4 | PHP_UU_DEC(*(s + 2)) >> 2;
      *p++ = PHP_UU_DEC(*(s + 2)) << 6 | PHP_UU_DEC(*(s + 3));
      s += 4;
    }

    if (len < 45) {
      break;
    }

    /* skip \n */
    s++;
  }

  if ((len = total_len > (p - dest))) {
    *p++ = PHP_UU_DEC(*s) << 2 | PHP_UU_DEC(*(s + 1)) >> 4;
    if (len > 1) {
      *p++ = PHP_UU_DEC(*(s + 1)) << 4 | PHP_UU_DEC(*(s + 2)) >> 2;
      if (len > 2) {
        *p++ = PHP_UU_DEC(*(s + 2)) << 6 | PHP_UU_DEC(*(s + 3));
      }
    }
  }

  *(dest + total_len) = '\0';

  return dest;

 err:
  free(dest);
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// base64

static const char base64_table[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static const char base64_pad = '=';

static const short base64_reverse_table[256] = {
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
  52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
  -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
  -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
  -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};

static unsigned char *php_base64_encode(const unsigned char *str, int length,
                                        int *ret_length) {
  const unsigned char *current = str;
  unsigned char *p;
  unsigned char *result;

  if ((length + 2) < 0 || ((length + 2) / 3) >= (1 << (sizeof(int) * 8 - 2))) {
    if (ret_length != NULL) {
      *ret_length = 0;
    }
    return NULL;
  }

  result = (unsigned char *)malloc(((length + 2) / 3) * 4 + 1);
  p = result;

  while (length > 2) { /* keep going until we have less than 24 bits */
    *p++ = base64_table[current[0] >> 2];
    *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
    *p++ = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
    *p++ = base64_table[current[2] & 0x3f];

    current += 3;
    length -= 3; /* we just handle 3 octets of data */
  }

  /* now deal with the tail end of things */
  if (length != 0) {
    *p++ = base64_table[current[0] >> 2];
    if (length > 1) {
      *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
      *p++ = base64_table[(current[1] & 0x0f) << 2];
      *p++ = base64_pad;
    } else {
      *p++ = base64_table[(current[0] & 0x03) << 4];
      *p++ = base64_pad;
      *p++ = base64_pad;
    }
  }
  if (ret_length != NULL) {
    *ret_length = (int)(p - result);
  }
  *p = '\0';
  return result;
}

static unsigned char *php_base64_decode(const unsigned char *str,
                                        int length, int *ret_length,
                                        bool strict) {
  const unsigned char *current = str;
  int ch, i = 0, j = 0, k;
  /* this sucks for threaded environments */
  unsigned char *result;

  result = (unsigned char *)malloc(length + 1);

  /* run through the whole string, converting as we go */
  while ((ch = *current++) != '\0' && length-- > 0) {
    if (ch == base64_pad) break;

    ch = base64_reverse_table[ch];
    if ((!strict && ch < 0) || ch == -1) {
      /* a space or some other separator character, we simply skip over */
      continue;
    } else if (ch == -2) {
      free(result);
      return NULL;
    }

    switch(i % 4) {
    case 0:
      result[j] = ch << 2;
      break;
    case 1:
      result[j++] |= ch >> 4;
      result[j] = (ch & 0x0f) << 4;
      break;
    case 2:
      result[j++] |= ch >>2;
      result[j] = (ch & 0x03) << 6;
      break;
    case 3:
      result[j++] |= ch;
      break;
    }
    i++;
  }

  k = j;
  /* mop things up if we ended on a boundary */
  if (ch == base64_pad) {
    switch(i % 4) {
    case 1:
      free(result);
      return NULL;
    case 2:
      k++;
    case 3:
      result[k++] = 0;
    }
  }
  if(ret_length) {
    *ret_length = j;
  }
  result[j] = '\0';
  return result;
}

char *string_base64_encode(const char *input, int &len) {
  return (char *)php_base64_encode((unsigned char *)input, len, &len);
}

char *string_base64_decode(const char *input, int &len, bool strict) {
  return (char *)php_base64_decode((unsigned char *)input, len, &len, strict);
}

///////////////////////////////////////////////////////////////////////////////

char *string_escape_shell_arg(const char *str) {
  int x, y, l;
  char *cmd;

  y = 0;
  l = strlen(str);

  cmd = (char *)malloc((l << 2) + 3); /* worst case */

  cmd[y++] = '\'';

  for (x = 0; x < l; x++) {
    switch (str[x]) {
    case '\'':
      cmd[y++] = '\'';
      cmd[y++] = '\\';
      cmd[y++] = '\'';
      /* fall-through */
    default:
      cmd[y++] = str[x];
    }
  }
  cmd[y++] = '\'';
  cmd[y] = '\0';
  return cmd;
}

char *string_escape_shell_cmd(const char *str) {
  register int x, y, l;
  char *cmd;
  char *p = NULL;

  l = strlen(str);
  cmd = (char *)malloc((l << 1) + 1);

  for (x = 0, y = 0; x < l; x++) {
    switch (str[x]) {
    case '"':
    case '\'':
      if (!p && (p = (char *)memchr(str + x + 1, str[x], l - x - 1))) {
        /* noop */
      } else if (p && *p == str[x]) {
        p = NULL;
      } else {
        cmd[y++] = '\\';
      }
      cmd[y++] = str[x];
      break;
    case '#': /* This is character-set independent */
    case '&':
    case ';':
    case '`':
    case '|':
    case '*':
    case '?':
    case '~':
    case '<':
    case '>':
    case '^':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '$':
    case '\\':
    case '\x0A': /* excluding these two */
    case '\xFF':
      cmd[y++] = '\\';
      /* fall-through */
    default:
      cmd[y++] = str[x];
    }
  }
  cmd[y] = '\0';
  return cmd;
}

///////////////////////////////////////////////////////////////////////////////
// json

#define REVERSE16(us) (((us & 0xf) << 12) | (((us >> 4) & 0xf) << 8) | (((us >> 8) & 0xf) << 4) | ((us >> 12) & 0xf))

char *string_json_escape(const char *s, int &len, bool loose) {
  StringBuffer sb;
  if (len == 0) {
    sb.append("\"\"", 2);
  } else {
    unsigned short *utf16 =
      (unsigned short *)malloc(len * sizeof(unsigned short));

    len = utf8_to_utf16(utf16, (char*)s, len, loose ? 1 : 0);
    if (len <= 0) {
      sb.append("\"\"", 2);
    } else {
      static const char digits[] = "0123456789abcdef";

      sb += '"';
      for (int pos = 0; pos < len; pos++) {
        unsigned short us = utf16[pos];
        switch (us) {
        case '"':  sb.append("\\\"", 2); break;
        case '\\': sb.append("\\\\", 2); break;
        case '/':  sb.append("\\/", 2);  break;
        case '\b': sb.append("\\b", 2);  break;
        case '\f': sb.append("\\f", 2);  break;
        case '\n': sb.append("\\n", 2);  break;
        case '\r': sb.append("\\r", 2);  break;
        case '\t': sb.append("\\t", 2);  break;
        default:
          if (us >= ' ' && (us & 127) == us) {
            sb.append((char)us);
          } else {
            sb.append("\\u", 2);
            us = REVERSE16(us);
            sb.append(digits[us & ((1 << 4) - 1)]); us >>= 4;
            sb.append(digits[us & ((1 << 4) - 1)]); us >>= 4;
            sb.append(digits[us & ((1 << 4) - 1)]); us >>= 4;
            sb.append(digits[us & ((1 << 4) - 1)]);
          }
          break;
        }
      }
      sb += '"';
    }

    free(utf16);
  }

  return sb.detach(len);
}

///////////////////////////////////////////////////////////////////////////////

void string_translate(char *str, int len, const char *str_from,
                      const char *str_to, int trlen) {
  int i;
  unsigned char xlat[256];

  if ((trlen < 1) || (len < 1)) {
    return;
  }

  for (i = 0; i < 256; xlat[i] = i, i++);
  for (i = 0; i < trlen; i++) {
    xlat[(unsigned char) str_from[i]] = str_to[i];
  }

  for (i = 0; i < len; i++) {
    str[i] = xlat[(unsigned char) str[i]];
  }
}

char *string_rot13(const char *input, int len) {
  ASSERT(input);

  static char rot13_from[] =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static char rot13_to[] =
    "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM";

  if (len == 0) {
    return NULL;
  }
  char *ret = string_duplicate(input, len);
  string_translate(ret, len, rot13_from, rot13_to, 52);
  return ret;
}

///////////////////////////////////////////////////////////////////////////////

static void string_similar_str(const char *txt1, int len1,
                               const char *txt2, int len2,
                               int *pos1, int *pos2, int *max) {
  const char *p, *q;
  const char *end1 = txt1 + len1;
  const char *end2 = txt2 + len2;
  int l;

  *max = 0;
  for (p = txt1; p < end1; p++) {
    for (q = txt2; q < end2; q++) {
      for (l = 0; (p + l < end1) && (q + l < end2) && (p[l] == q[l]); l++);
      if (l > *max) {
        *max = l;
        *pos1 = p - txt1;
        *pos2 = q - txt2;
      }
    }
  }
}

static int string_similar_char(const char *txt1, int len1,
                               const char *txt2, int len2) {
  int sum;
  int pos1 = 0, pos2 = 0, max;

  string_similar_str(txt1, len1, txt2, len2, &pos1, &pos2, &max);
  if ((sum = max)) {
    if (pos1 && pos2) {
      sum += string_similar_char(txt1, pos1, txt2, pos2);
    }
    if ((pos1 + max < len1) && (pos2 + max < len2)) {
      sum += string_similar_char(txt1 + pos1 + max, len1 - pos1 - max,
                                 txt2 + pos2 + max, len2 - pos2 - max);
    }
  }

  return sum;
}

int string_similar_text(const char *t1, int len1,
                        const char *t2, int len2, float *percent) {
  if (len1 == 0 && len2 == 0) {
    if (percent) *percent = 0.0;
    return 0;
  }

  int sim = string_similar_char(t1, len1, t2, len2);
  if (percent) *percent = sim * 200.0 / (len1 + len2);
  return sim;
}

///////////////////////////////////////////////////////////////////////////////

#define LEVENSHTEIN_MAX_LENTH 255

// reference implementation, only optimized for memory usage, not speed
int string_levenshtein(const char *s1, int l1, const char *s2, int l2,
                       int cost_ins, int cost_rep, int cost_del ) {
  int *p1, *p2, *tmp;
  int i1, i2, c0, c1, c2;

  if(l1==0) return l2*cost_ins;
  if(l2==0) return l1*cost_del;

  if((l1>LEVENSHTEIN_MAX_LENTH)||(l2>LEVENSHTEIN_MAX_LENTH))
    return -1;

  p1 = (int*)malloc((l2+1) * sizeof(int));
  p2 = (int*)malloc((l2+1) * sizeof(int));

  for(i2=0;i2<=l2;i2++) {
    p1[i2] = i2*cost_ins;
  }

  for(i1=0;i1<l1;i1++) {
    p2[0]=p1[0]+cost_del;
    for(i2=0;i2<l2;i2++) {
      c0=p1[i2]+((s1[i1]==s2[i2])?0:cost_rep);
      c1=p1[i2+1]+cost_del; if(c1<c0) c0=c1;
      c2=p2[i2]+cost_ins; if(c2<c0) c0=c2;
      p2[i2+1]=c0;
    }
    tmp=p1; p1=p2; p2=tmp;
  }

  c0=p1[l2];
  free(p1);
  free(p2);
  return c0;
}

///////////////////////////////////////////////////////////////////////////////
// crc32

/*
 * This code implements the AUTODIN II polynomial
 * The variable corresponding to the macro argument "crc" should
 * be an unsigned long.
 * Original code  by Spencer Garrett <srg@quick.com>
 */

#define CRC32(crc, ch) (crc = (crc >> 8) ^ crc32tab[(crc ^ (ch)) & 0xff])

/* generated using the AUTODIN II polynomial
 *  x^32 + x^26 + x^23 + x^22 + x^16 +
 *  x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + 1
 */
static const unsigned int crc32tab[256] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
  0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
  0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
  0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
  0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
  0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
  0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
  0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
  0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
  0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
  0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
  0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
  0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
  0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
  0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
  0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
  0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
  0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
  0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
  0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
  0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
  0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
  0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
  0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
  0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
  0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
  0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
  0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
  0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
  0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
  0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
  0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
  0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
  0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
  0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
  0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
  0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
  0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
  0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

int string_crc32(const char *p, int len) {
  uint32 crcinit = 0;
  register int32 crc = crcinit ^ 0xFFFFFFFF;
  for (; len--; ++p) {
    crc = ((crc >> 8) & 0x00FFFFFF) ^ crc32tab[(crc ^ (*p)) & 0xFF];
  }
  return crc ^ 0xFFFFFFFF;
}

///////////////////////////////////////////////////////////////////////////////
// crypt

#include <unistd.h>

static unsigned char itoa64[] =
  "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static void ito64(char *s, long v, int n) {
  while (--n >= 0) {
    *s++ = itoa64[v&0x3f];
    v >>= 6;
  }
}

char *string_crypt(const char *key, const char *salt) {
  ASSERT(key);
  ASSERT(salt);

  char random_salt[3];
  if (!*salt) {
    ito64(random_salt, rand(), 2);
    random_salt[2] = '\0';
    salt = random_salt;
  }

  static Mutex mutex;
  Lock lock(mutex);
  return strdup(crypt(key, salt));
}

///////////////////////////////////////////////////////////////////////////////

char *string_money_format(const char *format, double value) {
  bool check = false;
  const char *p = format;
  while ((p = strchr(p, '%'))) {
    if (*(p + 1) == '%') {
      p += 2;
    } else if (!check) {
      check = true;
      p++;
    } else {
      throw InvalidArgumentException
        ("format", "Only a single %%i or %%n token can be used");
    }
  }

  int format_len = strlen(format);
  int str_len = format_len + 1024;
  char *str = (char *)malloc(str_len);
  str_len = strfmon(str, str_len, format, value);
  ASSERT(str_len >= 0);
  str[str_len] = 0;
  return str;
}

///////////////////////////////////////////////////////////////////////////////

char *string_number_format(double d, int dec, char dec_point,
                           char thousand_sep) {
  char *tmpbuf = NULL, *resbuf;
  char *s, *t;  /* source, target */
  char *dp;
  int integral;
  int tmplen, reslen=0;
  int count=0;
  int is_negative=0;

  if (d < 0) {
    is_negative = 1;
    d = -d;
  }

  if (dec < 0) dec = 0;
  PHP_ROUND_WITH_FUZZ(d, dec);

  // departure from PHP: we got rid of dependencies on spprintf() here.
  tmpbuf = (char *)malloc(64);
  snprintf(tmpbuf, 64, "%.*F", dec, d);
  tmplen = strlen(tmpbuf);
  if (tmpbuf == NULL || !isdigit((int)tmpbuf[0])) {
    return tmpbuf;
  }

  /* find decimal point, if expected */
  if (dec) {
    dp = strpbrk(tmpbuf, ".,");
  } else {
    dp = NULL;
  }

  /* calculate the length of the return buffer */
  if (dp) {
    integral = dp - tmpbuf;
  } else {
    /* no decimal point was found */
    integral = tmplen;
  }

  /* allow for thousand separators */
  if (thousand_sep) {
    integral += (integral-1) / 3;
  }

  reslen = integral;

  if (dec) {
    reslen += dec;

    if (dec_point) {
      reslen++;
    }
  }

  /* add a byte for minus sign */
  if (is_negative) {
    reslen++;
  }
  resbuf = (char *) malloc(reslen+1); /* +1 for NUL terminator */

  s = tmpbuf+tmplen-1;
  t = resbuf+reslen;
  *t-- = '\0';

  /* copy the decimal places.
   * Take care, as the sprintf implementation may return less places than
   * we requested due to internal buffer limitations */
  if (dec) {
    int declen = dp ? s - dp : 0;
    int topad = dec > declen ? dec - declen : 0;

    /* pad with '0's */
    while (topad--) {
      *t-- = '0';
    }

    if (dp) {
      s -= declen + 1; /* +1 to skip the point */
      t -= declen;

      /* now copy the chars after the point */
      memcpy(t + 1, dp + 1, declen);
    }

    /* add decimal point */
    if (dec_point) {
      *t-- = dec_point;
    }
  }

  /* copy the numbers before the decimal point, adding thousand
   * separator every three digits */
  while(s >= tmpbuf) {
    *t-- = *s--;
    if (thousand_sep && (++count%3)==0 && s>=tmpbuf) {
      *t-- = thousand_sep;
    }
  }

  /* and a minus sign, if needed */
  if (is_negative) {
    *t-- = '-';
  }

  free(tmpbuf);
  return resbuf;
}

///////////////////////////////////////////////////////////////////////////////
// soundex

/* Simple soundex algorithm as described by Knuth in TAOCP, vol 3 */
char *string_soundex(const char *str) {
  ASSERT(str);

  int _small, code, last;
  char soundex[4 + 1];

  static char soundex_table[26] = {
    0,              /* A */
    '1',            /* B */
    '2',            /* C */
    '3',            /* D */
    0,              /* E */
    '1',            /* F */
    '2',            /* G */
    0,              /* H */
    0,              /* I */
    '2',            /* J */
    '2',            /* K */
    '4',            /* L */
    '5',            /* M */
    '5',            /* N */
    0,              /* O */
    '1',            /* P */
    '2',            /* Q */
    '6',            /* R */
    '2',            /* S */
    '3',            /* T */
    0,              /* U */
    '1',            /* V */
    0,              /* W */
    '2',            /* X */
    0,              /* Y */
    '2'             /* Z */
  };

  if (!*str) {
    return NULL;
  }

  /* build soundex string */
  last = -1;
  const char *p = str;
  for (_small = 0; *p && _small < 4; p++) {
    /* convert chars to upper case and strip non-letter chars */
    /* BUG: should also map here accented letters used in non */
    /* English words or names (also found in English text!): */
    /* esstsett, thorn, n-tilde, c-cedilla, s-caron, ... */
    code = toupper((int)(unsigned char)(*p));
    if (code >= 'A' && code <= 'Z') {
      if (_small == 0) {
        /* remember first valid char */
        soundex[_small++] = code;
        last = soundex_table[code - 'A'];
      } else {
        /* ignore sequences of consonants with same soundex */
        /* code in trail, and vowels unless they separate */
        /* consonant letters */
        code = soundex_table[code - 'A'];
        if (code != last) {
          if (code != 0) {
            soundex[_small++] = code;
          }
          last = code;
        }
      }
    }
  }
  /* pad with '0' and terminate with 0 ;-) */
  while (_small < 4) {
    soundex[_small++] = '0';
  }
  soundex[_small] = '\0';
  return strdup(soundex);
}

///////////////////////////////////////////////////////////////////////////////
// metaphone

/**
 * this is now the original code by Michael G Schwern:
 * i've changed it just a slightly bit (use emalloc,
 * get rid of includes etc)
 * - thies - 13.09.1999
 */

/*-----------------------------  */
/* this used to be "metaphone.h" */
/*-----------------------------  */

/* Special encodings */
#define  SH   'X'
#define  TH   '0'

/*-----------------------------  */
/* end of "metaphone.h"          */
/*-----------------------------  */

/*----------------------------- */
/* this used to be "metachar.h" */
/*----------------------------- */

/* Metachar.h ... little bits about characters for metaphone */
/*-- Character encoding array & accessing macros --*/
/* Stolen directly out of the book... */
char _codes[26] = { 1,16,4,16,9,2,4,16,9,2,0,2,2,2,1,4,0,2,4,4,1,0,0,0,8,0};

#define ENCODE(c) (isalpha(c) ? _codes[((toupper(c)) - 'A')] : 0)

#define isvowel(c)  (ENCODE(c) & 1)    /* AEIOU */

/* These letters are passed through unchanged */
#define NOCHANGE(c) (ENCODE(c) & 2)    /* FJMNR */

/* These form dipthongs when preceding H */
#define AFFECTH(c)  (ENCODE(c) & 4)    /* CGPST */

/* These make C and G soft */
#define MAKESOFT(c) (ENCODE(c) & 8)    /* EIY */

/* These prevent GH from becoming F */
#define NOGHTOF(c)  (ENCODE(c) & 16)  /* BDH */

/*----------------------------- */
/* end of "metachar.h"          */
/*----------------------------- */

/* I suppose I could have been using a character pointer instead of
 * accesssing the array directly... */

/* Look at the next letter in the word */
#define Next_Letter (toupper(word[w_idx+1]))
/* Look at the current letter in the word */
#define Curr_Letter (toupper(word[w_idx]))
/* Go N letters back. */
#define Look_Back_Letter(n)  (w_idx >= n ? toupper(word[w_idx-n]) : '\0')
/* Previous letter.  I dunno, should this return null on failure? */
#define Prev_Letter (Look_Back_Letter(1))
/* Look two letters down.  It makes sure you don't walk off the string. */
#define After_Next_Letter  (Next_Letter != '\0' ? toupper(word[w_idx+2]) \
                           : '\0')
#define Look_Ahead_Letter(n) (toupper(Lookahead(word+w_idx, n)))

/* Allows us to safely look ahead an arbitrary # of letters */
/* I probably could have just used strlen... */
static char Lookahead(unsigned char *word, int how_far) {
  char letter_ahead = '\0';  /* null by default */
  int idx;
  for (idx = 0; word[idx] != '\0' && idx < how_far; idx++);
  /* Edge forward in the string... */

  letter_ahead = (char)word[idx];  /* idx will be either == to how_far or
                                    * at the end of the string
                                    */
  return letter_ahead;
}

/* phonize one letter
 * We don't know the buffers size in advance. On way to solve this is to just
 * re-allocate the buffer size. We're using an extra of 2 characters (this
 * could be one though; or more too). */
#define Phonize(c)  {                                                   \
    if (p_idx >= max_buffer_len) {                                      \
      phoned_word = (char *)realloc(phoned_word, max_buffer_len + 2);   \
      max_buffer_len += 2;                                              \
    }                                                                   \
    phoned_word[p_idx++] = c;                                           \
  }
/* Slap a null character on the end of the phoned word */
#define End_Phoned_Word  {phoned_word[p_idx] = '\0';}
/* How long is the phoned word? */
#define Phone_Len  (p_idx)

/* Note is a letter is a 'break' in the word */
#define Isbreak(c)  (!isalpha(c))

char *string_metaphone(const char *input, int word_len, long max_phonemes,
                       int traditional) {
  unsigned char *word = (unsigned char *)input;
  char *phoned_word;

  int w_idx = 0;        /* point in the phonization we're at. */
  int p_idx = 0;        /* end of the phoned phrase */
  int max_buffer_len = 0;    /* maximum length of the destination buffer */

  /*-- Parameter checks --*/
  /* Negative phoneme length is meaningless */

  if (max_phonemes < 0)
    return NULL;

  /* Empty/null string is meaningless */
  /* Overly paranoid */
  /* assert(word != NULL && word[0] != '\0'); */

  if (word == NULL)
    return NULL;

  /*-- Allocate memory for our phoned_phrase --*/
  if (max_phonemes == 0) {  /* Assume largest possible */
    max_buffer_len = word_len;
    phoned_word = (char *)malloc(word_len + 1);
  } else {
    max_buffer_len = max_phonemes;
    phoned_word = (char *)malloc(max_phonemes +1);
  }

  /*-- The first phoneme has to be processed specially. --*/
  /* Find our first letter */
  for (; !isalpha(Curr_Letter); w_idx++) {
    /* On the off chance we were given nothing but crap... */
    if (Curr_Letter == '\0') {
      End_Phoned_Word
        return phoned_word;  /* For testing */
    }
  }

  switch (Curr_Letter) {
    /* AE becomes E */
  case 'A':
    if (Next_Letter == 'E') {
      Phonize('E');
      w_idx += 2;
    }
    /* Remember, preserve vowels at the beginning */
    else {
      Phonize('A');
      w_idx++;
    }
    break;
    /* [GKP]N becomes N */
  case 'G':
  case 'K':
  case 'P':
    if (Next_Letter == 'N') {
      Phonize('N');
      w_idx += 2;
    }
    break;
    /* WH becomes H,
       WR becomes R
       W if followed by a vowel */
  case 'W':
    if (Next_Letter == 'H' ||
      Next_Letter == 'R') {
      Phonize(Next_Letter);
      w_idx += 2;
    } else if (isvowel(Next_Letter)) {
      Phonize('W');
      w_idx += 2;
    }
    /* else ignore */
    break;
    /* X becomes S */
  case 'X':
    Phonize('S');
    w_idx++;
    break;
    /* Vowels are kept */
    /* We did A already
       case 'A':
       case 'a':
     */
  case 'E':
  case 'I':
  case 'O':
  case 'U':
    Phonize(Curr_Letter);
    w_idx++;
    break;
  default:
    /* do nothing */
    break;
  }

  /* On to the metaphoning */
  for (; Curr_Letter != '\0' &&
         (max_phonemes == 0 || Phone_Len < max_phonemes);
       w_idx++) {
    /* How many letters to skip because an eariler encoding handled
     * multiple letters */
    unsigned short int skip_letter = 0;


    /* THOUGHT:  It would be nice if, rather than having things like...
     * well, SCI.  For SCI you encode the S, then have to remember
     * to skip the C.  So the phonome SCI invades both S and C.  It would
     * be better, IMHO, to skip the C from the S part of the encoding.
     * Hell, I'm trying it.
     */

    /* Ignore non-alphas */
    if (!isalpha(Curr_Letter))
      continue;

    /* Drop duplicates, except CC */
    if (Curr_Letter == Prev_Letter &&
      Curr_Letter != 'C')
      continue;

    switch (Curr_Letter) {
      /* B -> B unless in MB */
    case 'B':
      if (Prev_Letter != 'M')
        Phonize('B');
      break;
      /* 'sh' if -CIA- or -CH, but not SCH, except SCHW.
       * (SCHW is handled in S)
       *  S if -CI-, -CE- or -CY-
       *  dropped if -SCI-, SCE-, -SCY- (handed in S)
       *  else K
       */
    case 'C':
      if (MAKESOFT(Next_Letter)) {  /* C[IEY] */
        if (After_Next_Letter == 'A' &&
          Next_Letter == 'I') {  /* CIA */
          Phonize(SH);
        }
        /* SC[IEY] */
        else if (Prev_Letter == 'S') {
          /* Dropped */
        } else {
          Phonize('S');
        }
      } else if (Next_Letter == 'H') {
        if ((!traditional) && (After_Next_Letter == 'R' ||
                               Prev_Letter == 'S')) {  /* Christ, School */
          Phonize('K');
        } else {
          Phonize(SH);
        }
        skip_letter++;
      } else {
        Phonize('K');
      }
      break;
      /* J if in -DGE-, -DGI- or -DGY-
       * else T
       */
    case 'D':
      if (Next_Letter == 'G' && MAKESOFT(After_Next_Letter)) {
        Phonize('J');
        skip_letter++;
      } else
        Phonize('T');
      break;
      /* F if in -GH and not B--GH, D--GH, -H--GH, -H---GH
       * else dropped if -GNED, -GN,
       * else dropped if -DGE-, -DGI- or -DGY- (handled in D)
       * else J if in -GE-, -GI, -GY and not GG
       * else K
       */
    case 'G':
      if (Next_Letter == 'H') {
        if (!(NOGHTOF(Look_Back_Letter(3)) || Look_Back_Letter(4) == 'H')) {
          Phonize('F');
          skip_letter++;
        } else {
          /* silent */
        }
      } else if (Next_Letter == 'N') {
        if (Isbreak(After_Next_Letter) ||
            (After_Next_Letter == 'E' && Look_Ahead_Letter(3) == 'D')) {
          /* dropped */
        } else
          Phonize('K');
      } else if (MAKESOFT(Next_Letter) && Prev_Letter != 'G') {
        Phonize('J');
      } else {
        Phonize('K');
      }
      break;
      /* H if before a vowel and not after C,G,P,S,T */
    case 'H':
      if (isvowel(Next_Letter) && !AFFECTH(Prev_Letter))
        Phonize('H');
      break;
      /* dropped if after C
       * else K
       */
    case 'K':
      if (Prev_Letter != 'C')
        Phonize('K');
      break;
      /* F if before H
       * else P
       */
    case 'P':
      if (Next_Letter == 'H') {
        Phonize('F');
      } else {
        Phonize('P');
      }
      break;
      /* K
       */
    case 'Q':
      Phonize('K');
      break;
      /* 'sh' in -SH-, -SIO- or -SIA- or -SCHW-
       * else S
       */
    case 'S':
      if (Next_Letter == 'I' &&
          (After_Next_Letter == 'O' || After_Next_Letter == 'A')) {
        Phonize(SH);
      } else if (Next_Letter == 'H') {
        Phonize(SH);
        skip_letter++;
      } else if ((!traditional) &&
                 (Next_Letter == 'C' && Look_Ahead_Letter(2) == 'H' &&
                  Look_Ahead_Letter(3) == 'W')) {
        Phonize(SH);
        skip_letter += 2;
      } else {
        Phonize('S');
      }
      break;
      /* 'sh' in -TIA- or -TIO-
       * else 'th' before H
       * else T
       */
    case 'T':
      if (Next_Letter == 'I' &&
        (After_Next_Letter == 'O' || After_Next_Letter == 'A')) {
        Phonize(SH);
      } else if (Next_Letter == 'H') {
        Phonize(TH);
        skip_letter++;
      } else {
        Phonize('T');
      }
      break;
      /* F */
    case 'V':
      Phonize('F');
      break;
      /* W before a vowel, else dropped */
    case 'W':
      if (isvowel(Next_Letter))
        Phonize('W');
      break;
      /* KS */
    case 'X':
      Phonize('K');
      Phonize('S');
      break;
      /* Y if followed by a vowel */
    case 'Y':
      if (isvowel(Next_Letter))
        Phonize('Y');
      break;
      /* S */
    case 'Z':
      Phonize('S');
      break;
      /* No transformation */
    case 'F':
    case 'J':
    case 'L':
    case 'M':
    case 'N':
    case 'R':
      Phonize(Curr_Letter);
      break;
    default:
      /* nothing */
      break;
    } /* END SWITCH */

    w_idx += skip_letter;
  } /* END FOR */

  End_Phoned_Word;
  return phoned_word;
}

///////////////////////////////////////////////////////////////////////////////
// Cyrillic

/**
 * This is codetables for different Cyrillic charsets (relative to koi8-r).
 * Each table contains data for 128-255 symbols from ASCII table.
 * First 256 symbols are for conversion from koi8-r to corresponding charset,
 * second 256 symbols are for reverse conversion, from charset to koi8-r.
 *
 * Here we have the following tables:
 * _cyr_win1251   - for windows-1251 charset
 * _cyr_iso88595  - for iso8859-5 charset
 * _cyr_cp866     - for x-cp866 charset
 * _cyr_mac       - for x-mac-cyrillic charset
 */
typedef unsigned char _cyr_charset_table[512];

static const _cyr_charset_table _cyr_win1251 = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,
  46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,
  154,174,190,46,159,189,46,46,179,191,180,157,46,46,156,183,
  46,46,182,166,173,46,46,158,163,152,164,155,46,46,46,167,
  225,226,247,231,228,229,246,250,233,234,235,236,237,238,239,240,
  242,243,244,245,230,232,227,254,251,253,255,249,248,252,224,241,
  193,194,215,199,196,197,214,218,201,202,203,204,205,206,207,208,
  210,211,212,213,198,200,195,222,219,221,223,217,216,220,192,209,
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,184,186,32,179,191,32,32,32,32,32,180,162,32,
  32,32,32,168,170,32,178,175,32,32,32,32,32,165,161,169,
  254,224,225,246,228,229,244,227,245,232,233,234,235,236,237,238,
  239,255,240,241,242,243,230,226,252,251,231,248,253,249,247,250,
  222,192,193,214,196,197,212,195,213,200,201,202,203,204,205,206,
  207,223,208,209,210,211,198,194,220,219,199,216,221,217,215,218,
};

static const _cyr_charset_table _cyr_cp866 = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  225,226,247,231,228,229,246,250,233,234,235,236,237,238,239,240,
  242,243,244,245,230,232,227,254,251,253,255,249,248,252,224,241,
  193,194,215,199,196,197,214,218,201,202,203,204,205,206,207,208,
  35,35,35,124,124,124,124,43,43,124,124,43,43,43,43,43,
  43,45,45,124,45,43,124,124,43,43,45,45,124,45,43,45,
  45,45,45,43,43,43,43,43,43,43,43,35,35,124,124,35,
  210,211,212,213,198,200,195,222,219,221,223,217,216,220,192,209,
  179,163,180,164,183,167,190,174,32,149,158,32,152,159,148,154,
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  205,186,213,241,243,201,32,245,187,212,211,200,190,32,247,198,
  199,204,181,240,242,185,32,244,203,207,208,202,216,32,246,32,
  238,160,161,230,164,165,228,163,229,168,169,170,171,172,173,174,
  175,239,224,225,226,227,166,162,236,235,167,232,237,233,231,234,
  158,128,129,150,132,133,148,131,149,136,137,138,139,140,141,142,
  143,159,144,145,146,147,134,130,156,155,135,152,157,153,151,154,
};

static const _cyr_charset_table _cyr_iso88595 = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,179,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  225,226,247,231,228,229,246,250,233,234,235,236,237,238,239,240,
  242,243,244,245,230,232,227,254,251,253,255,249,248,252,224,241,
  193,194,215,199,196,197,214,218,201,202,203,204,205,206,207,208,
  210,211,212,213,198,200,195,222,219,221,223,217,216,220,192,209,
  32,163,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,241,32,32,32,32,32,32,32,32,32,32,32,32,
  32,32,32,161,32,32,32,32,32,32,32,32,32,32,32,32,
  238,208,209,230,212,213,228,211,229,216,217,218,219,220,221,222,
  223,239,224,225,226,227,214,210,236,235,215,232,237,233,231,234,
  206,176,177,198,180,181,196,179,197,184,185,186,187,188,189,190,
  191,207,192,193,194,195,182,178,204,203,183,200,205,201,199,202,
};

static const _cyr_charset_table _cyr_mac = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  225,226,247,231,228,229,246,250,233,234,235,236,237,238,239,240,
  242,243,244,245,230,232,227,254,251,253,255,249,248,252,224,241,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,179,163,209,
  193,194,215,199,196,197,214,218,201,202,203,204,205,206,207,208,
  210,211,212,213,198,200,195,222,219,221,223,217,216,220,192,255,
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
  64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
  80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
  96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  160,161,162,222,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,221,180,181,182,183,184,185,186,187,188,189,190,191,
  254,224,225,246,228,229,244,227,245,232,233,234,235,236,237,238,
  239,223,240,241,242,243,230,226,252,251,231,248,253,249,247,250,
  158,128,129,150,132,133,148,131,149,136,137,138,139,140,141,142,
  143,159,144,145,146,147,134,130,156,155,135,152,157,153,151,154,
};

/**
 * This is the function that performs real in-place conversion of the string
 * between charsets.
 * Parameters:
 *    str - string to be converted
 *    from,to - one-symbol label of source and destination charset
 * The following symbols are used as labels:
 *    k - koi8-r
 *    w - windows-1251
 *    i - iso8859-5
 *    a - x-cp866
 *    d - x-cp866
 *    m - x-mac-cyrillic
 */
char *string_convert_cyrillic_string(const char *input, int length,
                                     char from, char to) {
  ASSERT(input);
  const unsigned char *from_table, *to_table;
  unsigned char tmp;
  unsigned char *str = (unsigned char *)string_duplicate(input, length);

  from_table = NULL;
  to_table   = NULL;

  switch (toupper((int)(unsigned char)from)) {
  case 'W': from_table = _cyr_win1251;  break;
  case 'A':
  case 'D': from_table = _cyr_cp866;    break;
  case 'I': from_table = _cyr_iso88595; break;
  case 'M': from_table = _cyr_mac;      break;
  case 'K':
    break;
  default:
    throw InvalidArgumentException(0, "Unknown source charset: %c", from);
  }

  switch (toupper((int)(unsigned char)to)) {
  case 'W': to_table = _cyr_win1251;    break;
  case 'A':
  case 'D': to_table = _cyr_cp866;      break;
  case 'I': to_table = _cyr_iso88595;   break;
  case 'M': to_table = _cyr_mac;        break;
  case 'K':
    break;
  default:
    throw InvalidArgumentException(0, "Unknown destination charset: %c", to);
  }

  if (!str) {
    return (char *)str;
  }

  for (int i = 0; i<length; i++) {
    tmp = (from_table == NULL)? str[i] : from_table[ str[i] ];
    str[i] = (to_table == NULL) ? tmp : to_table[tmp + 256];
  }
  return (char *)str;
}

///////////////////////////////////////////////////////////////////////////////
// Hebrew

#define _HEB_BLOCK_TYPE_ENG 1
#define _HEB_BLOCK_TYPE_HEB 2

#define isheb(c)                                                        \
  (((((unsigned char) c) >= 224) && (((unsigned char) c) <= 250)) ? 1 : 0)
#define _isblank(c)                                                     \
  (((((unsigned char) c) == ' '  || ((unsigned char) c) == '\t')) ? 1 : 0)
#define _isnewline(c)                                                   \
  (((((unsigned char) c) == '\n' || ((unsigned char) c) == '\r')) ? 1 : 0)

/**
 * Converts Logical Hebrew text (Hebrew Windows style) to Visual text
 * Cheers/complaints/flames - Zeev Suraski <zeev@php.net>
 */
char *string_convert_hebrew_string(const char *str, int &str_len,
                                   int max_chars_per_line,
                                   int convert_newlines) {
  ASSERT(str);
  const char *tmp;
  char *heb_str, *broken_str;
  char *target;
  int block_start, block_end, block_type, block_length, i;
  long max_chars=0;
  int begin, end, char_count, orig_begin;

  if (str_len == 0) {
    return NULL;
  }

  tmp = str;
  block_start=block_end=0;

  heb_str = (char *) malloc(str_len + 1);
  target = heb_str+str_len;
  *target = 0;
  target--;

  block_length=0;

  if (isheb(*tmp)) {
    block_type = _HEB_BLOCK_TYPE_HEB;
  } else {
    block_type = _HEB_BLOCK_TYPE_ENG;
  }

  do {
    if (block_type == _HEB_BLOCK_TYPE_HEB) {
      while ((isheb((int)*(tmp+1)) ||
              _isblank((int)*(tmp+1)) ||
              ispunct((int)*(tmp+1)) ||
              (int)*(tmp+1)=='\n' ) && block_end<str_len-1) {
        tmp++;
        block_end++;
        block_length++;
      }
      for (i = block_start; i<= block_end; i++) {
        *target = str[i];
        switch (*target) {
        case '(':  *target = ')';  break;
        case ')':  *target = '(';  break;
        case '[':  *target = ']';  break;
        case ']':  *target = '[';  break;
        case '{':  *target = '}';  break;
        case '}':  *target = '{';  break;
        case '<':  *target = '>';  break;
        case '>':  *target = '<';  break;
        case '\\': *target = '/';  break;
        case '/':  *target = '\\'; break;
        default:
          break;
        }
        target--;
      }
      block_type = _HEB_BLOCK_TYPE_ENG;
    } else {
      while (!isheb(*(tmp+1)) &&
             (int)*(tmp+1)!='\n' && block_end < str_len-1) {
        tmp++;
        block_end++;
        block_length++;
      }
      while ((_isblank((int)*tmp) ||
              ispunct((int)*tmp)) && *tmp!='/' &&
             *tmp!='-' && block_end > block_start) {
        tmp--;
        block_end--;
      }
      for (i = block_end; i >= block_start; i--) {
        *target = str[i];
        target--;
      }
      block_type = _HEB_BLOCK_TYPE_HEB;
    }
    block_start=block_end+1;
  } while (block_end < str_len-1);


  broken_str = (char *) malloc(str_len+1);
  begin=end=str_len-1;
  target = broken_str;

  while (1) {
    char_count=0;
    while ((!max_chars || char_count < max_chars) && begin > 0) {
      char_count++;
      begin--;
      if (begin <= 0 || _isnewline(heb_str[begin])) {
        while (begin > 0 && _isnewline(heb_str[begin-1])) {
          begin--;
          char_count++;
        }
        break;
      }
    }
    if (char_count == max_chars) { /* try to avoid breaking words */
      int new_char_count=char_count, new_begin=begin;

      while (new_char_count > 0) {
        if (_isblank(heb_str[new_begin]) || _isnewline(heb_str[new_begin])) {
          break;
        }
        new_begin++;
        new_char_count--;
      }
      if (new_char_count > 0) {
        char_count=new_char_count;
        begin=new_begin;
      }
    }
    orig_begin=begin;

    if (_isblank(heb_str[begin])) {
      heb_str[begin]='\n';
    }
    while (begin <= end && _isnewline(heb_str[begin])) {
      /* skip leading newlines */
      begin++;
    }
    for (i = begin; i <= end; i++) { /* copy content */
      *target = heb_str[i];
      target++;
    }
    for (i = orig_begin; i <= end && _isnewline(heb_str[i]); i++) {
      *target = heb_str[i];
      target++;
    }
    begin=orig_begin;

    if (begin <= 0) {
      *target = 0;
      break;
    }
    begin--;
    end=begin;
  }
  free((void*)heb_str);

  if (convert_newlines) {
    int count;
    char *ret = string_replace(broken_str, str_len, "\n", strlen("\n"),
                               "<br />\n", strlen("<br />\n"), count, true);
    if (ret) {
      free(broken_str);
      return ret;
    }
  }
  return broken_str;
}

#ifdef MAC_OS_X

  void *memrchr(const void *s, int c, size_t n) {
    for (const char *p = (const char *)s + n - 1; p >= s; p--) {
      if (*p == c) return (void *)p;
    }
    return NULL;
  }

#endif

///////////////////////////////////////////////////////////////////////////////
}