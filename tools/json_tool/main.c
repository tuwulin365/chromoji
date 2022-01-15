#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cJSON.h"

#define JSON_FILE_PATH  "emoji_pretty.json"
#define JSON_FILE_SIZE  (4*1024*1024)
#define OUT_FILE_PATH  "emoji_pretty.txt"
#define OUT_FILE_SIZE   (10*1024*1024)
#define IMG_FILE_WIDTH 3960    //正方形
#define EMOJI_WIDTH     64

#define BUILD_HALFBYTE(hh, hl, lh, ll)  ((cJSON_IsTrue(hh)?8:0) | \
                                         (cJSON_IsTrue(hl)?4:0) | \
                                         (cJSON_IsTrue(lh)?2:0) | \
                                         (cJSON_IsTrue(ll)?1:0))


static int iEmojiTotal = 0;

int StringToLower(char *pOut, const char *pIn)
{
    while (*pIn)
    {
        *pOut = tolower(*pIn);
        pOut++;
        pIn++;
    }
    *pOut = '\0';

    return 0;
}

//eg: "1234-5678" -> "\u1234\u5678"
//eg: "1F170" -> "\uD83C\uDD70"
static int FormatUnicodeString(char *pOut, const char *pIn)
{
    unsigned int uzNum[10] = {0};
    int iNumCnt;
    int iRet;

    if (!pOut || !pIn)
    {
        return -1;
    }

    iNumCnt = sscanf(pIn, "%x-%x-%x-%x-%x-%x-%x-%x-%x-%x", uzNum, uzNum+1, uzNum+2, uzNum+3, uzNum+4, uzNum+5, uzNum+6, uzNum+7, uzNum+8, uzNum+9);

    iRet = 0;
    for (int i = 0; i < iNumCnt; i++)
    {
        if (uzNum[i] > 0xFFFF)
        {
            unsigned int uHighPart,uLowPart;
            uHighPart = ((uzNum[i]-0x10000)>>10) + 0xD800;
            uLowPart = ((uzNum[i]-0x10000) & 0x3FF) + 0xDC00;

            iRet += sprintf(pOut + iRet, "\\u%04X\\u%04X", uHighPart, uLowPart);
        }
        else
        {
            iRet += sprintf(pOut + iRet, "\\u%04X", uzNum[i]);
        }

    }
    pOut[iRet] = '\0';

    return 0;
}

static int InsertEmojiData(char *pOut, cJSON *pJson)
{
    int iRet;
    cJSON *pJsonUnified = NULL;
    cJSON *pJSonNonQualified = NULL;
    cJSON *pJsonName = NULL;
    cJSON *pJsonX = NULL;
    cJSON *pJsonY = NULL;
    cJSON *pJsonApple = NULL;
    cJSON *pJsonGoogle = NULL;
    cJSON *pJsonTwitter = NULL;
    cJSON *pJsonFacebook = NULL;
    char czStringUnified[128] = {0};
    char czStringNonQualified[128] = {0};
    char czStringKey[128] = {0};

    if (!pOut || !pJson)
    {
        return -1;
    }

    iEmojiTotal++;

    pJsonUnified = cJSON_GetObjectItem(pJson, "unified");
    pJSonNonQualified = cJSON_GetObjectItem(pJson, "non_qualified");
    pJsonName = cJSON_GetObjectItem(pJson, "short_name");
    pJsonX = cJSON_GetObjectItem(pJson, "sheet_x");
    pJsonY = cJSON_GetObjectItem(pJson, "sheet_y");
    pJsonApple = cJSON_GetObjectItem(pJson, "has_img_apple");
    pJsonGoogle = cJSON_GetObjectItem(pJson, "has_img_google");
    pJsonTwitter = cJSON_GetObjectItem(pJson, "has_img_twitter");
    pJsonFacebook = cJSON_GetObjectItem(pJson, "has_img_facebook");

    (pJsonUnified->valuestring) ? FormatUnicodeString(czStringUnified, pJsonUnified->valuestring) : 0;
    (pJSonNonQualified->valuestring) ? FormatUnicodeString(czStringNonQualified, pJSonNonQualified->valuestring) : 0;
    StringToLower(czStringKey, (pJSonNonQualified->valuestring) ? pJSonNonQualified->valuestring : pJsonUnified->valuestring);

    if (pJSonNonQualified->valuestring)
    {
        iRet = sprintf(pOut, "    \"%s\": [[\"%s\", \"%s\"], \"\", \"\", [\"%s\"], %d, %d, %d, 0],\r\n",
                    czStringKey,
                    czStringUnified,
                    czStringNonQualified,
                    pJsonName->valuestring,
                    pJsonX->valueint,
                    pJsonY->valueint,
                    BUILD_HALFBYTE(pJsonApple, pJsonGoogle, pJsonTwitter, pJsonFacebook)
                );
    }
    else
    {
        iRet = sprintf(pOut, "    \"%s\": [[\"%s\"], \"\", \"\", [\"%s\"], %d, %d, %d, 0],\r\n",
                            czStringKey,
                            czStringUnified,
                            pJsonName->valuestring,
                            pJsonX->valueint,
                            pJsonY->valueint,
                            BUILD_HALFBYTE(pJsonApple, pJsonGoogle, pJsonTwitter, pJsonFacebook)
                        );
    }


    return iRet;
}

