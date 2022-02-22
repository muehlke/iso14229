#ifndef TEST_ISO14229_H
#define TEST_ISO14229_H

#include <assert.h>
#include <stdint.h>
#define ISO14229USERDEBUG printf
#include "iso14229.h"
#include "iso14229client.h"
#include "iso14229server.h"
#include "isotp-c/isotp.h"
#include "isotp-c/isotp_config.h"
#include "isotp-c/isotp_defines.h"

#define ASSERT_EQUAL(a, b)                                                                         \
    {                                                                                              \
        int _a = a;                                                                                \
        int _b = b;                                                                                \
        if ((_a) != (_b)) {                                                                        \
            printf("%s:%d (%d != %d)\n", __FILE__, __LINE__, _a, _b);                              \
            fflush(stdout);                                                                        \
            assert(0);                                                                             \
        }                                                                                          \
    }

#define ASSERT_MEMORY_EQUAL(a, b, len)                                                             \
    {                                                                                              \
        const uint8_t *_a = a;                                                                     \
        const uint8_t *_b = b;                                                                     \
        if (memcmp(_a, _b, len)) {                                                                 \
            printf("A:");                                                                          \
            for (int i = 0; i < len; i++) {                                                        \
                printf("%02x,", _a[i]);                                                            \
            }                                                                                      \
            printf(" (%s)\nB:", #a);                                                               \
            for (int i = 0; i < len; i++) {                                                        \
                printf("%02x,", _b[i]);                                                            \
            }                                                                                      \
            printf(" (%s)\n", #b);                                                                 \
            fflush(stdout);                                                                        \
            assert(0);                                                                             \
        }                                                                                          \
    }

#define SERVER_SEND_ID (0x1U)      /* server sends */
#define SERVER_PHYS_RECV_ID (0x2U) /* server listens for physically (1:1) addressed messages */
#define SERVER_FUNC_RECV_ID (0x4U) /* server listens for functionally (1:n) addressed messages */

#define CLIENT_SEND_ID SERVER_PHYS_RECV_ID /* client sends physically (1:1) by default */
#define CLIENT_RECV_ID SERVER_SEND_ID

// TODO: parameterize and fuzz this
#define ISOTP_BUFSIZE (2048U)

/**
 * @brief declare a client ISO-TP link on the stack
 */
#define CLIENT_ISOTP_LINK_DECLARE()                                                                \
    uint8_t clientIsotpSendBuf[ISOTP_BUFSIZE];                                                     \
    uint8_t clientIsotpRecvBuf[ISOTP_BUFSIZE];                                                     \
    IsoTpLink clientLink;

/**
 * @brief initialize a declared client ISO-TP link on the stack
 */
#define CLIENT_ISOTP_LINK_INIT()                                                                   \
    isotp_init_link(&clientLink, CLIENT_SEND_ID, clientIsotpSendBuf, sizeof(clientIsotpSendBuf),   \
                    clientIsotpRecvBuf, sizeof(clientIsotpRecvBuf), isotp_user_get_ms,             \
                    client_send_can, isotp_client_debug);

/**
 * @brief declare and initialize a client ISO-TP link on the stack
 */
#define CLIENT_ISOTP_LINK_SETUP()                                                                  \
    CLIENT_ISOTP_LINK_DECLARE();                                                                   \
    CLIENT_ISOTP_LINK_INIT();

/**
 * @brief declare an Iso14229Client and its ISO-TP link on the stack
 */
#define CLIENT_DECLARE()                                                                           \
    CLIENT_ISOTP_LINK_DECLARE();                                                                   \
    Iso14229Client client;                                                                         \
    Iso14229ClientConfig clientCfg = {                                                             \
        .link = &clientLink,                                                                       \
        .recv_id = CLIENT_RECV_ID,                                                                 \
        .send_id = CLIENT_SEND_ID,                                                                 \
        .tasks = NULL,                                                                             \
        .numtasks = 0,                                                                             \
        .userGetms = isotp_user_get_ms,                                                            \
    };

/**
 * @brief initialize a declared Iso14229Client and its ISO-TP link on the stack
 */
#define CLIENT_INIT()                                                                              \
    CLIENT_ISOTP_LINK_INIT();                                                                      \
    iso14229ClientInit(&client, &clientCfg);

/**
 * @brief declare and initialize an Iso14229Client and its ISO-TP link on the stack
 */
#define CLIENT_SETUP()                                                                             \
    CLIENT_DECLARE();                                                                              \
    CLIENT_INIT();

/**
 * @brief declare server ISO-TP links on the stack
 */
#define SERVER_ISOTP_LINK_DECLARE()                                                                \
    uint8_t serverIsotpPhysSendBuf[ISOTP_BUFSIZE];                                                 \
    uint8_t serverIsotpPhysRecvBuf[ISOTP_BUFSIZE];                                                 \
    uint8_t serverIsotpFuncSendBuf[ISOTP_BUFSIZE];                                                 \
    uint8_t serverIsotpFuncRecvBuf[ISOTP_BUFSIZE];                                                 \
    IsoTpLink serverPhysLink;                                                                      \
    IsoTpLink serverFuncLink;

/**
 * @brief initialize declared server ISO-TP links on the stack
 */
#define SERVER_ISOTP_LINK_INIT()                                                                   \
    isotp_init_link(&serverPhysLink, CLIENT_RECV_ID, serverIsotpPhysSendBuf,                       \
                    sizeof(serverIsotpPhysSendBuf), serverIsotpPhysRecvBuf,                        \
                    sizeof(serverIsotpPhysRecvBuf), isotp_user_get_ms, server_send_can,            \
                    isotp_server_phys_debug);                                                      \
    isotp_init_link(&serverFuncLink, CLIENT_RECV_ID, serverIsotpFuncSendBuf,                       \
                    sizeof(serverIsotpFuncSendBuf), serverIsotpFuncRecvBuf,                        \
                    sizeof(serverIsotpFuncRecvBuf), isotp_user_get_ms, server_send_can,            \
                    isotp_server_func_debug);

/**
 * @brief declare and initialize server ISO-TP links on the stack
 */
#define SERVER_ISOTP_LINK_SETUP()                                                                  \
    SERVER_ISOTP_LINK_DECLARE();                                                                   \
    SERVER_ISOTP_LINK_INIT();

/**
 * @brief declare an Iso14229Server and its ISO-TP links on the stack
 */
#define SERVER_DECLARE()                                                                           \
    SERVER_ISOTP_LINK_DECLARE();                                                                   \
    Iso14229Server server;                                                                         \
    uint8_t udsRecvBuf[ISOTP_BUFSIZE];                                                             \
    uint8_t udsSendBuf[ISOTP_BUFSIZE];                                                             \
    Iso14229ServerConfig serverCfg = {.phys_recv_id = SERVER_PHYS_RECV_ID,                         \
                                      .func_recv_id = SERVER_FUNC_RECV_ID,                         \
                                      .send_id = SERVER_SEND_ID,                                   \
                                      .phys_link = &serverPhysLink,                                \
                                      .func_link = &serverFuncLink,                                \
                                      .receive_buffer = udsRecvBuf,                                \
                                      .receive_buf_size = sizeof(udsRecvBuf),                      \
                                      .send_buffer = udsSendBuf,                                   \
                                      .send_buf_size = sizeof(udsSendBuf),                         \
                                      .userRDBIHandler = rdbiHandler,                              \
                                      .userGetms = isotp_user_get_ms,                              \
                                      .p2_ms = 50,                                                 \
                                      .p2_star_ms = 2000,                                          \
                                      .s3_ms = 5000,                                               \
                                      .middleware = NULL};

/**
 * @brief initialize an Iso14229Server on the stack
 */
#define SERVER_INIT()                                                                              \
    SERVER_ISOTP_LINK_INIT();                                                                      \
    Iso14229ServerInit(&server, &serverCfg);

/**
 * @brief declare and initialize an Iso14229Server on the stack
 */
#define SERVER_SETUP()                                                                             \
    SERVER_DECLARE();                                                                              \
    SERVER_INIT();

#define TEST_SETUP()                                                                               \
    printf("%s", __PRETTY_FUNCTION__);                                                             \
    g_ms = 0;                                                                                      \
    g_serverRecvQueueIdx = 0;                                                                      \
    g_clientRecvQueueIdx = 0;                                                                      \
    memset(&g_serverSvcCallCount, 0, sizeof(g_serverSvcCallCount));                                \
    memset(&g_serverServices, 0, sizeof(g_serverServices));

#define TEST_TEARDOWN() printf("OK\n");

/**
 * @brief Setup everything for server test
 */
#define SERVER_TEST_SETUP()                                                                        \
    TEST_SETUP();                                                                                  \
    SERVER_SETUP();                                                                                \
    CLIENT_ISOTP_LINK_SETUP();

/**
 * @brief Setup everything for client test
 */
#define CLIENT_TEST_SETUP()                                                                        \
    TEST_SETUP();                                                                                  \
    CLIENT_SETUP();                                                                                \
    SERVER_ISOTP_LINK_SETUP();

/**
 * @brief Setup everything for client and server test
 */
#define CLIENT_SERVER_TEST_SETUP()                                                                 \
    TEST_SETUP();                                                                                  \
    CLIENT_SETUP();                                                                                \
    SERVER_SETUP();

struct CANMessage {
    uint32_t arbId;
    uint8_t data[8];
    uint8_t size;
};

#endif