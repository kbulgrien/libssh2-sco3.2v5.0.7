/* Copyright (c) 2004-2009, Sara Golemon <sarag@libssh2.org>
 * Copyright (c) 2009 by Daniel Stenberg
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *
 *   Redistributions of source code must retain the above
 *   copyright notice, this list of conditions and the
 *   following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials
 *   provided with the distribution.
 *
 *   Neither the name of the copyright holder nor the names
 *   of any other contributors may be used to endorse or
 *   promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#ifndef LIBSSH2_H
#define LIBSSH2_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Allow alternate API prefix from CFLAGS or calling app */
#ifndef LIBSSH2_API
# ifdef LIBSSH2_WIN32
#  ifdef LIBSSH2_LIBRARY
#   define LIBSSH2_API __declspec(dllexport)
#  else
#   define LIBSSH2_API __declspec(dllimport)
#  endif /* LIBSSH2_LIBRARY */
# else /* !LIBSSH2_WIN32 */
#  define LIBSSH2_API
# endif /* LIBSSH2_WIN32 */
#endif /* LIBSSH2_API */

#if defined(LIBSSH2_DARWIN) || (defined(LIBSSH2_WIN32) && \
                                !defined(_MSC_VER) && !defined(__MINGW32__))
# include <sys/uio.h>
#endif

#if (defined(NETWARE) && !defined(__NOVELL_LIBC__))
# include <sys/bsdskt.h>
typedef unsigned int uint32_t;
#endif

#if (defined(_SCO_DS))
typedef int socklen_t;
#endif

#if defined(LIBSSH2_WIN32) && defined(_MSC_VER) && (_MSC_VER <= 1400)
typedef unsigned __int64 libssh2_uint64_t;
typedef __int64 libssh2_int64_t;
typedef unsigned int uint32_t;
#ifndef _SSIZE_T_DEFINED
typedef int ssize_t;
#define _SSIZE_T_DEFINED
#endif
#else
typedef unsigned long long libssh2_uint64_t;
typedef long long libssh2_int64_t;
#endif

/* We use underscore instead of dash when appending DEV in dev versions just
   to make the BANNER define (used by src/session.c) be a valid SSH
   banner. Release versions have no appended strings and may of course not
   have dashes either. */
#define LIBSSH2_VERSION                             "1.2.4_DEV"

/* The numeric version number is also available "in parts" by using these
   defines: */
#define LIBSSH2_VERSION_MAJOR                       1
#define LIBSSH2_VERSION_MINOR                       2
#define LIBSSH2_VERSION_PATCH                       4

/* This is the numeric version of the libssh2 version number, meant for easier
   parsing and comparions by programs. The LIBSSH2_VERSION_NUM define will
   always follow this syntax:

         0xXXYYZZ

   Where XX, YY and ZZ are the main version, release and patch numbers in
   hexadecimal (using 8 bits each). All three numbers are always represented
   using two digits.  1.2 would appear as "0x010200" while version 9.11.7
   appears as "0x090b07".

   This 6-digit (24 bits) hexadecimal number does not show pre-release number,
   and it is always a greater number in a more recent release. It makes
   comparisons with greater than and less than work.
*/
#define LIBSSH2_VERSION_NUM                         0x010204

/*
 * This is the date and time when the full source package was created. The
 * timestamp is not stored in the source code repo, as the timestamp is
 * properly set in the tarballs by the maketgz script.
 *
 * The format of the date should follow this template:
 *
 * "Mon Feb 12 11:35:33 UTC 2007"
 */
#define LIBSSH2_TIMESTAMP "DEV"

/* Part of every banner, user specified or not */
#define LIBSSH2_SSH_BANNER                  "SSH-2.0-libssh2_" LIBSSH2_VERSION

/* We *could* add a comment here if we so chose */
#define LIBSSH2_SSH_DEFAULT_BANNER                  LIBSSH2_SSH_BANNER
#define LIBSSH2_SSH_DEFAULT_BANNER_WITH_CRLF        LIBSSH2_SSH_DEFAULT_BANNER "\r\n"

/* Default generate and safe prime sizes for diffie-hellman-group-exchange-sha1 */
#define LIBSSH2_DH_GEX_MINGROUP     1024
#define LIBSSH2_DH_GEX_OPTGROUP     1536
#define LIBSSH2_DH_GEX_MAXGROUP     2048

/* Defaults for pty requests */
#define LIBSSH2_TERM_WIDTH      80
#define LIBSSH2_TERM_HEIGHT     24
#define LIBSSH2_TERM_WIDTH_PX   0
#define LIBSSH2_TERM_HEIGHT_PX  0

/* 1/4 second */
#define LIBSSH2_SOCKET_POLL_UDELAY      250000
/* 0.25 * 120 == 30 seconds */
#define LIBSSH2_SOCKET_POLL_MAXLOOPS    120

/* Maximum size to allow a payload to compress to, plays it safe by falling
   short of spec limits */
#define LIBSSH2_PACKET_MAXCOMP      32000

/* Maximum size to allow a payload to deccompress to, plays it safe by
   allowing more than spec requires */
#define LIBSSH2_PACKET_MAXDECOMP    40000

/* Maximum size for an inbound compressed payload, plays it safe by
   overshooting spec limits */
#define LIBSSH2_PACKET_MAXPAYLOAD   40000

/* Malloc callbacks */
#define LIBSSH2_ALLOC_FUNC(name)   void *name(size_t count, void **abstract)
#define LIBSSH2_REALLOC_FUNC(name) void *name(void *ptr, size_t count, \
                                              void **abstract)
