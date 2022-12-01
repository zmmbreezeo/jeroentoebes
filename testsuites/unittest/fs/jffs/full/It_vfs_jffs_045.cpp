/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#include "It_vfs_jffs.h"

static UINT32 Testcase(VOID)
{
    INT32 fd = -1;
    INT32 fd1 = -1;
    INT32 fd2 = -1;
    INT32 ret, len;
    CHAR filebuf[20] = "1234567890";
    CHAR readbuf[JFFS_STANDARD_NAME_LENGTH] = { 0 };
    CHAR pathname[JFFS_STANDARD_NAME_LENGTH] = { JFFS_PATH_NAME0 };
    off_t off;

    fd = open(pathname, O_CREAT | O_RDWR | O_EXCL, HIGHEST_AUTHORITY);
    ICUNIT_GOTO_NOT_EQUAL(fd, -1, fd, EXIT1);

    fd2 = open(pathname, O_CREAT | O_RDWR | O_EXCL, HIGHEST_AUTHORITY);
    ICUNIT_GOTO_EQUAL(fd2, -1, fd2, EXIT2);

    len = write(fd, filebuf, strlen(filebuf));
    ICUNIT_GOTO_EQUAL(len, JFFS_SHORT_ARRAY_LENGTH, len, EXIT2);

    fd1 = open(pathname, O_APPEND | O_RDWR, HIGHEST_AUTHORITY);
    ICUNIT_GOTO_NOT_EQUAL(fd1, -1, fd1, EXIT3);

    len = read(fd1, readbuf, JFFS_STANDARD_NAME_LENGTH);
    ICUNIT_GOTO_EQUAL(len, 0, len, EXIT3);

    len = write(fd1, "abcde", 6); // 6 means write len
    ICUNIT_GOTO_EQUAL(len, 6, len, EXIT3);

    off = lseek(fd1, 0, SEEK_SET);
    ICUNIT_GOTO_NOT_EQUAL(off, -1, off, EXIT3);

    len = read(fd1, readbuf, JFFS_STANDARD_NAME_LENGTH);
    ICUNIT_GOTO_EQUAL(len, 16, len, EXIT3); // 16 means path name len
    ICUNIT_GOTO_STRING_EQUAL(readbuf, "1234567890abcde", readbuf, EXIT3);

    ret = close(fd);
    ICUNIT_GOTO_EQUAL(ret, JFFS_NO_ERROR, ret, EXIT3);

    ret = close(fd1);
    ICUNIT_ASSERT_EQUAL(ret, JFFS_NO_ERROR, ret);

    ret = unlink(pathname);
    ICUNIT_ASSERT_EQUAL(ret, JFFS_NO_ERROR, ret);
#if 1
    close(fd1);
#endif
    return JFFS_NO_ERROR;
EXIT3:
    close(fd1);
EXIT2:
    close(fd2);
EXIT1:
    close(fd);
EXIT:
    remove(JFFS_PATH_NAME0);
    return JFFS_NO_ERROR;
}

VOID ItFsJffs045(VOID)
{
    TEST_ADD_CASE("IT_FS_JFFS_045", Testcase, TEST_VFS, TEST_JFFS, TEST_LEVEL2, TEST_FUNCTION);
}

