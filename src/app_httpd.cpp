// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "esp_http_server.h"
#include "Arduino.h"

#include "web_site.h"

#define LED_BUILTIN 33

httpd_handle_t stream_httpd = NULL;
httpd_handle_t main_httpd = NULL;

static int8_t local_variable_1 = 0;
static int8_t local_variable_2 = 0;
static int8_t local_variable_3 = 0;

static bool led_on = false;

static void turn_led_on()
{
    digitalWrite(LED_BUILTIN, LOW); // Inverted logic
    led_on = true;
}

static void turn_led_off()
{
    digitalWrite(LED_BUILTIN, HIGH); // Inverted logic
    led_on = false;
}

static void led_toggle()
{
    // If LED was on; turn it off, vice versa
    led_on ? turn_led_off() : turn_led_on();
    // {{condition}} ? {{do if true}} : {{do if false}};
}

static esp_err_t cmd_handler(httpd_req_t *req)
{
    char *buf;
    size_t buf_len;
    char variable[32] = {
        0,
    };
    char value[32] = {
        0,
    };

    int res = -1;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
        buf = (char *)malloc(buf_len);
        if (!buf)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK &&
                httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK)
            {
                // Enters if query has 'key=var' structure
            }
            else if (!strcmp(buf, "led_toggle"))
            {
                led_toggle();
                res = 0;
            }
            else if (!strcmp(buf, "led_on"))
            {
                turn_led_on();
                res = 0;
            }
            else if (!strcmp(buf, "led_off"))
            {
                turn_led_off();
                res = 0;
            }

            else
            {
                free(buf);
                httpd_resp_send_404(req);
                return ESP_FAIL;
            }
        }
        else
        {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    }
    else
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    int val = atoi(value);

    if (res)
    {
        res = 0;
        if (!strcmp(variable, "local_variable_1"))
            local_variable_1 = val;
        else if (!strcmp(variable, "local_variable_2"))
            local_variable_2 = val;
        else if (!strcmp(variable, "local_variable_3"))
            local_variable_3 = val;
        else
        {
            res = -1;
        }
    }

    if (res)
    {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t status_handler(httpd_req_t *req)
{
    static char json_response[1024];

    char *p = json_response;
    *p++ = '{';

    p += sprintf(p, "\"local_variable_1\":%u,", local_variable_1);
    p += sprintf(p, "\"local_variable_2\":%u,", local_variable_2);
    p += sprintf(p, "\"local_variable_3\":%u", local_variable_3);
    p += sprintf(p, "\"led_on\":%u", led_on);
    *p++ = '}';
    *p++ = 0;

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    return httpd_resp_send(req, (const char *)web_site_html_gz, web_site_html_gz_len);
}

void startWebServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL};

    httpd_uri_t status_uri = {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_handler,
        .user_ctx = NULL};

    httpd_uri_t cmd_uri = {
        .uri = "/control",
        .method = HTTP_GET,
        .handler = cmd_handler,
        .user_ctx = NULL};

    Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&main_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(main_httpd, &index_uri);
        httpd_register_uri_handler(main_httpd, &cmd_uri);
        httpd_register_uri_handler(main_httpd, &status_uri);
    }
}