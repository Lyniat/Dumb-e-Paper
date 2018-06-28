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

        err = nvs_get_str(my_handle, "ssid", ssid, &size);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("Restart counter = %d\n", ssid);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        err = nvs_get_str(my_handle, "password", password, &size);
        switch (err) {
            case ESP_OK:
                printf("Done\n");
                printf("Restart counter = %d\n", password);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }
    }

    nvs_close(my_handle);
}

void Storage::write(char *ssid, char *password)
{
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

        err = nvs_set_str(my_handle, "ssid", ssid);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        err = nvs_set_str(my_handle, "password", password);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");


        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
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