#define LIBSSH2_FREE_FUNC(name)    void name(void *ptr, void **abstract)

typedef struct _LIBSSH2_USERAUTH_KBDINT_PROMPT
{
    char* text;
    unsigned int length;
    unsigned char echo;
} LIBSSH2_USERAUTH_KBDINT_PROMPT;

typedef struct _LIBSSH2_USERAUTH_KBDINT_RESPONSE
{
    char* text;
    unsigned int length;
} LIBSSH2_USERAUTH_KBDINT_RESPONSE;

/* 'publickey' authentication callback */
#define LIBSSH2_USERAUTH_PUBLICKEY_SIGN_FUNC(name) \
  int name(LIBSSH2_SESSION *session, unsigned char **sig, size_t *sig_len, \
           const unsigned char *data, size_t data_len, void **abstract)

/* 'keyboard-interactive' authentication callback */
#define LIBSSH2_USERAUTH_KBDINT_RESPONSE_FUNC(name_) \
 void name_(const char* name, int name_len, const char* instruction, \
            int instruction_len, int num_prompts, \
            const LIBSSH2_USERAUTH_KBDINT_PROMPT* prompts, \
            LIBSSH2_USERAUTH_KBDINT_RESPONSE* responses, void **abstract)

/* Callbacks for special SSH packets */
#define LIBSSH2_IGNORE_FUNC(name) \
 void name(LIBSSH2_SESSION *session, const char *message, int message_len, \
           void **abstract)

#define LIBSSH2_DEBUG_FUNC(name) \
 void name(LIBSSH2_SESSION *session, int always_display, const char *message, \
           int message_len, const char *language, int language_len, \
           void **abstract)

#define LIBSSH2_DISCONNECT_FUNC(name) \
 void name(LIBSSH2_SESSION *session, int reason, const char *message, \
           int message_len, const char *language, int language_len, \
           void **abstract)

#define LIBSSH2_PASSWD_CHANGEREQ_FUNC(name) \
 void name(LIBSSH2_SESSION *session, char **newpw, int *newpw_len, \
           void **abstract)

#define LIBSSH2_MACERROR_FUNC(name) \
 int name(LIBSSH2_SESSION *session, const char *packet, int packet_len, \
          void **abstract)

#define LIBSSH2_X11_OPEN_FUNC(name) \
 void name(LIBSSH2_SESSION *session, LIBSSH2_CHANNEL *channel, \
           const char *shost, int sport, void **abstract)

#define LIBSSH2_CHANNEL_CLOSE_FUNC(name) \
  void name(LIBSSH2_SESSION *session, void **session_abstract, \
            LIBSSH2_CHANNEL *channel, void **channel_abstract)

/* libssh2_session_callback_set() constants */
#define LIBSSH2_CALLBACK_IGNORE             0
#define LIBSSH2_CALLBACK_DEBUG              1
#define LIBSSH2_CALLBACK_DISCONNECT         2
#define LIBSSH2_CALLBACK_MACERROR           3
#define LIBSSH2_CALLBACK_X11                4

/* libssh2_session_method_pref() constants */
#define LIBSSH2_METHOD_KEX          0
#define LIBSSH2_METHOD_HOSTKEY      1
#define LIBSSH2_METHOD_CRYPT_CS     2
#define LIBSSH2_METHOD_CRYPT_SC     3
#define LIBSSH2_METHOD_MAC_CS       4
#define LIBSSH2_METHOD_MAC_SC       5
#define LIBSSH2_METHOD_COMP_CS      6
#define LIBSSH2_METHOD_COMP_SC      7
#define LIBSSH2_METHOD_LANG_CS      8
#define LIBSSH2_METHOD_LANG_SC      9

/* session.flags bits */
#define LIBSSH2_FLAG_SIGPIPE        0x00000001

typedef struct _LIBSSH2_SESSION                     LIBSSH2_SESSION;
typedef struct _LIBSSH2_CHANNEL                     LIBSSH2_CHANNEL;
typedef struct _LIBSSH2_LISTENER                    LIBSSH2_LISTENER;
typedef struct _LIBSSH2_KNOWNHOSTS                  LIBSSH2_KNOWNHOSTS;
typedef struct _LIBSSH2_AGENT                       LIBSSH2_AGENT;

typedef struct _LIBSSH2_POLLFD {
    unsigned char type; /* LIBSSH2_POLLFD_* below */

    union {
        int socket; /* File descriptors -- examined with system select() call */
        LIBSSH2_CHANNEL *channel; /* Examined by checking internal state */
        LIBSSH2_LISTENER *listener; /* Read polls only -- are inbound
                                       connections waiting to be accepted? */
    } fd;

    unsigned long events; /* Requested Events */
    unsigned long revents; /* Returned Events */
} LIBSSH2_POLLFD;

/* Poll FD Descriptor Types */
#define LIBSSH2_POLLFD_SOCKET       1
#define LIBSSH2_POLLFD_CHANNEL      2
#define LIBSSH2_POLLFD_LISTENER     3

/* Note: Win32 Doesn't actually have a poll() implementation, so some of these
   values are faked with select() data */
/* Poll FD events/revents -- Match sys/poll.h where possible */
#define LIBSSH2_POLLFD_POLLIN           0x0001 /* Data available to be read or
                                                  connection available --
                                                  All */
#define LIBSSH2_POLLFD_POLLPRI          0x0002 /* Priority data available to
                                                  be read -- Socket only */
