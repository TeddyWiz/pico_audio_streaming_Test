/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "azure_samples.h"

/* Paste in the your IoT Hub connection string  */
const char pico_az_connectionString[] = "HostName=wiznetPerceptHub.azure-devices.net;DeviceId=pico_test_001;SharedAccessKey=tvSaEpc5ZXyybVsHxcKNuD3Hr4ZmiffZXPdPw4qCxDI=";//"[device connection string]";

const char pico_az_x509connectionString[] = "HostName=wiznetPerceptHub.azure-devices.net;DeviceId=picoevb001;x509=true";

const char pico_az_x509certificate[] =
"-----BEGIN CERTIFICATE-----""\n"
"MIIDlzCCAn8CFAdg9tH7RI03r0JvDcgyEb7nOp3iMA0GCSqGSIb3DQEBCwUAMIGH""\n"
"MQswCQYDVQQGEwJLUjERMA8GA1UECAwIR3llb25nZ2kxETAPBgNVBAcMCFNlb25n""\n"
"bmFtMQ8wDQYDVQQKDAZ3aXpuZXQxDDAKBgNVBAsMA0lvdDETMBEGA1UEAwwKcGlj""\n"
"b2V2YjAwMTEeMBwGCSqGSIb3DQEJARYPdGVkZHlAd2l6bmV0LmlvMB4XDTIyMDIy""\n"
"ODA2MzgxNFoXDTIzMDIyODA2MzgxNFowgYcxCzAJBgNVBAYTAktSMREwDwYDVQQI""\n"
"DAhHeWVvbmdnaTERMA8GA1UEBwwIU2VvbmduYW0xDzANBgNVBAoMBndpem5ldDEM""\n"
"MAoGA1UECwwDSW90MRMwEQYDVQQDDApwaWNvZXZiMDAxMR4wHAYJKoZIhvcNAQkB""\n"
"Fg90ZWRkeUB3aXpuZXQuaW8wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB""\n"
"AQCzFTX0sTkInNa27ZAbHP6aklDh0t39dOllW19kiq4DfUnYrlRBxzIgN7S5bEKN""\n"
"PP3ai8bU/W2uWIwR2OBMaMI+gXJRp3fTqOrGqnmR97tO4nU92KZpirQOENE94qUH""\n"
"iVMccoxYGN+wlLCOPAWyd/Q8rshPqdXbZdLwcwakvBkC6jNAU98kwh3f74k7223k""\n"
"nGVrOqqry0XemiygDuOMQn9zB7HpBfW8fXCFBxNjTHU86KgbPB84AdW+mOV1YC8v""\n"
"wBU5nHR72ICmoht6qOK494R2c2BA5sPbQuOUDTMKDvQUpov4rCcRcoDusgUmKJM+""\n"
"cdaJE3PSWXL7Uxb9Hshk54WNAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAEYIs1Sr""\n"
"6XXQMCfkA19NkA+QDaText6hmfEAvVCGtx4bc/HyowHGfYd1wEuq6En4KvIqh7HC""\n"
"FqL2UvdVlcnYG2zRzBne6oCRA7CV/tXuASADqpmfR+XApWez7rz/g0Gsl5x2at3M""\n"
"Z2poxb9pCW/Aey2/+AZQuVs1r1dDd4+/6NmTuE+vAuuqXc3niSKn1baevfX97zHD""\n"
"9LLvHKeVD+gFPd5wlDuRsNz8voSud7LAL4C5T06lt3l4Ytzxdg2NjWl8rKsHrZaT""\n"
"yBbTo7NP6FOfEuS1OppNFTTTDAyBX5xHmMnWdkX0ao7sktyxUMBqXo/L9kFq4K40""\n"
"pt8pP6qY/qj/CNo=""\n"
"-----END CERTIFICATE-----";

