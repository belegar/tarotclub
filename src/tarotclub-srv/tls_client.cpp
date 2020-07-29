/*
 *  SSL client demonstration program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#include <stdlib.h>
#define mbedtls_time            time
#define mbedtls_time_t          time_t
#define mbedtls_fprintf         fprintf
#define TLogError          printf
#define mbedtls_exit            exit
#define MBEDTLS_EXIT_SUCCESS    EXIT_SUCCESS
#define MBEDTLS_EXIT_FAILURE    EXIT_FAILURE
#endif /* MBEDTLS_PLATFORM_C */


#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include <string.h>

#include "tls_client.h"
#include "Log.h"

#define SERVER_PORT "443"

#define DEBUG_LEVEL 0


static void my_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
    ((void) level);
    ((void) ctx);
    ((void) file);
    ((void) line);
    ((void) str);

//    mbedtls_fprintf( (FILE *) ctx, "%s:%04d: %s", file, line, str );
//    fflush(  (FILE *) ctx  );
   // TLogError("%s:%04d: %s", file, line, str);
}



/*
 * Test recv/send functions that make sure each try returns
 * WANT_READ/WANT_WRITE at least once before sucesseding
 */

static int delayed_recv( void *ctx, unsigned char *buf, size_t len )
{
    static int first_try = 1;
    int ret;

    if( first_try )
    {
        first_try = 0;
        return( MBEDTLS_ERR_SSL_WANT_READ );
    }

    ret = mbedtls_net_recv( ctx, buf, len );
    if( ret != MBEDTLS_ERR_SSL_WANT_READ )
        first_try = 1; /* Next call will be a new operation */
    return( ret );
}

//static int delayed_send( void *ctx, const unsigned char *buf, size_t len )
//{
//    static int first_try = 1;
//    int ret;

//    if( first_try )
//    {
//        first_try = 0;
//        return( MBEDTLS_ERR_SSL_WANT_WRITE );
//    }

//    ret = mbedtls_net_send( ctx, buf, len );
//    if( ret != MBEDTLS_ERR_SSL_WANT_WRITE )
//        first_try = 1; /* Next call will be a new operation */
//    return( ret );
//}

static int recv_cb( void *ctx, unsigned char *buf, size_t len )
{
    io_ctx_t *io_ctx = (io_ctx_t*) ctx;
    size_t recv_len;
    int ret;

    ret = delayed_recv( io_ctx->net, buf, len );

    if( ret < 0 )
        return( ret );
    recv_len = (size_t) ret;

    return( (int) recv_len );
}

static int recv_timeout_cb( void *ctx, unsigned char *buf, size_t len,
                            uint32_t timeout )
{
    io_ctx_t *io_ctx = (io_ctx_t*) ctx;
    int ret;
    size_t recv_len;

    ret = mbedtls_net_recv_timeout( io_ctx->net, buf, len, timeout );
    if( ret < 0 )
        return( ret );
    recv_len = (size_t) ret;

    return( (int) recv_len );
}

static int send_cb( void *ctx, unsigned char const *buf, size_t len )
{
    io_ctx_t *io_ctx = (io_ctx_t*) ctx;

    //return( delayed_send( io_ctx->net, buf, len ) );

    return( mbedtls_net_send( io_ctx->net, buf, len ) );
}


bool SimpleTlsClient::Connect(const char *server_name)
{
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;

    uint32_t flags;
    const char *pers = "ssl_client1";

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( DEBUG_LEVEL );
#endif

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_net_init( &server_fd );
    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
    mbedtls_x509_crt_init( &cacert );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    mbedtls_entropy_init( &entropy );
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        TLogError( " failed\n  ! mbedtls_ctr_drbg_seed returned " + std::to_string(ret) );
        goto exit;
    }

    /*
     * 0. Initialize certificates
     */
    ret = mbedtls_x509_crt_parse( &cacert, (const unsigned char *) mbedtls_test_cas_pem,
                          mbedtls_test_cas_pem_len );
    if( ret < 0 )
    {
        TLogError( " failed\n  !  mbedtls_x509_crt_parse returned " + std::to_string(-ret) );
        goto exit;
    }

    /*
     * 1. Start the connection
     */
