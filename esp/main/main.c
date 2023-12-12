#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"
#include "driver/gpio.h"
#include <time.h>
#include "mqtt_client.h"

char *TAG = "BLE-Server";
uint8_t ble_addr_type;
void ble_app_advertise(void);

#define Rele 14
#define ledG 12
char tolken[50];

// MQTT Configurações
#define MQTT_SERVER "192.168.7.2:1883"
#define MQTT_TOPIC_REQUEST "token/access"
#define MQTT_TOPIC_RESPONSE "token/response"


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    // Lógica de tratamento de eventos MQTT
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            printf("Conectado ao servidor MQTT\n");
            break;
        case MQTT_EVENT_DISCONNECTED:
            printf("Desconectado do servidor MQTT\n");
            break;
        default:
            break;
    }
    return ESP_OK;
}

void enviarMensagemMQTT(const char *message)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_SERVER,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    // Espera pela conexão ao servidor MQTT
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Envia a mensagem para o tópico especificado
    int msg_id = esp_mqtt_client_publish(client, MQTT_TOPIC_REQUEST, message, 0, 1, 0);
    printf("Mensagem enviada: %s\n", message);
    printf("Mensagem ID: %d\n", msg_id);

    // Espera pela confirmação da entrega da mensagem
    vTaskDelay(pdMS_TO_TICKS(1000));

    esp_mqtt_client_stop(client);
    esp_mqtt_client_destroy(client);
}

// Write data to ESP32 defined as server
static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    char * data = (char *)ctxt->om->om_data;
    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    for(int i = 0; i < ctxt->om->om_len; i++){
        tolken[i] = ctxt->om->om_data[i];
    }
    printf("%s\n", tolken);
    printf("\n");

    // Subscrever ao tópico de resposta MQTT
    enviarMensagemMQTT(tolken);

    memset(tolken, 0, sizeof(tolken));
    
    return 0;
}

// Read data from ESP32 defined as server
static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}

// Array of pointers to other service definitions
// UUID - Universal Unique Identifier
static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x180),                 // Define UUID for device type
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(0xFEF4),           // Define UUID for reading
          .flags = BLE_GATT_CHR_F_READ,
          .access_cb = device_read},
         {.uuid = BLE_UUID16_DECLARE(0xDEAD),           // Define UUID for writing
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_write},
         {0}}},
    {0}};

// BLE event handling
// BLE event handling
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    // Advertise if connected
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    // Advertise again after completion of the event
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECTED");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}

// Define the BLE connection
void ble_app_advertise(void)
{
    // GAP - device name definition
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); // Read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable or non-connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // discoverable or non-discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

// The application
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_advertise();                     // Define the BLE connection
}

// The infinite task
void host_task(void *param)
{
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
}

void connectBLE()
{
    nvs_flash_init();                          // 1 - Initialize NVS flash using
    esp_nimble_hci_and_controller_init();      // 2 - Initialize ESP controller
    nimble_port_init();                        // 3 - Initialize the host stack
    ble_svc_gap_device_name_set("BLE-Server"); // 4 - Initialize NimBLE configuration - server name
    ble_svc_gap_init();                        // 4 - Initialize NimBLE configuration - gap service
    ble_svc_gatt_init();                       // 4 - Initialize NimBLE configuration - gatt service
    ble_gatts_count_cfg(gatt_svcs);            // 4 - Initialize NimBLE configuration - config gatt services
    ble_gatts_add_svcs(gatt_svcs);             // 4 - Initialize NimBLE configuration - queues gatt services.
    ble_hs_cfg.sync_cb = ble_app_on_sync;      // 5 - Initialize application
    nimble_port_freertos_init(host_task);      // 6 - Run the thread
}

static void init_led(void)
{
    gpio_reset_pin(Rele);
    gpio_set_direction(Rele, GPIO_MODE_OUTPUT);
    gpio_set_level(Rele, 1);
    
    gpio_reset_pin(ledG);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);
}

// void app_main()
// {   
//     init_led();
//     connectBLE();
// }


// Função para tratar mensagens MQTT recebidas
void mensagemMQTTCallback(void *handler_args, esp_mqtt_event_handle_t event) {
    printf("Mensagem recebida no tópico %.*s: %.*s\n", event->topic_len, event->topic, event->data_len, event->data);

    // Processar a mensagem recebida e acionar/desacionar o gpio_set_level conforme necessário
    if (strncmp((char *)event->data, "ok", event->data_len) == 0) {
        gpio_set_level(Rele, 0);
        gpio_set_level(ledG, 1);

        vTaskDelay(100);
        gpio_set_level(Rele, 1);
        gpio_set_level(ledG, 0);
    } else {
        gpio_set_level(Rele, 1);
        gpio_set_level(ledG, 0);
    }
}

void app_main()
{   
    init_led();
    connectBLE();

    // Configurar e iniciar o cliente MQTT
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_SERVER,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    while (1) {
        // Lidar com mensagens MQTT recebidas
        esp_mqtt_client_subscribe(client, MQTT_TOPIC_RESPONSE, 0); // Subscrever ao tópico MQTT

        esp_mqtt_event_handle_t event = NULL; // Evento para receber a mensagem
        esp_err_t err = esp_mqtt_client_wait_for_data(client, &event, 1000); // Esperar por dados por até 1000 ms

        if (err == ESP_OK && event != NULL) {
            mensagemMQTTCallback(NULL, event); // Chama a função de manipulação de mensagens MQTT
        }
    }
}