#define LIBSSH2_POLLFD_POLLEXT          0x0002 /* Extended data available to
                                                  be read -- Channel only */
#define LIBSSH2_POLLFD_POLLOUT          0x0004 /* Can may be written --
                                                  Socket/Channel */
/* revents only */
#define LIBSSH2_POLLFD_POLLERR          0x0008 /* Error Condition -- Socket */
#define LIBSSH2_POLLFD_POLLHUP          0x0010 /* HangUp/EOF -- Socket */
#define LIBSSH2_POLLFD_SESSION_CLOSED   0x0010 /* Session Disconnect */
#define LIBSSH2_POLLFD_POLLNVAL         0x0020 /* Invalid request -- Socket
                                                  Only */
#define LIBSSH2_POLLFD_POLLEX           0x0040 /* Exception Condition --
                                                  Socket/Win32 */
#define LIBSSH2_POLLFD_CHANNEL_CLOSED   0x0080 /* Channel Disconnect */
#define LIBSSH2_POLLFD_LISTENER_CLOSED  0x0080 /* Listener Disconnect */

#define HAVE_LIBSSH2_SESSION_BLOCK_DIRECTION
/* Block Direction Types */
#define LIBSSH2_SESSION_BLOCK_INBOUND                  0x0001
#define LIBSSH2_SESSION_BLOCK_OUTBOUND                 0x0002

/* Hash Types */
#define LIBSSH2_HOSTKEY_HASH_MD5                            1
#define LIBSSH2_HOSTKEY_HASH_SHA1                           2

/* Hostkey Types */
#define LIBSSH2_HOSTKEY_TYPE_UNKNOWN			    0
#define LIBSSH2_HOSTKEY_TYPE_RSA			    1
#define LIBSSH2_HOSTKEY_TYPE_DSS			    2

/* Disconnect Codes (defined by SSH protocol) */
#define SSH_DISCONNECT_HOST_NOT_ALLOWED_TO_CONNECT          1
#define SSH_DISCONNECT_PROTOCOL_ERROR                       2
#define SSH_DISCONNECT_KEY_EXCHANGE_FAILED                  3
#define SSH_DISCONNECT_RESERVED                             4
#define SSH_DISCONNECT_MAC_ERROR                            5
#define SSH_DISCONNECT_COMPRESSION_ERROR                    6
#define SSH_DISCONNECT_SERVICE_NOT_AVAILABLE                7
#define SSH_DISCONNECT_PROTOCOL_VERSION_NOT_SUPPORTED       8
#define SSH_DISCONNECT_HOST_KEY_NOT_VERIFIABLE              9
#define SSH_DISCONNECT_CONNECTION_LOST                      10
#define SSH_DISCONNECT_BY_APPLICATION                       11
#define SSH_DISCONNECT_TOO_MANY_CONNECTIONS                 12
#define SSH_DISCONNECT_AUTH_CANCELLED_BY_USER               13
#define SSH_DISCONNECT_NO_MORE_AUTH_METHODS_AVAILABLE       14
#define SSH_DISCONNECT_ILLEGAL_USER_NAME                    15

/* Error Codes (defined by libssh2) */
#define LIBSSH2_ERROR_NONE                      0
#define LIBSSH2_ERROR_SOCKET_NONE               -1
#define LIBSSH2_ERROR_BANNER_NONE               -2
#define LIBSSH2_ERROR_BANNER_SEND               -3
#define LIBSSH2_ERROR_INVALID_MAC               -4
#define LIBSSH2_ERROR_KEX_FAILURE               -5
#define LIBSSH2_ERROR_ALLOC                     -6
#define LIBSSH2_ERROR_SOCKET_SEND               -7
#define LIBSSH2_ERROR_KEY_EXCHANGE_FAILURE      -8
#define LIBSSH2_ERROR_TIMEOUT                   -9
#define LIBSSH2_ERROR_HOSTKEY_INIT              -10
#define LIBSSH2_ERROR_HOSTKEY_SIGN              -11
#define LIBSSH2_ERROR_DECRYPT                   -12
#define LIBSSH2_ERROR_SOCKET_DISCONNECT         -13
#define LIBSSH2_ERROR_PROTO                     -14
#define LIBSSH2_ERROR_PASSWORD_EXPIRED          -15
#define LIBSSH2_ERROR_FILE                      -16
#define LIBSSH2_ERROR_METHOD_NONE               -17
#define LIBSSH2_ERROR_AUTHENTICATION_FAILED     -18
#define LIBSSH2_ERROR_PUBLICKEY_UNRECOGNIZED    LIBSSH2_ERROR_AUTHENTICATION_FAILED
#define LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED      -19
#define LIBSSH2_ERROR_CHANNEL_OUTOFORDER        -20
#define LIBSSH2_ERROR_CHANNEL_FAILURE           -21
#define LIBSSH2_ERROR_CHANNEL_REQUEST_DENIED    -22
#define LIBSSH2_ERROR_CHANNEL_UNKNOWN           -23
#define LIBSSH2_ERROR_CHANNEL_WINDOW_EXCEEDED   -24
#define LIBSSH2_ERROR_CHANNEL_PACKET_EXCEEDED   -25
#define LIBSSH2_ERROR_CHANNEL_CLOSED            -26
#define LIBSSH2_ERROR_CHANNEL_EOF_SENT          -27
#define LIBSSH2_ERROR_SCP_PROTOCOL              -28
#define LIBSSH2_ERROR_ZLIB                      -29
#define LIBSSH2_ERROR_SOCKET_TIMEOUT            -30
#define LIBSSH2_ERROR_SFTP_PROTOCOL             -31
#define LIBSSH2_ERROR_REQUEST_DENIED            -32
#define LIBSSH2_ERROR_METHOD_NOT_SUPPORTED      -33
#define LIBSSH2_ERROR_INVAL                     -34
#define LIBSSH2_ERROR_INVALID_POLL_TYPE         -35
#define LIBSSH2_ERROR_PUBLICKEY_PROTOCOL        -36
#define LIBSSH2_ERROR_EAGAIN                    -37
#define LIBSSH2_ERROR_BUFFER_TOO_SMALL          -38
#define LIBSSH2_ERROR_BAD_USE                   -39
#define LIBSSH2_ERROR_COMPRESS                  -40
#define LIBSSH2_ERROR_OUT_OF_BOUNDARY           -41
#define LIBSSH2_ERROR_AGENT_PROTOCOL            -42