static int InsertEmojiVariationsData(char *pOut, cJSON *pJson)
{
    int iRet;
    cJSON *pJsonUnified = NULL;
    cJSON *pJsonX = NULL;
    cJSON *pJsonY = NULL;
    cJSON *pJsonApple = NULL;
    cJSON *pJsonGoogle = NULL;
    cJSON *pJsonTwitter = NULL;
    cJSON *pJsonFacebook = NULL;
    char czStringKey[128] = {0};

    if (!pOut || !pJson)
    {
        return -1;
    }

    iRet = 0;
    for (cJSON *pNode = pJson->child; pNode; pNode = pNode->next)
    {
        iEmojiTotal++;

        pJsonUnified = cJSON_GetObjectItem(pNode, "unified");
        pJsonX = cJSON_GetObjectItem(pNode, "sheet_x");
        pJsonY = cJSON_GetObjectItem(pNode, "sheet_y");
        pJsonApple = cJSON_GetObjectItem(pNode, "has_img_apple");
        pJsonGoogle = cJSON_GetObjectItem(pNode, "has_img_google");
        pJsonTwitter = cJSON_GetObjectItem(pNode, "has_img_twitter");
        pJsonFacebook = cJSON_GetObjectItem(pNode, "has_img_facebook");

        StringToLower(czStringKey, pJsonUnified->valuestring);

        iRet += sprintf(pOut + iRet, "    \"%s\": [%d, %d, %d],\r\n",
                                        czStringKey,
                                        pJsonX->valueint,
                                        pJsonY->valueint,
                                        BUILD_HALFBYTE(pJsonApple, pJsonGoogle, pJsonTwitter, pJsonFacebook)
                        );
    }

    return iRet;
}

static int CountWidth(int iPicFileWidth, int iEmojiWidth)
{
    int iRet;

    for (iRet = 40; iRet < iPicFileWidth / (iEmojiWidth + 2); iRet++);

    return iRet;
}

int main(int iParamNum, char *pczParam[])
{
    char *pczJsonFileBuf;
    char *pczOutFileBuf;
    FILE *pfFile;
    int iOutLen;
    cJSON *json = NULL;
    cJSON *node = NULL;

    char czJsonFilePath[1000];
    char czOutFilePath[1024];
    int iImgFileWidth;
    int iEmojiWidth;

    printf("emoji.json file tools.\r\n");
    printf("tuwulin365@126.com 2020-01-14\r\n");
    printf("usage: exe emoji.json pic_width emoji_width\r\n\r\n");

    if (iParamNum == 4)
    {
        strcpy(czJsonFilePath, pczParam[1]);
        iImgFileWidth = atoi(pczParam[2]);
        iEmojiWidth = atoi(pczParam[3]);
    }
    else
    {
        strcpy(czJsonFilePath, JSON_FILE_PATH);
        iImgFileWidth = IMG_FILE_WIDTH;
        iEmojiWidth = EMOJI_WIDTH;
    }
    sprintf(czOutFilePath, "%s.txt", czJsonFilePath);

    if ((iImgFileWidth == 0) || (iEmojiWidth == 0))
    {
        printf("size param error\r\n");
        return -1;
    }

    pczJsonFileBuf = (char*)calloc(JSON_FILE_SIZE + OUT_FILE_SIZE, sizeof(char));
    pczOutFileBuf = pczJsonFileBuf + JSON_FILE_SIZE;
    if (!pczJsonFileBuf)
    {
        printf("malloc error\n");
        return -1;
    }

    pfFile = fopen(czJsonFilePath, "rb");
    if (!pfFile)
    {
        printf("open json file error\n");
        return -2;
    }

    fread(pczJsonFileBuf, 1, JSON_FILE_SIZE, pfFile);
    fclose(pfFile);

    iOutLen = 0;
    iOutLen += sprintf(pczOutFileBuf + iOutLen, "emoji.sheet_size = %d;\r\n\r\n", CountWidth(iImgFileWidth, iEmojiWidth));


    //从缓冲区中解析出JSON结构
    json = cJSON_Parse(pczJsonFileBuf);
    //printf("%s\n", cJSON_Print(json));
    if (cJSON_IsArray(json))
    {
        iOutLen += sprintf(pczOutFileBuf + iOutLen, "emoji.data = {\r\n");
        cJSON_ArrayForEach(node, json)
        {
            iOutLen += InsertEmojiData(pczOutFileBuf + iOutLen, node);
        }
        iOutLen += sprintf(pczOutFileBuf + iOutLen, "};\r\n\r\n");


        iOutLen += sprintf(pczOutFileBuf + iOutLen, "emoji.variations_data = {\r\n");
        cJSON_ArrayForEach(node, json)
        {
            cJSON *tnode = cJSON_GetObjectItem(node, "skin_variations");
            if (tnode)
            {
                iOutLen += InsertEmojiVariationsData(pczOutFileBuf + iOutLen, tnode);
            }
        }
        iOutLen += sprintf(pczOutFileBuf + iOutLen, "};\r\n\r\n");
    }

    pfFile = fopen(czOutFilePath, "wb");
    if (!pfFile)
    {
        printf("open out file error\n");
        return -3;
    }
    fwrite(pczOutFileBuf, 1, iOutLen, pfFile);
    fclose(pfFile);

    free(pczJsonFileBuf);

    printf("DONE!\r\nemoji total %d\n", iEmojiTotal);

    return 0;
}
