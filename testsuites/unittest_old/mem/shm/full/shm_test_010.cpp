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
#include "it_test_shm.h"
#include "sys/types.h"

static int Testcase(void)
{
    const int memSize = 1024;
    int shmid;
    int ret;
    int status;

    ret = fork();
    if (ret == 0) {
        usleep(10000);
        shmid = shmget((key_t)1234, memSize, 0666 | IPC_CREAT);
        if (shmid < 0) {
            if (errno == EACCES) {
                exit(1);
            }
        }

        exit(0);
    } else {
        shmid = shmget((key_t)1234, memSize, 0000 | IPC_CREAT);
        ICUNIT_ASSERT_NOT_EQUAL(shmid, -1, shmid);
        usleep(20000);
        ret = shmctl(shmid, IPC_RMID, NULL);
        ICUNIT_ASSERT_NOT_EQUAL(ret, -1, ret);

        wait(&status);
        status = WEXITSTATUS(status);
        ICUNIT_ASSERT_EQUAL(status, 1, status);
    }

    return 0;
}

void ItTestShm010(void)
{
    TEST_ADD_CASE("IT_MEM_SHM_010", Testcase, TEST_LOS, TEST_MEM, TEST_LEVEL0, TEST_FUNCTION);
}
