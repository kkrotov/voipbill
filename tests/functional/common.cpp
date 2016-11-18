# include "common.h"

size_t CURL_WRITE_CALLBACK (void *contents_temp, size_t size, size_t nmemb, void *test)  {
    size_t realsize = size * nmemb;
    if (test)
        ((BaseTest*)test)->curlWriteCallback(contents_temp, realsize);
    return realsize;
}
