#include <time.h>
#include<stdlib.h>
#include <string>
#include <strsafe.h>
#include "simple_http_server.h"
#include "PathReader.h"
#include "Utils.h"

#define UNLIMITED_STRING 1024*1024
PCSTR renderUnicodeToByteStrHtml(const std::wstring& szOriginalUnicodeMessage, size_t resultMaxSize);

typedef struct CHUNKS_DATA
{
    PHTTP_DATA_CHUNK chunkArray;
    DWORD numberOfChunks;
}CHUNKS_DATA;

void fnFreeDataChunks(CHUNKS_DATA& data_chunks)
{
    data_chunks.numberOfChunks = 0;
    fnFree(data_chunks.chunkArray);
}

#define INITIALIZE_HTTP_RESPONSE( resp, status, reason )    \
    do                                                      \
    {                                                       \
        RtlZeroMemory( (resp), sizeof(*(resp)) );           \
        (resp)->StatusCode = (status);                      \
        (resp)->pReason = (reason);                         \
        (resp)->ReasonLength = (USHORT) strlen(reason);     \
    } while (FALSE)

#define ADD_KNOWN_HEADER(Response, HeaderId, RawValue)               \
    do                                                               \
    {                                                                \
        (Response).Headers.KnownHeaders[(HeaderId)].pRawValue =      \
                                                          (RawValue);\
        (Response).Headers.KnownHeaders[(HeaderId)].RawValueLength = \
            (USHORT) strlen(RawValue);                               \
    } while(FALSE)


PCWSTR formatDomainName(PCWSTR szDomainName, DWORD dwPort)
{
    const DWORD cbSizeToHoldPortString = 5 * sizeof(WCHAR);
    const PCWSTR szDomainFormat = L"%ws:%d/";
    const DWORD cbSizeToHoldFullDomainName = fnGetWStringSize((PWSTR)szDomainName, MAX_PATH)
                                             + cbSizeToHoldPortString
                                             + 1;
    STRSAFE_LPWSTR pszDomainString = static_cast<STRSAFE_LPWSTR>(fnAllocate(cbSizeToHoldFullDomainName));
    if (NULL != pszDomainString)
    {
        StringCbPrintfW(pszDomainString, cbSizeToHoldFullDomainName, szDomainFormat, szDomainName, dwPort);
        return pszDomainString;
    }

    return NULL;
}

//*** PATCH ***
// DO NOT REVIEW. (FOR MEANTIME)
PWSTR string_to_allocated_buffer(const std::wstring& str)
{
    unsigned long buffer_size = str.size() * sizeof(WCHAR);
    PWSTR buffer = static_cast<PWSTR>(fnAllocate(buffer_size));
    if (buffer != NULL) {
        CopyMemory(buffer, str.data(), buffer_size);
    }

    return buffer;

}


SimpleHttpServer::SimpleHttpServer(PCWSTR szDomainName, DWORD dwPort, PCWSTR szServerRootPath, fpLogger lpfnLoggerFunction)
	:m_dwPort(dwPort),
	m_szDomainName(formatDomainName(szDomainName, dwPort)),
    m_lpLoggerFunction(lpfnLoggerFunction),
    m_szDefaultMessage(L"Not Found."),
    m_cbRequestMaxSize(2048),
    m_szServerRootPath(szServerRootPath)
{
    
    isInitializedSuccessfully = fnSetupHttpServer() && fnRegisterUrl(m_szDomainName);
    logInitializtionMessage();
}

SimpleHttpServer::SimpleHttpServer(SimpleHttpServer&& other)
:m_cbRequestMaxSize(other.m_cbRequestMaxSize),
m_dwPort(other.m_dwPort),
m_szDomainName(m_szDomainName),
m_szDefaultMessage(m_szDefaultMessage),
m_szServerRootPath(m_szServerRootPath),
m_lpLoggerFunction(m_lpLoggerFunction)
{
    m_RequestQueueHandle = other.m_RequestQueueHandle;
    other.m_RequestQueueHandle = INVALID_HANDLE_VALUE;
    other.shutDown();
    
}


SimpleHttpServer::~SimpleHttpServer()
{
    m_lpLoggerFunction(L"[INFO] Closing the server...");
	CloseHandle(m_RequestQueueHandle);
    if (NULL != m_szDomainName)
        HeapFree(GetProcessHeap(), 0, (LPVOID)m_szDomainName);
    shutDown();

}



