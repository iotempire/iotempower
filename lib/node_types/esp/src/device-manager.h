// device-manager.h
// author: ulno
// created: 2018-07-16
// Manage a list of devices

#ifndef _IOTEMPOWER_DEVICE_MANAGER_H_
#define _IOTEMPOWER_DEVICE_MANAGER_H_

////AsyncMqttClient disabled in favor of PubSubClient
//#include <AsyncMqttClient.h>
#include <PubSubClient.h>
#include <toolbox.h>
#include <device.h>

// scheduler
//typedef void (*DO_LATER_CB_ID)(int16_t);
typedef std::function<void(int16_t)> DO_LATER_CB_ID;
bool do_later(unsigned long in_ms, int16_t id, DO_LATER_CB_ID callback);
//typedef void (*DO_LATER_CB_NO_ID)();
typedef std::function<void()> DO_LATER_CB_NO_ID;
bool do_later(unsigned long in_ms, DO_LATER_CB_NO_ID callback);
void do_later_check(); // should not be called by user

// send device soon to deep_sleep, duration 0 means wakeup only via RST line
void deep_sleep(unsigned long time_from_now_ms, unsigned long duration_ms=0);


class DeviceManager {
public:
    static DeviceManager& get_instance();

    bool add(Device &device);
    bool start();
    bool update();
    bool publish(PubSubClient& mqtt_client, Ustring& node_topic, bool publish_all);
    bool subscribe(PubSubClient& mqtt_client, Ustring& node_topic);
    bool publish_discovery_info(PubSubClient& mqtt_client);
    bool receive(Ustring& subtopic, Ustring& payload);
    void get_report_list(Fixed_Buffer& b);
    void log_length(); // For debugging

/* out of simplicity reasons, we don't allow the deletion of devices
 * as we dropped interactive configuration in the transition from
 * micropython to C */
//bool remove_device(const char* name);

/* check if changes have to be published
 * -> eventually publish
 * This is only called with enough time distance so send buffer doesn't run over.
 * Careful a value causing a change could already have been overwritten.
 * Return true if anything has been published.
 * Return false if nothing was published.
 */
////AsyncMqttClient disabled in favor of PubSubClient
//bool devices_publish(AsyncMqttClient& mqtt_client, Ustring& node_topic, bool publish_all = false);
//bool devices_publish_discovery_info(AsyncMqttClient& mqtt_client);

private:
    DeviceManager();
    ~DeviceManager();

    Fixed_Map<Device, IOTEMPOWER_MAX_DEVICES> device_list;

    // Prevent copying and assignment
    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;
};

#define device_manager (DeviceManager::get_instance())

#endif // _IOTEMPOWER_DEVICE_MANAGER_H_