/* Session API */
LIBSSH2_API LIBSSH2_SESSION *
libssh2_session_init_ex(LIBSSH2_ALLOC_FUNC((*my_alloc)),
                        LIBSSH2_FREE_FUNC((*my_free)),
                        LIBSSH2_REALLOC_FUNC((*my_realloc)), void *abstract);
#define libssh2_session_init() libssh2_session_init_ex(NULL, NULL, NULL, NULL)

LIBSSH2_API void **libssh2_session_abstract(LIBSSH2_SESSION *session);

LIBSSH2_API void *libssh2_session_callback_set(LIBSSH2_SESSION *session,
                                               int cbtype, void *callback);
LIBSSH2_API int libssh2_banner_set(LIBSSH2_SESSION *session,
                                   const char *banner);

LIBSSH2_API int libssh2_session_startup(LIBSSH2_SESSION *session, int sock);
LIBSSH2_API int libssh2_session_disconnect_ex(LIBSSH2_SESSION *session,
                                              int reason,
                                              const char *description,
                                              const char *lang);
#define libssh2_session_disconnect(session, description) \
  libssh2_session_disconnect_ex((session), SSH_DISCONNECT_BY_APPLICATION, \
                                (description), "")

LIBSSH2_API int libssh2_session_free(LIBSSH2_SESSION *session);

LIBSSH2_API const char *libssh2_hostkey_hash(LIBSSH2_SESSION *session,
                                             int hash_type);

LIBSSH2_API const char *libssh2_session_hostkey(LIBSSH2_SESSION *session,
                                                size_t *len, int *type);

LIBSSH2_API int libssh2_session_method_pref(LIBSSH2_SESSION *session,
                                            int method_type,
                                            const char *prefs);
LIBSSH2_API const char *libssh2_session_methods(LIBSSH2_SESSION *session,
                                                int method_type);
LIBSSH2_API int libssh2_session_last_error(LIBSSH2_SESSION *session,
                                           char **errmsg,
                                           int *errmsg_len, int want_buf);
LIBSSH2_API int libssh2_session_last_errno(LIBSSH2_SESSION *session);
LIBSSH2_API int libssh2_session_block_directions(LIBSSH2_SESSION *session);

LIBSSH2_API int libssh2_session_flag(LIBSSH2_SESSION *session, int flag,
                                     int value);

/* Userauth API */
LIBSSH2_API char *libssh2_userauth_list(LIBSSH2_SESSION *session,
                                        const char *username,
                                        unsigned int username_len);
LIBSSH2_API int libssh2_userauth_authenticated(LIBSSH2_SESSION *session);

LIBSSH2_API int libssh2_userauth_password_ex(LIBSSH2_SESSION *session,
                                             const char *username,
                                             unsigned int username_len,
                                             const char *password,
                                             unsigned int password_len,
                                             LIBSSH2_PASSWD_CHANGEREQ_FUNC((*passwd_change_cb)));

#define libssh2_userauth_password(session, username, password) \
 libssh2_userauth_password_ex((session), (username), strlen(username), \
                              (password), strlen(password), NULL)

LIBSSH2_API int
libssh2_userauth_publickey_fromfile_ex(LIBSSH2_SESSION *session,
                                       const char *username,
                                       unsigned int username_len,
                                       const char *publickey,
                                       const char *privatekey,
                                       const char *passphrase);

#define libssh2_userauth_publickey_fromfile(session, username, publickey, \
                                            privatekey, passphrase)     \
  libssh2_userauth_publickey_fromfile_ex((session), (username), \
                                         strlen(username), (publickey), \
                                         (privatekey), (passphrase))

LIBSSH2_API int
libssh2_userauth_publickey(LIBSSH2_SESSION *session,
                           const char *username,
                           const unsigned char *pubkeydata,
                           size_t pubkeydata_len,
                           LIBSSH2_USERAUTH_PUBLICKEY_SIGN_FUNC((*sign_callback)),
                           void **abstract);

LIBSSH2_API int
libssh2_userauth_hostbased_fromfile_ex(LIBSSH2_SESSION *session,
                                       const char *username,
                                       unsigned int username_len,
                                       const char *publickey,
                                       const char *privatekey,
                                       const char *passphrase,
                                       const char *hostname,
                                       unsigned int hostname_len,
                                       const char *local_username,
                                       unsigned int local_username_len);

#define libssh2_userauth_hostbased_fromfile(session, username, publickey, \
                                            privatekey, passphrase, hostname) \
 libssh2_userauth_hostbased_fromfile_ex((session), (username), \
                                        strlen(username), (publickey), \
                                        (privatekey), (passphrase), \
                                        (hostname), strlen(hostname), \
                                        (username), strlen(username))