//    TLogError( "  . Connecting to tcp/%s/%s...", server_name, SERVER_PORT );


    if( ( ret = mbedtls_net_connect( &server_fd, server_name,
                                         SERVER_PORT, MBEDTLS_NET_PROTO_TCP ) ) != 0 )
    {
        TLogError( " failed\n  ! mbedtls_net_connect returned " + std::to_string(ret) );
        goto exit;
    }

    /*
     * 2. Setup stuff
     */

    if( ( ret = mbedtls_ssl_config_defaults( &conf,
                    MBEDTLS_SSL_IS_CLIENT,
                    MBEDTLS_SSL_TRANSPORT_STREAM,
                    MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        TLogError( " failed\n  ! mbedtls_ssl_config_defaults returned " + std::to_string(ret) );
        goto exit;
    }

    /* OPTIONAL is not optimal for security,
     * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_ca_chain( &conf, &cacert, NULL );
    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
    mbedtls_ssl_conf_dbg( &conf, my_debug, stdout );

    if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
    {
        TLogError( " failed\n  ! mbedtls_ssl_setup returned " + std::to_string(ret) );
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl, server_name ) ) != 0 )
    {
        TLogError( " failed\n  ! mbedtls_ssl_set_hostname returned " + std::to_string(ret) );
        goto exit;
    }

//    mbedtls_ssl_set_bio( &ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL );

    io_ctx.ssl = &ssl;
    io_ctx.net = &server_fd;
    mbedtls_ssl_set_bio( &ssl, &io_ctx, send_cb, recv_cb, recv_timeout_cb);
    mbedtls_ssl_conf_read_timeout(&conf, 5000);

    /*
     * 4. Handshake
     */

    while( ( ret = mbedtls_ssl_handshake( &ssl ) ) != 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            TLogError( " failed\n  ! mbedtls_ssl_handshake returned " + std::to_string(-ret) );
            goto exit;
        }
    }

    /*
     * 5. Verify the server certificate
     */

    /* In real life, we probably want to bail out when ret != 0 */
    if( ( flags = mbedtls_ssl_get_verify_result( &ssl ) ) != 0 )
    {
        char vrfy_buf[512];

//        TLogError("Server certificate verification failed" );

        mbedtls_x509_crt_verify_info( vrfy_buf, sizeof( vrfy_buf ), "  ! ", flags );

//        TLogError( "%s\n", vrfy_buf );
    }

  //  printf("%s", read_buf->data);

  //  mbedtls_ssl_close_notify( &ssl );

    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:
#ifdef MBEDTLS_ERROR_C
    if( exit_code != MBEDTLS_EXIT_SUCCESS )
    {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        TLogError("Last error was: "  + std::to_string(ret) + " - " + std::string(error_buf) );
        Close();
    }
#endif

    return (exit_code == MBEDTLS_EXIT_SUCCESS);
}

void SimpleTlsClient::WaitData(read_buff_t *read_buf)
{
    int ret = 1, len;

    /*
     * 7. Read the HTTP response
     */
    read_buf->size = 0;
    len = sizeof( read_buf->data ) - 1;
    memset( read_buf->data, 0, sizeof( read_buf->data ) );
    do
    {
        ret = mbedtls_ssl_read( &ssl, &read_buf->data[read_buf->size], len );

        if (ret > 0)
        {
            len -= ret;
            read_buf->size += ret;
        }

        if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )
            continue;

     //   if( ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY )
     //   {
          //  read_buf->size = -1;
     //       break;
      //  }

        if( ret < 0 )
        {
         //   read_buf->size = -1;
//            TLogError( "failed\n  ! mbedtls_ssl_read returned %d", ret );
            break;
        }

        if( ret == 0 )
        {
            break;
        }
    }
    while( 1 );
}

bool SimpleTlsClient::Request(const uint8_t *data, uint32_t size, read_buff_t *read_buf)
{
    int ret = 1;
    bool success = true;
    read_buf->size = -1;

    /*
     * 3. Write the request
     */
//    TLogError( "  > Write to server:" );

    while( ( ret = mbedtls_ssl_write( &ssl, data, size ) ) <= 0 )
    {
        if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
        {
            TLogError( " failed\n  ! mbedtls_ssl_write returned " + std::to_string(ret) );
            success = false;
        }
    }

    if (success)
    {
        //    TLogError( " %d bytes written.", len);
        WaitData(read_buf);

    }
    return success;
}

void SimpleTlsClient::Close()
{
    mbedtls_net_free( &server_fd );
    mbedtls_x509_crt_free( &cacert );
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );
}

bool SimpleTlsClient::Read(read_buff_t *read_buf)
{
    int ret = 1, len;
    bool disconnected = false;

    /*
     * 7. Read the HTTP response
     */
    read_buf->size = 0;
    len = sizeof( read_buf->data ) - 1;
    memset( read_buf->data, 0, sizeof( read_buf->data ) );

    ret = mbedtls_ssl_read( &ssl, &read_buf->data[read_buf->size], len );

    if (ret > 0)
    {
        len -= ret;
        read_buf->size += ret;
    }

    if( ret == 0 )
    {
        disconnected = true;
    }
    return disconnected;
}
