/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "It_vfs_fat.h"

static VOID *PthreadF01(void *arg)
{
    INT32 i, j, ret, len;
    INT32 flag = 0;
    INT32 fd = -1;
    CHAR pathname[FAT_STANDARD_NAME_LENGTH] = "";
    CHAR readbuf[FAT_STANDARD_NAME_LENGTH] = "";
    CHAR filebuf[260] = "01234567890123456789abcedfghij9876543210abcdeabcde0123456789abcedfghij9876543210abcdeabcde0123"
        "456789abcedfghij9876543210abcdeabcde0123456789abcedfghij9876543210abcdeabcde0123456789abcedfgh"
        "ij9876543210abcdeabcde0123456789abcedfghij9876543210lalalalalalalala";
    CHAR *bufWrite = nullptr;
    CHAR *bufWrite1 = nullptr;
    CHAR *bufWrite2 = nullptr;
    CHAR buffile[FAT_NAME_LIMITTED_SIZE] = FAT_PATH_NAME0;
    struct stat buf1 = { 0 };
    struct stat buf2 = { 0 };

    printf("pthread_f01 is excecuting\n");
    bufWrite = (CHAR *)malloc(4 * BYTES_PER_MBYTES + 1); // 4MB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite, NULL, 0, EXIT1);
    (void)memset_s(bufWrite, BYTES_PER_MBYTES + 1, 0, 4 * BYTES_PER_MBYTES + 1); // 4mb

    bufWrite1 = (CHAR *)malloc(BYTES_PER_MBYTES + 1); // BYTES_PER_MBYTES = 1MB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite1, NULL, 0, EXIT2);
    (void)memset_s(bufWrite1, BYTES_PER_MBYTES + 1, 0, BYTES_PER_MBYTES + 1); // BYTES_PER_MBYTES = 1MB

    bufWrite2 = (CHAR *)malloc(16 * BYTES_PER_KBYTES + 1); // 16 * BYTES_PER_KBYTES = 16KB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite2, NULL, 0, EXIT3);
    (void)memset_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, 0, 16 * BYTES_PER_KBYTES + 1); // 16 kb

    for (j = 0; j < 16; j++) {                                         // loop 16 times
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
    }

    for (j = 0; j < 16; j++) { // loop 16  times
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
    }

    for (i = 0; i < 2; i++) { // loop 2 times
        (void)strcat_s(bufWrite, BYTES_PER_MBYTES, bufWrite1);
        (void)strcat_s(bufWrite, BYTES_PER_MBYTES, bufWrite1);
    }

    free(bufWrite1);
    free(bufWrite2);

    ret = stat(buffile, &buf1);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);
    FatStatPrintf(buf1);

    (void)snprintf_s(pathname, FAT_STANDARD_NAME_LENGTH, FAT_STANDARD_NAME_LENGTH, "/vs/sd/test0/file0.txt");
    fd = open(pathname, O_NONBLOCK | O_CREAT | O_RDWR | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO);

    for (j = 0; j < (8 * BYTES_PER_KBYTES / 8); j++) { // 8 kb
        len = write(fd, bufWrite, strlen(bufWrite));
        if (len <= 0) {
            flag = 1;
            break;
        }
    }

    LosTaskDelay(3); // delay 3 s
    printf("pthread_f01 is excecuting\n");

    ret = lseek(fd, 0, SEEK_SET);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    len = read(fd, readbuf, 10);            // read 10 bytes
    ICUNIT_GOTO_EQUAL(len, 10, len, EXIT1); // len must be 10
    ICUNIT_ASSERT_STRING_EQUAL_RET(readbuf, "0123456789", readbuf, NULL);

    ret = close(fd);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = stat(buffile, &buf2);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);
    FatStatPrintf(buf2);

    ret = unlink(pathname);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);

    g_testCount++;

    free(bufWrite);

    printf("pthread_f01 exiting\n");
    return NULL;
EXIT3:
    free(bufWrite1);
EXIT2:
    free(bufWrite);
EXIT1:
    close(fd);
EXIT:
    unlink(pathname);
    g_testCount = 0;
    free(bufWrite);
    return NULL;
}