/*
 * response_callback is provided with filled by library prompts array,
 * but client must allocate and fill individual responses. Responses
 * array is already allocated. Responses data will be freed by libssh2
 * after callback return, but before subsequent callback invokation.
 */
LIBSSH2_API int
libssh2_userauth_keyboard_interactive_ex(LIBSSH2_SESSION* session,
                                         const char *username,
                                         unsigned int username_len,
                                         LIBSSH2_USERAUTH_KBDINT_RESPONSE_FUNC((*response_callback)));

#define libssh2_userauth_keyboard_interactive(session, username, \
                                              response_callback) \
 libssh2_userauth_keyboard_interactive_ex((session), (username), \
                                          strlen(username), (response_callback))

LIBSSH2_API int libssh2_poll(LIBSSH2_POLLFD *fds, unsigned int nfds,
                             long timeout);

/* Channel API */
#define LIBSSH2_CHANNEL_WINDOW_DEFAULT  65536
#define LIBSSH2_CHANNEL_PACKET_DEFAULT  32768
#define LIBSSH2_CHANNEL_MINADJUST       1024

/* Extended Data Handling */
#define LIBSSH2_CHANNEL_EXTENDED_DATA_NORMAL        0
#define LIBSSH2_CHANNEL_EXTENDED_DATA_IGNORE        1
#define LIBSSH2_CHANNEL_EXTENDED_DATA_MERGE         2

#define SSH_EXTENDED_DATA_STDERR 1

/* Returned by any function that would block during a read/write opperation */
#define LIBSSH2CHANNEL_EAGAIN LIBSSH2_ERROR_EAGAIN

LIBSSH2_API LIBSSH2_CHANNEL *
libssh2_channel_open_ex(LIBSSH2_SESSION *session, const char *channel_type,
                        unsigned int channel_type_len,
                        unsigned int window_size, unsigned int packet_size,
                        const char *message, unsigned int message_len);

#define libssh2_channel_open_session(session) \
  libssh2_channel_open_ex((session), "session", sizeof("session") - 1, \
                          LIBSSH2_CHANNEL_WINDOW_DEFAULT, \
                          LIBSSH2_CHANNEL_PACKET_DEFAULT, NULL, 0)

LIBSSH2_API LIBSSH2_CHANNEL *
libssh2_channel_direct_tcpip_ex(LIBSSH2_SESSION *session, const char *host,
                                int port, const char *shost, int sport);
#define libssh2_channel_direct_tcpip(session, host, port) \
  libssh2_channel_direct_tcpip_ex((session), (host), (port), "127.0.0.1", 22)

LIBSSH2_API LIBSSH2_LISTENER *
libssh2_channel_forward_listen_ex(LIBSSH2_SESSION *session, const char *host,
                                  int port, int *bound_port, int queue_maxsize);
#define libssh2_channel_forward_listen(session, port) \
 libssh2_channel_forward_listen_ex((session), NULL, (port), NULL, 16)

LIBSSH2_API int libssh2_channel_forward_cancel(LIBSSH2_LISTENER *listener);

LIBSSH2_API LIBSSH2_CHANNEL *
libssh2_channel_forward_accept(LIBSSH2_LISTENER *listener);

LIBSSH2_API int libssh2_channel_setenv_ex(LIBSSH2_CHANNEL *channel,
                                          const char *varname,
                                          unsigned int varname_len,
                                          const char *value,
                                          unsigned int value_len);

#define libssh2_channel_setenv(channel, varname, value) \
 libssh2_channel_setenv_ex((channel), (varname), strlen(varname), (value), \
                           strlen(value))

LIBSSH2_API int libssh2_channel_request_pty_ex(LIBSSH2_CHANNEL *channel,
                                               const char *term,
                                               unsigned int term_len,
                                               const char *modes,
                                               unsigned int modes_len,
                                               int width, int height,
                                               int width_px, int height_px);
#define libssh2_channel_request_pty(channel, term) \
 libssh2_channel_request_pty_ex((channel), (term), strlen(term), NULL, 0, \
                                LIBSSH2_TERM_WIDTH, LIBSSH2_TERM_HEIGHT, \
                                LIBSSH2_TERM_WIDTH_PX, LIBSSH2_TERM_HEIGHT_PX)

LIBSSH2_API int libssh2_channel_request_pty_size_ex(LIBSSH2_CHANNEL *channel,
                                                    int width, int height,
                                                    int width_px,
                                                    int height_px);
#define libssh2_channel_request_pty_size(channel, width, height) \
  libssh2_channel_request_pty_size_ex( (channel), (width), (height), 0, 0)

LIBSSH2_API int libssh2_channel_x11_req_ex(LIBSSH2_CHANNEL *channel,
                                           int single_connection,
                                           const char *auth_proto,
                                           const char *auth_cookie,
                                           int screen_number);
#define libssh2_channel_x11_req(channel, screen_number) \
 libssh2_channel_x11_req_ex((channel), 0, NULL, NULL, (screen_number))

LIBSSH2_API int libssh2_channel_process_startup(LIBSSH2_CHANNEL *channel,
                                                const char *request,
                                                unsigned int request_len,
                                                const char *message,
                                                unsigned int message_len);
#define libssh2_channel_shell(channel) \
  libssh2_channel_process_startup((channel), "shell", sizeof("shell") - 1, \
                                  NULL, 0)
#define libssh2_channel_exec(channel, command) \
  libssh2_channel_process_startup((channel), "exec", sizeof("exec") - 1, \
                                  (command), strlen(command))
