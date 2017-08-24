#include <windows.h>
#include <wincrypt.h>

inline std::string GetFileMD5(const char* filename)
{
	std::string md5_str;
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		HCRYPTPROV hProv = NULL;
		if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		{
			HCRYPTPROV hHash = NULL;
			if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
			{
				DWORD dwFileSize = GetFileSize(hFile, 0);
				if (dwFileSize != 0xFFFFFFFF)
				{
					byte* lpReadFileBuffer = new byte[dwFileSize];
					if (lpReadFileBuffer)
					{
						DWORD lpReadNumberOfBytes;
						if (ReadFile(hFile, lpReadFileBuffer, dwFileSize, &lpReadNumberOfBytes, NULL))
						{
							if (CryptHashData(hHash, lpReadFileBuffer, lpReadNumberOfBytes, 0))
							{
								BYTE *pbHash;
								DWORD dwHashLen = sizeof(DWORD);
								if (CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0))
								{
									pbHash = (byte*)malloc(dwHashLen);
									if (CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))
									{
										for (DWORD i = 0; i < dwHashLen; i++)
										{
											char str[3] = { 0 };
											_snprintf(str, sizeof(str), "%02x", pbHash[i]);
											md5_str += str;
										}
									}
								}
							}
						}
						delete[] lpReadFileBuffer;
					}
				}
				CryptDestroyHash(hHash);
			}
			CryptReleaseContext(hProv, 0);
		}
		CloseHandle(hFile);
	}
	return md5_str;
}

typedef struct _DESKEY_BLOB
{
	BLOBHEADER header;
	int len;
	unsigned char key[8];
}DESKEY_BLOB, *PDESKEY_BLOB;

DWORD MSDes(VOID *pBuffer, DWORD dwInSize, DWORD dwOutSize, VOID *pKey, BOOL bEncrypt, DWORD dwMode = CRYPT_MODE_CBC)
{
	HCRYPTPROV        hCryptProv;
	HCRYPTKEY        hKey;
	BOOL            bRetVal;
	_DESKEY_BLOB desKeyBlob;
	DWORD            dwRetSize;
	DWORD            dwEncSize;
	int                nError;

	nError = ERROR_SUCCESS;
	dwRetSize = 0;
	hCryptProv = NULL;
	hKey = NULL;
	dwEncSize = (dwInSize + 7) / 8 * 8;
	if (dwOutSize < dwEncSize)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return 0;
	}

	if (CryptAcquireContextA(&hCryptProv, NULL, MS_ENHANCED_PROV_A, PROV_RSA_FULL, 0) ||
		CryptAcquireContextA(&hCryptProv, NULL, MS_ENHANCED_PROV_A, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		memset(&desKeyBlob, 0, sizeof(desKeyBlob));
		desKeyBlob.header.bType = PLAINTEXTKEYBLOB;
		desKeyBlob.header.bVersion = CUR_BLOB_VERSION;
		desKeyBlob.header.reserved = 0;
		desKeyBlob.header.aiKeyAlg = CALG_DES;
		desKeyBlob.len = 8;
		memcpy(desKeyBlob.key, pKey, 8);
		if (CryptImportKey(hCryptProv, (BYTE*)&desKeyBlob, sizeof(desKeyBlob), NULL, 0, &hKey))
		{
			if (CryptSetKeyParam(hKey, KP_MODE, (BYTE*)(&dwMode), 0))
			{
				if (CryptSetKeyParam(hKey, KP_IV, (BYTE*)pKey, 0))
				{
					dwMode = PKCS5_PADDING;
					if (CryptSetKeyParam(hKey, KP_PADDING, (BYTE*)(&dwMode), 0))
					{
						if (bEncrypt)
						{    //    注意使用FALSE, 不追加尾部, 长度必须8的倍数否则失败
							if (CryptEncrypt(hKey, NULL, TRUE, 0, (BYTE *)pBuffer, &dwEncSize, dwOutSize))
							{
								dwRetSize = dwEncSize;
							}
						}
						else
						{
							if (CryptDecrypt(hKey, NULL, TRUE, 0, (BYTE *)pBuffer, &dwInSize))
							{
								*((unsigned char*)pBuffer+dwInSize) = 0;
								dwRetSize = dwInSize;
							}
						}
					}
				}
			}
			CryptDestroyKey(hKey);
		}
		CryptReleaseContext(hCryptProv, 0);
	}

	return dwRetSize;
}

int StrToHex(const char* src,unsigned char* buffer)
{
	char tmp[5];
	int v;
	int len = strlen(src) / 2;
	memset(tmp,0,sizeof(tmp));
	tmp[0] = '0';
	tmp[1] = 'x';
	int i = 0;
	for (; i < len; i++)
	{
		memcpy(&tmp[2], &src[i*2], 2);
		StrToIntEx(tmp, STIF_SUPPORT_HEX, &v);
		buffer[i] = v;
	}
	return i;
}