void SimpleHttpServer::fnStart()
{
    if (!isInitializedSuccessfully) {
        throw ServerInitilizationError();
    }

    ULONG result;
    ULONG cbBytesReadFrom;
    DWORD cbSizeOfRequestBuffer = sizeof(HTTP_REQUEST) + m_cbRequestMaxSize;
    PHTTP_REQUEST lpRequestBuffer = static_cast<HTTP_REQUEST*>(fnAllocate(cbSizeOfRequestBuffer));

    if (NULL == lpRequestBuffer)
    {
        SetLastError(STATUS_NO_MEMORY);
        return;
    }
    
    lpRequestBuffer->RequestId = HTTP_NULL_ID;
    m_isRuning = TRUE;
    while(m_isRuning)
    {
       result = HttpReceiveHttpRequest(
            m_RequestQueueHandle,
            NULL,
            HTTP_RECEIVE_REQUEST_FLAG_COPY_BODY,
            lpRequestBuffer,
            cbSizeOfRequestBuffer,
            &cbBytesReadFrom,
            NULL
        );
       if (!m_isRuning) { break; }

       if (NO_ERROR == result)
       {
         //hnadle request
            PCWSTR sTextResponse = 
                fnHandleRequest((PHTTP_REQUEST)lpRequestBuffer);

            if (NULL != sTextResponse) {
                fnSendResponse(sTextResponse, lpRequestBuffer);
                HeapFree(GetProcessHeap(), 0, (LPVOID)sTextResponse);
            }
            else
            {              
                fnSendResponse(m_szDefaultMessage, lpRequestBuffer);
            }

       }
       Sleep(10);
    }

    HeapFree(GetProcessHeap(), 0, lpRequestBuffer);
}

void SimpleHttpServer::setIsRuningSwitch(BOOL isNeedToRun)
{
    m_isRuning = isNeedToRun;
}

BOOL SimpleHttpServer::fnSetupHttpServer()
{   
    //initialize service
    ULONG retCode = HttpInitialize(
        HTTPAPI_VERSION_1,
        HTTP_INITIALIZE_SERVER,   
        NULL
    );

    if (retCode != NO_ERROR)
    {
        return FALSE;
    }

    //set up requests handle:
    retCode = HttpCreateHttpHandle(
        &m_RequestQueueHandle,
        0);

    if (retCode != NO_ERROR)
    {
        return FALSE;
    }

	return TRUE;
}

BOOL SimpleHttpServer::fnRegisterUrl(PCWSTR szUrl) const
{
    if (NULL == szUrl)
    {
        return FALSE;
    }

    ULONG retCode = HttpAddUrl(
        m_RequestQueueHandle,
        szUrl,
        NULL
    );
    if (NO_ERROR != retCode)
    {
        return FALSE;
    }

    return TRUE;
}


PCWSTR appendToBasePath(PCWSTR basePath, PCWSTR path)
{
    DWORD sizeOfPath; 
    DWORD sizeOfBasePath;
    sizeOfBasePath = fnGetWStringLength((PWSTR)basePath, MAX_PATH);
    sizeOfPath = fnGetWStringLength((PWSTR)path, MAX_PATH);

    DWORD totalSizeOfFullPathBuffer = (sizeOfBasePath + sizeOfPath + 1) * 2;
    PWSTR fullPathWithBase = static_cast<PWSTR>(fnAllocate(totalSizeOfFullPathBuffer));
    
    StringCbCatW(static_cast<STRSAFE_LPWSTR>(fullPathWithBase), totalSizeOfFullPathBuffer, basePath);
    StringCbCatW(static_cast<STRSAFE_LPWSTR>(fullPathWithBase), totalSizeOfFullPathBuffer, path);
    return fullPathWithBase;


}

PCWSTR SimpleHttpServer::fnHandleRequest(LPVOID pDataStructure)
{
    PHTTP_REQUEST pRequest = reinterpret_cast<PHTTP_REQUEST>(pDataStructure);
    PCWSTR unicodeData;
    PCSTR renderedResponse;

    switch (pRequest->Verb)
    {
        case HttpVerbGET:
            //HandleGet
            m_lpLoggerFunction(L"[INFO] Got Valid Http Request!");
            unicodeData = fnHandleRequestGet(pRequest);        
            if (NULL == unicodeData)
            {
                m_lpLoggerFunction(L"[ERROR] Could not collect requested data!");
                return NULL;
            }

            renderedResponse = renderUnicodeToByteStrHtml(unicodeData, UNLIMITED_STRING);
            HeapFree(GetProcessHeap(), 0, (LPVOID)unicodeData);
            return reinterpret_cast<PCWSTR>(renderedResponse);
            


        default:
            m_lpLoggerFunction(L"[ERROR] Got Unrecognized Http Request!");
            return NULL;
    }
}


PCWSTR SimpleHttpServer::fnHandleRequestGet(LPVOID pDataStructure)
{
    PHTTP_REQUEST pRequest = reinterpret_cast<PHTTP_REQUEST>(pDataStructure);
    PCWSTR fullPathToRead = appendToBasePath(
        m_szServerRootPath,
        pRequest->CookedUrl.pAbsPath + 1); // ignore first '/'
    PathReader pathReader(fullPathToRead);
    std::wstring massage = std::wstring(L"[INFO] got file/path show request: ")
        + std::wstring(fullPathToRead);

    m_lpLoggerFunction(massage.c_str());
    std::wstring data_read((WCHAR*)pathReader.read_now()->data());
    //patch untill server will refactored too.
    PCWSTR pszDataToReturn = string_to_allocated_buffer(data_read);

    if (NULL != fullPathToRead)
    {
        HeapFree(GetProcessHeap(), 0, (LPVOID)fullPathToRead);
    }

    return pszDataToReturn;
}