#define libssh2_channel_subsystem(channel, subsystem) \
  libssh2_channel_process_startup((channel), "subsystem",              \
                                  sizeof("subsystem") - 1, (subsystem), \
                                  strlen(subsystem))

LIBSSH2_API ssize_t libssh2_channel_read_ex(LIBSSH2_CHANNEL *channel,
                                            int stream_id, char *buf,
                                            size_t buflen);
#define libssh2_channel_read(channel, buf, buflen) \
  libssh2_channel_read_ex((channel), 0, (buf), (buflen))
#define libssh2_channel_read_stderr(channel, buf, buflen) \
  libssh2_channel_read_ex((channel), SSH_EXTENDED_DATA_STDERR, (buf), (buflen))

LIBSSH2_API int libssh2_poll_channel_read(LIBSSH2_CHANNEL *channel,
                                          int extended);

LIBSSH2_API unsigned long
libssh2_channel_window_read_ex(LIBSSH2_CHANNEL *channel,
                               unsigned long *read_avail,
                               unsigned long *window_size_initial);
#define libssh2_channel_window_read(channel) \
  libssh2_channel_window_read_ex((channel), NULL, NULL)

/* libssh2_channel_receive_window_adjust is DEPRECATED, do not use! */
LIBSSH2_API unsigned long
libssh2_channel_receive_window_adjust(LIBSSH2_CHANNEL *channel,
                                      unsigned long adjustment,
                                      unsigned char force);

LIBSSH2_API int
libssh2_channel_receive_window_adjust2(LIBSSH2_CHANNEL *channel,
                                       unsigned long adjustment,
                                       unsigned char force,
                                       unsigned int *storewindow);

LIBSSH2_API ssize_t libssh2_channel_write_ex(LIBSSH2_CHANNEL *channel,
                                             int stream_id, const char *buf,
                                             size_t buflen);

#define libssh2_channel_write(channel, buf, buflen) \
  libssh2_channel_write_ex((channel), 0, (buf), (buflen))
#define libssh2_channel_write_stderr(channel, buf, buflen)  \
  libssh2_channel_write_ex((channel), SSH_EXTENDED_DATA_STDERR, (buf), (buflen))

LIBSSH2_API unsigned long
libssh2_channel_window_write_ex(LIBSSH2_CHANNEL *channel,
                                unsigned long *window_size_initial);
#define libssh2_channel_window_write(channel) \
  libssh2_channel_window_write_ex((channel), NULL)

LIBSSH2_API void libssh2_session_set_blocking(LIBSSH2_SESSION* session,
                                              int blocking);
LIBSSH2_API int libssh2_session_get_blocking(LIBSSH2_SESSION* session);

LIBSSH2_API void libssh2_channel_set_blocking(LIBSSH2_CHANNEL *channel,
                                              int blocking);

/* libssh2_channel_handle_extended_data is DEPRECATED, do not use! */
LIBSSH2_API void libssh2_channel_handle_extended_data(LIBSSH2_CHANNEL *channel,
                                                      int ignore_mode);
LIBSSH2_API int libssh2_channel_handle_extended_data2(LIBSSH2_CHANNEL *channel,
                                                      int ignore_mode);

/* libssh2_channel_ignore_extended_data() is defined below for BC with version
 * 0.1
 *
 * Future uses should use libssh2_channel_handle_extended_data() directly if
 * LIBSSH2_CHANNEL_EXTENDED_DATA_MERGE is passed, extended data will be read
 * (FIFO) from the standard data channel
 */
/* DEPRECATED */
#define libssh2_channel_ignore_extended_data(channel, ignore) \
  libssh2_channel_handle_extended_data((channel),                       \
                                       (ignore) ?                       \
                                       LIBSSH2_CHANNEL_EXTENDED_DATA_IGNORE : \
                                       LIBSSH2_CHANNEL_EXTENDED_DATA_NORMAL )

#define LIBSSH2_CHANNEL_FLUSH_EXTENDED_DATA     -1
#define LIBSSH2_CHANNEL_FLUSH_ALL               -2
LIBSSH2_API int libssh2_channel_flush_ex(LIBSSH2_CHANNEL *channel,
                                         int streamid);
#define libssh2_channel_flush(channel) libssh2_channel_flush_ex((channel), 0)
#define libssh2_channel_flush_stderr(channel) \
 libssh2_channel_flush_ex((channel), SSH_EXTENDED_DATA_STDERR)

LIBSSH2_API int libssh2_channel_get_exit_status(LIBSSH2_CHANNEL* channel);
LIBSSH2_API int libssh2_channel_send_eof(LIBSSH2_CHANNEL *channel);
LIBSSH2_API int libssh2_channel_eof(LIBSSH2_CHANNEL *channel);
LIBSSH2_API int libssh2_channel_wait_eof(LIBSSH2_CHANNEL *channel);
LIBSSH2_API int libssh2_channel_close(LIBSSH2_CHANNEL *channel);
LIBSSH2_API int libssh2_channel_wait_closed(LIBSSH2_CHANNEL *channel);
LIBSSH2_API int libssh2_channel_free(LIBSSH2_CHANNEL *channel);

LIBSSH2_API LIBSSH2_CHANNEL *libssh2_scp_recv(LIBSSH2_SESSION *session,
                                              const char *path,
                                              struct stat *sb);
LIBSSH2_API LIBSSH2_CHANNEL *libssh2_scp_send_ex(LIBSSH2_SESSION *session,
                                                 const char *path, int mode,
                                                 size_t size, long mtime,
                                                 long atime);