static VOID *PthreadF02(void *arg)
{
    INT32 i, j, ret, len;
    INT32 flag = 0;
    INT32 fd = -1;
    CHAR pathname[FAT_STANDARD_NAME_LENGTH] = "";
    CHAR readbuf[FAT_STANDARD_NAME_LENGTH] = "";
    CHAR filebuf[260] = "01234567890123456789abcedfghij9876543210abcdeabcde0123456789abcedfghij9876543210abcdeabcde0123"
        "456789abcedfghij9876543210abcdeabcde0123456789abcedfghij9876543210abcdeabcde0123456789abcedfgh"
        "ij9876543210abcdeabcde0123456789abcedfghij9876543210lalalalalalalala";
    CHAR *bufWrite = nullptr;
    CHAR *bufWrite1 = nullptr;
    CHAR *bufWrite2 = nullptr;
    CHAR buffile[FAT_NAME_LIMITTED_SIZE] = FAT_PATH_NAME0;
    struct stat buf1 = { 0 };
    struct stat buf2 = { 0 };

    printf("pthread_f02 is excecuting\n");
    bufWrite = (CHAR *)malloc(4 * BYTES_PER_MBYTES + 1); // 4MB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite, NULL, 0, EXIT1);
    (void)memset_s(bufWrite, BYTES_PER_MBYTES + 1, 0, 4 * BYTES_PER_MBYTES + 1); // 4mb

    bufWrite1 = (CHAR *)malloc(BYTES_PER_MBYTES + 1); // BYTES_PER_MBYTES = 1MB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite1, NULL, 0, EXIT2);
    (void)memset_s(bufWrite1, BYTES_PER_MBYTES + 1, 0, BYTES_PER_MBYTES + 1); // BYTES_PER_MBYTES = 1MB

    bufWrite2 = (CHAR *)malloc(16 * BYTES_PER_KBYTES + 1); // 16 * BYTES_PER_KBYTES = 16KB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite2, NULL, 0, EXIT3);
    (void)memset_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, 0, 16 * BYTES_PER_KBYTES + 1); // 16 kb

    for (j = 0; j < 16; j++) {                                         // loop 16 times
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
    }

    for (j = 0; j < 16; j++) { // loop 16  times
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
    }

    for (i = 0; i < 2; i++) { // loop 2 times
        (void)strcat_s(bufWrite, BYTES_PER_MBYTES, bufWrite1);
        (void)strcat_s(bufWrite, BYTES_PER_MBYTES, bufWrite1);
    }

    free(bufWrite1);
    free(bufWrite2);

    ret = stat(buffile, &buf1);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);
    FatStatPrintf(buf1);

    (void)snprintf_s(pathname, FAT_STANDARD_NAME_LENGTH, FAT_STANDARD_NAME_LENGTH, "/vs/sd/test0/file1.txt");
    fd = open(pathname, O_NONBLOCK | O_CREAT | O_RDWR | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO);

    for (j = 0; j < (8 * BYTES_PER_KBYTES / 8); j++) { // 8 kb
        len = write(fd, bufWrite, strlen(bufWrite));
        if (len <= 0) {
            flag = 1;
            break;
        }
    }

    LosTaskDelay(3); // delay 3 s
    printf("pthread_f02 is excecuting\n");

    ret = lseek(fd, 0, SEEK_SET);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    len = read(fd, readbuf, 10);            // read 10 bytes
    ICUNIT_GOTO_EQUAL(len, 10, len, EXIT1); // len must be 10
    ICUNIT_ASSERT_STRING_EQUAL_RET(readbuf, "0123456789", readbuf, NULL);

    ret = close(fd);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = stat(buffile, &buf2);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);
    FatStatPrintf(buf2);

    ret = unlink(pathname);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);

    g_testCount++;

    free(bufWrite);

    printf("pthread_f02 exiting\n");
    return NULL;
EXIT3:
    free(bufWrite1);
EXIT2:
    free(bufWrite);
EXIT1:
    close(fd);
EXIT:
    unlink(pathname);
    g_testCount = 0;
    free(bufWrite);
    return NULL;
}

