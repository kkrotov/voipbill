#pragma once

#include "../../src-bill/classes/BDb.h"
#include <curl/curl.h>
#include "BaseTest.h"


size_t CURL_WRITE_CALLBACK (void *contents_temp, size_t size, size_t nmemb, void *test);