#define libssh2_scp_send(session, path, mode, size) \
  libssh2_scp_send_ex((session), (path), (mode), (size), 0, 0)

LIBSSH2_API int libssh2_base64_decode(LIBSSH2_SESSION *session, char **dest,
                                      unsigned int *dest_len,
                                      const char *src, unsigned int src_len);

LIBSSH2_API
const char *libssh2_version(int req_version_num);

#define HAVE_LIBSSH2_KNOWNHOST_API 0x010101 /* since 1.1.1 */
#define HAVE_LIBSSH2_VERSION_API   0x010100 /* libssh2_version since 1.1 */

struct libssh2_knownhost {
    unsigned int magic;  /* magic stored by the library */
    void *node; /* handle to the internal representation of this host */
    char *name; /* this is NULL if no plain text host name exists */
    char *key;  /* key in base64/printable format */
    int typemask;
};

/*
 * libssh2_knownhost_init
 *
 * Init a collection of known hosts. Returns the pointer to a collection.
 *
 */
LIBSSH2_API LIBSSH2_KNOWNHOSTS *
libssh2_knownhost_init(LIBSSH2_SESSION *session);

/*
 * libssh2_knownhost_add
 *
 * Add a host and its associated key to the collection of known hosts.
 *
 * The 'type' argument specifies on what format the given host is:
 *
 * plain  - ascii "hostname.domain.tld"
 * sha1   - SHA1(<salt> <host>) base64-encoded!
 * custom - another hash
 *
 * If 'sha1' is selected as type, the salt must be provided to the salt
 * argument. This too base64 encoded.
 *
 * The SHA-1 hash is what OpenSSH can be told to use in known_hosts files.  If
 * a custom type is used, salt is ignored and you must provide the host
 * pre-hashed when checking for it in the libssh2_knownhost_check() function.
 *
 */

/* host format (2 bits) */
#define LIBSSH2_KNOWNHOST_TYPE_MASK    0xffff
#define LIBSSH2_KNOWNHOST_TYPE_PLAIN   1
#define LIBSSH2_KNOWNHOST_TYPE_SHA1    2 /* always base64 encoded */
#define LIBSSH2_KNOWNHOST_TYPE_CUSTOM  3

/* key format (2 bits) */
#define LIBSSH2_KNOWNHOST_KEYENC_MASK     (3<<16)
#define LIBSSH2_KNOWNHOST_KEYENC_RAW      (1<<16)
#define LIBSSH2_KNOWNHOST_KEYENC_BASE64   (2<<16)

/* type of key (2 bits) */
#define LIBSSH2_KNOWNHOST_KEY_MASK     (3<<18)
#define LIBSSH2_KNOWNHOST_KEY_SHIFT    18
#define LIBSSH2_KNOWNHOST_KEY_RSA1     (1<<18)
#define LIBSSH2_KNOWNHOST_KEY_SSHRSA   (2<<18)
#define LIBSSH2_KNOWNHOST_KEY_SSHDSS   (3<<18)

LIBSSH2_API int
libssh2_knownhost_add(LIBSSH2_KNOWNHOSTS *hosts,
                      const char *host,
                      const char *salt,
                      const char *key, size_t keylen, int typemask,
                      struct libssh2_knownhost **store);

/*
 * libssh2_knownhost_check
 *
 * Check a host and its associated key against the collection of known hosts.
 *
 * The type is the type/format of the given host name.
 *
 * plain  - ascii "hostname.domain.tld"
 * custom - prehashed base64 encoded. Note that this cannot use any salts.
 *
 *
 * 'knownhost' may be set to NULL if you don't care about that info.
 *
 * Returns:
 *
 * LIBSSH2_KNOWNHOST_CHECK_* values, see below
 *
 */

#define LIBSSH2_KNOWNHOST_CHECK_MATCH    0
#define LIBSSH2_KNOWNHOST_CHECK_MISMATCH 1
#define LIBSSH2_KNOWNHOST_CHECK_NOTFOUND 2
#define LIBSSH2_KNOWNHOST_CHECK_FAILURE  3

LIBSSH2_API int
libssh2_knownhost_check(LIBSSH2_KNOWNHOSTS *hosts,
                        const char *host, const char *key, size_t keylen,
                        int typemask,
                        struct libssh2_knownhost **knownhost);

/*
 * libssh2_knownhost_del
 *
 * Remove a host from the collection of known hosts. The 'entry' struct is
 * retrieved by a call to libssh2_knownhost_check().
 *
 */
LIBSSH2_API int
libssh2_knownhost_del(LIBSSH2_KNOWNHOSTS *hosts,
                      struct libssh2_knownhost *entry);

/*
 * libssh2_knownhost_free
 *
 * Free an entire collection of known hosts.
 *
 */
LIBSSH2_API void
libssh2_knownhost_free(LIBSSH2_KNOWNHOSTS *hosts);

/*
 * libssh2_knownhost_readline()
 *
 * Pass in a line of a file of 'type'. It makes libssh2 read this line.
 *
 * LIBSSH2_KNOWNHOST_FILE_OPENSSH is the only supported type.
 *
 */
LIBSSH2_API int
libssh2_knownhost_readline(LIBSSH2_KNOWNHOSTS *hosts,
                           const char *line, size_t len, int type);

/*
 * libssh2_knownhost_readfile
 *
 * Add hosts+key pairs from a given file.
 *
 * Returns a negative value for error or number of successfully added hosts.
 *
 * This implementation currently only knows one 'type' (openssh), all others
 * are reserved for future use.
 */