const char pico_az_x509privatekey[] =
"-----BEGIN PRIVATE KEY-----""\n"
"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCzFTX0sTkInNa2""\n"
"7ZAbHP6aklDh0t39dOllW19kiq4DfUnYrlRBxzIgN7S5bEKNPP3ai8bU/W2uWIwR""\n"
"2OBMaMI+gXJRp3fTqOrGqnmR97tO4nU92KZpirQOENE94qUHiVMccoxYGN+wlLCO""\n"
"PAWyd/Q8rshPqdXbZdLwcwakvBkC6jNAU98kwh3f74k7223knGVrOqqry0Xemiyg""\n"
"DuOMQn9zB7HpBfW8fXCFBxNjTHU86KgbPB84AdW+mOV1YC8vwBU5nHR72ICmoht6""\n"
"qOK494R2c2BA5sPbQuOUDTMKDvQUpov4rCcRcoDusgUmKJM+cdaJE3PSWXL7Uxb9""\n"
"Hshk54WNAgMBAAECggEBAKNcGu8OZ0+C+p6+ELGa/mn0RNHrMFOPvYSXGGq30sse""\n"
"FCoTxIqIciJPjo2Cwybh11PwI72RPOdIoOV66Ajrmx23qZfw427x7NCzG9jamkP4""\n"
"ciirpAos29jk4GrKf+5jB6ywlXObpRoIWRI2kJ/fqq1cZx+8dnf/568tzoahi1NK""\n"
"1/zkJ/CMtDXgnFNNwkmAPpUtD3KAdDZy/LgaMUh2C93cOlfnUmLQ60XjAKbn03KR""\n"
"C9BC8SDdGpgvM1MR7A7GFEHXPVRwNp4Ska3/b9V0RVs69JJ7F9m0dIu6AZ/om+aP""\n"
"+anOAG/M/HXkto5D+kYMHFofzaUec0QMnckiCDx+y6ECgYEA3aBWSB5mmBhLU3Af""\n"
"iAWY+OoyjbtvwpwHzorj/mslWjJvogkaowh9vB/O2X9J78TxPKgLMKYBxfI4ewPH""\n"
"nIMqGCSl82Mq/VfaKFnZ6k26rVQjKnN45JmmW//0fPJq4Q1+thbw5F4N2MO2iT9d""\n"
"8FMD2h6bwlhhDXGheM2To41lCnUCgYEAztuv/mbNPHeF6tUzkrKgZyh5V+bY5CGf""\n"
"voZkcg53ywPQDOBpwbaMuWOjaintQ2OXAnrvL7PkenhHwLB+2GBG/t4t3y5Ujzt4""\n"
"1oZ1LtL6Btf5Srbx7ZKIxmNrPniCw6c5GaOkRd25iWE197y63gEz5rIf29ThSMFg""\n"
"Cyi22sRSO7kCgYA50EGxvnx04KGFupH/Ibat+CoH3wVgduNydbjT/Y9Y4B9O3aEd""\n"
"NnHWSVXkVtgqu+1SLWP95NBBmtYxAONpaK3qbmT8ALqOLAS00fuOq4gu/uvNfyHi""\n"
"QFKtXS+iCHrpCmQrjAB7Ei36hdNwKh+POZifpaxsHjHzF9lPchE5cQESoQKBgQCx""\n"
"BWey+h6gpFhCAnnEDIEgRo6xHqh1ciDWVwQWZzM01OpyWommcKY08IOkEoUsqklM""\n"
"og+WbwgTlmMxDtk+KgYXjeMLnwZWHLroOuCFVZ8JxEFeIvkeKcxKmkHokBC3hp9i""\n"
"xZuK/pgMbNhWTXhNDBJyVfTVl3PDY1jhs9HnnSISYQKBgG6bYT5kWyRu0G0Dvhda""\n"
"3nf8EamgzUDCbQJV65f4sseIBAor3dKG5sCsVJUJJp3GPRH0aNcAi+rgqxsky2qx""\n"
"CYEHOkGMywqso+bCgM4BgnL2jNwdoKZYy5p9aqrkZK5PXZlwnp+8mSx6h+MpsqXM""\n"
"JYUWCf4LCQVvVIFGCmkeyFqA""\n"
"-----END PRIVATE KEY-----";

const char pico_az_id_scope[] = "[ID Scope]";

const char pico_az_COMMON_NAME[] = "pico_test_001";

const char pico_az_CERTIFICATE[] = 
"-----BEGIN CERTIFICATE-----""\n"
"-----END CERTIFICATE-----";

const char pico_az_PRIVATE_KEY[] = 
"-----BEGIN PRIVATE KEY-----""\n"
"-----END PRIVATE KEY-----";