static VOID *PthreadF03(void *arg)
{
    INT32 i, j, ret, len;
    INT32 flag = 0;
    INT32 fd = -1;
    CHAR pathname[FAT_STANDARD_NAME_LENGTH] = "";
    CHAR readbuf[FAT_STANDARD_NAME_LENGTH] = "";
    CHAR filebuf[260] = "01234567890123456789abcedfghij9876543210abcdeabcde0123456789abcedfghij9876543210abcdeabcde0123"
        "456789abcedfghij9876543210abcdeabcde0123456789abcedfghij9876543210abcdeabcde0123456789abcedfgh"
        "ij9876543210abcdeabcde0123456789abcedfghij9876543210lalalalalalalala";
    CHAR *bufWrite = nullptr;
    CHAR *bufWrite1 = nullptr;
    CHAR *bufWrite2 = nullptr;
    CHAR buffile[FAT_NAME_LIMITTED_SIZE] = FAT_PATH_NAME0;
    struct stat buf1 = { 0 };
    struct stat buf2 = { 0 };

    printf("pthread_f03 is excecuting\n");
    bufWrite = (CHAR *)malloc(4 * BYTES_PER_MBYTES + 1); // 4MB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite, NULL, 0, EXIT1);
    (void)memset_s(bufWrite, BYTES_PER_MBYTES + 1, 0, 4 * BYTES_PER_MBYTES + 1); // 4mb

    bufWrite1 = (CHAR *)malloc(BYTES_PER_MBYTES + 1); // BYTES_PER_MBYTES = 1MB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite1, NULL, 0, EXIT2);
    (void)memset_s(bufWrite1, BYTES_PER_MBYTES + 1, 0, BYTES_PER_MBYTES + 1); // BYTES_PER_MBYTES = 1MB

    bufWrite2 = (CHAR *)malloc(16 * BYTES_PER_KBYTES + 1); // 16 * BYTES_PER_KBYTES = 16KB
    ICUNIT_GOTO_NOT_EQUAL(bufWrite2, NULL, 0, EXIT3);
    (void)memset_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, 0, 16 * BYTES_PER_KBYTES + 1); // 16 kb

    for (j = 0; j < 16; j++) {                                         // loop 16 times
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
        (void)strcat_s(bufWrite2, 16 * BYTES_PER_KBYTES + 1, filebuf); // 16 kb
    }

    for (j = 0; j < 16; j++) { // loop 16  times
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
        (void)strcat_s(bufWrite1, BYTES_PER_MBYTES + 1, bufWrite2);
    }

    for (i = 0; i < 2; i++) { // loop 2 times
        (void)strcat_s(bufWrite, BYTES_PER_MBYTES, bufWrite1);
        (void)strcat_s(bufWrite, BYTES_PER_MBYTES, bufWrite1);
    }

    free(bufWrite1);
    free(bufWrite2);

    ret = stat(buffile, &buf1);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);
    FatStatPrintf(buf1);

    (void)snprintf_s(pathname, FAT_STANDARD_NAME_LENGTH, FAT_STANDARD_NAME_LENGTH, "/vs/sd/test0/file2.txt");
    fd = open(pathname, O_NONBLOCK | O_CREAT | O_RDWR | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO);
    for (j = 0; j < (8 * BYTES_PER_KBYTES / 8); j++) { // 8 kb
        len = write(fd, bufWrite, strlen(bufWrite));
        if (len <= 0) {
            flag = 1;
            break;
        }
    }

    LosTaskDelay(3); // delay 3 s
    printf("pthread_f03 is excecuting\n");

    ret = lseek(fd, 0, SEEK_SET);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    len = read(fd, readbuf, 10);            // read 10 bytes
    ICUNIT_GOTO_EQUAL(len, 10, len, EXIT1); // len must be 10
    ICUNIT_ASSERT_STRING_EQUAL_RET(readbuf, "0123456789", readbuf, NULL);

    ret = close(fd);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = stat(buffile, &buf2);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);
    FatStatPrintf(buf2);

    ret = unlink(pathname);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);

    g_testCount++;

    free(bufWrite);

    printf("pthread_f03 exiting\n");
    return NULL;
EXIT3:
    free(bufWrite1);
EXIT2:
    free(bufWrite);
EXIT1:
    close(fd);
EXIT:
    unlink(pathname);
    g_testCount = 0;
    free(bufWrite);
    return NULL;
}

static UINT32 TestCase(VOID)
{
    INT32 ret, i;
    CHAR bufname1[FAT_STANDARD_NAME_LENGTH] = FAT_PATH_NAME0;
    pthread_attr_t attr[FAT_MAX_THREADS];
    pthread_t threadId[FAT_MAX_THREADS];

    g_testCount = 0;

    ret = mkdir(bufname1, S_IRWXU | S_IRWXG | S_IRWXO);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);
    ret = PosixPthreadInit(&attr[0], TASK_PRIO_TEST - 2); // 2 less than TASK_PRIO_TEST
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = pthread_create(&threadId[0], &attr[0], PthreadF01, (void *)0);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = PosixPthreadInit(&attr[1], TASK_PRIO_TEST - 2); // 2 less than TASK_PRIO_TEST
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = pthread_create(&threadId[1], &attr[1], PthreadF02, (void *)1);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = PosixPthreadInit(&attr[2], TASK_PRIO_TEST - 2); // 2 less than TASK_PRIO_TEST
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ret = pthread_create(&threadId[2], &attr[2], PthreadF03, (void *)2); // the no 2 thread
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);
    for (i = 0; i < FAT_MAX_THREADS; i++) {
        ret = pthread_join(threadId[i], NULL);
        ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);
    }

    for (i = 0; i < FAT_MAX_THREADS; i++) {
        ret = pthread_attr_destroy(&attr[i]);
        ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);
    }

    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT1);

    ICUNIT_ASSERT_EQUAL(g_testCount, 3, g_testCount); // there 3 threads

    ret = rmdir(bufname1);
    ICUNIT_GOTO_EQUAL(ret, FAT_NO_ERROR, ret, EXIT);

    return FAT_NO_ERROR;

EXIT1:
    for (i = 0; i < FAT_MAX_THREADS; i++) {
        pthread_join(threadId[i], NULL);
    }
    for (i = 0; i < FAT_MAX_THREADS; i++) {
        pthread_attr_destroy(&attr[i]);
    }
EXIT:
    rmdir(bufname1);

    return FAT_NO_ERROR;
}

/* *
* - @test IT_FS_FAT_MUTIPTHREAD_005
* - @tspec function test
* - @ttitle multithreading reads the same file at the same time
* - @tbrief
1. Create a file to open
2. Create three threads to simultaneously read this file.
3. Delete all files and threads;
* - @ tprior 1
* - @ tauto TRUE
* - @ tremark
*/

VOID ItFsFatMutipthread005(VOID)
{
    TEST_ADD_CASE("IT_FS_FAT_MUTIPTHREAD_005", TestCase, TEST_VFS, TEST_VFAT, TEST_LEVEL4, TEST_PRESSURE);
}