CHUNKS_DATA fnGetResponseChunks(PCSTR dataToSend, DWORD dwDataSize, DWORD dwSizeForChunk)
{
    DWORD dwNumberOfChunks = ceil((double)dwDataSize / dwSizeForChunk); //round up
    DWORD cbSizeOfLastChunk = dwDataSize % dwSizeForChunk;
    PCSTR pToNextChunk = dataToSend;
    PHTTP_DATA_CHUNK chunks = (PHTTP_DATA_CHUNK)fnAllocate(sizeof(HTTP_DATA_CHUNK) * dwNumberOfChunks);
    if (NULL != chunks)
    {
        for (size_t i = 0; i < dwNumberOfChunks; i++, pToNextChunk += dwSizeForChunk)
        {
            chunks[i].DataChunkType = HttpDataChunkFromMemory;
            chunks[i].FromMemory.pBuffer = (PVOID)pToNextChunk;
            chunks[i].FromMemory.BufferLength = dwSizeForChunk;
        }
    }
    //set last chunk size
    if (NULL != chunks)
    {
        chunks[dwNumberOfChunks - 1].FromMemory.BufferLength = cbSizeOfLastChunk;
    }

    return CHUNKS_DATA{ chunks, dwNumberOfChunks};

}

void SimpleHttpServer::fnSendResponse(PCWSTR sTextToSend, LPVOID referenceRequest)
{
    PHTTP_REQUEST pReferenceRequest = reinterpret_cast<PHTTP_REQUEST>(referenceRequest);
    DWORD bytesSent;
    HTTP_RESPONSE  response;
    
    CHUNKS_DATA chunks_data = fnGetResponseChunks(reinterpret_cast<PCSTR>(sTextToSend),
        fnGetWStringSize(sTextToSend, UNLIMITED_STRING),
        m_cbRequestMaxSize);
    ULONG StatusCode = 200;
    INITIALIZE_HTTP_RESPONSE(&response, StatusCode, "OK");
    ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/html");
    ADD_KNOWN_HEADER(response, HttpHeaderAcceptEncoding, "UTF-8");
    ADD_KNOWN_HEADER(response, HttpHeaderContentEncoding, "UTF-8");

    if (NULL == chunks_data.numberOfChunks)
    {
        fnFreeDataChunks(chunks_data);
        return;
    }

    response.EntityChunkCount = chunks_data.numberOfChunks;
    response.pEntityChunks = chunks_data.chunkArray;

    DWORD result = HttpSendHttpResponse(
        m_RequestQueueHandle,
        pReferenceRequest->RequestId,
        HTTP_SEND_RESPONSE_FLAG_MORE_DATA,
        &response,
        NULL,
        &bytesSent,
        NULL,
        0,
        NULL,
        NULL
    );
    
    fnFreeDataChunks(chunks_data);

    if (NO_ERROR != result)
    {
        m_lpLoggerFunction(L"[ERROR] could not send the repsponse");
    }

    else
    {
        m_lpLoggerFunction(L"[INFO] Response Sent Successfully!");

    }
}



void SimpleHttpServer::logInitializtionMessage()const
{
    if(isInitializedSuccessfully)
    {
        m_lpLoggerFunction(L"[INFO] Server setup completed!");
        m_lpLoggerFunction(L"[INFO] Listening on:");
        m_lpLoggerFunction(m_szDomainName);
    }

    else {
        m_lpLoggerFunction(L"[ERROR] Could not setup server");
    }
    
}

void SimpleHttpServer::shutDown()
{
    HttpTerminate(HTTP_INITIALIZE_SERVER, NULL);
}



PCSTR renderUnicodeToByteStrHtml(const std::wstring& szOriginalUnicodeMessage, size_t resultMaxSize)
{
    CHAR szHtmlTemplate[] = "<html><head><meta charset=\"utf-8\"/><pre>%ws</pre></head></html>";
    DWORD cbSizeOfRenderedMessage =
        szOriginalUnicodeMessage.size() * sizeof(WCHAR)
        + sizeof(szHtmlTemplate) + 1;


    PCHAR rendered = static_cast<PCHAR>(fnAllocate(cbSizeOfRenderedMessage));
    if (NULL == rendered || NULL == szOriginalUnicodeMessage.data())
    {
        return NULL;
    }

    StringCbPrintfA(rendered, cbSizeOfRenderedMessage, szHtmlTemplate, szOriginalUnicodeMessage.c_str());
    return rendered;
}