#define LIBSSH2_KNOWNHOST_FILE_OPENSSH 1

LIBSSH2_API int
libssh2_knownhost_readfile(LIBSSH2_KNOWNHOSTS *hosts,
                           const char *filename, int type);

/*
 * libssh2_knownhost_writeline()
 *
 * Ask libssh2 to convert a known host to an output line for storage.
 *
 * Note that this function returns LIBSSH2_ERROR_BUFFER_TOO_SMALL if the given
 * output buffer is too small to hold the desired output.
 *
 * This implementation currently only knows one 'type' (openssh), all others
 * are reserved for future use.
 *
 */
LIBSSH2_API int
libssh2_knownhost_writeline(LIBSSH2_KNOWNHOSTS *hosts,
                            struct libssh2_knownhost *known,
                            char *buffer, size_t buflen,
                            size_t *outlen, /* the amount of written data */
                            int type);

/*
 * libssh2_knownhost_writefile
 *
 * Write hosts+key pairs to a given file.
 *
 * This implementation currently only knows one 'type' (openssh), all others
 * are reserved for future use.
 */

LIBSSH2_API int
libssh2_knownhost_writefile(LIBSSH2_KNOWNHOSTS *hosts,
                            const char *filename, int type);

/*
 * libssh2_knownhost_get()
 *
 * Traverse the internal list of known hosts. Pass NULL to 'prev' to get
 * the first one. Or pass a poiner to the previously returned one to get the
 * next.
 *
 * Returns:
 * 0 if a fine host was stored in 'store'
 * 1 if end of hosts
 * [negative] on errors
 */
LIBSSH2_API int
libssh2_knownhost_get(LIBSSH2_KNOWNHOSTS *hosts,
                      struct libssh2_knownhost **store,
                      struct libssh2_knownhost *prev);

#define HAVE_LIBSSH2_AGENT_API 0x010202 /* since 1.2.2 */

struct libssh2_agent_publickey {
    unsigned int magic;              /* magic stored by the library */
    void *node;	    /* handle to the internal representation of key */
    unsigned char *blob;           /* public key blob */
    size_t blob_len;               /* length of the public key blob */
    char *comment;                 /* comment in printable format */
};

/*
 * libssh2_agent_init
 *
 * Init an ssh-agent handle. Returns the pointer to the handle.
 *
 */
LIBSSH2_API LIBSSH2_AGENT *
libssh2_agent_init(LIBSSH2_SESSION *session);

/*
 * libssh2_agent_connect()
 *
 * Connect to an ssh-agent.
 *
 * Returns 0 if succeeded, or a negative value for error.
 */
LIBSSH2_API int
libssh2_agent_connect(LIBSSH2_AGENT *agent);

/*
 * libssh2_agent_list_identities()
 *
 * Request an ssh-agent to list identities.
 *
 * Returns 0 if succeeded, or a negative value for error.
 */
LIBSSH2_API int
libssh2_agent_list_identities(LIBSSH2_AGENT *agent);

/*
 * libssh2_agent_get_identity()
 *
 * Traverse the internal list of public keys. Pass NULL to 'prev' to get
 * the first one. Or pass a poiner to the previously returned one to get the
 * next.
 *
 * Returns:
 * 0 if a fine public key was stored in 'store'
 * 1 if end of public keys
 * [negative] on errors
 */
LIBSSH2_API int
libssh2_agent_get_identity(LIBSSH2_AGENT *agent,
			   struct libssh2_agent_publickey **store,
			   struct libssh2_agent_publickey *prev);

/*
 * libssh2_agent_userauth()
 *
 * Do publickey user authentication with the help of ssh-agent.
 *
 * Returns 0 if succeeded, or a negative value for error.
 */
LIBSSH2_API int
libssh2_agent_userauth(LIBSSH2_AGENT *agent,
		       const char *username,
		       struct libssh2_agent_publickey *identity);

/*
 * libssh2_agent_disconnect()
 *
 * Close a connection to an ssh-agent.
 *
 * Returns 0 if succeeded, or a negative value for error.
 */
LIBSSH2_API int
libssh2_agent_disconnect(LIBSSH2_AGENT *agent);

/*
 * libssh2_agent_free()
 *
 * Free an ssh-agent handle.  This function also frees the internal
 * collection of public keys.
 */
LIBSSH2_API void
libssh2_agent_free(LIBSSH2_AGENT *agent);

/* NOTE NOTE NOTE
   libssh2_trace() has no function in builds that aren't built with debug
   enabled
 */
LIBSSH2_API int libssh2_trace(LIBSSH2_SESSION *session, int bitmask);
#define LIBSSH2_TRACE_TRANS (1<<1)
#define LIBSSH2_TRACE_KEX   (1<<2)
#define LIBSSH2_TRACE_AUTH  (1<<3)
#define LIBSSH2_TRACE_CONN  (1<<4)
#define LIBSSH2_TRACE_SCP   (1<<5)
#define LIBSSH2_TRACE_SFTP  (1<<6)
#define LIBSSH2_TRACE_ERROR (1<<7)
#define LIBSSH2_TRACE_PUBLICKEY (1<<8)
#define LIBSSH2_TRACE_SOCKET (1<<9)

typedef void (*libssh2_trace_handler_func)(LIBSSH2_SESSION*,
                                           void*,
                                           const char *,
                                           size_t);
LIBSSH2_API int libssh2_trace_sethandler(LIBSSH2_SESSION *session,
                                         void* context,
                                         libssh2_trace_handler_func callback);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LIBSSH2_H */
