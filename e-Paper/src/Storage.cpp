#include "Storage.hpp"

char *Storage::ssid = new char[32];
char *Storage::password = new char[32];

size_t size = 32;

esp_err_t err;

//code from https://github.com/espressif/esp-idf/blob/master/examples/storage/nvs_rw_value/main/nvs_value_example_main.c

void Storage::init()
{
    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

void Storage::read()
{
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

        size_t required_size_ssid;
        size_t required_size_password;
        nvs_get_str(my_handle, "ssid", NULL, &required_size_ssid);
        ssid = (char*) malloc(required_size_ssid);
        nvs_get_str(my_handle, "ssid", ssid, &required_size_ssid);

        nvs_get_str(my_handle, "password", NULL, &required_size_password);
        password = (char*) malloc(required_size_password);
        nvs_get_str(my_handle, "password", password, &required_size_password);

        Serial.println(ssid);
        Serial.println(password);
    }

    nvs_close(my_handle);
}

void Storage::write(char *ssid, char *password)
{

    Serial.println(Storage::ssid);

    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

        err = nvs_set_str(my_handle, "ssid", ssid);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        err = nvs_set_str(my_handle, "password", password);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    }

    nvs_close(my_handle);
}

char *Storage::readSSID()
{
    read();
    return ssid;
}

char *Storage::readPassword()
{
    read();
    return password;
}
